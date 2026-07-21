-- =============================================================================
-- 003_scoring_key_restructure.sql
-- Move points_per to live only on spam_rules (rename points_per_override ->
-- points_per, drop it from spam_events), and add spam_rules.score_globally
-- to control whether a rule's scoring key aggregates across all channels/
-- privmsgs for a user or is scoped per channel (the new default).
-- =============================================================================

ALTER TABLE spam_events DROP COLUMN points_per;

ALTER TABLE spam_rules RENAME COLUMN points_per_override TO points_per;
UPDATE spam_rules SET points_per = 'CLIENT' WHERE points_per IS NULL;
ALTER TABLE spam_rules ALTER COLUMN points_per SET DEFAULT 'CLIENT';
ALTER TABLE spam_rules ALTER COLUMN points_per SET NOT NULL;

ALTER TABLE spam_rules ADD COLUMN score_globally bool NOT NULL DEFAULT false;
