-- "$Id: ccontrol.sql,v 1.25 2002/05/25 15:03:57 mrbean_ Exp $"

-- 2002-25-02 : |MrBean|
-- Added the Misc table

-- 2001-10-14 : nighty
-- corrected fieldname typo in "suspendReason" -> "suspend_reason"

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
	saccess INT4 NOT NULL DEFAULT '0',
-- For a full list of access mask see CControlCommands.h
	server VARCHAR (128) NOT NULL DEFAULT 'undernet.org',          -- the server the oper is assosiated to
	flags INT4 NOT NULL DEFAULT '0',
	isSuspended BOOLEAN NOT NULL DEFAULT 'n',
	suspend_expires INT4,
	suspend_level INT4,
	suspended_by VARCHAR(128),
	suspend_Reason VARCHAR(256),
	isUHS BOOLEAN NOT NULL DEFAULT 'n',
	isOPER BOOLEAN NOT NULL DEFAULT 'n',
	isADMIN BOOLEAN NOT NULL DEFAULT 'n',
	isSMT BOOLEAN NOT NULL DEFAULT 'n',
	isCODER BOOLEAN NOT NULL DEFAULT 'n',
	getLOGS BOOLEAN NOT NULL DEFAULT 'n',
	needOP BOOLEAN NOT NULL DEFAULT 'n',
	email VARCHAR(128),
	last_updated_by VARCHAR (128),		-- nick!user@host
	last_updated INT4 NOT NULL,
	notice BOOLEAN NOT NULL DEFAULT 't',
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
	LastNumeric VARCHAR(4),
	SplitReason VARCHAR(512),
	Version VARCHAR(256),
	AddedOn INT4 NOT NULL,
	LastUpdated INT4 NOT NULL
	);

CREATE TABLE comlog (
	ts INT4 NOT NULL,
	oper text,
	command VARCHAR(512)
	);
		
CREATE TABLE opernews (
	MessageId SERIAL,
	MessageFlags INT4 NOT NULL DEFAULT '0',
	PostTime INT4 NOT NULL,
	PostedBy VARCHAR(128) NOT NULL,
	Message VARCHAR(512) NOT NULL,
	ExpiresOn INT4
	);

CREATE TABLE exceptions (
	Host VARCHAR(128) NOT NULL,
	Connections INT4 NOT NULL,
	AddedBy VARCHAR(128) NOT NULL,
	AddedOn INT4 NOT NULL DEFAULT now()::abstime::int4
	);	
	
CREATE TABLE commands (
	RealName VARCHAR(128) NOT NULL UNIQUE,
	Name     VARCHAR(128) NOT NULL UNIQUE,
	Flags    INT4 NOT NULL,
	IsDisabled BOOLEAN NOT NULL DEFAULT 'n',
	NeedOp     BOOLEAN NOT NULL DEFAULT 'n',
	NoLog      BOOLEAN NOT NULL DEFAULT 'n',
	MinLevel   INT4 NOT NULL DEFAULT '1',
	SAccess BOOLEAN NOT NULL DEFAULT 'n'
	);

CREATE TABLE notes (
	user_id	INT4 NOT NULL REFERENCES opers (user_id),
	sentby TEXT NOT NULL,
	postedOn INT4 NOT NULL,
	IsNew BOOLEAN NOT NULL DEFAULT 'n',
	note VARCHAR (512)
	);
	
CREATE TABLE Misc (
	VarName VARCHAR(30) NOT NULL,
	Value1 INT4,
	Value2 INT4,
	Value3 INT4,
	Value4 VARCHAR(40),
	Value5 VARCHAR(128)
	);

CREATE TABLE BadChannels (
	Name VARCHAR(400) NOT NULL,
	Reason VARCHAR(512) NOT NULL,
	AddedBy VARCHAR(200) NOT NULL
	);
		