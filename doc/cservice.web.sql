------------------------------------------------------------------------------------
-- "$Id: cservice.web.sql,v 1.17 2001/10/16 00:31:46 nighty Exp $"
-- Channel service DB SQL file for PostgreSQL.
--
-- Tables specific to webbased registration process.
--
-- Perry Lorier <perry@coders.net>
-- nighty <nighty@undernet.org> - Corrected tables and added missing fields

CREATE TABLE pendingusers (
	user_name VARCHAR(12),
	cookie VARCHAR(32),
	email VARCHAR(255),
	expire INT4,
	question_id INT2,
	verificationdata VARCHAR(30),
	language INT4 NOT NULL
);

CREATE INDEX pendingusers_cookie_idx ON pendingusers(cookie);

CREATE TABLE pending_emailchanges (
	cookie VARCHAR(128) NOT NULL,
	user_id INT4 NOT NULL,
	old_email VARCHAR(255) NOT NULL,
	new_email VARCHAR(255) NOT NULL,
	expiration INT4 NOT NULL
);

CREATE INDEX pending_emailchanges_cookie_idx ON pending_emailchanges(cookie);
CREATE INDEX pending_emailchanges_user_id_idx ON pending_emailchanges(user_id);
CREATE INDEX pending_emailchanges_expiration_idx ON pending_emailchanges(expiration);

-- This table stores the timestamp of the last request 
-- from a particular IP. 
-- Used to block abuse, such as requesting a password 50,000
-- times a minute.

CREATE TABLE lastrequests (
	ip VARCHAR(15) DEFAULT '0.0.0.0',
	last_request_ts INT4
);

CREATE INDEX lastrequests_ip_idx ON lastrequests(ip);

 -- list of admins that have the ability to modify NOREG entries (other admins may only list them) 

CREATE TABLE webaccessteam ( 
	admin_id int4 REFERENCES users(id) NOT NULL,
	level INT4 NOT NULL DEFAULT '0'
);
 
-- recorded objections for channels.
CREATE TABLE objections ( 
	channel_id int4 REFERENCES channels(id) NOT NULL, 
	user_id int4 REFERENCES users(id) NOT NULL, 
	comment text NOT NULL, 
	created_ts int4 NOT NULL,
	admin_only varchar(1) DEFAULT 'N'
-- 'Y' : the objection is an admin comment on only * users sees it.
-- 'N' : the objection is a regular one and everyone can see it.
);

-- Table used to store run-time configurable settings.

CREATE TABLE variables (
	var_name VARCHAR(30),
	contents text,
	PRIMARY KEY(var_name)
);

CREATE TABLE timezones (
	tz_index SERIAL,
	tz_name VARCHAR(128) NOT NULL,
	tz_countrycode VARCHAR(5) NOT NULL,
	tz_acronym VARCHAR(10) NOT NULL,
	deleted INT2 DEFAULT '0',
	last_updated INT4 NOT NULL
);


