------------------------------------------------------------------------------------
-- "$Id: local_db.sql,v 1.10 2001/11/16 18:08:42 nighty Exp $"
-- Channel service DB SQL file for PostgreSQL.
--
--
-- nighty <nighty@undernet.org>

CREATE TABLE webcookies (
        user_id INT4,
        cookie VARCHAR(32) UNIQUE,
        expire INT4
);

CREATE INDEX webcookies_user_id_idx ON webcookies(user_id);


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

CREATE TABLE helpmgr_users (
	user_id INT4 NOT NULL,
	language_id INT2,
	flags INT2 DEFAULT '1'
-- CAN_EDIT	: 0x01
-- CAN_ADD	: 0x02
);

CREATE INDEX helpmgr_users_user_id_idx ON helpmgr_users(user_id);
CREATE INDEX helpmgr_language_id_idx ON helpmgr_users(language_id);
CREATE INDEX helpmgr_flags_idx ON helpmgr_users(flags);


--
-- IP_CHECK lock IPs after 3 attempts (failed) in wheter "login" or "forgotten password" sections.
-- to reduce the load on the db host.
--

CREATE TABLE exclusions (
   excluded varchar(15) DEFAULT '0.0.0.0' NOT NULL,
   CONSTRAINT exclusions_pkey PRIMARY KEY (excluded)
);

CREATE  INDEX exclusions_excluded_key ON exclusions (excluded);


CREATE TABLE ips (
   ipnum varchar(15) DEFAULT '0.0.0.0' NOT NULL,
   user_name varchar(20) NOT NULL,
   expiration int4 NOT NULL,
   hit_counts int4,
   set_on int4 NOT NULL,
   CONSTRAINT ips_pkey PRIMARY KEY (expiration, ipnum, user_name)
);

CREATE  INDEX hit_counts_ips_key ON ips (hit_counts);
CREATE  INDEX ips_expiration_key ON ips (expiration);
CREATE  INDEX ips_set_on_key ON ips (set_on);
CREATE  INDEX ips_ipnum_key ON ips (ipnum);
CREATE  INDEX ips_user_name_key ON ips (user_name);

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

CREATE TABLE xatadmins (
	admin_id INT4 NOT NULL,
	admin_type INT2 DEFAULT 0
);

CREATE INDEX xatadmins_admin_id_idx ON xatadmins(admin_id);


