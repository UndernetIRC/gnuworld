--
-- $Id: update_variables_20071227.sql,v 1.2 2008/01/09 22:42:17 kewlio Exp $
--
-- Update max_ban_duration from hours to seconds (multiply by 3600)
--

-- WARNING: run this only ONCE!

UPDATE variables SET last_updated=now()::abstime::int4,hint='Maximum ban duration (in seconds)',contents=(contents::int4*3600) WHERE var_name='MAX_BAN_DURATION';

DELETE FROM help WHERE topic='BAN' AND language_id=1;

COPY "help" FROM stdin;
BAN	1	/msg X ban <#channel> <nick|*!*user@*.host> [duration] [level] [reason]\nAdds a specific *!*user@*.host to the X banlist of your channel.\nYou may place a ban on a nick if the person is online or ban their *!*user@*.host if the person is not online. \nBans that cover a broader range than previously set bans will replace previous bans.\nAny user in the channel that matches the added ban will be kicked out if the ban level is 75+. \nThe duration can be specified in the following format: 400s, 300m, 200h, 100d (secs,mins,hours,days)\nThe ban level can range from 1 to your own level. The ban reason can be a maximum of 128 characters long. \nIf no duration or level is specified, the default duration will be 3 hours, and the level will be 75.\nBan Levels: 1-74 = Prevents the user from having ops (+o) on the channel. \nBan Levels: 75-500 = Prevents the user from being in the channel at all.
\.

