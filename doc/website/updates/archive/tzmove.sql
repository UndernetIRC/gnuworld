------------------------------------------------------------------------------------
-- "$Id: tzmove.sql,v 1.1 2002/08/09 01:10:46 reedloden Exp $"
-- Channel service DB SQL file for PostgreSQL.

-- Script to run ONCE if you just updated with the new TimeZone feature stored in db
-- and you were previously using a version that was going thru browser cookies.
--
-- You should run this only once anyway. (psql local_db < update_tzmove_l.sql)

ALTER TABLE webcookies ADD tz_setting VARCHAR(255) DEFAULT '';
ALTER TABLE webcookies ADD is_admin INT2 DEFAULT 0;

