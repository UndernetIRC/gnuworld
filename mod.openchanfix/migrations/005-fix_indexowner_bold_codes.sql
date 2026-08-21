-- Migration: Fix HELP <INDEXOWNER> entry and add missing DELSCORE help
--
-- 1. The help.contents row for topic '<INDEXOWNER>' contains the literal
--    4-character text "\002" (backslash, 0, 0, 2) instead of the real 0x02
--    (STX / bold-toggle) control byte that every other <INDEX*> entry
--    uses. This makes "/msg C help" print the literal text
--    "\002Owner (+o)\002: ..." instead of bolding "Owner (+o)" like
--    "Chanfixer (+f)" and the other index lines. It was most likely
--    introduced by a manual UPDATE/INSERT run through psql (where a plain
--    '' string literal takes backslashes literally), rather than loaded
--    via doc/chanfix.help.sql's COPY format (where \002 is correctly
--    decoded to the raw byte).
--
-- 2. The DELSCORE command (added alongside this migration) was never added
--    to the help system: it has no HELP DELSCORE entry, and it is missing
--    from the <INDEXOWNER> command list. Both are fixed here in the same
--    UPDATE/INSERT as the bold-code fix above.
--
-- Idempotent: the UPDATE unconditionally sets <INDEXOWNER> to its final,
-- correct value (safe to rerun), and the INSERT is guarded to only add the
-- DELSCORE row if it does not already exist (the help table has no unique
-- constraint to use ON CONFLICT with).

UPDATE help
SET contents = chr(2) || 'Owner (+o)' || chr(2)
    || ': DELSCORE DUMPSQL LASTCOM REHASH RELOAD SET SHUTDOWN'
WHERE topic = '<INDEXOWNER>'
  AND language_id = 1;

DO $$
BEGIN
    IF NOT EXISTS (
        SELECT 1 FROM help WHERE topic = 'DELSCORE' AND language_id = 1
    ) THEN
        INSERT INTO help (topic, language_id, contents)
        VALUES ('DELSCORE', 1,
            'Deletes the chanfix score of the specified account in the specified channel.');
    END IF;
END $$;
