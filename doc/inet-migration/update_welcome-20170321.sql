-- support channel WELCOME message feature
ALTER TABLE channels ADD COLUMN welcome VARCHAR(300) DEFAULT '';