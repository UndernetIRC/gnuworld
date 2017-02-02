--
-- NOTAKE specific help topics
-- last updated by Empus @ 2017.01.23
--

--
-- Copy & paste reference to simple overwrite later
-- DELETE FROM help WHERE topic IN ('SET NOTAKE', 'SET TAKEREVENGE');
--

INSERT INTO help VALUES ('SET NOTAKE', '1', E'/msg X set <#channel> NOTAKE <on|off>\nThis channel flag (for channel managers) controls whether X will punish users for attempting to takeover the channel by banning all clients.\nThe punish action can be set via TAKEREVENGE.  For more information use: /msg X help SET TAKEREVENGE');

INSERT INTO help VALUES ('SET TAKEREVENGE', '1', E'/msg X set <#channel> TAKEREVENGE <none|ban|suspend>\nConfigure the action that X will take when a user triggers NOTAKE prevention \(if enabled\), whereby:\n    NONE = prevent the ban but take no other action\n    BAN = kickban the user\n    SUSPEND = suspend the user\'s channel access *and* kickban the user\nNOTE -- Channel BAN or SUSPEND actions will be placed @ level 500 for 7 days');
