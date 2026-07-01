-- 002_rename_event_types.sql
--
-- Part 1: Rename PRIVMSG_REGEX, QUIT_MSG, and PART_MSG event types to TEXT.
--
-- Part 2: Target bitmask rename (CHAN=1 ? CHAN_PRIV=1, NOTICE=4 ? CHAN_NOT=4,
--         new NOTICE=32 for direct notices to bot/spy).
--   ? Bit VALUES for existing sources are UNCHANGED ? no per-row bit migration
--     is required for chan_priv (1), privmsg (2), chan_not (4), part (8), quit (16).
--   ? The new NOTICE=32 bit is additive and was not active before; existing rows
--     will not have it set ? opt-in by updating target explicitly.
--   ? Old ALL=31 did not include NOTICE=32. The UPDATE below adds the new bit
--     to every row whose target already covered all previously-defined sources
--     (target & 31 = 31), on the assumption that "all" means all sources.
--     Comment it out if you prefer manual opt-in.
--
-- Apply with:
--   psql -d <dbname> -f 002_rename_event_types.sql

-- Part 1: event_type renames
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

-- Part 2: extend old ALL=31 rows to new ALL=63
-- (adds the new NOTICE=32 bit to rows that previously used every source)
UPDATE spam_events
    SET target = target | 32
    WHERE (target & 31) = 31;
