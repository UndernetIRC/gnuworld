------------------------------------------------------------------------------------
-- "$Id: local_db.sql,v 1.2 2001/10/21 02:17:14 nighty Exp $"
-- Channel service DB SQL file for PostgreSQL.
--
-- Tables specific to webcookies (needs to be LOCAL).
-- If your whole DB is local then this table NEEDS to be in a db named "cmaster"
-- If your local DB is named "cmaster" .. then just 'psql cmaster < webcookies.sql'
-- otherwise,
--                createdb cmaster
--                psql cmaster < webcookies.sql
--
-- nighty <nighty@undernet.org>

CREATE TABLE webcookies (
        user_id INT4,
        cookie VARCHAR(32) UNIQUE,
        expire INT4
);

CREATE INDEX webcookies_user_id_idx ON webcookies(user_id);


CREATE TABLE pending_pwreset (
        cookie VARCHAR(128) NOT NULL,
        user_id INT4 NOT NULL,
        question_id INT2 NOT NULL,
        verificationdata VARCHAR(30) NOT NULL,
        expiration INT4 NOT NULL
);

CREATE INDEX pending_pwreset_cookie_idx ON pending_pwreset(cookie);
CREATE INDEX pending_pwreset_user_id_idx ON pending_pwreset(user_id);
CREATE INDEX pending_pwreset_expiration_idx ON pending_pwreset(expiration);

CREATE TABLE helpmgr_users (
	user_id INT4 NOT NULL,
	language_id INT2,
	flags INT2 DEFAULT '0'
);

CREATE INDEX helpmgr_users_user_id_idx ON helpmgr_users(user_id);
CREATE INDEX helpmgr_language_id_idx ON helpmgr_users(language_id);
CREATE INDEX helpmgr_flags_idx ON helpmgr_users(flags);

