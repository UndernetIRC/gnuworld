-- Update channels table to accept new max_bans feature
--
-- $Id: update_channels_20070325.sql,v 1.1 2007/03/25 16:42:24 kewlio Exp $
--

ALTER TABLE channels ADD COLUMN max_bans INT4 DEFAULT '0';

