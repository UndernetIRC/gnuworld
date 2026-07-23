-- 013_spam_rules_silent.sql
--
-- Add spam_rules.silent: when true, a rule that fires with zero enabled
-- GLINE/KILL actions linked (i.e. it would only report, never take real
-- action) suppresses its console/report line entirely. A rule with at
-- least one GLINE/KILL action always reports, regardless of this flag -
-- silent can never hide an actual GLINE/KILL. Defaults to false so every
-- existing rule keeps reporting exactly as it does today.

ALTER TABLE spam_rules ADD COLUMN IF NOT EXISTS silent bool NOT NULL DEFAULT false;
