------------------------------------------------------------------------------------
-- "$Id: cservice.web.sql,v 1.28 2003/01/01 04:43:51 gte Exp $"
-- Channel service DB SQL file for PostgreSQL.
--
-- Tables specific to webbased registration process.
--
-- Perry Lorier <perry@coders.net>
-- nighty <nighty@undernet.org> - Corrected tables and added missing fields



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
	count_type INT2,
-- type: 1 : NEW USERS
	count_count INT4
);

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
	expiration INT4 NOT NULL
);

CREATE INDEX pending_emailchanges_cookie_idx ON pending_emailchanges(cookie);
CREATE INDEX pending_emailchanges_user_id_idx ON pending_emailchanges(user_id);
CREATE INDEX pending_emailchanges_expiration_idx ON pending_emailchanges(expiration);

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

