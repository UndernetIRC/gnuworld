------------------------------------------------------------------------------------
-- "$Id: cservice.web.sql,v 1.11 2001/09/14 01:17:51 nighty Exp $"
-- Channel service DB SQL file for PostgreSQL.
--
-- Tables specific to webbased registration process.
--
-- Perry Lorier <perry@coders.net>

CREATE TABLE webcookies (
	user_id INT4 CONSTRAINT users_supporters_ref REFERENCES users( id ),
	cookie VARCHAR(32) UNIQUE,
	expire INT4
);

CREATE INDEX webcookies_user_id_idx ON webcookies(user_id);

CREATE TABLE pendingusers (
	user_name VARCHAR(63),
	cookie VARCHAR(32),
	email VARCHAR(255),
	expire INT4,
	question_id INT2,
	verificationdata VARCHAR(30)
);

CREATE INDEX pendingusers_cookie_idx ON pendingusers(cookie);

-- This table stores the timestamp of the last request 
-- from a particular IP. 
-- Used to block abuse, such as requesting a password 50,000
-- times a minute.

CREATE TABLE lastrequests (
	ip VARCHAR(15),
	last_request_ts INT4
);

CREATE INDEX lastrequests_ip_idx ON lastrequests(ip);

 -- list of admins that have the ability to modify NOREG entries (other admins may only list them) 

CREATE TABLE regteam ( 
	admin_id int4 REFERENCES users(id) NOT NULL
);

CREATE TABLE abuseteam ( 
	admin_id int4 REFERENCES users(id) NOT NULL
);

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
)

