-- $Id: update_complaints_20031026.sql,v 1.2 2003/10/26 11:55:35 nighty Exp $
--
-- Use this file if you have a database schema older than oct 26 2003.
-- /usr/local/pgsql/bin/psql cservice < update_complaints_20031026.sql
--

ALTER TABLE "complaints" ADD "current_owner" int4 NOT NULL;
UPDATE complaints SET current_owner=0;

