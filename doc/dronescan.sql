CREATE TABLE users (
	id serial,
	user_name varchar(20) NOT NULL,
	created int4 NOT NULL DEFAULT 0,
	last_seen int4 NOT NULL DEFAULT 0,
	last_updated int4 NOT NULL DEFAULT 0,
	last_updated_by varchar(128) NOT NULL,
	flags int4 NOT NULL DEFAULT 0,
	access int4 NOT NULL DEFAULT 0,
	PRIMARY KEY (id)
);
-----------------------
-- Legacy table; kept for backward compatibility with existing fakeClient code.
-- Not used for spam monitoring. See spyclients below.
CREATE TABLE fakeclients (
	id serial, 
	nickname varchar(20) NOT NULL, 
	username varchar(20) NOT NULL, 
	hostname varchar(20) NOT NULL, 
	realname varchar(50) NOT NULL, 
	created_by int4 NOT NULL, 
	created_on int4 NOT NULL, 
	last_updated int4 NOT NULL,
	flags int4 DEFAULT 0 NOT NULL,
	PRIMARY KEY (id), 
	FOREIGN KEY (created_by) REFERENCES users (id) ON UPDATE RESTRICT ON DELETE CASCADE,
	UNIQUE (nickname)
);
-------------------------
-- Fake IRC clients spawned by dronescan to monitor channels for spam.
-- These are introduced to the network as real clients via the P10 protocol.
--
-- ip: standard IPv4 or IPv6 address string (e.g. "1.2.3.4" or "2001:db8::1").
--   The code converts to Base64 P10 encoding on the fly when introducing the client.
-- modes: IRC user mode string applied at introduce time (e.g. "+i").
-- account / account_id: optional services authentication.
--   account_id 0 means not authenticated.
-- enabled: soft-disable without removing from the table.
CREATE TABLE spyclients (
	id          serial       PRIMARY KEY,
	nickname    varchar(30)  NOT NULL,
	username    varchar(20)  NOT NULL,
	hostname    varchar(100) NOT NULL,
	ip          varchar(45)  NOT NULL,
	realname    varchar(50)  NOT NULL,
	account     varchar(20)           DEFAULT NULL,
	account_id  int4         NOT NULL  DEFAULT 0,
	modes       varchar(20)  NOT NULL  DEFAULT '+i',
	enabled     bool         NOT NULL  DEFAULT true,
	created_by  int                   DEFAULT NULL
		REFERENCES users(id) ON DELETE SET NULL,
	created_ts  int4         NOT NULL  DEFAULT 0,
	modified_ts int4         NOT NULL  DEFAULT 0,
	modified_by int                   DEFAULT NULL
		REFERENCES users(id) ON DELETE SET NULL,
	UNIQUE (nickname)
);
-------------------------
CREATE TABLE exceptionalChannels (
	id serial,
	name text unique not null
);

-- =============================================================================
-- Spam detection system
-- =============================================================================

