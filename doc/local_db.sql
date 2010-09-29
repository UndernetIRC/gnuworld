------------------------------------------------------------------------------------
-- "$Id: local_db.sql,v 1.19 2002/12/27 00:51:39 nighty Exp $"
-- Channel service DB SQL file for PostgreSQL.
--
-- .. if you wonder why some tables have moved and you have them here when
-- you set up your dbs, then you may need to have a look in the script 'movetables'
-- after you modified the parameters in it to fit your configuration, run it to
-- properly move your tables from the 'local' db to the 'remote' db.
-- If you use a single db for both cservice*.sql and local_db.sql then just dont run anything.
--
-- nighty <nighty@undernet.org>

CREATE TABLE webcookies (
        user_id INT4,
        cookie VARCHAR(32) UNIQUE,
        expire INT4,
	tz_setting VARCHAR(255) DEFAULT '',
	is_admin INT2 DEFAULT 0
);

CREATE INDEX webcookies_user_id_idx ON webcookies(user_id);
CREATE INDEX webcookies_cookie_idx ON webcookies(cookie);
CREATE INDEX webcookies_expire_idx ON webcookies(expire);


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

CREATE TABLE newu_ipcheck (
   ts int4 NOT NULL,
   ip varchar(15) DEFAULT '0.0.0.0' NOT NULL,
   expiration int4 NOT NULL,
   CONSTRAINT newu_ipcheck_pkeys PRIMARY KEY (ip)
);

CREATE INDEX newu_ipcheck_ts ON newu_ipcheck (ts);
CREATE INDEX newu_ipcheck_ip ON newu_ipcheck (ip);
CREATE INDEX newu_ipcheck_expiration ON newu_ipcheck (expiration);


CREATE TABLE gfxcodes (
	code	VARCHAR(25) NOT NULL,
	crc	VARCHAR(128) NOT NULL,
	expire	INT NOT NULL
);
CREATE INDEX gfxcodes_idx ON gfxcodes(code,crc,expire);


