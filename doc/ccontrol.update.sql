
-- "$Id: ccontrol.update.sql,v 1.2 2001/07/23 12:09:08 mrbean_ Exp $"

-- ccontrol database changes update
-- this file will add the new features to an old database
-- this script will only update the opers who doesnt have getlogs enabled
-- because of misfunctionality of postgresql

-- Add missing columns 
ALTER TABLE opers ADD isSuspended BOOLEAN NOT NULL DEFAULT 'n';
ALTER TABLE opers ADD suspendReason VARCHAR(256);
ALTER TABLE opers ADD email VARCHAR(128);
ALTER TABLE opers ADD isUHS BOOLEAN NOT NULL DEFAULT 'n';
ALTER TABLE opers ADD isOPER BOOLEAN NOT NULL DEFAULT 'n';
ALTER TABLE opers ADD isADMIN BOOLEAN NOT NULL DEFAULT 'n';
ALTER TABLE opers ADD isSMT BOOLEAN NOT NULL DEFAULT 'n';
ALTER TABLE opers ADD isCODER BOOLEAN NOT NULL DEFAULT 'n';
ALTER TABLE opers ADD getLOGS BOOLEAN NOT NULL DEFAULT 'n';
ALTER TABLE opers ADD needOP BOOLEAN NOT NULL DEFAULT 'n';

-- Update all the other opers to the new settings

UPDATE opers set isOPER = 't' where flags = 2;
UPDATE opers set isADMIN = 't' where flags = 4;
UPDATE opers set isSMT = 't' where flags = 8;
UPDATE opers set isCODER = 't' where flags = 32;
