--
-- $Id: update.gfxcode.sql,v 1.1 2002/12/27 00:51:39 nighty Exp $
--
-- If you had updated recently and want to enable the "graphical code check" for new usernames,
-- you need to add this to your "local_db".
-- You can issue the following : /usr/local/pgsql/bin/psql local_db < update.gfxcode.sql
--
-- nighty <nighty@undernet.org>
--

CREATE TABLE gfxcodes (
	code	VARCHAR(25) NOT NULL,
	crc	VARCHAR(128) NOT NULL,
	expire	INT NOT NULL
);

CREATE INDEX gfxcodes_idx ON gfxcodes(code,crc,expire);

