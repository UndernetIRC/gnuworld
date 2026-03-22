-- Migration: Normalize day columns into chanops_daily table
--
-- Run this once against an existing database when upgrading to the
-- normalized daily table schema. Moves data from day0-day13 columns
-- into the new chanops_daily table, then drops the columns.
--
-- Safe to run multiple times — checks for table existence.

-- Step 1: Create the normalized table if it doesn't exist
CREATE TABLE IF NOT EXISTS chanops_daily (
    channel VARCHAR(200) NOT NULL,
    account VARCHAR(24) NOT NULL,
    day SMALLINT NOT NULL,
    points SMALLINT NOT NULL DEFAULT 0,
    PRIMARY KEY (channel, account, day)
);

-- Step 2: Migrate non-zero day data from columns to rows
INSERT INTO chanops_daily (channel, account, day, points)
SELECT channel, account, 0, day0 FROM chanOps WHERE day0 > 0
ON CONFLICT DO NOTHING;
INSERT INTO chanops_daily (channel, account, day, points)
SELECT channel, account, 1, day1 FROM chanOps WHERE day1 > 0
ON CONFLICT DO NOTHING;
INSERT INTO chanops_daily (channel, account, day, points)
SELECT channel, account, 2, day2 FROM chanOps WHERE day2 > 0
ON CONFLICT DO NOTHING;
INSERT INTO chanops_daily (channel, account, day, points)
SELECT channel, account, 3, day3 FROM chanOps WHERE day3 > 0
ON CONFLICT DO NOTHING;
INSERT INTO chanops_daily (channel, account, day, points)
SELECT channel, account, 4, day4 FROM chanOps WHERE day4 > 0
ON CONFLICT DO NOTHING;
INSERT INTO chanops_daily (channel, account, day, points)
SELECT channel, account, 5, day5 FROM chanOps WHERE day5 > 0
ON CONFLICT DO NOTHING;
INSERT INTO chanops_daily (channel, account, day, points)
SELECT channel, account, 6, day6 FROM chanOps WHERE day6 > 0
ON CONFLICT DO NOTHING;
INSERT INTO chanops_daily (channel, account, day, points)
SELECT channel, account, 7, day7 FROM chanOps WHERE day7 > 0
ON CONFLICT DO NOTHING;
INSERT INTO chanops_daily (channel, account, day, points)
SELECT channel, account, 8, day8 FROM chanOps WHERE day8 > 0
ON CONFLICT DO NOTHING;
INSERT INTO chanops_daily (channel, account, day, points)
SELECT channel, account, 9, day9 FROM chanOps WHERE day9 > 0
ON CONFLICT DO NOTHING;
INSERT INTO chanops_daily (channel, account, day, points)
SELECT channel, account, 10, day10 FROM chanOps WHERE day10 > 0
ON CONFLICT DO NOTHING;
INSERT INTO chanops_daily (channel, account, day, points)
SELECT channel, account, 11, day11 FROM chanOps WHERE day11 > 0
ON CONFLICT DO NOTHING;
INSERT INTO chanops_daily (channel, account, day, points)
SELECT channel, account, 12, day12 FROM chanOps WHERE day12 > 0
ON CONFLICT DO NOTHING;
INSERT INTO chanops_daily (channel, account, day, points)
SELECT channel, account, 13, day13 FROM chanOps WHERE day13 > 0
ON CONFLICT DO NOTHING;

-- Step 3: Drop the day columns from chanOps
ALTER TABLE chanOps DROP COLUMN IF EXISTS day0;
ALTER TABLE chanOps DROP COLUMN IF EXISTS day1;
ALTER TABLE chanOps DROP COLUMN IF EXISTS day2;
ALTER TABLE chanOps DROP COLUMN IF EXISTS day3;
ALTER TABLE chanOps DROP COLUMN IF EXISTS day4;
ALTER TABLE chanOps DROP COLUMN IF EXISTS day5;
ALTER TABLE chanOps DROP COLUMN IF EXISTS day6;
ALTER TABLE chanOps DROP COLUMN IF EXISTS day7;
ALTER TABLE chanOps DROP COLUMN IF EXISTS day8;
ALTER TABLE chanOps DROP COLUMN IF EXISTS day9;
ALTER TABLE chanOps DROP COLUMN IF EXISTS day10;
ALTER TABLE chanOps DROP COLUMN IF EXISTS day11;
ALTER TABLE chanOps DROP COLUMN IF EXISTS day12;
ALTER TABLE chanOps DROP COLUMN IF EXISTS day13;
