CREATE TABLE users (
	id serial,
	user_name varchar(20) NOT NULL,
	created int4 NOT NULL DEFAULT 0,
	last_seen int4 NOT NULL DEFAULT 0,
	last_updated int4 NOT NULL DEFAULT 0,
	last_updated_by varchar(128) NOT NULL,
	flags int4 NOT NULL DEFAULT 0,
	access int4 NOT NULL DEFAULT 0,
	PRIMARY KEY (id)
);
-----------------------
CREATE TABLE fakeclients (
	id serial, 
	nickname varchar(20) NOT NULL, 
	username varchar(20) NOT NULL, 
	hostname varchar(20) NOT NULL, 
	realname varchar(50) NOT NULL, 
	created_by int4 NOT NULL, 
	created_on int4 NOT NULL, 
	last_updated int4 NOT NULL,
	flags int4 DEFAULT 0 NOT NULL,
	PRIMARY KEY (id), 
	FOREIGN KEY (created_by) REFERENCES users (id) ON UPDATE RESTRICT ON DELETE CASCADE,
	UNIQUE (nickname)
);
-------------------------
CREATE TABLE exceptionalChannels (
	id serial,
	name text unique not null
);
