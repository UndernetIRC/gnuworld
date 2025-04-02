--
-- $Id: update.ip_restrict.sql,v 1.1 2004/03/11 02:35:59 nighty Exp $
--
-- Update your REMOTEDB scheme with this file in order to support IP restrictions on logins
--
-- /usr/local/pgsql/bin/psql cservice < update.ip_restrict.sql
--
--

CREATE TABLE ip_restrict (
	id 	SERIAL,
	user_id	int4 NOT NULL,
	allowmask 	varchar(255) NOT NULL,
	allowrange1 	int4 NOT NULL,
	allowrange2 	int4 NOT NULL,
	added 	int4 NOT NULL,
	added_by 	int4 NOT NULL,
	type 	int4 NOT NULL
);

CREATE INDEX ip_restrict_idx ON ip_restrict(user_id,type);

