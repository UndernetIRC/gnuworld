--
-- Adds the last_ip column to the users_lastseen table

ALTER TABLE users_lastseen ADD COLUMN  last_ip VARCHAR( 256 );

