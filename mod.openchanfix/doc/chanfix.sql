-- mod.chanfix SQL Database Script
-- (c) 2003 Matthias Crauwels <ultimate_@wol.be>
-- (c) 2005 Reed Loden <reed@reedloden.com>
-- $Id: chanfix.sql,v 1.4 2006/12/09 00:29:20 buzlip01 Exp $

CREATE TABLE languages (
        id SERIAL,
        code VARCHAR( 16 ) UNIQUE,
        name VARCHAR( 16 ),
        last_updated INT4 NOT NULL,
        deleted INT2 DEFAULT '0',
        PRIMARY KEY(id)
);

CREATE TABLE translations (
        language_id INT4 CONSTRAINT translations_language_id_ref REFERENCES languages ( id ),
        response_id INT4 NOT NULL DEFAULT '0',
        text TEXT,
        last_updated INT4 NOT NULL,
        deleted INT2 DEFAULT '0',

        PRIMARY KEY (language_id, response_id)
);

CREATE TABLE help (
        topic VARCHAR(20) NOT NULL,
        language_id INT4 CONSTRAINT help_language_id_ref REFERENCES languages ( id ),
        contents TEXT
);

CREATE INDEX help_topic_idx ON help (topic);
CREATE INDEX help_language_id_idx ON help (language_id);

CREATE TABLE chanOps (
	channel VARCHAR(200) NOT NULL,
	account VARCHAR(24) NOT NULL,
	last_seen_as VARCHAR(128),
	ts_firstopped INT4 DEFAULT 0,
	ts_lastopped INT4 DEFAULT 0,
	day0 INT2 NOT NULL DEFAULT 0,
	day1 INT2 NOT NULL DEFAULT 0,
	day2 INT2 NOT NULL DEFAULT 0,
	day3 INT2 NOT NULL DEFAULT 0,
	day4 INT2 NOT NULL DEFAULT 0,
	day5 INT2 NOT NULL DEFAULT 0,
	day6 INT2 NOT NULL DEFAULT 0,
	day7 INT2 NOT NULL DEFAULT 0,
	day8 INT2 NOT NULL DEFAULT 0,
	day9 INT2 NOT NULL DEFAULT 0,
	day10 INT2 NOT NULL DEFAULT 0,
	day11 INT2 NOT NULL DEFAULT 0,
	day12 INT2 NOT NULL DEFAULT 0,
	day13 INT2 NOT NULL DEFAULT 0
);

CREATE TABLE channels (
	id SERIAL,
	channel TEXT NOT NULL UNIQUE,
	flags INT4 NOT NULL DEFAULT 0,
	PRIMARY KEY (id)
);

CREATE UNIQUE INDEX channels_name_idx ON channels(LOWER(channel));

CREATE TABLE users (
	id SERIAL,
	user_name TEXT NOT NULL UNIQUE,
	created INT4 NOT NULL DEFAULT 0,
	last_seen INT4 NOT NULL DEFAULT 0,
	last_updated INT4 NOT NULL DEFAULT 0,
	last_updated_by VARCHAR(128),
	language_id INT4 CONSTRAINT language_id_ref REFERENCES languages (id) NOT NULL DEFAULT 1,
	faction VARCHAR(128) NOT NULL DEFAULT 'undernet.org',
	flags INT2 NOT NULL DEFAULT 0,
	-- 0x01 - server admin (limited access to +u commands)
	-- 0x02 - can block/unblock channels
	-- 0x04 - can add/del notes/alerts to channels
	-- 0x08 - can manual chanfix
	-- 0x10 - oper not required
	-- 0x20 - owner
	-- 0x40 - user management rights
	-- 0x80 - requires authentication
	isSuspended BOOLEAN NOT NULL DEFAULT FALSE,
	-- currently suspended
	useNotice BOOLEAN NOT NULL DEFAULT TRUE,
	-- use notice instead of privmsg
	needOper BOOLEAN NOT NULL DEFAULT TRUE,
	-- require operator privileges from staff
	PRIMARY KEY (id)
);

CREATE INDEX users_username_idx ON users( lower(user_name) );

CREATE TABLE hosts (
	user_id INT4 CONSTRAINT hosts_user_id_ref REFERENCES users ( id ),
	host VARCHAR(128) NOT NULL
);

CREATE INDEX hosts_user_id_idx ON hosts(user_id);

CREATE TABLE notes (
	id SERIAL,
	ts INT4,
	channelID INT4 CONSTRAINT notes_channelID_ref REFERENCES channels ( id ),
	user_name VARCHAR(128),
	event INT2 DEFAULT 0,
	-- Defines the note event type, so we can filter nice reports.
-- 1  -- EV_MISC - Uncategorised event.
-- 2  -- EV_NOTE - Miscellaneous notes about a channel.
-- 3  -- EV_CHANFIX - When someone manual chanfixes a channel.
-- 4  -- EV_BLOCK - When someone blocks a channel.
-- 5  -- EV_UNBLOCK - When somebody unblocks a channel.
-- 6  -- EV_ALERT - When someone sets alert flag on a channel.
-- 7  -- EV_UNALERT - When somebody removes alert flag from a channel.
-- 8  -- EV_REQUESTOPS - When a normal client requestops a channel.

	message TEXT,
	PRIMARY KEY(id, channelID)
);

CREATE TABLE comlog (
	ts INT4 NOT NULL,
	user_name text,
	command VARCHAR(512)
);

CREATE INDEX notes_channelID_idx ON notes(channelID);
CREATE INDEX notes_event_idx ON notes(event);