-- -----------------------------------------------------------------------------
-- spam_events
-- -----------------------------------------------------------------------------
-- Defines what to detect. event_type controls how param is interpreted:
--   TEXT           : param is a PCRE2 regex pattern matched against
--                    incoming text selected by the `target` bitmask.
--                    Active targets: chan_priv, chan_not, privmsg, notice,
--                    part, quit.
--   TEXT_REPEAT    : param is unused; matched text is tracked in memory.
--                    See repeat_* columns below for TEXT_REPEAT-specific options.
--   ENTROPY_TEXT   : param is a float threshold (as text) for message entropy
--   ENTROPY_NICK   : param is a float threshold (as text) for nick entropy
--   JOIN_CHANNEL   : param is the exact channel name to watch
--   USERMODE       : param is the mode string to match (e.g. "+x")
--   KICK_MSG       : param is a PCRE2 regex matched against kick reasons
--   KICK_COUNT     : param is an integer; fires when a user has been kicked
--                    from that many different channels
--
-- target: integer bitmask controlling which message sources this event watches.
--   Bit values (combine with bitwise OR):
--     CHAN_PRIV = 1   channel PRIVMSG
--     PRIVMSG   = 2   PRIVMSG sent directly to the bot or a spy client
--     CHAN_NOT   = 4   NOTICE sent to a channel
--     PART      = 8   part messages
--     QUIT      = 16  quit messages
--     NOTICE    = 32  NOTICE sent directly to the bot or a spy client
--     CTCP_PRIV = 64  CTCP sent directly to the bot or a spy client
--     CTCP_CHAN = 128 CTCP sent to a channel, seen by the bot or a spy client
--                     (e.g. CTCP ACTION/"/me", or a DCC request - match DCC with a
--                     regex like "^DCC" scoped to ctcp_priv/ctcp_chan, no separate DCC bit)
--   Examples: 1=chan_priv only, 5=chan(priv+not), 255=all targets
--   "chan" is an alias for CHAN_PRIV|CHAN_NOT (=5) in the SPAM command;
--   "ctcp" is an alias for CTCP_PRIV|CTCP_CHAN (=192).
--   For event types where direction is implicit (ENTROPY_NICK, KICK_COUNT, etc.),
--   the target bitmask is ignored at runtime.
--
-- max_occurrence: points stop accumulating after this many occurrences per
--   scoring unit (CLIENT or IP, see spam_rules.points_per). NULL = unlimited.
--
-- requires_event_id: secondary event guard ? points are only scored if the
--   referenced event has also recently fired for the same user.
--
-- repeat_crossuser: TEXT_REPEAT only.
--   false (default): only self-repeats count (a user repeating their own text).
--   true: any client repeating the same text in the same channel/target scores
--   points, AND points are awarded to ALL clients involved (including the first
--   sender).
-- repeat_min_count: TEXT_REPEAT only. Minimum occurrences of the same text
--   before the event fires. Default 2.
-- repeat_exclusion_regex: TEXT_REPEAT only. PCRE2 pattern; text matching this
--   regex is never tracked for repetition (e.g. to ignore URLs or greetings).
-- -----------------------------------------------------------------------------
CREATE TABLE spam_events (
	id                     serial       PRIMARY KEY,
	name                   varchar(100) NOT NULL,
	description            text,
	event_type             varchar(30)  NOT NULL,
	-- PCRE2 pattern, integer threshold, or float threshold depending on event_type
	param                  text,
	-- Bitmask: CHAN=1 PRIVMSG=2 NOTICE=4 PART=8 QUIT=16  (31 = all targets)
	target                 int          NOT NULL DEFAULT 31,
	-- TEXT_REPEAT only: case-folds the dedup key used to detect repeated
	-- text (see dronescan.cc processRepeatEvent()). Has no effect on TEXT
	-- event regex matching; use an inline "(?i)" prefix in the pattern
	-- itself for case-insensitive TEXT matching (native PCRE2 syntax).
	case_sensitive         bool         NOT NULL DEFAULT true,
	points                 int          NOT NULL DEFAULT 1,
	-- Seconds before accumulated points from this event decay (per scoring unit, in memory)
	point_expiry           int          NOT NULL DEFAULT 60,
	-- Points stop accumulating after this many occurrences; NULL = unlimited
	max_occurrence         int                   DEFAULT NULL,
	-- Only score this event if the referenced event has also fired for the same user
	requires_event_id      int                   DEFAULT NULL
		REFERENCES spam_events(id) ON DELETE SET NULL,
	enabled                bool         NOT NULL DEFAULT true,
	-- TEXT_REPEAT-specific columns (ignored for all other event types)
	repeat_crossuser       bool         NOT NULL DEFAULT false,
	repeat_min_count       int          NOT NULL DEFAULT 2,
	repeat_exclusion_regex text                  DEFAULT NULL,
	created_ts             int4         NOT NULL DEFAULT 0,
	modified_ts            int4         NOT NULL DEFAULT 0,
	modified_by            int                   DEFAULT NULL
		REFERENCES users(id) ON DELETE SET NULL
);

