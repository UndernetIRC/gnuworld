-- "$Id: ccontrol.sql,v 1.6 2001/05/02 21:10:18 mrbean_ Exp $"
-- 2001-13-02  : |MrBean|
-- Added level patch for ccontrol module

-- 2001-22-02 : |MrBean|
-- Added help table for a new help system being developed

-- 2001-13-03 : |MrBean|
-- Added Glines table 

-- 2001-30-04 : |MrBean|
-- Added servers table

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

CREATE TABLE help (
	command VARCHAR(40) NOT NULL,
	subcommand VARCHAR(40),
	line INT4 NOT NULL DEFAULT '1',
	help VARCHAR(255)
);

CREATE TABLE glines (
	Id SERIAL,
	Host VARCHAR(128) UNIQUE NOT NULL,
	AddedBy VARCHAR(128) NOT NULL,
	AddedOn INT4 NOT NULL,
	ExpiresAt INT4 NOT NULL,
	Reason VARCHAR(255)
	);
				
CREATE TABLE servers (
	Name VARCHAR(100) NOT NULL,
	LastUplink VARCHAR(100),
	LastConnected INT4 NOT NULL DEFAULT '0',
	SplitedOn INT4 NOT NULL DEFAULT '0',
	LastNumeric VARCHAR(4)
	);

CREATE TABLE comlog (
	ts INT4 NOT NULL,
	oper text,
	command VARCHAR(512)
	);
		
	