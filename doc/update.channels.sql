--
-- This file reflects the latest changes in mod.cservice (26/12/2001)
-- so, you NEED to update your cservice database using that file if you
-- upgraded mode.cservice to a version after 26/12/2001 and you are using an older db schema.
--

ALTER TABLE channels ADD COLUMN limit_offset INT4 DEFAULT '3';
ALTER TABLE channels ADD COLUMN limit_period INT4 DEFAULT '20';
UPDATE channels SET limit_offset = 3;
UPDATE channels SET limit_period = 20;

