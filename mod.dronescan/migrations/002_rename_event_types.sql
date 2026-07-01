-- 002_rename_event_types.sql
--
-- Renames the PRIVMSG_REGEX, QUIT_MSG, and PART_MSG event types introduced in
-- 001_spam_detection.sql to the unified TEXT event type.
--
-- PRIVMSG_REGEX ? TEXT (target bitmask is unchanged; it already encoded the
--                       intended sources)
-- QUIT_MSG      ? TEXT with QUIT bit (16) added to target
-- PART_MSG      ? TEXT with PART bit  (8)  added to target
--
-- Apply with:
--   psql -d <dbname> -f 002_rename_event_types.sql

UPDATE spam_events
    SET event_type = 'TEXT'
    WHERE event_type = 'PRIVMSG_REGEX';

UPDATE spam_events
    SET event_type = 'TEXT',
        target     = target | 16
    WHERE event_type = 'QUIT_MSG';

UPDATE spam_events
    SET event_type = 'TEXT',
        target     = target | 8
    WHERE event_type = 'PART_MSG';
