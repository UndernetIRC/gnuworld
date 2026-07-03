-- =============================================================================
-- 005_rename_event_param.sql
-- Rename spam_events.event_param to spam_events.param. The column is a
-- generic per-event-type value (regex, threshold, channel name, mode
-- string, etc. depending on event_type) rather than something specific to
-- "events" as opposed to rules/actions, so the event_ prefix was dropped.
--
-- Also clears the leftover placeholder value ('.') that was previously
-- typed into this column for TEXT_REPEAT events, since the SPAM EVENT ADD
-- command required a value there even though TEXT_REPEAT never reads it
-- (TEXT_REPEAT behavior is controlled entirely by the repeat_* columns).
--
-- Apply with:
--   psql -d <dbname> -f 005_rename_event_param.sql
-- =============================================================================

ALTER TABLE spam_events RENAME COLUMN event_param TO param;

UPDATE spam_events SET param = NULL WHERE event_type = 'TEXT_REPEAT';
