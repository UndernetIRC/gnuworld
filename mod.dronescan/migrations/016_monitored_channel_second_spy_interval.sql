-- =============================================================================
-- 016_monitored_channel_second_spy_interval.sql
-- Optional per-channel override for how often (in minutes) a second,
-- otherwise-idle spy client may temporarily join a channel that already has
-- one assigned, for extra coverage. NULL = use the module's global
-- secondSpyJoinIntervalMin config default. 0 = disabled for this channel.
--
-- Apply with:
--   psql -d <dbname> -f 016_monitored_channel_second_spy_interval.sql
-- =============================================================================

ALTER TABLE monitored_channels ADD COLUMN IF NOT EXISTS second_spy_join_interval_min int4 DEFAULT NULL;