-- -----------------------------------------------------------------------------
-- spam_rules
-- -----------------------------------------------------------------------------
-- A rule defines a score threshold. When a user's accumulated score from its
-- linked events crosses `threshold`, all linked actions are fired.
--
-- wait_on_rule_id: this rule only activates if the referenced rule has already
--   fired for the same user (chained escalation).
--   Example: "only GLINE if the REPORT rule already fired."
--
-- allchans: controls which monitored_channels rows this rule applies to.
--   true  (default): rule applies to ALL monitored channels.
--     Rows in spam_rule_channels are treated as an EXCLUSION list (channels
--     to skip for this rule).
--   false: rule applies ONLY to channels listed in spam_rule_channels
--     (INCLUSION / whitelist mode).
--   IMPORTANT: when toggling allchans, all spam_rule_channels rows for this
--   rule must be cleared first. This is enforced by the SPAM RULE command,
--   not by a DB constraint.
--
-- points_per: granularity for point accumulation for this rule's scoring key.
--   CLIENT = per YYXXX numeric nick (same as per iClient, default)
--   IP     = per IP address (aggregates points across all nicks sharing the same IP)
--
-- score_globally: controls whether this rule's scoring key aggregates points
--   across all channels/privmsgs for a user (true), or is scoped separately
--   per channel/privmsg (false, default). See dronescan.cc buildScoringKey().
-- -----------------------------------------------------------------------------
CREATE TABLE spam_rules (
	id                  serial       PRIMARY KEY,
	name                varchar(100) NOT NULL,
	description         text,
	threshold           int          NOT NULL DEFAULT 10,
	-- If set, this rule only fires after the referenced rule has already triggered
	-- for the same user
	wait_on_rule_id     int                   DEFAULT NULL
		REFERENCES spam_rules(id) ON DELETE SET NULL,
	-- true = all monitored channels (spam_rule_channels = exclusion list)
	-- false = only channels listed in spam_rule_channels (inclusion list)
	allchans            bool         NOT NULL DEFAULT true,
	-- CLIENT = per numeric nick (YYXXX / iClient);  IP = per IP address
	points_per          varchar(10)  NOT NULL DEFAULT 'CLIENT',
	-- false = score per channel/privmsg separately; true = score globally for the user
	score_globally      bool         NOT NULL DEFAULT false,
	enabled             bool         NOT NULL DEFAULT true,
	created_ts          int4         NOT NULL DEFAULT 0,
	modified_ts         int4         NOT NULL DEFAULT 0,
	modified_by         int                   DEFAULT NULL
		REFERENCES users(id) ON DELETE SET NULL
);

-- -----------------------------------------------------------------------------
-- spam_rule_events
-- -----------------------------------------------------------------------------
-- Many-to-many join between rules and events.
-- The same event can contribute to multiple rules with different point values.
-- points_override: NULL means use spam_events.points for this rule.
-- -----------------------------------------------------------------------------
CREATE TABLE spam_rule_events (
	rule_id         int NOT NULL REFERENCES spam_rules(id)  ON DELETE CASCADE,
	event_id        int NOT NULL REFERENCES spam_events(id) ON DELETE CASCADE,
	-- NULL = use the default points defined in spam_events
	points_override int          DEFAULT NULL,
	PRIMARY KEY (rule_id, event_id)
);

-- -----------------------------------------------------------------------------
-- spam_actions
-- -----------------------------------------------------------------------------
-- Reusable action templates. action_type drives which fields are relevant:
--   GLINE  : duration (seconds the gline lasts), reason
--   KILL   : reason (sent with the kill)
--   REPORT : no extra fields; logs to the console channel
--
-- delay: base seconds to wait before executing the action after trigger.
-- rand_min / rand_max: if both are set, a random value in [rand_min, rand_max]
--   is added to delay for timing jitter. Useful to avoid predictable patterns.
-- -----------------------------------------------------------------------------
CREATE TABLE spam_actions (
	id          serial       PRIMARY KEY,
	name        varchar(100) NOT NULL,
	action_type varchar(20)  NOT NULL,
	-- Duration in seconds (GLINE only)
	duration    int                   DEFAULT 3600,
	-- Reason string sent with the action (GLINE, KILL)
	reason      text                  DEFAULT NULL,
	-- Base delay in seconds before the action fires after the rule triggers
	delay       int          NOT NULL DEFAULT 0,
	-- Optional jitter: actual_delay = delay + random(rand_min, rand_max)
	rand_min    int                   DEFAULT NULL,
	rand_max    int                   DEFAULT NULL,
	enabled     bool         NOT NULL DEFAULT true,
	created_ts  int4         NOT NULL DEFAULT 0,
	modified_ts int4         NOT NULL DEFAULT 0,
	modified_by int                   DEFAULT NULL
		REFERENCES users(id) ON DELETE SET NULL
);

-- -----------------------------------------------------------------------------
-- spam_rule_actions
-- -----------------------------------------------------------------------------
-- Many-to-many join between rules and actions, with per-rule overrides.
-- Each row is an independent binding ? the same action template can appear
-- multiple times in the same rule with different override values.
--
-- action_type is denormalized here (copied from spam_actions) to allow fast
-- reads without an extra join at action-dispatch time.
--
-- Override columns: NULL means "use the value from spam_actions".
--   action_duration_override : overrides spam_actions.duration
--   action_reason_override   : overrides spam_actions.reason
--   delay_override           : overrides spam_actions.delay
-- -----------------------------------------------------------------------------
CREATE TABLE spam_rule_actions (
	id                       serial      PRIMARY KEY,
	rule_id                  int         NOT NULL REFERENCES spam_rules(id)   ON DELETE CASCADE,
	action_id                int         NOT NULL REFERENCES spam_actions(id) ON DELETE CASCADE,
	-- Denormalized from spam_actions.action_type for fast dispatch without extra join
	action_type              varchar(20) NOT NULL,
	-- NULL = use spam_actions.duration
	action_duration_override int                  DEFAULT NULL,
	-- NULL = use spam_actions.reason
	action_reason_override   text                 DEFAULT NULL,
	-- NULL = use spam_actions.delay
	delay_override           int                  DEFAULT NULL
);