CREATE TABLE themes (
	id	SERIAL,
	name	VARCHAR(50) NOT NULL,
	tstart	VARCHAR(5) DEFAULT '01/01' NOT NULL,
	tend	VARCHAR(5) DEFAULT '12/31' NOT NULL,
	created_ts	INT4 NOT NULL,
	created_by	INT4 DEFAULT 0 NOT NULL,

	sub_dir	VARCHAR(128) NOT NULL, -- Must be a real directory name, alone, with no slashes or spaces or weird chars.
				       -- This will indicate in which [...]gnuworld/docs/website/themes/data/<sub_dir> the themes files will be found.

	left_bgcolor	VARCHAR(6) NOT NULL,
	left_bgimage	VARCHAR(255) DEFAULT '' NOT NULL,
	left_textcolor	VARCHAR(6) NOT NULL,
	left_linkcolor	VARCHAR(6) NOT NULL,
	left_linkover	VARCHAR(6) NOT NULL, -- Visable on Internet Explorer Only.

	left_loadavg0	VARCHAR(6) NOT NULL, -- LoadAvg color when status is "OK" (<5)
	left_loadavg1	VARCHAR(6) NOT NULL, -- LoadAvg color when status is "HEAVY" (>=5 && <=15)
	left_loadavg2	VARCHAR(6) NOT NULL, -- LoadAvg color when status is "CRITICAL" (>15)

	top_bgcolor	VARCHAR(6) NOT NULL,
	top_bgimage	VARCHAR(255) DEFAULT '' NOT NULL,
	top_logo	VARCHAR(255) NOT NULL,

	bottom_bgcolor	VARCHAR(6) NOT NULL,
	bottom_bgimage	VARCHAR(255) DEFAULT '' NOT NULL,
	bottom_textcolor	VARCHAR(6) NOT NULL,
	bottom_linkcolor	VARCHAR(6) NOT NULL,
	bottom_linkover	VARCHAR(6) NOT NULL, -- Visable on Internet Explorer Only.

	main_bgcolor	VARCHAR(6) NOT NULL,
	main_bgimage	VARCHAR(255) DEFAULT '' NOT NULL,
	main_textcolor	VARCHAR(6) NOT NULL,
	main_textlight	VARCHAR(6) NOT NULL,
	main_linkcolor	VARCHAR(6) NOT NULL,
	main_linkover	VARCHAR(6) NOT NULL, -- Visable on Internet Explorer Only.
	main_warnmsg	VARCHAR(6) NOT NULL, -- Color of some warning message, usually those you put in red, but depends on other colors ;P
	main_no		VARCHAR(6) NOT NULL, -- color of word 'No'.
	main_yes	VARCHAR(6) NOT NULL, -- color of word 'Yes'.

	main_appst0	VARCHAR(6) NOT NULL, -- color for application in (Incoming (status = 0)).
	main_appst1	VARCHAR(6) NOT NULL, -- color for application in (Pending (traffic check) (status = 1)).
	main_appst2	VARCHAR(6) NOT NULL, -- color for application in (Pending (notification) (status = 2)).
	main_appst3	VARCHAR(6) NOT NULL, -- color for application in (Accepted (status = 3)).
	main_appst4	VARCHAR(6) NOT NULL, -- color for application in (Cancelled (status = 4)).

	main_appst8	VARCHAR(6) NOT NULL, -- color for application in (Ready for review (status = 8)).
	main_appst9	VARCHAR(6) NOT NULL, -- color for application in (Rejected (status = 9)).

	main_vlinkcolor	VARCHAR(6) NOT NULL, -- color for links that need to be displayed as 'visited'.

	main_support	VARCHAR(6) NOT NULL, -- color for a person supporting the channel in "view_app"
	main_nonsupport	VARCHAR(6) NOT NULL, -- color for a person NOT supporting the channel in "view_app"
	main_notyet	VARCHAR(6) NOT NULL, -- color for a person that has made no choice regarding support in "view_app"

	main_frauduser	VARCHAR(6) NOT NULL, -- color for Fraud Usernames representation

	main_xat_revert	VARCHAR(6) NOT NULL,
	main_xat_goperm	VARCHAR(6) NOT NULL,
	main_xat_deny	VARCHAR(6) NOT NULL,
	main_xat_accept	VARCHAR(6) NOT NULL,

	main_acl_create	VARCHAR(6) NOT NULL,
	main_acl_edit	VARCHAR(6) NOT NULL,

	table_bgcolor	VARCHAR(6) NOT NULL,
	table_bgimage	VARCHAR(255) DEFAULT '' NOT NULL,
	table_headcolor	VARCHAR(6) NOT NULL,
	table_headtextcolor VARCHAR(6) NOT NULL,
	table_sepcolor VARCHAR(6) NOT NULL,
	table_septextcolor VARCHAR(6) NOT NULL,
	table_tr_enlighten	VARCHAR(6) NOT NULL,
	table_tr_enlighten2	VARCHAR(6) NOT NULL,
	table_tr_enlighten3	VARCHAR(6) NOT NULL,
	table_headimage	VARCHAR(255) DEFAULT '' NOT NULL,

	PRIMARY KEY (name)

);

INSERT INTO themes VALUES (
-- head
'1',
'default',
'01/01',
'12/31',
'31337',
'0',

'default',

-- left
'60659c',
'',
'000000',
'aaaaaa',
'ffffff',

'aaaaaa',
'000000',
'ffff00',

-- top
'60659c',
'',
'default_logo.jpg',

-- bottom (footer)
'60659c',
'',
'000000',
'aaaaaa',
'ffffff',

-- main
'aaafe4',
'',
'000000',
'505050',
'60659c',
'ff7700',
'ff0000',

'990000',
'009900',

-- main/regproc
'ffff00',
'0000ff',
'0000ff',
'00ff00',
'eeeeee',

'990099',
'ff0000',

'60659c',

'00ff00',
'ff0000',
'ffffff',
'ffeeff',

'00ffff',
'eeeeee',
'990000',
'007700',

'ddffdd',
'ffdddd',

-- tables
'ffffff',
'',
'60659c',
'ffffff',
'dddddd',
'4c4c4c',
'ffff00',
'777777',
'60659c',
''

);

