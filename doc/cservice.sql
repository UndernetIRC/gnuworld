------------------------------------------------------------------------------------
-- "$Id: cservice.sql,v 1.87 2008/11/12 20:45:42 mrbean_ Exp $"
-- Channel service DB SQL file for PostgreSQL.

-- ChangeLog:
-- 2025-04-01: Empus
--             Added ident column to user_sec_history table
--             Added deleted column to user_sec_history table
--             Added materialized views functions to track linked users by IP and ident
--             Added indexes on user_sec_history table to improve performance
--             Added get_linked_users function to get the linked users for a given username
-- 2024-08-05: MrIron
--             Added columns for the JOINLIM feature written by Telac.
-- 2017-01-24:Empus
--             Added table 'pending_chanfix_scores' to integrate 
--				channel applications with mod.openchanfix
-- 2014-08-11: Seven
--             Added table 'user_sec_history' to track user login history
-- 2013-12-07: Seven
--             Changed 'pending_traffic' table 'ip_number' column type to inet
--             Changed 'whitelist' table 'IP' column type to inet
--             Updated table structure 'ip_restrict';
-- 2012-05-25: MrBean
--	       Added 'totp_key' colum  to 'users' table	
-- 2011-12-12: Spike
--	       Added gline and whitelist tables.
-- 2006-08-10: nighty
--             Modified flags definitions for table channels, according to actual
--             truth, thus adding a previously undocumented flag : 0x200 for FLOATLIM Active.
-- 2006-07-17: nighty
--             Added description for MIA Review Tag
-- 2005-11-17: nighty
--             Moved table 'ip_restrict' from cservice.web.sql to cservice.sql
--             Added indexes to table 'adminlog'
--             Added a new table for X to process notices to info channel from the website
-- 2002-03-09: nighty
--             Added 'maxlogins' column in 'users' table.
-- 2002-03-07: nighty
--             Updated channel_log/user_log events types.
-- 2002-02-16: nighty
--             Added one field to 'users' for TimeZone.
-- 2001-12-29: nighty
--             Added two fields to 'channels' to reflect changes on FLOATING LIMIT in mod.cservice
-- 2001-12-06: nighty
--             Added two fields to 'users' table to take care of an abuse hunting module.
-- 2001-10-14: nighty
--             Corrected bogus table entries and added missing fields / tables.
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
	last_updated INT4 NOT NULL,
	deleted INT2 DEFAULT '0',
	PRIMARY KEY(id)
);

-- Translations for multi-lingual support.
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
-- 0x0000 0100 - Channel Locked.
-- 0x0000 0200 - FLOATLIM Active
-- 0x0000 0400 - MIA Review Tag

-- 0x0001 0000 - AlwaysOp
-- 0x0002 0000 - StrictOp
-- 0x0004 0000 - NoOp
-- 0x0008 0000 - AutoTopic
-- 0x0010 0000 - OpOnly (Depricated).
-- 0x0020 0000 - AutoJoin

	-- Do we want to keep either or both of these?
	-- nb: removed nickflood pro.  not useful.
	mass_deop_pro INT2 NOT NULL DEFAULT 3,
	flood_pro INT4 NOT NULL DEFAULT '0',
	url VARCHAR (128),
	description VARCHAR (300),
	-- Any administrative comments that apply globally to this
	-- channel.
	comment VARCHAR (300),
	keywords VARCHAR(300),
	registered_ts INT4,
	channel_ts INT4 NOT NULL,
	channel_mode VARCHAR(26),

-- USERFLAGS: Defaults new access records to:
-- 0: No Default
-- 1: AutoOP
-- 2: AutoVOICE

	userflags INT2 DEFAULT '0',

	limit_offset INT4 DEFAULT '3',
	limit_period INT4 DEFAULT '20',
	limit_grace INT4 DEFAULT '1',
	limit_max INT4 DEFAULT '0',
	no_take INT4 DEFAULT '0',

	last_updated INT4 NOT NULL,
	deleted INT2 DEFAULT '0',
-- max_bans: override global max_bans setting
--   if set to 0, use global setting - there is NO unlimited option.
	max_bans INT4 DEFAULT '0',

	welcome VARCHAR(300) DEFAULT '',

	limit_joinmax INT4 DEFAULT '3',
	limit_joinsecs INT4 DEFAULT '1',
	limit_joinperiod INT4 DEFAULT '180',
	limit_joinmode VARCHAR(255) DEFAULT '+rb *!~*@*',
	PRIMARY KEY (id)
);

-- A channel is inactive if the manager hasn't logged in for 21 days
CREATE UNIQUE INDEX channels_name_idx ON channels(LOWER(name));

