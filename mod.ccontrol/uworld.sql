-- 2001-13-02 
-- Added by |MrBean| (MrBean@toughguy.net)
-- Level patch for ccontrol module


CREATE TABLE opers (
	user_id SERIAL,	
	user_name TEXT NOT NULL UNIQUE,
	password VARCHAR (40) NOT NULL,
	access INT4 NOT NULL DEFAULT '0',
-- For a full list of access mask see CControlCommands.h
	last_updated_by VARCHAR (128),		-- nick!user@host
	last_updated INT4 NOT NULL,
	flags INT4 NOT NULL DEFAULT '0',
	suspend_expires INT4,
	suspended_by VARCHAR(128),
	PRIMARY KEY( user_id )
);

CREATE TABLE hosts (
	user_id INT4 NOT NULL,
	host VARCHAR(128) NOT NULL
	);
	
