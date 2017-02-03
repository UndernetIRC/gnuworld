--
-- FLOODPRO specific help topics
-- last updated by Empus @ 2017.01.23
--

--
-- Copy & paste reference to simple overwrite later
-- DELETE FROM help WHERE topic IN ('SET FLOODPRO', 'SET MSGFLOOD', 'SET NOTICEFLOOD', 'SET CTCPFLOOD', 'SET REPEATFLOOD', 'SET FLOODPERIOD');
--

INSERT INTO help VALUES ('SET FLOODPRO', '1', E'/msg X set <#channel> FLOODPRO <defaults|kick|ban|off>\nThis channel flag (for level >=450 users) controls the behaviour of channel flood protection, to punish those flooding a channel via channel MESSAGE/NOTICE/CTCP/REPEAT\nEach floodtype can have a limit set which when breached within FLOODPERIOD time, the client(s) will be punished via KICK or KICKBAN (depending on the mode)\nie. To cause X to kickban users who send 5 messages to a channel within 2 seconds, one could do the following config commands:\n    1). /msg X set #chan FLOODPRO BAN\n    2). /msg X set #chan MSGFLOOD 5\n    3). /msg X set #chan FLOODPERIOD 2\nNOTE -- When BAN action is used the ban will be placed @ level 75 for 3 hours.\nCurrent settings can be viewed in the channel via: /msg X STATUS <chan>\nChannel operators and those with any access in X will not be punished by FLOODPRO.  Floods ocurring from floodnets will be acted upon accordingly and will result in X temporarily increasing the action from KICK->BAN for 5 minutes to help curb subsequent abuse.\nDefault settings (when first enabled, or when \'DEFAULTS\' are set) are: MSGFLOOD:15, NOTICEFLOOD:5, CTCPFLOOD:5, REPEATFLOOD:5, FLOODPERIOD:15');

INSERT INTO help VALUES ('SET MSGFLOOD', '1', E'/msg X set <#channel> MSGFLOOD <0|2-255>\nClients exceeding this number of messages (any PRIVMSG or ACTION text) to a channel within FLOODPERIOD will result in the configured action (KICK or BAN) to be taken against a flooder when FLOODPRO is enabled on a channel.\nSetting a MSGFLOOD value of 0 will disable protection for this flood type.\nFor more help, try: /msg X HELP SET FLOODPRO');

INSERT INTO help VALUES ('SET NOTICEFLOOD', '1', E'/msg X set <#channel> NOTICEFLOOD <0-15>\nClients exceeding this number of notices (any /NOTICE text) to a channel within FLOODPERIOD will result in the configured action (KICK or BAN) to be taken against a flooder when FLOODPRO is enabled on a channel.\nSetting a NOTICEFLOOD value of 0 will disable protection for this flood type.\nFor more help, try: /msg X HELP SET FLOODPRO');

INSERT INTO help VALUES ('SET CTCPFLOOD', '1', E'/msg X set <#channel> CTCPFLOOD <0-15>\nClients exceeding this number of CTCPs to a channel within FLOODPERIOD will result in the configured action (KICK or BAN) to be taken against a flooder when FLOODPRO is enabled on a channel.\nSetting a CTCPFLOOD value of 0 will disable protection for this flood type.\nFor more help, try: /msg X HELP SET FLOODPRO');

INSERT INTO help VALUES ('SET REPEATFLOOD', '1', E'/msg X set <#channel> REPEATFLOOD <0|2-15>\nClients exceeding this number of line repeats to a channel (PRIVMSG, ACTION, /NOTICE, or /PART reason) within FLOODPERIOD will result in the configured action (KICK or BAN) to be taken against a flooder when FLOODPRO is enabled on a channel.\nSetting a REPEATFLOOD value of 0 will disable protection for this flood type.\nFor more help, try: /msg X HELP SET FLOODPRO');

INSERT INTO help VALUES ('SET FLOODPERIOD', '1', E'/msg X set <#channel> FLOODPERIOD <0-15>\nThe amount of time one of the MSGFLOOD/REPEATFLOOD/NOTICEFLOOD/CTCPFLOOD counters must be met within, for the FLOODPRO action (KICK or BAN) to be taken against a flooder.\nSetting a value of 0 will disable flood protection.\nFor more help, try: /msg X HELP SET FLOODPRO');
