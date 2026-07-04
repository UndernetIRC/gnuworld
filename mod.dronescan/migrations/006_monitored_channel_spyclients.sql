-- =============================================================================
-- 006_monitored_channel_spyclients.sql
-- Optional restricted spy-client list per monitored channel. When a channel
-- has rows here, dronescan::findBestSpyClient() only considers these spy
-- clients (starting at a random position and walking down the list) instead
-- of the full spy client pool. Channels with no rows here are unaffected
-- (existing full-pool selection behavior).
--
-- Apply with:
--   psql -d <dbname> -f 006_monitored_channel_spyclients.sql
-- =============================================================================

CREATE TABLE monitored_channel_spyclients (
	channel_id   int NOT NULL REFERENCES monitored_channels(id) ON DELETE CASCADE,
	spyclient_id int NOT NULL REFERENCES spyclients(id)         ON DELETE CASCADE,
	PRIMARY KEY (channel_id, spyclient_id)
);
