------------------------------------------------------------------------------------
-- "$Id: cservice.sql,v 1.46 2001/08/05 20:30:58 gte Exp $"
-- Channel service DB SQL file for PostgreSQL.

-- ChangeLog:
-- 2001-04-30: Gte
--             Redesigned deletion system, new table called deletion_transactions
--             to store deletion details for CMaster's to clear cached values.
-- 2001-01-04: Gte
--             Added 'deleted' flag, to flag records as deleted
--             (To enable CMaster to see deletions - can be *really* deleted during
--             routine maintainence/vacuum).
--             If deleted, value is non zero - null value treated as undeleted.
-- 2000-12-30: Gte
--             Added some update notification events for CMaster to listen
--             on and refresh its internal cache.
--
-- 2000-12-22: Gte
--             Fixed invalid UserID reference in userlog table.
--
-- 2000-12-10: Gte
--             Fixed a few typo's, changed TIMESTAMP's to INT4's.
--	       
-- 2000-10-22: Isomer
--             Removed 'ChannelManager' information from channels table
--             Added email/url/public_key to users table
--             Defined the flags
--             added channellog/userlog for combined ilc databases etc.
--             Changed many strings to 'TEXT'.
--             removed nick_flood_pro.
--             added defaults for flood_pro's.
--             changed the key type on the bans table.
--             added supporters table.
--             lotsa misc things
--             added 'added_by' and 'added_by_ts' to access table
--             users now have language, not channels
--             checked current CS sources in case we missed something.
--
-- 2000-09-21: Bleep
--             Added last_updated timestamps
--
-- Prior: Maintained by moof.           
 
-- The service supports multiple languages, defined in language
-- files.

CREATE TABLE languages (
	id SERIAL,
	code VARCHAR( 16 ) UNIQUE,
	name VARCHAR( 16 ),
	last_updated INT4 NOT NULL 
--	PRIMARY KEY(id)
);

-- Translations for multi-lingual support.

CREATE TABLE translations ( 
	language_id INT4 CONSTRAINT translations_language_id_ref REFERENCES languages ( id ),
	response_id INT4 NOT NULL DEFAULT '0',
	text TEXT,
	last_updated INT4 NOT NULL, 

	PRIMARY KEY (language_id, response_id)
);

CREATE TABLE help (
	topic VARCHAR(20) NOT NULL UNIQUE,
	language_id INT4 CONSTRAINT help_language_id_ref REFERENCES languages ( id ),
	contents text
);

CREATE INDEX help_topic_idx ON help (topic);
 
-- Create the channel table first since we'll be referring back to it
-- frequently.

CREATE TABLE channels (
	id SERIAL,
	name TEXT NOT NULL UNIQUE,
	flags INT4 NOT NULL DEFAULT '0',
-- 0x0000 0001 - No Purge
-- 0x0000 0002 - Special Channel
-- 0x0000 0004 - No Reg -- Don't register for a time period after which is
			-- reviewed by cservice admin.
-- 0x0000 0008 - Never Reg -- Never register
-- 0x0000 0010 - Channel Suspended
			-- everyone (including the 500) is suspended.
-- 0x0000 0020 - Temp Manager
-- 0x0000 0040 - Cautioned
-- 0x0000 0080 - Manager on Vacation

-- 0x0001 0000 - AlwaysOp
-- 0x0002 0000 - StrictOp
-- 0x0004 0000 - NoOp
-- 0x0008 0000 - AutoTopic
-- 0x0010 0000 - OpOnly
-- 0x0020 0000 - AutoJoin
		
	-- Do we want to keep either or both of these?
	-- nb: removed nickflood pro.  not useful.
	mass_deop_pro INT2 NOT NULL DEFAULT 3,
	flood_pro INT2 NOT NULL DEFAULT 7,
	url VARCHAR (128),
	description VARCHAR (128),
	-- Any administrative comments that apply globally to this
	-- channel.
	comment VARCHAR (300),
	keywords VARCHAR(128),
	registered_ts INT4,
	channel_ts INT4 NOT NULL,
	channel_mode VARCHAR(26), 

-- USERFLAGS: Defaults new access records to:
-- 0: No Default
-- 1: AutoOP
-- 2: AutoVOICE

	userflags INT2 DEFAULT '0',
	last_updated INT4 NOT NULL, 

	PRIMARY KEY (id)
);

-- A channel is inactive if the manager hasn't logged in for 21 days

CREATE INDEX channel_name_idx ON channels (lower(name));

-- Table for bans; channel_id references the channel entry this ban belongs to.

