
-- migrate whitelist table to 'inet' datatype
ALTER TABLE whitelist ALTER COLUMN ip TYPE inet USING ip::inet;

-- support enhanced traffic check on first pass
ALTER TABLE pending ADD COLUMN first_init CHAR NOT NULL DEFAULT 'N';
ALTER TABLE supporters ADD COLUMN noticed CHAR NOT NULL DEFAULT 'N';

-- support feature to send users notices for interesting events
CREATE TABLE notices (
	message_id SERIAL,
	user_id INT4 CONSTRAINT users_notes_ref REFERENCES users( id ),
	message VARCHAR( 300 ),
	last_updated INT4 NOT NULL,
	PRIMARY KEY(message_id, user_id)
);

-- support channel suspension reasons
ALTER TABLE levels ADD COLUMN suspend_reason VARCHAR( 300 );

-- support channel NOTAKE protection feature
ALTER TABLE channels ADD COLUMN no_take INT4 DEFAULT '0';

-- support channel FLOODPRO protection feature
ALTER TABLE channels ALTER COLUMN flood_pro SET DATA TYPE INT4;

-- store only IP in last_ip for website lookups (nick!user@host was redundant)
-- Empus <empus@undernet.org> (2016.09.19)
UPDATE users_lastseen SET last_ip = split_part(last_ip, '@', 2);
UPDATE users_lastseen SET last_ip = null WHERE last_ip = '';
ALTER TABLE users_lastseen ALTER COLUMN last_ip TYPE inet USING last_ip::inet;

-- add 'hint' column to 'variables' tables
-- how and when did this get missed?!
-- Empus (2016.10.18)
ALTER TABLE variables ADD COLUMN hint text;

# -- fix reference to 'MASSDEOP' for MASSDEOPPRO
UPDATE help SET contents = replace(contents, 'MASSDEOP', 'MASSDEOPPRO') WHERE topic='SET' and language_id IN (1, 8);
UPDATE help SET contents = replace(contents, 'MASSDEOPPROPRO', 'MASSDEOPPRO') WHERE topic IN ('SET', 'SET MASSDEOP');
UPDATE help SET topic='SET MASSDEOPPRO' WHERE topic='SET MASSDEOP';

# -- support for new login history tracking (in SCANHOST command)
CREATE TABLE user_sec_history (
        user_id INT4 NOT NULL,
        user_name TEXT NOT NULL,
        command TEXT NOT NULL,
        ip VARCHAR( 256 ) NOT NULL,
        hostmask VARCHAR( 256 ) NOT NULL,
        timestamp INT4 NOT NULL
);
