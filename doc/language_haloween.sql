-- Muhauahuuahha.
-- INSERT INTO languages VALUES(15, 'HA', 'Haloween', 31337);

DELETE FROM translations WHERE language_id = 15;

COPY "translations" FROM stdin;
15	1	Sorry, You are already authenticated as %s	31337
15	2	AUTHENTICATION SUCCESSFUL as %s	31337
15	3	Sorry, you have insufficient access to perform that command	31337
15	4	Sorry, the channel %s is empty	31337
15	5	I don't see %s anywhere	31337
15	6	I can't find %s on channel %s	31337
15	7	The channel %s doesn't appear to be registered	31337
15	8	You're opped by %s (%s) on %s	31337
15	9	You're voiced by %s (%s) on %s	31337
15	10	%s: You are not in that channel	31337
15	11	%s is already opped in %s	31337
15	12	%s is already voiced in %s	31337
15	13	You're deopped by %s (%s)	31337
15	14	You're devoiced by %s (%s)	31337
15	15	%s is not opped in %s	31337
15	16	%s is not voiced in %s	31337
15	17	AUTHENTICATION FAILED as %s (Invalid Password)	31337
15	18	I'm not in that channel!	31337
15	19	Invalid banlevel range. Valid range is 1-%i.	31337
15	20	Invalid ban duration. Your ban duration can be a maximum of 336 hours.	31337
15	21	Ban reason cannot exceed 128 chars	31337
15	22	Specified ban is already in my banlist!	31337
15	23	The ban %s is already covered by %s	31337
15	24	Invalid minimum level.	31337
15	25	Invalid maximum level.	31337
15	26	USER: %s ACCESS: %s %s	31337
15	27	CHANNEL: %s -- AUTOMODE: %s	31337
15	28	LAST MODIFIED: %s (%s ago)	31337
15	29	** SUSPENDED ** - Expires in %s (Level %i)	31337
15	30	LAST SEEN: %s ago.	31337
15	31	There are more than 15 matching entries.	31337
15	32	Please restrict your query.	31337
15	33	End of access list	31337
15	34	No Match!	31337
15	35	Cannot add a user with equal or higher access than your own.	31337
15	36	Invalid access level.	31337
15	37	%s is already added to %s with access level %i.	31337
15	38	Added user %s to %s with access level %i	31337
15	39	Something went wrong: %s	31337
15	40	%s: End of ban list	31337
15	41	Unable to view user details (Invisible)	31337
15	42	Information about: %s (%i)	31337
15	43	Currently logged on via: %s	31337
15	44	URL: %s	31337
15	45	Language: %i	31337
15	46	Channels: %s	31337
15	47	Input Flood Points: %i	31337
15	48	Ouput Flood (Bytes): %i	31337
15	49	%s is registered by:	31337
15	50	%s - last seen: %s ago	31337
15	51	Desc: %s	31337
15	52	Flood me will you? I'm not going to listen to you anymore	31337
15	53	I think I've sent you a little too much data, I'm going to ignore you for a while.	31337
15	54	Incomplete command	31337
15	55	To use %s, you must /msg %s@%s	31337
15	56	Sorry, You must be logged in to use this command.	31337
15	57	The channel %s has been suspended by a cservice administrator.	31337
15	58	Your access on %s has been suspended.	31337
15	59	The NOOP flag is set on %s	31337
15	60	The STRICTOP flag is set on %s	31337
15	61	You just deopped more than %i people	31337
15	62	SYNTAX: %s	31337
15	63	Temporarily increased your access on channel %s to %i	31337
15	64	%s is registered.	31337
15	65	%s is not registered.	31337
15	66	I don't think %s would appreciate that.	31337
15	67	\002*** Ban List for channel %s ***\002	31337
15	68	%s %s Level: %i	31337
15	69	ADDED BY: %s (%s)	31337
15	70	SINCE: %s	31337
15	71	EXP: %s	31337
15	72	\002*** END ***\002	31337
15	73	I don't know who %s is.	31337
15	74	You are not authorised with me anymore.	31337
15	75	%s doesn't appear to have access in %s.	31337
15	76	Cannot modify a user with equal or higher access than your own.	31337
15	77	Cannot give a user higher or equal access to your own.	31337
15	78	Modified %s's access level on channel %s to %i	31337
15	79	Set AUTOMODE to OP for %s on channel %s	31337
15	80	Set AUTOMODE to VOICE for %s on channel %s	31337
15	81	Set AUTOMODE to NONE for %s on channel %s	31337	
15	82	Your passphrase cannot be your username or current nick - syntax is: NEWPASS <new passphrase>	31337
15	83	Password successfully changed.	31337
15	84	The NOOP flag is set on %s	31337
15	85	The STRICTOP flag is set on %s (and %s isn't authenticated)	31337
15	86	The STRICTOP flag is set on %s (and %s has insufficient access)	31337
15	87	Purged channel %s	31337
15	88	%s is already registered with me.	31337
15	89	Invalid channel name.	31337
15	90	Registered channel %s	31337
15	91	Removed %s from my silence list	31337
15	92	Couldn't find %s in my silence list	31337
15	93	Cannot remove a user with equal or higher access than your own	31337
15	94	You can't remove yourself from a channel you own	31337
15	95	Removed user %s from %s	31337
15	96	Your INVISIBLE setting is now ON.	31337
15	97	Your INVISIBLE setting is now OFF.	31337
15	98	%s for %s is %s	31337
15	99	value of %s must be ON or OFF	31337
15	100	Invalid USERFLAGS setting. Correct values are 0, 1, 2.	31337
15	101	USERFLAGS for %s is %i	31337
15	102	value of MASSDEOPPRO has to be 0-7	31337
15	103	MASSDEOPPRO for %s is set to %d	31337
15	104	value of FLOODPRO has to be 0-7	31337
15	105	FLOODPRO for %s is set to %d	31337
15	106	The DESCRIPTION can be a maximum of 80 chars!	31337
15	107	DESCRIPTION for %s is cleared.	31337
15	108	DESCRIPTION for %s is: %s	31337
15	109	The URL can be a maximum of 75 chars!	31337
15	110	URL for %s is cleared.	31337
15	111	URL for %s is: %s	31337
15	112	The string of keywords cannot exceed 80 chars!	31337
15	113	KEYWORDS for %s are: %s	31337
15	114	Language is set to %s.	31337
15	115	ERROR: Invalid language selection.	31337
15	116	Can't locate channel %s on the network!	31337
15	117	ERROR: Invalid channel setting.	31337
15	118	Ignore list:	31337
15	119	Ignore list is empty	31337
15	120	-- End of Ignore List	31337
15	121	CMaster Channel Services internal status:	31337
15	122	[     Channel Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337
15	123	[        User Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337
15	124	[Access Level Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337
15	125	[         Ban Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337
15	126	Last received User NOTIFY: %i	31337
15	127	Last received Channel NOTIFY: %i	31337
15	128	Last received Level NOTIFY: %i	31337
15	129	Last received Ban NOTIFY: %i	31337
15	130	Custom data containers allocated: %i	31337
15	131	\002Uptime:\002 %s	31337
15	132	Channel %s has %d users (%i operators)	31337
15	133	Mode is: %s	31337
15	134	Flags set: %s	31337
15	135	Cannot suspend a user with equal or higher access than your own.	31337
15	136	bogus time units	31337
15	137	Invalid suspend duration.	31337
15	138	SUSPENSION for %s is cancelled	31337
15	139	%s is already suspended on %s	31337
15	140	SUSPENSION for %s will expire in %s	31337
15	141	ERROR: Topic cannot exceed 145 chars	31337
15	142	You have insufficient access to remove the ban %s from %s's database	31337
15	143	Removed %i bans that matched %s	31337
15	144	Removed your temporary access of %i from channel %s	31337
15	145	You don't appear to have a forced access in %s, perhaps it expired?	31337
15	146	%s isn't suspended on %s	31337
15	147	%s is an IRC operator	31337
15	148	%s is NOT logged in.	31337
15	149	%s is logged in as %s%s	31337
15	150	%s is an Official CService Representative%s and logged in as %s	31337
15	151	%s is an Official CService Administrator%s and logged in as %s	31337
15	152	%s is an Official CService Developer%s and logged in as %s	31337
15	153	There are more than %i entries matching [%s]	31337
15	154	Please restrict your search mask	31337
15	155	No matching entries for [%s]	31337
15	156	%s: Cleared channel modes.	31337
15	158	Invalid option.	31337
15	159	%s is an Official Undernet Service Bot.	31337
15	160	%s is an Official Coder-Com Representative%s and logged in as %s	31337
15	161	%s is an Official Coder-Com Contributer%s and logged in as %s	31337
15	162	%s is an Official Coder-Com Developer%s and logged in as %s	31337
15	163	%s is an Official Coder-Com Senior%s and logged in as %s	31337
15	164	 and an IRC operator	31337
15	165	Added ban %s to %s at level %i	31337
15	166	%s: ban list is empty.		31337
15	167	I'm already in that channel!	31337
15	168	This command is reserved to IRC Operators	31337
15	169	I'm not opped on %s	31337
15	170	%s for %i minutes	31337
\.