CREATE TABLE bans (

	id SERIAL,
	channel_id INT4 CONSTRAINT bans_channel_id_ref REFERENCES channels (id),
	banmask VARCHAR (128) NOT NULL,
	set_by VARCHAR (128),			-- nick!user@host
	set_ts INT4,
	level INT2,
	expires INT4,					-- Expiration timestamp.
	reason VARCHAR (128), 
	last_updated INT4 NOT NULL, 

	PRIMARY KEY (banmask,channel_id)
);

CREATE INDEX bans_expires_idx ON bans(expires);

-- Access entries; admin access kept on channel '*'.

CREATE TABLE users (

	id SERIAL,
	user_name TEXT NOT NULL,
	password VARCHAR (40) NOT NULL, 
	email TEXT,
	url  VARCHAR(128),
-- Which question the user provided the answer too from the signup page.
	question_id INT2,
-- The answer to the question.
	verificationdata VARCHAR(30), 
	language_id INT4 CONSTRAINT language_channel_id_ref REFERENCES languages (id),
	public_key TEXT,
	flags INT2 NOT NULL DEFAULT '0',
-- 0x00 01 -- Suspended globally
-- 0x00 02 -- Logged in
-- 0x00 04 -- Invisible
	last_updated_by VARCHAR (128),		-- nick!user@host
	last_updated INT4 NOT NULL, 

	PRIMARY KEY ( id )
) ;

CREATE INDEX users_username_idx ON users( lower(user_name) );
CREATE INDEX users_email_idx ON users( lower(email) );
 
-- This table used to store the "Last Seen" informatation previously
-- routinely updated in the users table.
 
CREATE TABLE users_lastseen (
	user_id INT4 CONSTRAINT lastseen_users_id_ref REFERENCES users ( id ),
	last_seen INT4,
	last_hostmask VARCHAR( 256 ),
	last_updated INT4 NOT NULL, 
	PRIMARY KEY (user_id)
);

CREATE TABLE levels (

	channel_id INT4 CONSTRAINT levels_channel_id_ref REFERENCES channels ( id ),
	user_id INT4 CONSTRAINT levels_users_id_ref REFERENCES users ( id ),
	access INT4 NOT NULL DEFAULT '0',
	flags INT2 NOT NULL DEFAULT '0',
-- 0x00 01 -- AutoOp
-- 0x00 02 -- Protect  (From CS source, unused)
-- 0x00 04 -- Temp forced access - Temp used by bot, ignore.
-- 0x00 08 -- AutoVoice
	suspend_expires INT4 DEFAULT '0',
	suspend_level INT4 DEFAULT '0',
	suspend_by VARCHAR( 128 ), 
	added INT4,
	added_By VARCHAR( 128 ),
	last_Modif INT4,
	last_Modif_By VARCHAR( 128 ),
	last_Updated INT4 NOT NULL, 

	PRIMARY KEY( channel_id, user_id )
);
 
CREATE INDEX levels_access_idx ON levels( access ) ;
CREATE INDEX levels_userid_idx ON levels( user_id ) ;
CREATE INDEX levels_suspendexpires_idx ON levels( suspend_expires ) ;

CREATE TABLE channellog (
	ts INT4,
	channelID INT4 CONSTRAINT channel_log_ref REFERENCES channels ( id ),
	event INT2 DEFAULT '0',
	-- Defines the message event type, so we can filter nice reports.
-- 1  -- EV_MISC - Uncategorised event.
-- 2  -- EV_JOIN - When someone 'JOIN's the bot.
-- 3  -- EV_PART - When someone 'PART's the bot. 
-- 4  -- EV_OPERJOIN - When an oper 'JOIN's the bot.
-- 5  -- EV_OPERPART - When an oper 'PART's the bot. 
-- 6  -- EV_FORCE - When someone FORCE's access in a channel.
-- 7  -- EV_REGISTER - When this channel is (re)registered.
-- 8  -- EV_PURGE - When this channle is purged.
-- 9  -- EV_COMMENT - Generic comments.
-- 10 -- EV_REMOVEALL - When REMOVEALL command is used.
	message TEXT,
	last_updated INT4 NOT NULL 
);

CREATE INDEX channellog_channelID_idx ON channellog(channelID);
CREATE INDEX channellog_event_idx ON channellog(event);

CREATE TABLE userlog (
	ts INT4,
	user_id INT4 CONSTRAINT user_log_ref REFERENCES users ( id ),
	event INT4 DEFAULT '0',
-- 1 -- EV_SUSPEND - Notification/Reason for suspension. 
	message TEXT,
	last_updated INT4 NOT NULL 
);

CREATE INDEX userlog_channelID_idx ON userlog(user_id);
CREATE INDEX userlog_event_idx ON userlog(event);

