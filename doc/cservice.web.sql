------------------------------------------------------------------------------------
-- "$Id: cservice.web.sql,v 1.6 2001/03/18 22:46:04 gte Exp $"
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