-- Table for bans; channel_id references the channel entry this ban belongs to.
CREATE TABLE bans (

	id SERIAL,
	channel_id INT4 CONSTRAINT bans_channel_id_ref REFERENCES channels (id),
	banmask VARCHAR (128) NOT NULL,
	set_by VARCHAR (128),			-- nick!user@host
	set_ts INT4,
	level INT2,
	expires INT4,					-- Expiration timestamp.
	reason VARCHAR (300),
	last_updated INT4 NOT NULL,
	deleted INT2 DEFAULT '0',

	PRIMARY KEY (banmask,channel_id)
);

CREATE INDEX bans_expires_idx ON bans(expires);
CREATE INDEX bans_channelkey_idx ON bans(channel_id);

-- Access entries; admin access kept on channel '*'.
CREATE TABLE users (

	id SERIAL,
	user_name TEXT NOT NULL,
	password VARCHAR (40) NOT NULL,
	email TEXT,
	url  VARCHAR(128),
-- Which question the user provided the answer too from the signup page.
	question_id INT2,
-- The answer to the question. 42?
	verificationdata VARCHAR(30),
	language_id INT4 CONSTRAINT language_channel_id_ref REFERENCES languages (id),
	public_key TEXT,
	post_forms int4 DEFAULT 0 NOT NULL,
	flags INT2 NOT NULL DEFAULT '0',
-- 0x00 01 -- Suspended globally.
-- 0x00 02 -- Logged in (Depricated).
-- 0x00 04 -- Invisible.
-- 0x00 08 -- Fraud username.
-- 0x00 10 -- "No-Notes" - We don't want to be sent notes.
-- 0x00 20 -- "No Purge" - Don't remove the username for being idle.
-- 0x00 40 -- "No Admin" - Any * authentication is disabled (verify/commands) (its the DISABLEAUTH ON/OFF setting online)
-- 0x00 80 -- "Alumni" - Any * privileged is gone, *excepted* the verify; more like an honorary position
-- 0x01 00 -- IRC Operator - User is flagged as being 'supposed' to be an official operator, as for allowing them
	   --                special accesses such as posting complaints even when system is closed to public.
-- 0x02 00 -- "No ADDUSER" - prevent anyone adding username to channels (user-set flag, default disabled)
	last_updated_by VARCHAR (128),		-- nick!user@host
	last_updated INT4 NOT NULL,
	deleted INT2 DEFAULT '0',
	tz_setting VARCHAR(255) DEFAULT '',
	signup_cookie VARCHAR(255) DEFAULT '',
	signup_ts INT4,
	signup_ip VARCHAR(15),
	maxlogins INT4 DEFAULT 1,
	totp_key  VARCHAR(60) DEFAULT '',
	PRIMARY KEY ( id )
) ;

CREATE INDEX users_username_idx ON users( lower(user_name) );
CREATE INDEX users_email_idx ON users( lower(email) );
CREATE INDEX users_signup_ts_idx ON users( signup_ts );
CREATE INDEX users_signup_ip_idx ON users( signup_ip );

-- This table used to store the "Last Seen" informatation previously
-- routinely updated in the users table.
CREATE TABLE users_lastseen (
	user_id INT4 CONSTRAINT lastseen_users_id_ref REFERENCES users ( id ),
	last_seen INT4,
	last_hostmask VARCHAR( 256 ),
	last_ip VARCHAR( 256 ),
	last_updated INT4 NOT NULL,
	PRIMARY KEY (user_id)
);

-- Create a table to track user login history
CREATE TABLE user_sec_history (
	user_id INT4 NOT NULL,
	user_name TEXT NOT NULL,
	command TEXT NOT NULL,
	ip VARCHAR( 256 ) NOT NULL,
	ident TEXT NOT NULL,
	hostmask VARCHAR( 256 ) NOT NULL,
	timestamp INT4 NOT NULL,
	deleted TEXT NOT NULL DEFAULT 'N'
);

CREATE INDEX idx_user_sec_history_user_id ON user_sec_history(user_id);
CREATE INDEX idx_user_sec_history_hostmask ON user_sec_history(hostmask);
CREATE INDEX idx_user_sec_history_deleted ON user_sec_history(deleted);
CREATE INDEX idx_user_sec_history_ip_hostmask ON user_sec_history(ip, hostmask);
CREATE INDEX idx_ip_ident_username ON user_sec_history(ip, ident, user_name);

-- Create a materialized view to track the number of unique users per IP and ident
CREATE MATERIALIZED VIEW multiusers_ip_ident AS
SELECT 
  ip,
  ident,
  COUNT(*) AS user_count,
  array_agg(user_name ORDER BY user_name) AS user_names
