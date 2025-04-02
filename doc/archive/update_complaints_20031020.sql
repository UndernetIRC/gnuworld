-- $Id: update_complaints_20031020.sql,v 1.1 2003/10/20 02:26:50 nighty Exp $
--
-- Use this file if you have a database schema older than oct 20 2003.
-- /usr/local/pgsql/bin/psql cservice < update_complaints_20031020.sql
--

CREATE TABLE complaints_reference (
	complaints_ref int4 NOT NULL CONSTRAINT complaints_reference_ref REFERENCES complaints (id),
	referenced_by int4 NOT NULL,
	referenced_to int4 NOT NULL,
	reference_ts int4 NOT NULL,
	is_new int4 DEFAULT '1' NOT NULL
);

CREATE INDEX complaints_ref_ref ON complaints_reference(complaints_ref,referenced_to);