CREATE TABLE supporters (
	channel_id INT4 CONSTRAINT channel_supporters_ref REFERENCES channels ( id ),
	user_id INT4 CONSTRAINT users_supporters_ref REFERENCES users( id ),
	support CHAR,
-- NULL - not answered yet
-- Y - Supports this channel
-- N - Doesn't support this channel
	reason TEXT,
-- Reason for not supporting it if required.
	join_count INT4 DEFAULT '0',
-- Number of times this 'supporter' has joined the channel.
-- Field updated by CMaster to reflect channel 'traffic'.
	last_updated INT4 NOT NULL, 
	PRIMARY KEY(channel_id,user_id)
);

CREATE INDEX supporters_support_idx ON supporters(support);

CREATE TABLE pending (
	channel_id INT4 CONSTRAINT pending_channel_ref REFERENCES channels (id),
	manager_id INT4 CONSTRAINT pending_manager_ref REFERENCES users (id),
	created_ts INT4 NOT NULL,
	check_start_ts INT4 NOT NULL,
	status INT4 DEFAULT '0',
	-- Status of 'pending' channel:
	-- 0 = 'Pending Supporters Confirmation'
	-- 1 = 'Traffic Check'
	-- 2 = 'Notification'
	-- 3 = 'Completed'
	-- 4 = 'Cancelled by applicant'
	-- 8 = 'Pending Admin Review'
	-- 9 = 'Rejected'
	join_count INT4 DEFAULT '0',
	unique_join_count INT4 DEFAULT '0',
	decision_ts INT4,
	decision VARCHAR (80), 
	managername VARCHAR (80),
	reg_acknowledged CHAR DEFAULT 'N',
	comments TEXT,
	last_updated INT4 NOT NULL, 
	PRIMARY KEY(channel_id)
);

CREATE INDEX pending_status_idx ON pending(status);
CREATE INDEX pending_manager_id_idx ON pending(manager_id);

CREATE TABLE pending_traffic (
	channel_id INT4 CONSTRAINT pending_traffic_channel_ref REFERENCES channels (id),
	ip_number INT4,
	join_count INT4,
	PRIMARY KEY(channel_id, ip_number)
);

CREATE INDEX pending_traffic_channel_id_idx ON pending_traffic(channel_id);

CREATE TABLE domain (
	id SERIAL,
	domain varchar(1024) NOT NULL UNIQUE,
	flags INT2 NOT NULL DEFAULT '1',
-- Flags are exclusive and can be SELECT'd on.
-- 0x00 01 - Bad Domain.
-- 0x00 02 - Good Domain.
-- 0x00 04 - Pending Domain.
	last_updated INT4 NOT NULL, 
	PRIMARY KEY(id)
); 

CREATE INDEX domain_domain_idx ON domain(domain);

CREATE TABLE deletion_transactions (
	tableID INT4,
-- Table Types:
-- 1 = users
-- 2 = channels
-- 3 = levels
-- 4 = bans
	key1 INT4,
	key2 INT4,
	key3 INT4,
-- Up to 3 key's that uniquely identify the data
-- being deleted in this table. See CMaster source
-- to determine how this is interpreted.
	last_updated INT4 NOT NULL
);

-- Table to deal with the whole no-reg schema.
-- We use username and channelname instead of id's because these records may 
-- exist past the lifetime of particular user accounts, and we'll want
-- to make sure certain email address's remain unable to register, etc.
-- Specific flags are INT4's becuase postgres does not want to index on anything
-- smaller :/

CREATE TABLE noreg (
	id SERIAL,
	user_name TEXT,
	email TEXT,
	channel_name TEXT,
	type INT4 NOT NULL,
	-- 1 - Non-support registered against this channel/manager application.
	-- 2 - Abuse.
	-- 3 - Elective.
	never_reg INT4 NOT NULL DEFAULT '0',
	-- Never, ever register this channel, or user or pair.
	for_review INT4 NOT NULL DEFAULT '0',
	-- Don't automatically expire this, post for review.
	expire_time INT4,
	created_ts INT4,
 	set_by TEXT,
	reason TEXT
);

CREATE INDEX noreg_user_name_idx ON noreg (lower(user_name));
CREATE INDEX noreg_email_idx ON noreg (lower(email));
CREATE INDEX noreg_channel_name_idx ON noreg (lower(channel_name));
CREATE INDEX noreg_expire_time_idx ON noreg (expire_time);

-- Update notification rules.
-- (N.B: Disabled, aparently conditional RULES are no longer
-- supported in postgres 7.1.x).

