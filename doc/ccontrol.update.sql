
-- "$Id: ccontrol.update.sql,v 1.8 2001/10/17 12:30:15 mrbean_ Exp $"

-- ccontrol database changes update
-- this file will add the new features to an old database
-- this script will only update the opers who doesnt have getlogs enabled
-- because of misfunctionality of postgresql

-- Add missing columns 
-- ALTER TABLE opers ADD isSuspended BOOLEAN NOT NULL DEFAULT 'n';
-- ALTER TABLE opers ADD suspend_Reason VARCHAR(256);
-- ALTER TABLE opers ADD email VARCHAR(128);
-- ALTER TABLE opers ADD isUHS BOOLEAN NOT NULL DEFAULT 'n';
-- ALTER TABLE opers ADD isOPER BOOLEAN NOT NULL DEFAULT 'n';
-- ALTER TABLE opers ADD isADMIN BOOLEAN NOT NULL DEFAULT 'n';
-- ALTER TABLE opers ADD isSMT BOOLEAN NOT NULL DEFAULT 'n';
-- ALTER TABLE opers ADD isCODER BOOLEAN NOT NULL DEFAULT 'n';
-- ALTER TABLE opers ADD getLOGS BOOLEAN NOT NULL DEFAULT 'n';
-- ALTER TABLE opers ADD needOP BOOLEAN NOT NULL DEFAULT 'n';
-- ALTER TABLE opers ADD saccess INT4 NOT NULL DEFAULT '0';

-- Update all the other opers to the new settings

-- UPDATE opers set isOPER = 't' where flags = 2;
-- UPDATE opers set isADMIN = 't' where flags = 4;
-- UPDATE opers set isSMT = 't' where flags = 8;
-- UPDATE opers set isCODER = 't' where flags = 32;

-- Create the commands table

-- CREATE TABLE commands (
--	RealName VARCHAR(128) NOT NULL UNIQUE,
--	Name     VARCHAR(128) NOT NULL UNIQUE,
--	Flags    INT4 NOT NULL,
--	IsDisabled BOOLEAN NOT NULL DEFAULT 'n',
--	NeedOp     BOOLEAN NOT NULL DEFAULT 'n',
--	NoLog      BOOLEAN NOT NULL DEFAULT 'n',
--	MinLevel  INT4 NOT NULL DEFAULT '1'
--	SAccess BOOLEAN NOT NULL DEFAULT 'n'
--	);

-- For those who already have the commands table
--ALTER  TABLE commands add SAccess BOOLEAN NOT NULL DEFAULT 'n';

-- Add the notes table
--CREATE TABLE notes (
--	user_id	INT4 NOT NULL REFERENCES opers (user_id),
--	sentby TEXT NOT NULL,
--	postedOn INT4 NOT NULL,
--	IsNew BOOLEAN NOT NULL DEFAULT 'n',
--	note VARCHAR (512)
--	);

ALTER TABLE opers add suspend_level INT4 NOT NULL DEFAULT '0';
ALTER TABLE opers rename suspendreason to suspend_Reason;