FROM (
  SELECT DISTINCT ip, ident, user_name
  FROM user_sec_history
  WHERE deleted = 'N'
) AS unique_rows
GROUP BY ip, ident
HAVING COUNT(*) > 3;

CREATE INDEX idx_multiusers_ip_ident_usercount ON multiusers_ip_ident(user_count DESC);
-- REFRESH MATERIALIZED VIEW multiusers_ip_ident;

-- Create a materialized view to correlate linked users by IP and ident, with a linked_count and list of users
CREATE MATERIALIZED VIEW multiusers_linked AS
WITH user_fingerprints AS (
  SELECT DISTINCT ip, ident, user_name
  FROM user_sec_history
  WHERE ident IS NOT NULL AND deleted = 'N'
),
linked_pairs AS (
  SELECT DISTINCT a.user_name AS user_name, b.user_name AS linked_user
  FROM user_fingerprints a
  JOIN user_fingerprints b
    ON a.ip = b.ip AND a.ident = b.ident
   AND a.user_name <> b.user_name
)
SELECT
  user_name,
  COUNT(*) AS user_count,
  array_agg(linked_user ORDER BY linked_user) AS linked_usernames
FROM linked_pairs
GROUP BY user_name
HAVING COUNT(*) > 0
ORDER BY user_count DESC;

CREATE INDEX idx_multiusers_linked_username ON multiusers_linked(user_name);
CREATE INDEX idx_multiusers_linked_usercount ON multiusers_linked(user_count DESC);
-- REFRESH MATERIALIZED VIEW multiusers_linked;

-- Create a function to get the linked users for a given username
CREATE OR REPLACE FUNCTION get_linked_users(user_id INTEGER)
RETURNS TABLE (
  total_usernames INTEGER,
  all_usernames TEXT[]
)
AS $$
DECLARE
  uname TEXT;
BEGIN
  -- Get the most recent user_name for this user_id (in case of renames)
  SELECT ush.user_name INTO uname
  FROM user_sec_history ush
  WHERE ush.user_id = get_linked_users.user_id AND deleted = 'N'
  ORDER BY timestamp DESC
  LIMIT 1;

  IF uname IS NULL THEN
    RETURN;
  END IF;

  -- Recursively get all linked usernames
  RETURN QUERY
  WITH RECURSIVE link_graph(user_name) AS (
    SELECT user_name
    FROM multiusers_linked
    WHERE user_name = uname

    UNION

    SELECT unnest(linked_usernames)
    FROM multiusers_linked
    JOIN link_graph ON multiusers_linked.user_name = link_graph.user_name
  )
  SELECT
    COUNT(DISTINCT user_name)::INTEGER,
    array_agg(DISTINCT user_name ORDER BY user_name)
  FROM link_graph
  WHERE user_name <> uname;
END;
$$ LANGUAGE plpgsql STABLE;




-- Channel access table
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
	suspend_reason VARCHAR( 300 ),
	added INT4,
	added_By VARCHAR( 128 ),
	last_Modif INT4,
	last_Modif_By VARCHAR( 128 ),
	last_Updated INT4 NOT NULL,
	deleted INT2 DEFAULT '0',

	PRIMARY KEY( channel_id, user_id )
);

CREATE INDEX levels_access_idx ON levels( access ) ;
CREATE INDEX levels_userid_idx ON levels( user_id ) ;

-- Note: The below index is a new postgres 7.2 feature which vastly speeds up the
-- checking of expired suspension levels. If you still use <7.2, stick to the index
-- commented out below.
-- CREATE INDEX levels_suspendexpires_idx ON levels( suspend_expires ) ;
CREATE INDEX levels_suspendexpires_idx ON levels( suspend_expires ) WHERE suspend_expires <> 0;

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
-- 11 -- EV_IDLE - When a channel is idle for > 48 hours.
-- 12 -- EV_MGRCHANGE - When a channel switched managers either temporarily or permanently
-- 13 -- EV_ADMREJECT - When an application gets manually rejected by an admin on the web
-- 14 -- EV_WITHDRAW - When an application gets cancelled by its applicant on the web
-- 15 -- EV_NEWAPP - When a new application is posted by a user on the web
-- 16 -- EV_NONSUPPORT - When an application gets rejected due to NON-SUPPORT from one of the supporters on the web
-- 17 -- EV_ADMREVIEW - When an admins "Review" an application
-- 18 -- EV_CLRREVIEW - When a Reviewed application get its "Reviewed" flag cleared by an admin.
-- 19 -- EV_SUSPEND - When a channel is suspended by an administrator (channel.flags & 0x10 is 'set')
-- 20 -- EV_UNSUSPEND - When a channel is unsuspended by an administrator (channels.flags & 0x10 is 'unset')
	message TEXT,
	last_updated INT4 NOT NULL,
	deleted INT2 DEFAULT '0'
);

