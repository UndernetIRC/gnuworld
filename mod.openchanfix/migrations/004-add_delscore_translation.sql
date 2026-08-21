-- Migration: Add English translations for the new DELSCORE command
--
-- Adds response ids 227 (score_deleted) and 228 (error_deleting_score),
-- used by the new DELSCORE command. Safe to run against an older database
-- (rows don't exist yet) or one already seeded from the current
-- doc/chanfix.language.english.sql (rows already present).

INSERT INTO translations (language_id, response_id, text, last_updated, deleted)
VALUES (1, 227, 'Deleted score for account %s in channel %s (was %u points).', extract(epoch from now())::int, 0)
ON CONFLICT (language_id, response_id) DO NOTHING;

INSERT INTO translations (language_id, response_id, text, last_updated, deleted)
VALUES (1, 228, 'Error deleting score for account %s on channel %s.', extract(epoch from now())::int, 0)
ON CONFLICT (language_id, response_id) DO NOTHING;
