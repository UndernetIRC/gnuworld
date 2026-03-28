-- Migration: Create variables table for persistent configuration tracking
--
-- Stores runtime configuration values that need to persist across restarts.
-- Initially used to track the current daySamples value so that changes
-- to this config parameter can be detected and chanops_daily.day values
-- can be properly migrated.

CREATE TABLE IF NOT EXISTS variables (
    var_name VARCHAR(64) NOT NULL,
    var_value VARCHAR(256) NOT NULL,
    PRIMARY KEY (var_name)
);

-- Seed with the original default daySamples value.
-- If the user had a different value before this migration, the C++ code
-- will detect the mismatch on next startup and perform the migration.
INSERT INTO variables (var_name, var_value) VALUES ('daysamples', '14')
ON CONFLICT (var_name) DO NOTHING;
