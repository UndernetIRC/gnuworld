--
-- $Id: update.default_messages.sql,v 1.1 2004/03/07 22:05:22 nighty Exp $
--
-- import this file into the REMOTE_DB :
-- /usr/local/pgsql/bin/psql cservice < update.default_messages.sql
--
-- This table will be used for :
--	Channel reviews : ACCEPT (type=1) or REJECT (type=2)
--	Complaint replies (type=3)
-- or more eventually ...
--

CREATE TABLE default_msgs (
	id	SERIAL,
	type	int4 NOT NULL,
	label	varchar(255) NOT NULL,
	content	text NOT NULL
);

CREATE INDEX default_msgs_idx ON default_msgs(type);