-- -----------------------------------------------------------------------------
-- spam_exclusions
-- -----------------------------------------------------------------------------
-- Entries in this table bypass all spam detection entirely.
-- exclusion_type: CHAN | NICK | IP | OPER
-- value: exact match or glob/mask pattern depending on type.
-- -----------------------------------------------------------------------------
CREATE TABLE spam_exclusions (
	id             serial       PRIMARY KEY,
	exclusion_type varchar(10)  NOT NULL,
	value          varchar(200) NOT NULL,
	created_ts     int4         NOT NULL DEFAULT 0,
	modified_ts    int4         NOT NULL DEFAULT 0,
	modified_by    int                   DEFAULT NULL
		REFERENCES users(id) ON DELETE SET NULL,
	UNIQUE (exclusion_type, value)
);

-- -----------------------------------------------------------------------------
-- monitored_channels
-- -----------------------------------------------------------------------------
-- Channels that dronescan actively monitors for spam.
-- Spy clients (spyclients table) are introduced into these channels to observe
-- traffic. The main xClient can also monitor a channel directly (joinasservice).
--
-- forcejoin: if true, the spy client will join the channel regardless of
--   blocking channel modes (+i invite-only, +k keyed, +l full) or bans.
--   The join is forced via server-level protocol override. Use with care.
-- joinasservice: if true, the main xClient bot itself monitors this channel
--   directly with no spy client involved. Useful for channels where a fake
--   client presence would be undesirable or impractical.
--
-- last_triggered_ts / last_triggered_rule: updated by
--   dronescan::evaluateSpamRules() whenever a rule's threshold is crossed
--   for this channel. NULL = no rule has ever fired here.
-- -----------------------------------------------------------------------------
CREATE TABLE monitored_channels (
	id                  serial       PRIMARY KEY,
	name                varchar(200) NOT NULL UNIQUE,
	-- Force join even if channel is +i, +k, +l (full), or spy client is banned
	forcejoin           bool         NOT NULL DEFAULT false,
	-- Monitor via the main xClient bot directly; no spy client spawned
	joinasservice       bool         NOT NULL DEFAULT false,
	enabled             bool         NOT NULL DEFAULT true,
	last_triggered_ts   int4                  DEFAULT NULL,
	last_triggered_rule varchar(100)          DEFAULT NULL,
	created_ts          int4         NOT NULL DEFAULT 0,
	modified_ts         int4         NOT NULL DEFAULT 0,
	modified_by         int                   DEFAULT NULL
		REFERENCES users(id) ON DELETE SET NULL
);

-- -----------------------------------------------------------------------------
-- spam_rule_channels
-- -----------------------------------------------------------------------------
-- Per-rule channel list whose meaning depends on spam_rules.allchans:
--   allchans = true  : rows here are EXCLUDED from the rule (blacklist mode).
--   allchans = false : rows here are the ONLY channels the rule applies to
--                      (whitelist / inclusion mode).
--
-- IMPORTANT: when spam_rules.allchans is toggled, all rows for that rule_id
--   must be cleared first. This is enforced by the SPAM RULE command, not
--   by a DB constraint.
--
-- channel_name is stored by value and is NOT a foreign key to
--   monitored_channels.name, so that rule-channel associations can be
--   pre-configured before a channel is added to monitored_channels.
-- -----------------------------------------------------------------------------
CREATE TABLE spam_rule_channels (
	rule_id      int          NOT NULL REFERENCES spam_rules(id) ON DELETE CASCADE,
	channel_name varchar(200) NOT NULL,
	PRIMARY KEY (rule_id, channel_name)
);

-- -----------------------------------------------------------------------------
-- monitored_channel_spyclients
-- -----------------------------------------------------------------------------
-- Optional restricted spy-client list per monitored channel. When a channel
-- has rows here, dronescan::findBestSpyClient() only considers these spy
-- clients (starting at a random position and walking down the list) instead
-- of the full spy client pool. Channels with no rows here are unaffected
-- (existing full-pool selection behavior).
-- -----------------------------------------------------------------------------
CREATE TABLE monitored_channel_spyclients (
	channel_id   int NOT NULL REFERENCES monitored_channels(id) ON DELETE CASCADE,
	spyclient_id int NOT NULL REFERENCES spyclients(id)         ON DELETE CASCADE,
	PRIMARY KEY (channel_id, spyclient_id)
);
