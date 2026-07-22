# SPAM detection (mod.dronescan)

This document describes the spam-detection subsystem added to `mod.dronescan`
since commit `8475c0edc40d873dc792d66b0a23c1f27e86402`. It covers the data
model, the runtime scoring pipeline, the spy-client infrastructure, and the
full `SPAM` admin command.

## Overview

Spam detection works on a **score-and-threshold** model:

1. **Events** (`SPAM EVENT`) describe *what to detect* (a regex match, a
   flood of repeated text, etc.) and how many points a hit is worth.
2. **Rules** (`SPAM RULE`) link one or more events together with a point
   **threshold**. When an actor's accumulated points from a rule's linked
   events reach the threshold, the rule fires.
3. **Actions** (`SPAM ACTION`) describe what happens when a rule fires
   (GLINE, KILL, or REPORT), and are linked to rules via `SPAM RULE
   ADDACTION`.
4. **Monitored channels** (`SPAM CHAN`) are the channels dronescan actually
   watches. Watching is done either by the bot itself (`joinasservice`) or
   by a **spy client** (`SPAM SPYCLIENT`) - a fake IRC client introduced via
   P10 that sits in the channel like a normal user.

```
EVENT ---(scores points on match)---> RULE ---(threshold crossed)---> ACTION
                                         |
                                  scoped to CHAN(s)
```

## Data model

All tables are defined in [`doc/dronescan.sql`](../doc/dronescan.sql); the
column-level comments there are the authoritative reference. Summary:

| Table | Purpose |
|---|---|
| `spam_events` | What to detect: type, regex/threshold `param`, `target` bitmask, points, expiry, TEXT_REPEAT-specific columns. |
| `spam_rules` | A point `threshold`, scoring granularity (`points_per`, `score_globally`), optional `wait_on_rule_id` chaining, `allchans` channel-scope flag. |
| `spam_rule_events` | Many-to-many rule<->event link, with optional `points_override`. |
| `spam_actions` | Reusable action templates: `GLINE`/`KILL`/`REPORT`, duration, reason, delay (+ jitter via `rand_min`/`rand_max`), `prefix_auto` (GLINE reason prefix). |
| `spam_rule_actions` | Many-to-many rule<->action link, with per-binding overrides for duration/reason/delay. |
| `spam_exclusions` | `CHAN`/`NICK`/`IP`/`OPER` entries that bypass scanning entirely, plus `GATEWAYIP` entries that force `user@ip` gline masks. |
| `monitored_channels` | Channels dronescan watches; `forcejoin`, `joinasservice`, and last-triggered-rule tracking. |
| `spam_rule_channels` | Per-rule channel exclusion/inclusion list (meaning depends on `spam_rules.allchans`). |
| `monitored_channel_spyclients` | Optional restricted spy-client pool per channel. |
| `spyclients` | Fake IRC clients (nick/user/host/ip/realname/account/modes) available to cover monitored channels. |

Row identity for `spam_events`, `spam_rules`, and `spam_actions` is a
case-insensitive-unique `name` (migration `004_spam_names_unique.sql`) - the
admin command never asks for a numeric id for these three objects.

Schema evolves via the numbered files in
[`migrations/`](../mod.dronescan/migrations/); each is applied automatically
by dronescan on the next restart (see `migrations/README`), never by hand.

## Event types (`spam_events.event_type`)

| Type | `param` meaning | Notes |
|---|---|---|
| `TEXT` | PCRE2 regex | Matched against text selected by the `target` bitmask. Case-sensitive unless the pattern is prefixed `(?i)`. |
| `TEXT_REPEAT` | unused | Detects the same text repeated `repeat_min_count`+ times within `point_expiry` seconds. See below. |
| `ENTROPY_TEXT` | float threshold | Message entropy. |
| `ENTROPY_NICK` | float threshold | Nick entropy. |
| `JOIN_CHANNEL` | channel name | |
| `USERMODE` | mode string (e.g. `+x`) | |
| `KICK_MSG` | PCRE2 regex | Matched against kick reasons. |
| `KICK_COUNT` | integer | Fires after a user is kicked from that many distinct channels. |

Only `TEXT` and `TEXT_REPEAT` are wired into the live scoring engine today
(`dronescan::processSpamText`); the others are recognized by the schema and
command layer but not yet evaluated at runtime.

### `target` bitmask

Controls which traffic sources a `TEXT`/`TEXT_REPEAT` event watches
(`mod.dronescan/constants.h`, `gnuworld::ds::spam_target`):

