------------------------------------------------------------------------------------
-- "$Id: cservice.web.sql,v 1.3 2001/02/22 23:17:52 gte Exp $"
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

CREATE TABLE pendingusers (
	user_name VARCHAR(63),
	cookie VARCHAR(32),
	expire INT4,
	question_id INT2,
	verificationdata VARCHAR(30)
);
