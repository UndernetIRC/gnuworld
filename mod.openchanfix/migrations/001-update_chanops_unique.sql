-- Migration: Add UNIQUE constraint on chanOps(channel, account)
--
-- Required before upgrading to the incremental SQL sync code.
-- The new syncToDB() uses INSERT ... ON CONFLICT (channel, account) DO UPDATE,
-- which requires a unique constraint on these columns.
--
-- Run this once against an existing database before deploying the new code.
-- Safe to run multiple times (the ALTER will fail harmlessly if constraint exists).

-- Step 1: Deduplicate any existing rows (keep the row with the later ctid)
DELETE FROM chanOps a USING chanOps b
  WHERE a.channel = b.channel AND a.account = b.account
  AND a.ctid < b.ctid;

-- Step 2: Add the unique constraint
ALTER TABLE chanOps ADD CONSTRAINT chanops_channel_account_unique
  UNIQUE (channel, account);

-- Step 3: Add DUMPSQL command to help system
INSERT INTO help (language_id, topic, contents)
  VALUES (1, 'DUMPSQL', 'Dumps all in-memory chanOp data as SQL UPSERT statements to a timestamped file on disk. Use as emergency data recovery when the database is unreachable.');

UPDATE help SET contents = '\002Owner (+o)\002: DUMPSQL LASTCOM REHASH RELOAD SET SHUTDOWN'
  WHERE topic = '<INDEXOWNER>';
