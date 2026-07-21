-- =============================================================================
-- 007_monitored_channel_last_triggered.sql
-- Tracks the last time a spam rule fired in a monitored channel, and which
-- rule it was, for visibility in SPAM CHAN LIST/SHOW. Updated by
-- dronescan::evaluateSpamRules() whenever a rule's threshold is crossed for
-- a channel that is currently monitored.
--
-- Apply with:
--   psql -d <dbname> -f 007_monitored_channel_last_triggered.sql
-- =============================================================================

ALTER TABLE monitored_channels ADD COLUMN last_triggered_ts   int4        DEFAULT NULL;
ALTER TABLE monitored_channels ADD COLUMN last_triggered_rule varchar(100) DEFAULT NULL;
