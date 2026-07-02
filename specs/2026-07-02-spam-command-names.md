---
mode: sequential
complexity: medium
type: refactor
playwright: false
frontend-design: false
spec-version: 1
created: 2026-07-02T14:00:00
branch: feat/spam-command-names
---

# Plan: SPAM command name-based identifiers

## Task Description
Refactor `mod.dronescan/SPAMCommand.cc` so that `SPAM EVENT`, `SPAM RULE`, and `SPAM ACTION` subcommands identify rules/events/actions by their `name` column instead of their numeric database `id`. This applies to `SHOW`, `DEL`, `SET` (including the `wait_on_rule_id` and `requires_event_id` reference fields), `ADDEVENT`/`REMEVENT`, `ADDACTION`/`REMACTION`, and `ADDCHAN`/`REMCHAN`. `SPAM RULE REMACTION` in particular moves from identifying a binding by the surrogate `spam_rule_actions.id` to identifying it by `<rule_name> <action_name>`.

`SPAM EXCLUSION`, `SPAM SPYCLIENT` (already nickname-keyed), and `SPAM MONITORCHAN` (already channel-keyed) are out of scope — `spam_exclusions` has no `name` column.

## Objective
After this change, no SPAM EVENT/RULE/ACTION subcommand accepts or requires a numeric database id as an argument. All references are by name, names are unique (case-insensitively) per table, and `SPAM RULE REMACTION` unambiguously identifies a single rule-action binding by `<rule_name> <action_name>` because a rule can only bind a given action once.

## Problem Statement
Today `spam_events.name`, `spam_rules.name`, and `spam_actions.name` are not enforced unique, and all SPAM subcommands take the internal serial `id` as their argument (e.g. `SPAM RULE ADDEVENT 1 2`, `SPAM RULE REMACTION 7`). This is unfriendly for interactive admin use — operators have to `SHOW`/`LIST` first to find the numeric id for anything they want to reference. `spam_rule_actions` also currently permits binding the same action to a rule more than once (with different overrides), which is why `REMACTION` needs the surrogate join-table id today instead of `<rule> <action>`.

## Solution Approach
Command-layer name resolution only — the internal engine and in-memory caches are untouched:

- `spamEventsMap`, `spamRulesMap`, `spamActionsMap` in `dronescan.h`/`dronescan.cc` remain `std::map<int, T*>` keyed by DB id. The live scoring/matching engine in `dronescan.cc` is not modified by this spec.
- Three new case-insensitive lookup helpers are added to `SPAMCommand.cc`: `findRuleByName`, `findEventByName`, `findActionByName`. Each does a linear scan of the relevant map (dozens of rows at most, admin-only command path — not worth a secondary index).
- Every SPAM EVENT/RULE/ACTION subcommand that currently takes an `<id>` argument resolves a name to a pointer via these helpers at the top of its branch, then proceeds exactly as before using `->getId()` internally wherever the old code used the numeric id (SQL queries, map keys, FK columns).
- A new migration adds a case-insensitive `UNIQUE` index on `name` for `spam_events`, `spam_rules`, `spam_actions`, and a `UNIQUE(rule_id, action_id)` constraint on `spam_rule_actions` (mirroring the existing `PRIMARY KEY (rule_id, event_id)` on `spam_rule_events`), which is what makes `REMACTION <rule_name> <action_name>` unambiguous.
- Per CLAUDE.md, this build only **creates** the migration file — it does not apply it to any database. The user applies it themselves (dronescan applies pending migrations automatically on next restart, per `mod.dronescan/migrations/README`).

## Relevant Files
- `mod.dronescan/SPAMCommand.cc` — the file being refactored; contains `handleEvent`, `handleRule`, `handleAction`, and the helper functions section near the top.
- `mod.dronescan/HELPCommand.cc` — `helpSpamEvent`, `helpSpamRule`, `helpSpamAction` contain the user-facing syntax reference and examples that must match the new argument shapes.
- `mod.dronescan/dronescan.h` — defines `spamEventsMap`/`spamRulesMap`/`spamActionsMap`/`spamRuleEventsMap`/`spamRuleActionsMap` types (read-only reference, not modified).
- `mod.dronescan/sqlSpamRule.h`, `sqlSpamEvent.h`, `sqlSpamAction.h`, `sqlSpamRuleAction.h` — accessor classes already expose `getName()`/`getId()` needed by the new helpers (read-only reference, not modified).
- `mod.dronescan/migrations/003_scoring_key_restructure.sql` — most recent migration, establishes the numbering convention this spec's new migration continues.
- `mod.dronescan/migrations/README` — documents the migration file naming/application convention followed here.

### New Files
- `mod.dronescan/migrations/004_spam_names_unique.sql` — adds case-insensitive uniqueness on `spam_events.name`, `spam_rules.name`, `spam_actions.name`, and `UNIQUE(rule_id, action_id)` on `spam_rule_actions`.

## Implementation Phases

