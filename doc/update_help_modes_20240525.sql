--
-- MODE, KICK and (UN)BAN specific help topics
-- last updated by MrIron @ 2024.07.01
--

UPDATE help SET contents = E'/msg X kick <#channel> <nicks | *!*@*.host> [reason]\nMakes X kick one or more persons from your channel.\nYou can make X kick several nicknames separates by a comma. If your access level is 200 or higher you can also kick by pattern.' WHERE topic = 'KICK' and language_id = 1;

UPDATE help SET contents = E'/msg X ban <#channel> <nicks | *!*user@*.host> [duration] [level] [reason]\nAdds a specific *!*user@*.host to the X banlist of your channel.\nYou may place a ban on one or several nicks separated by comma if the persons are online or ban their *!*user@*.host if the persons are not online.\nBans that cover a broader range than previously set bans will replace previous bans. \nAny user in the channel that matches the added ban will be kicked out if the ban level is 75+. \nThe duration can be specified in the following format: 400s, 300m, 200h, 100d (secs,mins,hours,days)\nSpecifying a duration of 0 will set a permanent ban (no expiry) \nThe ban level can range from 1 to your own level. The ban reason can be a maximum of 128 characters long. \nIf no duration or level is specified, the default duration will be 3 hours, and the level will be 75.\nBan Levels: 1-74 = Prevents the user from having ops (+o) on the channel. \nBan Levels: 75-500 = Prevents the user from being in the channel at all.'  WHERE topic = 'BAN' and language_id = 1;

UPDATE help SET contents = E'/msg X unban <#channel> <nicks | *!*user@*.host>\nRemoves the ban on one or more nicks or *!*user@*.hosts separated by comma from the X banlist of your channel and any matching bans from the channel banlist also. \nYou can only remove bans from X''s banlist that are equal to or lower than your own access. \nTo search X''s banlist, refer to the LBANLIST command (level 0).\nIf more than one ban matches the mask you are trying to remove, and one is an exact match, will only remove the exact match. \nResend the command to remove the rest.'  WHERE topic = 'UNBAN' and language_id = 1;

UPDATE translations SET text = 'The ban %s is already in my banlist!' WHERE language_id=1 AND response_id=22;

INSERT INTO help VALUES ('MODE', '1', E'/msg X mode <#channel> <modes> [arguments]\nMakes X set or remove a channel mode in your channel.\nThe mode must start with either + or - to specify whether a mode shall be set or removed.\nFor channel mode l(imit) or k(ey), the limit or key must be provided.\nUse the OP, DEOP, VOICE and DEVOICE commands to give or remove ops or voice.');
