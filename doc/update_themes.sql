--
-- $Id: update_themes.sql,v 1.1 2002/05/20 23:59:36 nighty Exp $
-- Use this file to update your db structure if you dont have "themes" enabled.
-- run /usr/local/pgsql/bin/psql local_db < update_themes.sql
-- ** run this only ONCE **
--
-- 05/21/2002 : <nighty@undernet.org>
--

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
'd0e5ff',
'',
'000000',
'aaaaaa',
'0000ff',

'aaaaaa',
'000000',
'0000ff',

-- top
'd0e5ff',
'',
'top_logo.jpg',

-- bottom (footer)
'd0e5ff',
'',
'4c4c4c',
'000033',
'0000aa',

-- main
'dddddd',
'',
'000000',
'505050',
'004400',
'0000ff',
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

'ff1111',

'00ff00',
'ff0000',
'ffffff',
'ee1166',

'00ffff',
'eeeeee',
'990000',
'007700',

'ddffdd',
'ffdddd',

-- tables
'ffffff',
'',
'003366',
'ffffff',
'006633',
'ffffff',
'ffff00',
'777777',
'ffdddd',
''

);
