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