CREATE INDEX channellog_channelID_idx ON channellog(channelID);
CREATE INDEX channellog_event_idx ON channellog(event);

CREATE TABLE userlog (
	ts INT4,
	user_id INT4 CONSTRAINT user_log_ref REFERENCES users ( id ),
	event INT4 DEFAULT '0',
-- 1 -- EV_SUSPEND - Notification/Reason for suspension.
-- 2 -- EV_UNSUSPEND - Notification of an unsuspend.
-- 3 -- EV_MODIF - Modification of user record by an admin.
-- 4 -- EV_MISC - Uncategorised event.
-- 5 -- EV_COMMENT - Admin comment on username.
-- 6 -- EV_MGRCHANGE - When a user status is to swith manager with another user (also logged) on a channel
-- 7 -- EV_MAILCHANGE - When a user's email gets changed
-- 8 -- EV_PWRESET - When a user's verif q/a get changed
-- 9 -- EV_FPASSWD - When a user uses "Forgotten Password"
-- 10-- EV_PWCHANGE - When a user changes his password on the web (New Password)
-- 11-- EV_POSTCOMPLAINT - When a user identified posts a complaint on the web
-- 12-- EV_POSTCLOSE - When a Ticket/Complaint is closed or resolved.
	message TEXT,
	last_updated INT4 NOT NULL
);

CREATE INDEX userlog_channelID_idx ON userlog(user_id);
CREATE INDEX userlog_event_idx ON userlog(event);

CREATE TABLE supporters (
	channel_id INT4 CONSTRAINT channel_supporters_ref REFERENCES channels ( id ),
	user_id INT4 CONSTRAINT users_supporters_ref REFERENCES users( id ),
	support CHAR DEFAULT '?',
-- ? - Not answered yet.
-- Y - Supports this channel.
-- N - Doesn't support this channel.
	noticed CHAR NOT NULL DEFAULT 'N',
	reason TEXT,
-- Reason for not supporting it if required.
	join_count INT4 DEFAULT '0',
-- Number of times this 'supporter' has joined the channel.
-- Field updated by CMaster to reflect channel 'traffic'.
	last_updated INT4 NOT NULL,
	deleted INT2 DEFAULT '0',

	PRIMARY KEY(channel_id,user_id)
);

CREATE INDEX supporters_support_idx ON supporters(support);
CREATE INDEX supporters_user_id_idx ON supporters(user_id);

-- Pending channel applications table
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
	decision TEXT,
	managername VARCHAR (80),
	reg_acknowledged CHAR DEFAULT 'N',
	comments TEXT,
	last_updated INT4 NOT NULL,
	description TEXT,
	reviewed CHAR NOT NULL DEFAULT 'N',
	first_init CHAR NOT NULL DEFAULT 'N',
	reviewed_by_id INT4 CONSTRAINT pending_review_ref REFERENCES users (id),
	PRIMARY KEY(channel_id)
);

CREATE INDEX pending_status_idx ON pending(status);
CREATE INDEX pending_manager_id_idx ON pending(manager_id);

-- Traffic checking during channel applications
CREATE TABLE pending_traffic (
	channel_id INT4 CONSTRAINT pending_traffic_channel_ref REFERENCES channels (id),
	ip_number inet,
	join_count INT4,
	PRIMARY KEY(channel_id, ip_number)
);

CREATE INDEX pending_traffic_channel_id_idx ON pending_traffic(channel_id);

-- Chanfix scores during channel applications
CREATE TABLE pending_chanfix_scores (
	channel_id INT4 CONSTRAINT pending_chanfix_scores_channel_ref REFERENCES channels (id),
	user_id TEXT NOT NULL DEFAULT '0',
	rank INT4 NOT NULL DEFAULT '0',
	score INT4 NOT NULL DEFAULT '0',
	account VARCHAR(20) NOT NULL,
	first_opped VARCHAR(10),
	last_opped VARCHAR(20),
	last_updated INT4 NOT NULL DEFAULT date_part('epoch', CURRENT_TIMESTAMP)::int,
	first CHAR NOT NULL DEFAULT 'Y'
);

CREATE INDEX pending_chanfix_scores_channel_id_idx ON pending_chanfix_scores(channel_id);


