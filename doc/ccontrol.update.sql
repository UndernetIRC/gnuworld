
-- "$Id: ccontrol.update.sql,v 1.28 2009/07/25 18:12:33 hidden1 Exp $"

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

-- ALTER TABLE opers add suspend_level INT4 NOT NULL DEFAULT '0';
-- ALTER TABLE opers rename suspendreason to suspend_Reason;
-- ALTER TABLE servers add SplitReason VARCHAR(512);

-- 28/12/01 - Add some missing feilds to the servers table

-- alter TABLE servers add Version VARCHAR(256);
-- alter TABLE servers add AddedOn INT4 NOT NULL;
-- alter TABLE servers add LastUpdated INT4 NOT NULL;

-- 30/12/01 - Add notice column to the opers table 

-- alter TABLE opers add Notice BOOLEAN NOT NULL DEFAULT 't';
-- update opers set notice = 't';

-- 02/01/02 - update all opers so that they'll have access for scan

-- update opers set saccess = (saccess | 65536);

-- 25/01/02 - update all opers who had access to LEARNNET to the new flags

-- update opers set saccess = (saccess | 131072) where (saccess & 1)=1;

-- 25/02/02 - Add the Misc table

-- CREATE TABLE Misc (
--	VarName VARCHAR(30) NOT NULL,
--	Value1 INT4,
--	Value2 INT4,
--	Value3 INT4,
--	Value4 VARCHAR(40),
--	Value5 VARCHAR(128)
--	);
-- 14/03/02 Add MaxUsers command for opers
	
--update opers set saccess = (saccess | 262144);

-- 20/03/02 Add LASTCOM command for opers
-- update opers set saccess = (saccess | 8);

-- 16/05/02 Add the BadChannels table

-- CREATE TABLE BadChannels (
--        Name VARCHAR(400) NOT NULL,
--        Reason VARCHAR(512) NOT NULL,
--        AddedBy VARCHAR(200) NOT NULL
--        );

-- update opers set saccess = (saccess | 2097152) where isCODER = 't';

-- 27/08/02 Add REOP command to opers
--  update opers set saccess = (saccess | 4194304);
-- 16/12/02 Added lastupdated to glines
--  alter table glines add LastUpdated INT4 NOT NULL DEFAULT now()::abstime::int4;

-- 28/04/02 Add reason to the exceptions table
--   alter table exceptions add Reason VARCHAR(450);

-- 13/03/04 Add is deleted fields
--   alter table opers add is_deleted BOOLEAN NOT NULL DEFAULT 'n';   

-- 25/03/04 Add ReportMissing to servers
--    alter table servers add ReportMissing boolean not null default 't';

-- 04/06/04 Add UNJUPE command to the opers
--  update opers set saccess = (saccess | 33554432);

-- 05/06/04 Move MODUSER to the opers
--  update opers set access = (access | 2097152);

-- 2009/07/24 Add GETLAG and LastPassChangeTS to opers
alter TABLE opers add LastPassChangeTS INT4 NOT NULL DEFAULT '0';
alter TABLE opers add GetLAG BOOLEAN NOT NULL DEFAULT 'n';

