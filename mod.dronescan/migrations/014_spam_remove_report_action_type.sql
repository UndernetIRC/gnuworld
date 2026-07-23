-- 014_spam_remove_report_action_type.sql
--
-- The REPORT action type is removed: reporting is no longer a per-action
-- opt-in - every rule now always prints a single combined console/report
-- line when it fires (see spam_rules.silent, migration 013, for the one
-- case where that line is suppressed). Delete spam_rule_actions rows bound
-- to a REPORT-type spam_actions row first (FK dependency), then delete the
-- REPORT spam_actions rows themselves. GLINE/KILL rows are untouched.

DELETE FROM spam_rule_actions WHERE action_type = 'REPORT';
DELETE FROM spam_actions WHERE action_type = 'REPORT';
