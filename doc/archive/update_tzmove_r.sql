------------------------------------------------------------------------------------
-- "$Id: update_tzmove_r.sql,v 1.3 2002/08/30 10:28:42 nighty Exp $"
-- Channel service DB SQL file for PostgreSQL.

-- Script to run ONCE if you just updated with the new TimeZone feature stored in db
-- and you were previously using a version that was going thru browser cookies.
--
-- You should run this only once anyway. (psql cservice < update_tzmove_r.sql)

ALTER TABLE users ADD tz_setting VARCHAR(255) DEFAULT '';


