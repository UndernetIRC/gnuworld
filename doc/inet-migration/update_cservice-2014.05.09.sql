-- WARNING: Run this ONLY after you made your backup!
DROP TABLE pending_traffic;
CREATE TABLE pending_traffic (
        channel_id INT4 CONSTRAINT pending_traffic_channel_ref REFERENCES channels (id),
        ip_number inet,
        join_count INT4,
        PRIMARY KEY(channel_id, ip_number)
);

DROP TABLE ip_restrict;
CREATE TABLE ip_restrict (
        id      SERIAL,
        user_id int4 NOT NULL,
        value   inet NOT NULL,
        last_updated    INT4 NOT NULL DEFAULT now()::abstime::int4,
        last_used    int4 NOT NULL DEFAULT 0,
        expiry  int4 NOT NULL,
        description  VARCHAR(255),
        added   int4 NOT NULL,
        added_by        int4 NOT NULL,
        type    int4 NOT NULL DEFAULT 0
);

ALTER TABLE pending ADD COLUMN first_init CHAR NOT NULL DEFAULT 'N';
ALTER TABLE supporters ADD COLUMN noticed CHAR NOT NULL DEFAULT 'N';

CREATE TABLE notices (
	message_id SERIAL,
	user_id INT4 CONSTRAINT users_notes_ref REFERENCES users( id ),
	message VARCHAR( 300 ),
	last_updated INT4 NOT NULL,

	PRIMARY KEY(message_id, user_id)
);

ALTER TABLE levels ADD COLUMN suspend_reason VARCHAR( 300 );
ALTER TABLE channels ADD COLUMN no_take INT4 DEFAULT '0';
ALTER TABLE channels ALTER COLUMN flood_pro SET DATA TYPE INT4;

-- Store only IP in last_ip for website lookups (nick!user@host was redundant)
-- Empus <empus@undernet.org> (2016.09.19)
UPDATE users_lastseen SET last_ip = split_part(last_ip, '@', 2);
UPDATE users_lastseen SET last_ip = null WHERE last_ip = '';
ALTER TABLE users_lastseen ALTER COLUMN last_ip TYPE inet USING last_ip::inet;
