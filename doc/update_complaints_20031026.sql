-- $Id: update_complaints_20031026.sql,v 1.3 2003/10/27 15:19:20 nighty Exp $
--
-- Use this file if you have a database schema older than oct 26 2003.
-- /usr/local/pgsql/bin/psql cservice < update_complaints_20031026.sql
--

ALTER TABLE "complaints" ADD "current_owner" int4;
UPDATE complaints SET current_owner=0;
ALTER TABLE complaints ALTER column current_owner SET NOT NULL;