| Bit | Value | Source |
|---|---|---|
| `CHAN_PRIV` | 1 | Channel PRIVMSG |
| `PRIVMSG` | 2 | PRIVMSG direct to the bot or a spy client |
| `CHAN_NOT` | 4 | Channel NOTICE |
| `PART` | 8 | Part reasons |
| `QUIT` | 16 | Quit reasons |
| `NOTICE` | 32 | NOTICE direct to the bot or a spy client |
| `CTCP_PRIV` | 64 | CTCP direct to the bot or a spy client |
| `CTCP_CHAN` | 128 | CTCP seen in a channel (e.g. ACTION/`/me`, DCC) |
| `ALL` | 255 | Everything |

`chan` is a command-layer alias for `CHAN_PRIV|CHAN_NOT` (5); `ctcp` is an
alias for `CTCP_PRIV|CTCP_CHAN` (192). There is no separate DCC bit - match
DCC requests with a regex like `^DCC` scoped to `ctcp_priv`/`ctcp_chan`.

### TEXT_REPEAT

Tracks identical text (case-folded per `case_sensitive`) per
channel/privmsg scope:
- `repeat_min_count` (default 2): occurrences needed before it fires.
- `repeat_crossuser` (default false): if true, *any* client repeating text
  scores, and points are awarded to every participant, not just the
  repeater.
- `repeat_exclusion_regex`: text matching this pattern is never tracked
  (e.g. to ignore URLs/greetings).

The tracking entry is not cleared on fire, so later repeaters within the
same window keep scoring.

## Scoring pipeline (`dronescan.cc`)

1. **`processSpamText(theClient, text, target_bit, channel_name)`** is
   called from every relevant IRC handler (channel PRIVMSG/NOTICE, direct
   PRIVMSG/NOTICE to the bot or a spy client, part, quit, CTCP). It walks
   all enabled events whose `target` bitmask matches `target_bit`, running
   the PCRE2 regex (`TEXT`) or repeat tracking (`TEXT_REPEAT`), and calls
   `scoreEvent()` on a match.
2. **`scoreEvent(ev, actor, channel, now, text)`** finds every rule linked
   to the event, builds a scoring key via `buildScoringKey()`, and
   increments an in-memory occurrence counter for that
   `(rule, actor-or-IP, channel)` bucket - capped at `max_occurrence` if
   set, and reset if the event's `point_expiry` window has elapsed.
3. **`buildScoringKey(rule, actor, channel)`** produces
   `"<rule_id>.<channel-or-privmsg>.<unit>"` (or without the channel
   segment if `score_globally` is true). `unit` is the client's numeric
   nick, or its IP if `points_per = IP` - so IP-scoped rules aggregate
   points across all nicks sharing an address.
4. **`evaluateSpamRules(actor, channel, displayChannels)`** runs after
   scoring, for every affected actor. For each enabled rule in scope for
   the channel (per `allchans`/`spam_rule_channels`), it sums
   `count * points` (or `points_override`) across the rule's linked
   events, ignoring any whose window has expired. If the total reaches
   `threshold`, **`fireRuleActions()`** runs and the rule's score buckets
   are reset to prevent immediate re-triggering.
5. **`fireRuleActions(rule, actor, displayChannels, triggerText)`** resolves
   every enabled action linked to the rule - reason, duration, and effective
   delay (`spam_rule_actions.delay_override`, else `spam_actions.delay`,
   plus jitter from `rand_min`/`rand_max` when both are set:
   `actual_delay = delay + random(rand_min, rand_max)`). An action whose
   effective delay is `<= 0` runs immediately via **`executeSpamAction()`**;
   otherwise it's captured (by value - action type, reason, duration, the
   `SpamActor` snapshot, rule name, channels, trigger text) into a
   `PendingSpamAction` and scheduled on a one-shot timer
   (`pendingSpamActionTimers`), the same delayed-timer pattern used for spy
   client joins (`scheduleSpyClientJoin`). `OnTimer()` calls
   `executeSpamAction()` when it fires.