--CREATE RULE cm1 AS ON UPDATE TO channels DO NOTIFY channels_u; 
--CREATE RULE cm2 AS ON UPDATE TO bans DO NOTIFY bans_u; 
--CREATE RULE cm3 AS ON UPDATE TO users DO NOTIFY users_u; 
--CREATE RULE cm4 AS ON UPDATE TO levels DO NOTIFY levels_u; 
 
CREATE FUNCTION update_users() RETURNS OPAQUE AS '
BEGIN
	NOTIFY users_u;
	RETURN NEW;
END;
' LANGUAGE 'plpgsql'; 

CREATE FUNCTION update_channels() RETURNS OPAQUE AS '
BEGIN
	NOTIFY channels_u;
	RETURN NEW;
END;
' LANGUAGE 'plpgsql';

CREATE FUNCTION update_levels() RETURNS OPAQUE AS '
BEGIN
	NOTIFY levels_u;
	RETURN NEW;
END;
' LANGUAGE 'plpgsql';

CREATE FUNCTION update_bans() RETURNS OPAQUE AS '
BEGIN 
	NOTIFY bans_u;
	RETURN NEW;
END;
' LANGUAGE 'plpgsql';

CREATE TRIGGER t_update_users AFTER UPDATE ON users FOR EACH ROW EXECUTE PROCEDURE update_users();
CREATE TRIGGER t_update_bans AFTER UPDATE ON bans FOR EACH ROW EXECUTE PROCEDURE update_bans();
CREATE TRIGGER t_update_channels AFTER UPDATE ON channels FOR EACH ROW EXECUTE PROCEDURE update_channels();
CREATE TRIGGER t_update_levels AFTER UPDATE ON levels FOR EACH ROW EXECUTE PROCEDURE update_levels();
 
-- Function to create a new users_lastseen record for each new user added.
-- If the function fails, you may need to add the plpgsql scripting language support
-- to your database:
-- /usr/local/pgsql/bin/createlang plpgsql dbname -L /usr/local/pgsql/lib/
 
CREATE FUNCTION new_user() RETURNS OPAQUE AS '
-- creates the users associated last_seen record
BEGIN
	INSERT INTO users_lastseen (user_id, last_seen, last_updated) VALUES(NEW.id, now()::abstime::int4,  now()::abstime::int4);
	RETURN NEW;
END;
' LANGUAGE 'plpgsql';

-- Trigger to call the function upon insert to users.

CREATE TRIGGER t_new_user AFTER INSERT ON users FOR EACH ROW EXECUTE PROCEDURE new_user(); 

-- Functions to automatically generate "Deletion Stubs" for removed records, so CMaster
-- can pick up on these and clear its cache.

CREATE FUNCTION delete_user() RETURNS OPAQUE AS '
BEGIN
	INSERT INTO deletion_transactions (tableID, key1, key2, key3, last_updated)
	VALUES(1, OLD.id, 0, 0, now()::abstime::int4);
	RETURN OLD;
END;
' LANGUAGE 'plpgsql';

CREATE TRIGGER t_delete_user AFTER DELETE ON users FOR EACH ROW EXECUTE PROCEDURE delete_user();

-- Channel table Deletion Stubs
--

CREATE FUNCTION delete_channel() RETURNS OPAQUE AS '
BEGIN
	INSERT INTO deletion_transactions (tableID, key1, key2, key3, last_updated)
	VALUES(2, OLD.id, 0, 0, now()::abstime::int4);
	RETURN OLD;
END;
' LANGUAGE 'plpgsql';

CREATE TRIGGER t_delete_channel AFTER DELETE ON channels FOR EACH ROW EXECUTE PROCEDURE delete_channel(); 

-- Level table Deletion Stubs
--

CREATE FUNCTION delete_level() RETURNS OPAQUE AS '
BEGIN
	INSERT INTO deletion_transactions (tableID, key1, key2, key3, last_updated)
	VALUES(3, OLD.channel_id, OLD.user_id, 0, now()::abstime::int4);
	RETURN OLD;
END;
' LANGUAGE 'plpgsql';

CREATE TRIGGER t_delete_level AFTER DELETE ON levels FOR EACH ROW EXECUTE PROCEDURE delete_level();

-- Ban table Deletion Stubs
--

CREATE FUNCTION delete_ban() RETURNS OPAQUE AS '
BEGIN
	INSERT INTO deletion_transactions (tableID, key1, key2, key3, last_updated)
	VALUES(4, OLD.id, 0, 0, now()::abstime::int4);
	RETURN OLD;
END;
' LANGUAGE 'plpgsql';

CREATE TRIGGER t_delete_ban AFTER DELETE ON bans FOR EACH ROW EXECUTE PROCEDURE delete_ban();


-----------------------------------------------------------------------------------------

