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