6. **`executeSpamAction(actionType, reason, duration, prefixAuto, actor,
   ruleName, displayChannels, triggerText)`** runs a single resolved action:
   - `REPORT`: logs a sanitized one-line summary (nick/user/host/ip, rule
     name, channels, and the triggering text) to the console channel.
   - `GLINE`: queues a GLINE using the resolved duration and reason. The
     mask is `*@ip`, unless `ip` matches a `GATEWAYIP` exclusion entry, in
     which case it's `user@ip` instead (see [EXCLUSION](#exclusion)). Every
     queued gline (SPAM-triggered or not) gets a `[N] ` prefix from
     `processGlineQueue()` - `N` is the number of clients currently
     connected from that IP - and, when the action's `prefix_auto` is true
     (the default), an additional `AUTO ` ahead of that:
     `AUTO [N] reason` vs `[N] reason`.
   - `KILL`: looks up the offender's still-connected `iClient` by numeric
     (`Network->findClient`) and kills it with the resolved reason; silently
     skipped (with a console log line) if the client has since quit.

   The `SpamActor` snapshot (nick/user/host/ip/numeric) is captured at
   match time, so REPORT/GLINE still resolve correctly even if the offender
   has since quit (or a delayed action outlives their connection) - this is
   what makes crossuser `TEXT_REPEAT` reporting work. KILL is the one action
   that needs the offender still connected. Because `PendingSpamAction`
   holds no pointer into the triggering `sqlSpamRule`/`sqlSpamAction`, a
   `RULE DEL`/`ACTION DEL` while a delayed action is in flight cannot leave
   a dangling reference - the action still fires with the values resolved
   at trigger time (same principle already relied on for `glineQueue`).

