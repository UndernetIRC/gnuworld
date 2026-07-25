-- Migration: Add UNIQUE constraint on chanOps(channel, account)
--
-- Required before upgrading to the incremental SQL sync code.
-- The new syncToDB() uses INSERT ... ON CONFLICT (channel, account) DO UPDATE,
-- which requires a unique constraint on these columns.
--
-- Run this once against an existing database before deploying the new code.
-- Safe to run against a database already built from the current chanfix.sql,
-- which already declares an inline UNIQUE(channel, account) on chanOps.

-- Step 1: Deduplicate any existing rows (keep the row with the later ctid)
DELETE FROM chanOps a USING chanOps b
  WHERE a.channel = b.channel AND a.account = b.account
  AND a.ctid < b.ctid;

-- Step 2: Add the unique constraint, unless one already covers these
-- columns (the base chanfix.sql schema already declares an inline,
-- unnamed UNIQUE(channel, account)).
DO $$
DECLARE
  chan_attnum smallint;
  acct_attnum smallint;
BEGIN
  SELECT attnum INTO chan_attnum FROM pg_attribute WHERE attrelid = 'chanops'::regclass AND attname = 'channel';
  SELECT attnum INTO acct_attnum FROM pg_attribute WHERE attrelid = 'chanops'::regclass AND attname = 'account';

  IF NOT EXISTS (
    SELECT 1 FROM pg_constraint
    WHERE conrelid = 'chanops'::regclass
      AND contype = 'u'
      AND conkey = ARRAY[LEAST(chan_attnum, acct_attnum), GREATEST(chan_attnum, acct_attnum)]::smallint[]
  ) THEN
    ALTER TABLE chanOps ADD CONSTRAINT chanops_channel_account_unique
      UNIQUE (channel, account);
  END IF;
END $$;

-- Step 3: Add DUMPSQL command to help system
INSERT INTO help (language_id, topic, contents)
  VALUES (1, 'DUMPSQL', 'Dumps all in-memory chanOp data as SQL UPSERT statements to a timestamped file on disk. Use as emergency data recovery when the database is unreachable.');

UPDATE help SET contents = '\002Owner (+o)\002: DUMPSQL LASTCOM REHASH RELOAD SET SHUTDOWN'
  WHERE topic = '<INDEXOWNER>';