### Phase 1: Foundation
Create the migration file (task 1) and the name-resolution helpers (task 2) that every later task depends on.

### Phase 2: Core Implementation
Convert each SPAM subcommand group to name-based arguments: EVENT (task 3), RULE core + `wait_on_rule_id` (task 4), RULE event links (task 5), RULE action links + duplicate-binding guard + ACTION DEL (task 6), RULE channel links (task 7).

### Phase 3: Integration & Polish
Update the in-file syntax comment and `HELPCommand.cc` so documented syntax matches the new behavior (task 8), then review and validate.

## Step by Step Tasks

### 1. Create the uniqueness migration
- **Task ID**: create-migration
- **Depends On**: none
- **Description**:
  - Create `mod.dronescan/migrations/004_spam_names_unique.sql` with exactly this content:
    ```sql
    -- =============================================================================
    -- 004_spam_names_unique.sql
    -- Enforce case-insensitive uniqueness on spam_events.name, spam_rules.name,
    -- and spam_actions.name so the SPAM command can identify rows by name
    -- instead of by numeric id. Also enforce one action binding per rule in
    -- spam_rule_actions (mirroring spam_rule_events' existing PK), which is what
    -- makes "SPAM RULE REMACTION <rule_name> <action_name>" unambiguous.
    --
    -- IMPORTANT: run these checks first if this database has existing data.
    -- Any group returned by these queries must be renamed manually (UPDATE ...
    -- SET name = ...) before this migration will apply successfully.
    --   SELECT lower(name) AS dup, count(*) FROM spam_events  GROUP BY lower(name) HAVING count(*) > 1;
    --   SELECT lower(name) AS dup, count(*) FROM spam_rules   GROUP BY lower(name) HAVING count(*) > 1;
    --   SELECT lower(name) AS dup, count(*) FROM spam_actions GROUP BY lower(name) HAVING count(*) > 1;
    -- Similarly, any (rule_id, action_id) pair bound more than once in
    -- spam_rule_actions must be reduced to a single row first:
    --   SELECT rule_id, action_id, count(*) FROM spam_rule_actions GROUP BY rule_id, action_id HAVING count(*) > 1;
    --
    -- Apply with:
    --   psql -d <dbname> -f 004_spam_names_unique.sql
    -- =============================================================================

    CREATE UNIQUE INDEX ux_spam_events_name_lower  ON spam_events  (lower(name));
    CREATE UNIQUE INDEX ux_spam_rules_name_lower   ON spam_rules   (lower(name));
    CREATE UNIQUE INDEX ux_spam_actions_name_lower ON spam_actions (lower(name));

    ALTER TABLE spam_rule_actions
        ADD CONSTRAINT ux_spam_rule_actions_rule_action UNIQUE (rule_id, action_id);
    ```
  - Do not run this migration against any database. Do not modify any other migration file.
- **Files**:
  - creates: mod.dronescan/migrations/004_spam_names_unique.sql
- **Tests**: N/A (SQL migration file only; not applied or executed by this build)

### 2. Add name-resolution helpers
- **Task ID**: add-name-helpers
- **Depends On**: create-migration
- **Description**:
  - In `mod.dronescan/SPAMCommand.cc`, immediately after `isValidExclusionType()` (which ends the "Helpers: valid event types / action types" section, right before `parseTargetBitmask`), add three new static helper functions:
    ```cpp
    // ---------------------------------------------------------------------------
    // Helpers: case-insensitive name -> object lookups
    // ---------------------------------------------------------------------------
    static sqlSpamRule* findRuleByName(dronescan* bot, const string& name)
    {
        const string lname = string_lower(name);
        for (dronescan::spamRulesMapType::const_iterator it = bot->spamRulesMap.begin();
             it != bot->spamRulesMap.end(); ++it) {
            if (string_lower(it->second->getName()) == lname)
                return it->second;
        }
        return nullptr;
    }

    static sqlSpamEvent* findEventByName(dronescan* bot, const string& name)
    {
        const string lname = string_lower(name);
        for (dronescan::spamEventsMapType::const_iterator it = bot->spamEventsMap.begin();
             it != bot->spamEventsMap.end(); ++it) {
            if (string_lower(it->second->getName()) == lname)
                return it->second;
        }
        return nullptr;
    }

    static sqlSpamAction* findActionByName(dronescan* bot, const string& name)
    {
        const string lname = string_lower(name);
        for (dronescan::spamActionsMapType::const_iterator it = bot->spamActionsMap.begin();
             it != bot->spamActionsMap.end(); ++it) {
            if (string_lower(it->second->getName()) == lname)
                return it->second;
        }
        return nullptr;
    }
    ```
  - These three functions are used by every later task in this spec — do not rename them.
- **Files**:
  - modifies: mod.dronescan/SPAMCommand.cc