`wait_on_rule_id` chains rules ("only GLINE if the REPORT rule already
fired for this actor") and `requires_event_id` gates an event on another
event having recently fired for the same user.

### Logging

Every `executeSpamAction()` outcome (REPORT, GLINE, KILL) is written, in
addition to the console-channel announcement above, to a persistent
`spam-action.log` file via the `gnuworld.ds.spam.action` log4cplus category
(`bin/logging.properties`, same `DailyRollingFileAppender` mechanism as the
existing `jf-glined.log`/`jf-cservice.log` join-flood logs). Unlike the
console announcement — which truncates the trigger text to 200 bytes
(`sanitizeSpamTextForReport()`) to keep it IRC-message-sized — the log file
always holds the complete actor identity (nick!user@host/ip), rule name,
and reason/trigger text, run through `sanitizeSpamTextForLog()` (same
control-byte stripping, but no length cap and no `"..."`). Requires
`--with-log4cplus` (see `CLAUDE.md`); logging is silently skipped if the
module was built without it.

## Spy clients and monitored channels

- `monitored_channels` lists the channels dronescan watches. Each can be
  watched by the bot directly (`joinasservice`) or by a spy client.
- `spyclients` are P10-introduced fake clients (nick/user/host/ip/realname,
  optional services account, user modes) that join monitored channels to
  observe traffic dronescan itself isn't in.
- `dronescan::findBestSpyClient(chanName, forcejoin)` picks which spy
  client covers a channel. If `monitored_channel_spyclients` has rows for
  that channel, selection is restricted to that list (starting at a random
  entry and walking down it); otherwise any configured, enabled spy client
  may be picked from the full pool.
- `forcejoin` forces the join past `+i`/`+k`/`+l` or bans via a
  server-level protocol override.

## Admin command: `SPAM`

Full syntax reference lives in the doc comment at the top of
[`SPAMCommand.cc`](../mod.dronescan/SPAMCommand.cc) and in the live
`HELP SPAM [object]` output (`HELPCommand.cc`); both are kept in sync with
behavior. Top-level shape:

```
SPAM <object> <verb> [args]
```

Objects: `EVENT`, `RULE`, `ACTION`, `EXCLUSION`, `SPYCLIENT`, `CHAN`.

Multi-word arguments (reason, description, param, realname, etc.) must be
wrapped in double quotes, e.g. `"Spam detected"`; a literal `"` inside one
is written `\"`. Quoting is optional for single-word values.

### Typical workflow

```
SPAM EVENT  ADD pill_regex TEXT chan,privmsg "buy.*cheap.*pills" 10 60
SPAM ACTION ADD gline_1h GLINE 3600 "Spam detected" 0
SPAM RULE   ADD anti_spam_global 50
SPAM RULE   ADDEVENT  anti_spam_global pill_regex
SPAM RULE   ADDACTION anti_spam_global gline_1h 3600 "Spam detected" 0
SPAM RULE   ADDCHAN   anti_spam_global #watch-me
SPAM CHAN   ADD #watch-me
SPAM SPYCLIENT ADD SpyBot spy spy.host.com 1.2.3.4 "Observer"
```

### EVENT / RULE / ACTION

- Identified by **name**, matched case-insensitively, unique per table.
- `SET <name> <field> <value>` edits any field in place, including renaming
  (`SET <name> name <new_name>`).
- `EVENT ADD` accepts repeatable `-rule <rule_name>` to link the new event
  to existing rules immediately, and a mandatory `-repeat_count <n>` when
  `<type>` is `TEXT_REPEAT` (invalid otherwise).
- `RULE ADD` accepts repeatable `-action <action_name>` to link existing
  actions immediately (no override values in that shorthand - use
  `RULE ADDACTION` afterward for duration/reason/delay overrides).
- `RULE SET wait_on_rule_id` and `EVENT SET requires_event_id` take a
  rule/event **name** as the value, or `none`/empty to clear the
  reference.
- `RULE ADDACTION` rejects a duplicate `(rule, action)` binding with a
  clear message (enforced by a `UNIQUE(rule_id, action_id)` DB constraint,
  migration `004`); `RULE REMACTION <rule_name> <action_name>` is
  unambiguous as a result.
- `RULE allchans`: `1` (default) = applies to every monitored channel,
  with `ADDCHAN` entries acting as an *exclusion* list; `0` = applies only
  to channels added via `ADDCHAN` (inclusion/whitelist). Toggling requires
  clearing existing `ADDCHAN` entries first (enforced by the command, not
  the DB).

### EXCLUSION

`CHAN`/`NICK`/`IP`/`OPER` entries that exempt scanning entirely; still
id-keyed (no `name` column). Checked once per `processSpamText()` call, right
after the actor's identity is captured and before any event is evaluated -
a match bypasses all spam detection for that call. `value` is a glob mask
(`gnuworld::match()`, case-insensitive; IP/CIDR masks are matched natively):
`CHAN` against the channel the traffic occurred in (never matches direct
PRIVMSG/NOTICE, which has no channel), `NICK`/`IP` against the actor's
nick/ip, and `OPER` against the actor's nick but only when the client
currently has `+o` (IRC operator) set - use `*` there to exempt opers
unconditionally.

`GATEWAYIP` is different: it does **not** bypass detection. It marks an IP
(typically a shared IRC gateway such as irccloud or mibbit, where many
unrelated users share one address) as requiring `user@ip` instead of the
usual wildcard `*@ip` in any GLINE issued against it (see
[executeSpamAction](#scoring-pipeline-dronescancc) above), so the gline
doesn't collateral-damage every other user behind the same gateway. `value`
may be a plain IP or a CIDR block, IPv4 or IPv6, e.g.
`SPAM EXCLUSION ADD GATEWAYIP 5.254.36.56/29`.

### SPYCLIENT

Keyed by nickname (or id, which survives nick changes). `ENABLE`/`DISABLE`
soft-toggle without deleting the row; `SET ... enabled <0|1>` is
equivalent.

### CHAN

Keyed by channel name. `ADDSPY`/`REMSPY` restrict which spy clients may
cover a given channel (see [Spy clients](#spy-clients-and-monitored-channels)
above). `LIST`/`SHOW` surface `last_triggered_ts`/`last_triggered_rule` so
operators can see what fired most recently in a channel.

### Command-line abbreviations

Object and verb tokens may be shortened when unambiguous: objects
`E`=EVENT, `R`=RULE, `A`=ACTION, `EX`=EXCLUSION, `S`=SPYCLIENT, `C`=CHAN;
verbs `A`=ADD, `D`=DEL, `L`=LIST, `E`=ENABLE. Verbs that collide on their
first letter within an object (`SHOW`/`SET`; `DEL`/`DISABLE` in SPYCLIENT
and CHAN) must be typed in full there. Compound verbs use two letters:
`AE`=ADDEVENT, `RE`=REMEVENT, `AA`=ADDACTION, `RA`=REMACTION,
`AC`=ADDCHAN, `RC`=REMCHAN, `AS`=ADDSPY, `RS`=REMSPY.

### Access control

Every mutating subcommand checks `theUser->getAccess()` against a required
level (`mod.dronescan/levels.h`) before proceeding; insufficient access
returns "Access denied."

## Known gaps

- `ENTROPY_TEXT`, `ENTROPY_NICK`, `JOIN_CHANNEL`, `USERMODE`, `KICK_MSG`,
  `KICK_COUNT` event types are recognized by the schema/command layer but
  not yet evaluated by `processSpamText`.
- `spam_rules.wait_on_rule_id` and `spam_events.requires_event_id` are
  loaded from the DB and fully settable via `RULE SET`/`EVENT SET`, but
  nothing in `evaluateSpamRules`/`scoreEvent` actually consults them yet -
  rule chaining and event gating are not enforced despite being described
  above as if they were.
- `spam_exclusions.value` for `OPER` is matched only against the actor's
  nick; there's no way to exempt by oper hostmask or services account.