CREATE TABLE domain (
	id SERIAL,
	domain varchar(1024) NOT NULL UNIQUE,
	flags INT2 NOT NULL DEFAULT '1',
-- Flags are exclusive and can be SELECT'd on.
-- 0x00 01 - Bad Domain.
-- 0x00 02 - Good Domain.
-- 0x00 04 - Pending Domain.
	last_updated INT4 NOT NULL,
	deleted INT2 DEFAULT '0',

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
	-- 4 - Fraud Username.
	-- 5 - Username pattern'd lock (able to lock any new username from matching *mp3* for example)
	-- 6 - Verification answer lock (stored in 'user_name', planning on db fields renaming for more customisation later)
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

-- User notes table
CREATE TABLE notes (
	message_id SERIAL,
	user_id INT4 CONSTRAINT users_notes_ref REFERENCES users( id ),
	from_user_id INT4 CONSTRAINT users_notes_ref2 REFERENCES users( id ),
	message VARCHAR( 300 ),
	last_updated INT4 NOT NULL,
	PRIMARY KEY(message_id, user_id)
);


CREATE TABLE notices (
	message_id SERIAL,
	user_id INT4 CONSTRAINT users_notes_ref REFERENCES users( id ),
	message VARCHAR( 300 ),
	last_updated INT4 NOT NULL,

	PRIMARY KEY(message_id, user_id)
);

--CREATE TABLE mailq (
--	user_id INT4 CONSTRAINT mailq_users_ref REFERENCES users(id),
--	channel_id INT4 CONSTRAINT mailq_channels_ref REFERENCES channels(id),
--	created_ts INT4,
--	template INT4,
--	var1 CHAR(128),
--	var2 CHAR(128),
--	var3 CHAR(128),
--	var4 CHAR(128),
--	var5 CHAR(128)
--);

-- Values of Templates:
-- MT_SUPPORTER		1
-- MT_REJECTED		2
-- MT_REGISTERED	3

-- Template arguments:
-- MT_REJECTED
-- var1		Reject Reason

-- Update notification rules.
-- (N.B: Disabled, aparently conditional RULES are no longer
-- supported in postgres 7.1.x).

--CREATE RULE cm1 AS ON UPDATE TO channels DO NOTIFY channels_u;
--CREATE RULE cm2 AS ON UPDATE TO bans DO NOTIFY bans_u;
--CREATE RULE cm3 AS ON UPDATE TO users DO NOTIFY users_u;
--CREATE RULE cm4 AS ON UPDATE TO levels DO NOTIFY levels_u;

CREATE FUNCTION update_users() RETURNS TRIGGER AS '
BEGIN
	NOTIFY users_u;
	RETURN NEW;
END;
' LANGUAGE 'plpgsql';

CREATE FUNCTION update_channels() RETURNS TRIGGER AS '
BEGIN
	NOTIFY channels_u;
	RETURN NEW;
END;
' LANGUAGE 'plpgsql';

CREATE FUNCTION update_levels() RETURNS TRIGGER AS '
BEGIN
	NOTIFY levels_u;
	RETURN NEW;
END;
' LANGUAGE 'plpgsql';

CREATE FUNCTION update_bans() RETURNS TRIGGER AS '
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

CREATE FUNCTION new_user() RETURNS TRIGGER AS '
-- creates the users associated last_seen record
BEGIN
	INSERT INTO users_lastseen (user_id, last_seen, last_updated) VALUES(NEW.id, extract(epoch FROM now())::int, extract(epoch FROM now())::int);
	RETURN NEW;
END;
' LANGUAGE 'plpgsql';

-- Trigger to call the function upon insert to users.

CREATE TRIGGER t_new_user AFTER INSERT ON users FOR EACH ROW EXECUTE PROCEDURE new_user();

-- Functions to automatically generate "Deletion Stubs" for removed records, so CMaster
-- can pick up on these and clear its cache.

CREATE FUNCTION delete_user() RETURNS TRIGGER AS '
BEGIN
	INSERT INTO deletion_transactions (tableID, key1, key2, key3, last_updated)
	VALUES(1, OLD.id, 0, 0, extract(epoch FROM now())::int);
	RETURN OLD;
END;
' LANGUAGE 'plpgsql';

CREATE TRIGGER t_delete_user AFTER DELETE ON users FOR EACH ROW EXECUTE PROCEDURE delete_user();

-- Channel table Deletion Stubs
--

CREATE FUNCTION delete_channel() RETURNS TRIGGER AS '
BEGIN
	INSERT INTO deletion_transactions (tableID, key1, key2, key3, last_updated)
	VALUES(2, OLD.id, 0, 0, extract(epoch FROM now())::int);
	RETURN OLD;
END;
' LANGUAGE 'plpgsql';

CREATE TRIGGER t_delete_channel AFTER DELETE ON channels FOR EACH ROW EXECUTE PROCEDURE delete_channel();

-- Level table Deletion Stubs
--

CREATE FUNCTION delete_level() RETURNS TRIGGER AS '
BEGIN
	INSERT INTO deletion_transactions (tableID, key1, key2, key3, last_updated)
	VALUES(3, OLD.channel_id, OLD.user_id, 0, extract(epoch FROM now())::int);
	RETURN OLD;
END;
' LANGUAGE 'plpgsql';

CREATE TRIGGER t_delete_level AFTER DELETE ON levels FOR EACH ROW EXECUTE PROCEDURE delete_level();

-- Ban table Deletion Stubs
--

CREATE FUNCTION delete_ban() RETURNS TRIGGER AS '
BEGIN
	INSERT INTO deletion_transactions (tableID, key1, key2, key3, last_updated)
	VALUES(4, OLD.id, 0, 0, extract(epoch FROM now())::int);
	RETURN OLD;
END;
' LANGUAGE 'plpgsql';

CREATE TRIGGER t_delete_ban AFTER DELETE ON bans FOR EACH ROW EXECUTE PROCEDURE delete_ban();

-- Table used to store run-time configurable settings.
CREATE TABLE variables (
	var_name VARCHAR(30),
	contents text,
	hint text,
	last_updated INT4,
	PRIMARY KEY(var_name)
);

-- Table used to store the admin log (converted from file to db).
CREATE TABLE adminlog (
	id SERIAL,
	user_id INT4 NOT NULL,
	cmd VARCHAR(100),
	args VARCHAR(255),
	timestamp INT4 NOT NULL,
	issue_by VARCHAR(255),
	PRIMARY KEY(id)
);

CREATE INDEX adminlog_c_idx ON adminlog(cmd,timestamp);
CREATE INDEX adminlog_u_idx ON adminlog(user_id,timestamp);
CREATE INDEX adminlog_a_idx ON adminlog(args);
CREATE INDEX adminlog_i_idx ON adminlog(issue_by);

-- IPR entries table
CREATE TABLE ip_restrict (
	id		SERIAL,
	user_id		int4 NOT NULL,
	added		int4 NOT NULL,
	added_by	int4 NOT NULL,
	type		int4 NOT NULL DEFAULT 0,
	value		inet NOT NULL,
	last_updated	int4 NOT NULL DEFAULT date_part('epoch', CURRENT_TIMESTAMP)::int,
	last_used	int4 NOT NULL DEFAULT 0,
	expiry		int4 NOT NULL,
	description	VARCHAR(255)
);

CREATE INDEX ip_restrict_idx ON ip_restrict(user_id,type);

-- Table to store messages from website for channel relay
CREATE TABLE webnotices (
	id 	SERIAL,
	created_ts 	int4 NOT NULL,
	contents 	VARCHAR(255) NOT NULL,
	PRIMARY KEY(id)
);

-- Network glines table
CREATE TABLE glines (
        Id SERIAL,
        Host VARCHAR(128) UNIQUE NOT NULL,
        AddedBy VARCHAR(128) NOT NULL,
        AddedOn INT4 NOT NULL,
        ExpiresAt INT4 NOT NULL,
        LastUpdated INT4 NOT NULL DEFAULT date_part('epoch', CURRENT_TIMESTAMP)::int,
        Reason VARCHAR(255)
);

-- Whitelist entry table to be read by website
CREATE TABLE whitelist (
        Id SERIAL,
        IP inet UNIQUE NOT NULL,
        AddedBy VARCHAR(128) NOT NULL,
        AddedOn INT4 NOT NULL,
        ExpiresAt INT4 NOT NULL,
        Reason VARCHAR(255)
);


-----------------------------------------------------------------------------------------
-- CService Website Related Tables
-- TODO: Determine which are required by X even without the website.

CREATE TABLE acl (
	acl_id SERIAL,
	user_id INT4 NOT NULL,
	isstaff INT2 NOT NULL,
	flags INT4 DEFAULT '0' NOT NULL,
-- 0x0001 - ACL_XCHGMGR_REVIEW
-- 0x0002 - ACL_XCHGMGR_ADMIN
-- 0x0004 - ACL_XMAILCH_REVIEW
-- 0x0008 - ACL_XMAILCH_ADMIN
-- 0x0016 - ACL_XHELP
-- 0x0032 - ACL_XHELP_CAN_ADD
-- 0x0064 - ACL_XHELP_CAN_EDIT
-- 0x0128 - ACL_WEBAXS_2
-- 0x0256 - ACL_WEBAXS_3
-- .. to be completed in the future ..
	xtra INT4 NOT NULL,
-- may vary, for example if 0x0016 is set in 'flags',
-- 'xtra' will contain the 'language_id' the user have power over (or '0' for *all*)
--
	last_updated INT4 NOT NULL,
	last_updated_by INT4 NOT NULL,
	suspend_expire INT4 DEFAULT '0' NOT NULL,
	suspend_by INT4 DEFAULT '0' NOT NULL,
	deleted INT2 DEFAULT '0' NOT NULL
);


CREATE TABLE fraud_lists (
	id SERIAL,
	name VARCHAR(255) NOT NULL
);

CREATE TABLE fraud_list_data (
	list_id INT4 NOT NULL,
	user_id INT4 REFERENCES users(id) NOT NULL
);


CREATE TABLE pending_pwreset (
        cookie VARCHAR(128) NOT NULL,
        user_id INT4 NOT NULL,
        question_id INT2 NOT NULL,
        verificationdata VARCHAR(30) NOT NULL,
        expiration INT4 NOT NULL
);

CREATE INDEX pending_pwreset_cookie_idx ON pending_pwreset(cookie);
CREATE INDEX pending_pwreset_user_id_idx ON pending_pwreset(user_id);
CREATE INDEX pending_pwreset_expiration_idx ON pending_pwreset(expiration);

CREATE TABLE locks (
	section INT2,
-- section: 1 : GLOBAL SITE LOCK
-- section: 2 : NEW REGISTRATIONS LOCK
-- section: 3 : NEW USERS LOCK
	since INT4,
	by INT4
);

CREATE TABLE counts (
-- OBSOLETE !!!
	count_type INT2,
-- type: 1 : NEW USERS
	count_count INT4
);

CREATE TABLE statistics (
	users_id	INT4 NOT NULL,
	stats_type	INT4 NOT NULL,
--	1	Total reviewed applications count
--	...	more capabilities
	stats_value_int	INT4 DEFAULT 0 NOT NULL,
	stats_value_chr	VARCHAR(255) DEFAULT '' NOT NULL,
	last_updated	INT4 NOT NULL
);

CREATE INDEX statistics_users_id_idx ON statistics(users_id);

--CREATE TABLE helpmgr_users (
--	user_id INT4 NOT NULL,
--	language_id INT2,
--	flags INT2 DEFAULT '1'
-- CAN_EDIT	: 0x01
-- CAN_ADD	: 0x02
--);

--CREATE INDEX helpmgr_users_user_id_idx ON helpmgr_users(user_id);
--CREATE INDEX helpmgr_language_id_idx ON helpmgr_users(language_id);
--CREATE INDEX helpmgr_flags_idx ON helpmgr_users(flags);

CREATE TABLE pending_mgrchange (
	id SERIAL,
	channel_id INT4 NOT NULL,
	manager_id INT4 NOT NULL,
	new_manager_id INT4 NOT NULL,
	change_type INT2,
-- change_type : 0 : temporary
-- change_type : 1 : permanent
	opt_duration INT4,
-- duration in seconds if temporary, 0 if permanent.
	reason TEXT,
	expiration INT4 DEFAULT 0,
	crc VARCHAR(128),
	confirmed INT2 DEFAULT 0,
	from_host VARCHAR(15) DEFAULT '0.0.0.0'
);

CREATE INDEX pending_mgrchange_id_idx ON pending_mgrchange(id);
CREATE INDEX pending_mgrchange_channel_id_idx ON pending_mgrchange(channel_id);
CREATE INDEX pending_mgrchange_manager_id_idx ON pending_mgrchange(manager_id);
CREATE INDEX pending_mgrchange_new_manager_id_idx ON pending_mgrchange(new_manager_id);
CREATE INDEX pending_mgrchange_change_type_idx ON pending_mgrchange(change_type);
CREATE INDEX pending_mgrchange_opt_duration_idx ON pending_mgrchange(opt_duration);
CREATE INDEX pending_mgrchange_expiration_idx ON pending_mgrchange(expiration);
CREATE INDEX pending_mgrchange_crc_idx ON pending_mgrchange(crc);
CREATE INDEX pending_mgrchange_confirmed_idx ON pending_mgrchange(confirmed);

--CREATE TABLE xatadmins (
--	admin_id INT4 NOT NULL,
--	admin_type INT2 DEFAULT 0
--);

--CREATE INDEX xatadmins_admin_id_idx ON xatadmins(admin_id);


CREATE TABLE pendingusers (
	user_name VARCHAR(12),
	cookie VARCHAR(32),
	email VARCHAR(255),
	expire INT4,
	question_id INT2,
	verificationdata VARCHAR(30),
	poster_ip VARCHAR(15) DEFAULT '',
	language INT4 NOT NULL
);

CREATE INDEX pendingusers_cookie_idx ON pendingusers(cookie);

CREATE TABLE pending_emailchanges (
	cookie VARCHAR(128) NOT NULL,
	user_id INT4 NOT NULL,
	old_email VARCHAR(255) NOT NULL,
	new_email VARCHAR(255) NOT NULL,
	expiration INT4 NOT NULL,
	phase INT4 NOT NULL
);

CREATE INDEX pending_emailchanges_cookie_idx ON pending_emailchanges(cookie);
CREATE INDEX pending_emailchanges_user_id_idx ON pending_emailchanges(user_id);
CREATE INDEX pending_emailchanges_expiration_idx ON pending_emailchanges(expiration);

CREATE TABLE pending_passwordchanges (
	cookie VARCHAR(128) NOT NULL,
	user_id INT4 NOT NULL,
	old_crypt VARCHAR(40) NOT NULL,
	new_crypt VARCHAR(40) NOT NULL,
	new_clrpass VARCHAR(255) NOT NULL,
	created_ts INT4 NOT NULL
);

-- This table stores the timestamp of the last request
-- from a particular IP.
-- Used to block abuse, such as requesting a password 50,000
-- times a minute.

CREATE TABLE lastrequests (
	ip VARCHAR(15) DEFAULT '0.0.0.0',
	last_request_ts INT4
);

CREATE INDEX lastrequests_ip_idx ON lastrequests(ip);

 -- list of admins that have the ability to modify NOREG entries (other admins may only list them)

--CREATE TABLE webaccessteam (
--	admin_id int4 REFERENCES users(id) NOT NULL,
--	level INT4 NOT NULL DEFAULT '0'
--);

-- recorded objections for channels.
CREATE TABLE objections (
	channel_id int4 REFERENCES channels(id) NOT NULL,
	user_id int4 REFERENCES users(id) NOT NULL,
	comment text NOT NULL,
	created_ts int4 NOT NULL,
	admin_only varchar(1) DEFAULT 'N'
-- 'Y' : the objection is an admin comment on only * users sees it.
-- 'N' : the objection is a regular one and everyone can see it.
);

CREATE TABLE timezones (
	tz_index SERIAL,
	tz_name VARCHAR(128) NOT NULL,
	tz_countrycode VARCHAR(5) NOT NULL,
	tz_acronym VARCHAR(10) NOT NULL,
	deleted INT2 DEFAULT '0',
	last_updated INT4 NOT NULL
);

CREATE TABLE complaints (
	id SERIAL,
	from_id int4 NOT NULL,
	from_email varchar (255) NOT NULL,
	inrec_email varchar (255) NOT NULL,
	complaint_type int4 NOT NULL,
	complaint_text text NOT NULL,
	complaint_logs text NOT NULL,
	complaint_channel1_id int4 NOT NULL,
	complaint_channel1_name text NOT NULL,
	complaint_channel2_id int4 NOT NULL,
	complaint_channel2_name text NOT NULL,
	complaint_users_id int4 NOT NULL,
	status int4 NOT NULL,
	nicelevel int4 NOT NULL,
	reviewed_by_id int4 NOT NULL,
	reviewed_ts int4 NOT NULL,
	created_ts int4 NOT NULL,
	created_ip varchar (15) DEFAULT '0.0.0.0' NOT NULL,
	created_crc varchar (128) NOT NULL,
	crc_expiration int4 NOT NULL,
	ticket_number varchar(32) NOT NULL,
	current_owner int4 NOT NULL,
	PRIMARY KEY (id)
);

CREATE TABLE complaints_threads (
	id SERIAL,
	complaint_ref int4 NOT NULL CONSTRAINT complaints_threads_ref REFERENCES complaints (id),
	reply_by int4 NOT NULL,
	reply_ts int4 NOT NULL,
	reply_text text NOT NULL,
	actions_text text NOT NULL,
	in_reply_to int4 NOT NULL,
	PRIMARY KEY (id)
);


CREATE TABLE complaint_types (
	-- not used for now...
	id SERIAL,
	complaint_label varchar(255) NOT NULL,
	PRIMARY KEY (id)
);

DELETE FROM complaint_types;
COPY "complaint_types" FROM stdin;
1	My username is suspended
2	Members of a registered channel are spamming my channel
3	I object to this channel application but I want to do so anonymously
4	My channel was purged and I want you to reconsider
5	My channel was purged and I want to know why
99	Other complaint
\.

CREATE TABLE complaints_reference (
	complaints_ref int4 NOT NULL CONSTRAINT complaints_reference_ref REFERENCES complaints (id),
	referenced_by int4 NOT NULL,
	referenced_to int4 NOT NULL,
	reference_ts int4 NOT NULL,
	is_new int4 DEFAULT '1' NOT NULL
);

CREATE INDEX complaints_ref_ref ON complaints_reference(complaints_ref,referenced_to);


CREATE TABLE default_msgs (
	id	SERIAL,
	type	int4 NOT NULL,
	label	varchar(255) NOT NULL,
	content	text NOT NULL
);

CREATE INDEX default_msgs_idx ON default_msgs(type);