- **Tests**: N/A (no unit test framework in this module; behavior verified by compiling and by later tasks' handlers exercising these helpers)

### 3. Convert EVENT SHOW/DEL/SET (and requires_event_id) to names
- **Task ID**: event-names
- **Depends On**: add-name-helpers
- **Description**:
  - In `handleEvent()`'s `SHOW` branch: change the usage string to `"Usage: SPAM EVENT SHOW <name>"`. Replace the `atoi`/map-`find` id lookup with:
    ```cpp
    sqlSpamEvent* ev = findEventByName(bot, st[3]);
    if (!ev) {
        bot->Reply(theClient, "Event '%s' not found.", st[3].c_str());
        return;
    }
    ```
    Leave the rest of the `SHOW` body (all the `ev->...` Reply calls) unchanged.
  - In `handleEvent()`'s `DEL` branch: change the usage string to `"Usage: SPAM EVENT DEL <name>"`. Replace the body with:
    ```cpp
    sqlSpamEvent* ev = findEventByName(bot, st[3]);
    if (!ev) {
        bot->Reply(theClient, "Event '%s' not found.", st[3].c_str());
        return;
    }
    const int id = ev->getId();
    const string evName = ev->getName();
    if (!ev->remove()) {
        bot->Reply(theClient, "Failed to delete spam event '%s'.", evName.c_str());
        return;
    }
    delete ev;
    bot->spamEventsMap.erase(id);
    bot->Reply(theClient, "Spam event '%s' deleted.", evName.c_str());
    return;
    ```
  - In `handleEvent()`'s `SET` branch: change the usage comment/string to `SPAM EVENT SET <name> <field> <value>` / `"Usage: SPAM EVENT SET <name> <field> <value>"`. Replace the id lookup with:
    ```cpp
    sqlSpamEvent* ev = findEventByName(bot, st[3]);
    if (!ev) {
        bot->Reply(theClient, "Event '%s' not found.", st[3].c_str());
        return;
    }
    const int id = ev->getId();
    const string field = string_lower(st[4]);
    const string value = (st.size() >= 6) ? st[5] : string();
    ```
    (This replaces the old `int id = atoi(st[3].c_str());` plus map-`find`/`it->second` lines. `id` must remain in scope exactly as before since it is used later in the same branch for `bot->spamRegexCache.find(id)` / `bot->spamRepeatExclusionCache.find(id)`.)
  - Within that same `SET` branch, replace the `requires_event_id` field handler:
    ```cpp
    } else if (field == "requires_event_id") {
        ev->setRequiresEventId(atoi(value.c_str()));
    }
    ```
    with:
    ```cpp
    } else if (field == "requires_event_id") {
        if (value.empty() || string_lower(value) == "none") {
            ev->setRequiresEventId(0);
        } else {
            sqlSpamEvent* reqEv = findEventByName(bot, value);
            if (!reqEv) {
                bot->Reply(theClient, "Event '%s' not found.", value.c_str());
                return;
            }
            ev->setRequiresEventId(reqEv->getId());
        }
    }
    ```
  - In `handleEvent()`'s `ADD` branch, change the insert-failure message from `"Failed to add spam event."` to `"Failed to add spam event (duplicate name?)."`.
- **Files**:
  - modifies: mod.dronescan/SPAMCommand.cc
- **Tests**: N/A (no unit test framework in this module; verified by compile + Acceptance Criteria manual checks)

### 4. Convert RULE SHOW/DEL/SET (and wait_on_rule_id) to names
- **Task ID**: rule-names
- **Depends On**: event-names
- **Description**:
  - Apply the same pattern as task 3 to `handleRule()`'s `SHOW` and `DEL` branches, using `findRuleByName(bot, st[3])` in place of `findEventByName`, `sqlSpamRule*` in place of `sqlSpamEvent*`, `bot->spamRulesMap` in place of `bot->spamEventsMap`, usage strings `"Usage: SPAM RULE SHOW <name>"` / `"Usage: SPAM RULE DEL <name>"`, and messages `"Rule '%s' not found."` / `"Failed to delete spam rule '%s'."` / `"Spam rule '%s' deleted."`.
  - For `DEL` specifically: after resolving `rule` and capturing `const int id = rule->getId();` and `const string ruleName = rule->getName();`, keep the existing cleanup of `bot->spamRuleEventsMap.erase(id);` and the `spamRuleActionsMap` iteration/delete loop unchanged (they already operate on the numeric `id`, which is still available).
  - In `handleRule()`'s `SET` branch: change the usage comment/string to `SPAM RULE SET <name> <field> <value>` / `"Usage: SPAM RULE SET <name> <field> <value>"`. Replace the id lookup the same way as task 3's SET change, using `findRuleByName`/`sqlSpamRule*`/`"Rule '%s' not found."`. Note the `allchans` field handler uses `id` in its `DELETE FROM spam_rule_channels WHERE rule_id = ` query and `bot->spamRuleChannelsMap.erase(id)` — keep `id` defined exactly as in task 3's pattern so this still compiles.
  - Within that same `SET` branch, replace the `wait_on_rule_id` field handler:
    ```cpp
    } else if (field == "wait_on_rule_id") {
        rule->setWaitOnRuleId(atoi(value.c_str()));
    }
    ```
    with:
    ```cpp
    } else if (field == "wait_on_rule_id") {
        if (value.empty() || string_lower(value) == "none") {
            rule->setWaitOnRuleId(0);
        } else {
            sqlSpamRule* waitRule = findRuleByName(bot, value);
            if (!waitRule) {
                bot->Reply(theClient, "Rule '%s' not found.", value.c_str());
                return;
            }
            rule->setWaitOnRuleId(waitRule->getId());
        }
    }
    ```
  - In `handleRule()`'s `ADD` branch, change the insert-failure message from `"Failed to add spam rule."` to `"Failed to add spam rule (duplicate name?)."`.
- **Files**:
  - modifies: mod.dronescan/SPAMCommand.cc
- **Tests**: N/A (no unit test framework in this module; verified by compile + Acceptance Criteria manual checks)

### 5. Convert RULE ADDEVENT/REMEVENT to names
- **Task ID**: rule-event-links-names
- **Depends On**: rule-names
- **Description**:
  - In `handleRule()`'s `ADDEVENT` branch: change the usage comment/string to `SPAM RULE ADDEVENT <rule_name> <event_name> [points_override]` / `"Usage: SPAM RULE ADDEVENT <rule_name> <event_name> [points_override]"`. Replace:
    ```cpp
    int rule_id  = atoi(st[3].c_str());
    int event_id = atoi(st[4].c_str());

    if (bot->spamRulesMap.find(rule_id) == bot->spamRulesMap.end()) {
        bot->Reply(theClient, "Rule %d not found.", rule_id);
        return;
    }
    if (bot->spamEventsMap.find(event_id) == bot->spamEventsMap.end()) {
        bot->Reply(theClient, "Event %d not found.", event_id);
        return;
    }
    ```
    with:
    ```cpp
    sqlSpamRule* rule = findRuleByName(bot, st[3]);
    if (!rule) {
        bot->Reply(theClient, "Rule '%s' not found.", st[3].c_str());
        return;
    }
    sqlSpamEvent* ev = findEventByName(bot, st[4]);
    if (!ev) {
        bot->Reply(theClient, "Event '%s' not found.", st[4].c_str());
        return;
    }
    const int rule_id  = rule->getId();
    const int event_id = ev->getId();
    ```
    Leave the rest of the branch (the `INSERT INTO spam_rule_events` query, `bot->spamRuleEventsMap[rule_id].push_back(...)`) unchanged since it already operates on `rule_id`/`event_id` ints. Change the final confirmation message from `"Event %d linked to rule %d%s."` / args `event_id, rule_id, ...` to `"Event '%s' linked to rule '%s'%s."` / args `ev->getName().c_str(), rule->getName().c_str(), ...`.
  - In `handleRule()`'s `REMEVENT` branch: change the usage string to `"Usage: SPAM RULE REMEVENT <rule_name> <event_name>"`. Replace:
    ```cpp
    int rule_id  = atoi(st[3].c_str());
    int event_id = atoi(st[4].c_str());
    ```
    with:
    ```cpp
    sqlSpamRule* rule = findRuleByName(bot, st[3]);
    if (!rule) {
        bot->Reply(theClient, "Rule '%s' not found.", st[3].c_str());
        return;
    }
    sqlSpamEvent* ev = findEventByName(bot, st[4]);
    if (!ev) {
        bot->Reply(theClient, "Event '%s' not found.", st[4].c_str());
        return;
    }
    const int rule_id  = rule->getId();
    const int event_id = ev->getId();
    ```
    Leave the `DELETE FROM spam_rule_events` query and the in-memory `spamRuleEventsMap` vector removal loop unchanged. Change the final confirmation message from `"Event %d unlinked from rule %d."` / args `event_id, rule_id` to `"Event '%s' unlinked from rule '%s'."` / args `ev->getName().c_str(), rule->getName().c_str()`.
- **Files**:
  - modifies: mod.dronescan/SPAMCommand.cc
- **Tests**: N/A (no unit test framework in this module; verified by compile + Acceptance Criteria manual checks)

### 6. Convert RULE ADDACTION/REMACTION and ACTION DEL to names, add duplicate-binding guard
- **Task ID**: rule-action-links-names
- **Depends On**: rule-event-links-names
- **Description**:
  - In `handleRule()`'s `ADDACTION` branch: change the usage comment/string to `SPAM RULE ADDACTION <rule_name> <action_name> [dur_override] [reason_override] [delay_override]` / matching usage message. Replace:
    ```cpp
    int rule_id   = atoi(st[3].c_str());
    int action_id = atoi(st[4].c_str());

    if (bot->spamRulesMap.find(rule_id) == bot->spamRulesMap.end()) {
        bot->Reply(theClient, "Rule %d not found.", rule_id);
        return;
    }
    dronescan::spamActionsMapType::const_iterator ait =
        bot->spamActionsMap.find(action_id);
    if (ait == bot->spamActionsMap.end()) {
        bot->Reply(theClient, "Action %d not found.", action_id);
        return;
    }
    ```
    with:
    ```cpp
    sqlSpamRule* rule = findRuleByName(bot, st[3]);
    if (!rule) {
        bot->Reply(theClient, "Rule '%s' not found.", st[3].c_str());
        return;
    }
    sqlSpamAction* action = findActionByName(bot, st[4]);
    if (!action) {
        bot->Reply(theClient, "Action '%s' not found.", st[4].c_str());
        return;
    }
    const int rule_id   = rule->getId();
    const int action_id = action->getId();

    // A rule may only bind a given action once (spam_rule_actions has a
    // UNIQUE(rule_id, action_id) constraint) so REMACTION can identify a
    // binding unambiguously by <rule_name> <action_name>.
    dronescan::spamRuleActionsMapType::const_iterator existingIt =
        bot->spamRuleActionsMap.find(rule_id);
    if (existingIt != bot->spamRuleActionsMap.end()) {
        for (size_t i = 0; i < existingIt->second.size(); ++i) {
            if (existingIt->second[i]->getActionId() == action_id) {
                bot->Reply(theClient,
                    "Action '%s' is already linked to rule '%s'.",
                    action->getName().c_str(), rule->getName().c_str());
                return;
            }
        }
    }
    ```
    Replace the subsequent `ra->setActionType(ait->second->getActionType());` with `ra->setActionType(action->getActionType());`. Change the final confirmation message from `"Action %d linked to rule %d (sra id: %d)."` / args `action_id, rule_id, ra->getId()` to `"Action '%s' linked to rule '%s' (sra id: %d)."` / args `action->getName().c_str(), rule->getName().c_str(), ra->getId()`.
  - Replace `handleRule()`'s entire `REMACTION` branch:
    ```cpp
    // -- REMACTION -----------------------------------------------------------
    if (verb == "REMACTION") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM RULE REMACTION <rule_name> <action_name>
        if (st.size() < 5) {
            bot->Reply(theClient, "Usage: SPAM RULE REMACTION <rule_name> <action_name>");
            return;
        }
        sqlSpamRule* rule = findRuleByName(bot, st[3]);
        if (!rule) {
            bot->Reply(theClient, "Rule '%s' not found.", st[3].c_str());
            return;
        }
        sqlSpamAction* action = findActionByName(bot, st[4]);
        if (!action) {
            bot->Reply(theClient, "Action '%s' not found.", st[4].c_str());
            return;
        }

        dronescan::spamRuleActionsMapType::iterator rit =
            bot->spamRuleActionsMap.find(rule->getId());
        if (rit != bot->spamRuleActionsMap.end()) {
            std::vector<sqlSpamRuleAction*>& vec = rit->second;
            for (std::vector<sqlSpamRuleAction*>::iterator vi = vec.begin();
                 vi != vec.end(); ++vi) {
                if ((*vi)->getActionId() == action->getId()) {
                    if (!(*vi)->remove()) {
                        bot->Reply(theClient, "Failed to remove rule-action binding.");
                        return;
                    }
                    delete *vi;
                    vec.erase(vi);
                    bot->Reply(theClient, "Action '%s' unlinked from rule '%s'.",
                               action->getName().c_str(), rule->getName().c_str());
                    return;
                }
            }
        }
        bot->Reply(theClient, "Action '%s' is not linked to rule '%s'.",
                   action->getName().c_str(), rule->getName().c_str());
        return;
    }
    ```
    (This entirely replaces the old "find it across all rule buckets by sra id" loop — the rule is now known up front, so only that rule's action vector needs to be scanned.)
  - In `handleAction()`'s `DEL` branch: change the usage string to `"Usage: SPAM ACTION DEL <name>"`. Replace the body with the same pattern as task 3's EVENT DEL, using `findActionByName(bot, st[3])`, `sqlSpamAction*`, `bot->spamActionsMap`, and messages `"Action '%s' not found."` / `"Failed to delete spam action '%s'."` / `"Spam action '%s' deleted."`.
  - In `handleAction()`'s `ADD` branch, change the insert-failure message from `"Failed to add spam action."` to `"Failed to add spam action (duplicate name?)."`.
- **Files**:
  - modifies: mod.dronescan/SPAMCommand.cc
- **Tests**: N/A (no unit test framework in this module; verified by compile + Acceptance Criteria manual checks)

### 7. Convert RULE ADDCHAN/REMCHAN to rule name
- **Task ID**: rule-channel-links-names
- **Depends On**: rule-action-links-names
- **Description**:
  - In `handleRule()`'s `ADDCHAN` branch: change the usage comment/string to `SPAM RULE ADDCHAN <rule_name> <#channel>` / `"Usage: SPAM RULE ADDCHAN <rule_name> <#channel>"`. Replace:
    ```cpp
    int rule_id = atoi(st[3].c_str());
    if (bot->spamRulesMap.find(rule_id) == bot->spamRulesMap.end()) {
        bot->Reply(theClient, "Rule %d not found.", rule_id);
        return;
    }
    ```
    with:
    ```cpp
    sqlSpamRule* rule = findRuleByName(bot, st[3]);
    if (!rule) {
        bot->Reply(theClient, "Rule '%s' not found.", st[3].c_str());
        return;
    }
    const int rule_id = rule->getId();
    ```
    Leave the rest of the branch (duplicate-channel check, `INSERT INTO spam_rule_channels`, `bot->spamRuleChannelsMap[rule_id].push_back(...)`) unchanged since it already operates on `rule_id`/`chanName`. Change the messages `"Channel '%s' is already listed for rule %d."` and `"Channel '%s' added to rule %d."` (and the failure message) to use `rule->getName().c_str()` in place of `rule_id` with `%s` in place of `%d`.
  - In `handleRule()`'s `REMCHAN` branch: change the usage comment/string to `SPAM RULE REMCHAN <rule_name> <#channel>` / `"Usage: SPAM RULE REMCHAN <rule_name> <#channel>"`. Replace:
    ```cpp
    int rule_id = atoi(st[3].c_str());
    ```
    with:
    ```cpp
    sqlSpamRule* rule = findRuleByName(bot, st[3]);
    if (!rule) {
        bot->Reply(theClient, "Rule '%s' not found.", st[3].c_str());
        return;
    }
    const int rule_id = rule->getId();
    ```
    Leave the rest of the branch (`DELETE FROM spam_rule_channels`, in-memory vector removal) unchanged. Change the messages `"Failed to remove channel '%s' from rule %d."`, `"Channel '%s' was not found in rule %d's list."`, and `"Channel '%s' removed from rule %d."` to use `rule->getName().c_str()` in place of `rule_id` with `%s` in place of `%d`.
- **Files**:
  - modifies: mod.dronescan/SPAMCommand.cc
- **Tests**: N/A (no unit test framework in this module; verified by compile + Acceptance Criteria manual checks)

### 8. Update syntax documentation
- **Task ID**: update-docs
- **Depends On**: rule-channel-links-names
- **Description**:
  - In `mod.dronescan/SPAMCommand.cc`, replace the file-header syntax comment block (the `SPAM EVENT`/`SPAM RULE`/`SPAM ACTION` lines inside the `/** ... */` doc comment at the top of the file, lines currently reading `SPAM EVENT DEL <id>` through `SPAM RULE REMCHAN <rule_id> <#channel>` and `SPAM ACTION DEL <id>`) with:
    ```
     *   SPAM EVENT     ADD    <name> <type> <target> <param> <points> <expiry> [max_occ]
     *   SPAM EVENT     DEL    <name>
     *   SPAM EVENT     LIST
     *   SPAM EVENT     SHOW   <name>
     *   SPAM EVENT     SET    <name> <field> <value>
     *   SPAM RULE      ADD    <name> <threshold>
     *   SPAM RULE      DEL    <name>
     *   SPAM RULE      LIST
     *   SPAM RULE      SHOW   <name>
     *   SPAM RULE      SET    <name> <field> <value>
     *   SPAM RULE      ADDEVENT   <rule_name> <event_name> [points_override]
     *   SPAM RULE      REMEVENT   <rule_name> <event_name>
     *   SPAM RULE      ADDACTION  <rule_name> <action_name> [dur_override] [reason_override] [delay_override]
     *   SPAM RULE      REMACTION  <rule_name> <action_name>
     *   SPAM RULE      ADDCHAN    <rule_name> <#channel>
     *   SPAM RULE      REMCHAN    <rule_name> <#channel>
     *   SPAM ACTION    ADD    <name> <type> [duration] [reason] [delay]
     *   SPAM ACTION    DEL    <name>
     *   SPAM ACTION    LIST
    ```
    Leave the `SPAM EXCLUSION`/`SPAM SPYCLIENT`/`SPAM MONITORCHAN` lines below it unchanged. Immediately after the existing `target bitmask:` comment paragraph, add a new paragraph:
    ```
     * Rule/event/action names are matched case-insensitively and must be
     * unique within their table. RULE SET wait_on_rule_id and EVENT SET
     * requires_event_id take a rule/event name as their value, or "none"
     * (or an empty value) to clear the reference.
    ```
  - In `mod.dronescan/HELPCommand.cc`, function `helpSpamEvent`: change `"  DEL    <id>"` to `"  DEL    <name>"`, `"  SHOW   <id>"` to `"  SHOW   <name>"`, `"  SET    <id> <field> <value>"` to `"  SET    <name> <field> <value>"`, and `"            points  point_expiry  max_occurrence  requires_event_id"` to `"            points  point_expiry  max_occurrence  requires_event_id (event name, or \"none\")"`. Change the four example lines `"  SPAM EVENT SET 1 enabled yes"`, `"  SPAM EVENT SET 1 event_param \"new.*regex.*pattern\""`, `"  SPAM EVENT SET 1 target all"`, `"  SPAM EVENT DEL 1"` to `"  SPAM EVENT SET pill_regex enabled yes"`, `"  SPAM EVENT SET pill_regex event_param \"new.*regex.*pattern\""`, `"  SPAM EVENT SET pill_regex target all"`, `"  SPAM EVENT DEL pill_regex"`.
  - In `helpSpamRule`: change `"  DEL        <id>"` to `"  DEL        <name>"`, `"  SHOW       <id>"` to `"  SHOW       <name>"`, `"  SET        <id> <field> <value>"` to `"  SET        <name> <field> <value>"`, `"  ADDEVENT   <rule_id> <event_id> [points_override]"` to `"  ADDEVENT   <rule_name> <event_name> [points_override]"`, `"  REMEVENT   <rule_id> <event_id>"` to `"  REMEVENT   <rule_name> <event_name>"`, `"  ADDACTION  <rule_id> <action_id> [dur_override] [reason_override] [delay_override]"` to `"  ADDACTION  <rule_name> <action_name> [dur_override] [reason_override] [delay_override]"`, `"  REMACTION  <spam_rule_action_id>"` to `"  REMACTION  <rule_name> <action_name>"`, `"  ADDCHAN    <rule_id> <#channel>"` to `"  ADDCHAN    <rule_name> <#channel>"`, `"  REMCHAN    <rule_id> <#channel>"` to `"  REMCHAN    <rule_name> <#channel>"`, and `"SET fields: description  threshold  wait_on_rule_id  enabled"` to `"SET fields: description  threshold  wait_on_rule_id (rule name, or \"none\")  enabled"`. Change the example lines `"  SPAM RULE SET 1 allchans yes"`, `"  SPAM RULE SET 1 threshold 30"`, `"  SPAM RULE ADDEVENT 1 2"`, `"  SPAM RULE ADDEVENT 1 3 20     <- use 20 pts instead of event default"`, `"  SPAM RULE ADDACTION 1 1 3600 \"Spam detected\" 0"`, `"  SPAM RULE ADDCHAN  1 #watch-me"`, `"  SPAM RULE REMCHAN  1 #watch-me"` to `"  SPAM RULE SET anti_spam_global allchans yes"`, `"  SPAM RULE SET anti_spam_global threshold 30"`, `"  SPAM RULE ADDEVENT anti_spam_global pill_regex"`, `"  SPAM RULE ADDEVENT anti_spam_global repeat_flood 20     <- use 20 pts instead of event default"`, `"  SPAM RULE ADDACTION anti_spam_global gline_1h 3600 \"Spam detected\" 0"`, `"  SPAM RULE ADDCHAN  anti_spam_global #watch-me"`, `"  SPAM RULE REMCHAN  anti_spam_global #watch-me"`.
  - In `helpSpamAction`: change `"  DEL    <id>"` to `"  DEL    <name>"` and the example `"  SPAM ACTION DEL 3"` to `"  SPAM ACTION DEL report_only"`.
- **Files**:
  - modifies: mod.dronescan/SPAMCommand.cc
  - modifies: mod.dronescan/HELPCommand.cc
- **Tests**: N/A (documentation/comment-only changes)

### 9. Code Review
- **Task ID**: review-all
- **Depends On**: create-migration, add-name-helpers, event-names, rule-names, rule-event-links-names, rule-action-links-names, rule-channel-links-names, update-docs
- **Description**: Review your own work: re-read `mod.dronescan/SPAMCommand.cc`, `mod.dronescan/HELPCommand.cc`, and `mod.dronescan/migrations/004_spam_names_unique.sql` in full. Check specifically for:
  - Every remaining reference to `atoi(st[...])` in `handleEvent`/`handleRule`/`handleAction` where the value is meant to identify a rule/event/action (as opposed to a genuine numeric field like `points`, `threshold`, `duration`, `delay`, `points_override`) has been converted to a name lookup.
  - No branch left over that still does a raw `bot->spamRulesMap.find(id)` / `bot->spamEventsMap.find(id)` / `bot->spamActionsMap.find(id)` style lookup where a name lookup should now be used.
  - All `%d` format specifiers paired with a removed `_id`/numeric variable in user-facing `bot->Reply(...)` calls have been updated to `%s` with the corresponding name.
  - The `REMACTION` rewrite compiles logically: `sqlSpamRuleAction` has no `setActionType`/etc. calls needed here, only `remove()`; iterator invalidation after `vec.erase(vi)` is not an issue because the function returns immediately after erasing.
  - `requires_event_id` and `wait_on_rule_id` sentinel handling (`"none"` or empty clears the reference) is symmetric between EVENT and RULE.
  - The migration file's SQL is syntactically valid Postgres and matches the style (comment header, `IF NOT EXISTS` not used elsewhere in this project's migrations so it's consistently omitted) of `001_spam_detection.sql`/`003_scoring_key_restructure.sql`.
  - Fix anything found before proceeding to validation.
- **Files**: N/A (review task; may touch mod.dronescan/SPAMCommand.cc and mod.dronescan/HELPCommand.cc if fixes are needed)
- **Tests**: N/A

### 10. Final Validation
- **Task ID**: validate-all
- **Depends On**: review-all
- **Description**: Run the validation commands below and verify every acceptance criterion is met. `Makefile.am` is not modified by this spec (no new `.cc`/`.h` source files were added — only an existing `.cc` file, an existing help file, and a runtime-loaded `.sql` migration), so the `aclocal`/`autoconf`/`automake`/`./configure` step is not required; a plain `make` picks up the changed `.cc` files.

## Documentation Requirements
- The in-file syntax comment block at the top of `mod.dronescan/SPAMCommand.cc` must reflect the new name-based argument shapes for every EVENT/RULE/ACTION subcommand (task 8).
- `mod.dronescan/HELPCommand.cc`'s `helpSpamEvent`, `helpSpamRule`, and `helpSpamAction` functions (the live `HELP SPAM EVENT`/`HELP SPAM RULE`/`HELP SPAM ACTION` output) must reflect the new syntax and use realistic name-based examples consistent with each other (e.g. the same rule/event/action names recur across the RULE examples) (task 8).
- `mod.dronescan/migrations/004_spam_names_unique.sql` must include the pre-migration duplicate-check queries as comments, since this module's migrations are applied automatically on restart and a duplicate-name failure would otherwise be a confusing startup error (task 1).

## Acceptance Criteria
- `mod.dronescan/migrations/004_spam_names_unique.sql` exists, is not applied to any database by this build, and creates case-insensitive unique indexes on `spam_events.name`, `spam_rules.name`, `spam_actions.name` plus `UNIQUE(rule_id, action_id)` on `spam_rule_actions`.
- `findRuleByName`, `findEventByName`, `findActionByName` exist in `SPAMCommand.cc`, perform case-insensitive matching, and return `nullptr` when no match is found.
- `SPAM EVENT SHOW`, `SPAM EVENT DEL`, `SPAM EVENT SET` take an event name as their identifying argument; `requires_event_id` in `SET` takes an event name or `none`/empty to clear.
- `SPAM RULE SHOW`, `SPAM RULE DEL`, `SPAM RULE SET` take a rule name as their identifying argument; `wait_on_rule_id` in `SET` takes a rule name or `none`/empty to clear.
- `SPAM RULE ADDEVENT`/`REMEVENT` take `<rule_name> <event_name>`.
- `SPAM RULE ADDACTION` takes `<rule_name> <action_name>` and rejects a duplicate binding with a clear message before hitting the DB constraint.
- `SPAM RULE REMACTION` takes `<rule_name> <action_name>` (no longer the `spam_rule_actions.id`).
- `SPAM RULE ADDCHAN`/`REMCHAN` take `<rule_name> <#channel>`.
- `SPAM ACTION DEL` takes an action name.
- No SPAM EVENT/RULE/ACTION subcommand's usage string or implementation still references a bare numeric `<id>` for a rule/event/action.
- `SPAM EXCLUSION`, `SPAM SPYCLIENT`, `SPAM MONITORCHAN` subcommands are unchanged.
- `mod.dronescan/dronescan.cc` and the `sqlSpam*.{h,cc}` files are unchanged (internal engine stays id-keyed).
- The project builds cleanly with the Validation Commands below.

## Validation Commands
```
cd /home/hidden/dev/git/gnuworld && make && make install
```
- Confirm the build produces no compiler errors or new warnings in `mod.dronescan/SPAMCommand.cc` or `mod.dronescan/HELPCommand.cc`.
- `git diff --stat` should show changes only to `mod.dronescan/SPAMCommand.cc`, `mod.dronescan/HELPCommand.cc`, and the new `mod.dronescan/migrations/004_spam_names_unique.sql`.

## Cleanup
N/A — no servers, background processes, or temporary files are created by this build.

## Notes
- Per CLAUDE.md: never modify the database directly. The migration file is created but not applied; tell the user it's ready to review and apply themselves.
- Per CLAUDE.md: never push changes; commit locally with an appropriate message once the build compiles.
- If the live database already has duplicate names (case-insensitively) within `spam_events`, `spam_rules`, or `spam_actions`, or duplicate `(rule_id, action_id)` bindings in `spam_rule_actions`, migration 004 will fail to apply until the user manually deduplicates — this is called out in the migration's comment header and cannot be resolved by this build.
- Encoding for this project is ISO-8859-1 — avoid introducing any non-ASCII characters in the new help text/comments.
