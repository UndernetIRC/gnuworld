------------------------------------------------------------------------------------
-- "$Id: cservice.web.sql,v 1.2 2001/01/05 05:27:33 gte Exp $"
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
	expire INT4
);
