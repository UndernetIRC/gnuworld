-- Empty out English data.
DELETE FROM translations where language_id = 1;

COPY "translations" FROM stdin;
1	1	Sorry, You are already authenticated as %s	31337	0
1	2	AUTHENTICATION SUCCESSFUL as %s	31337	0
1	3	Sorry, you have insufficient access to perform that command	31337	0
1	4	Sorry, the basket %s is empty	31337	0
1	5	I don't see %s anywhere	31337	0
1	6	I can't find %s on basket %s	31337	0
1	7	The basket %s doesn't appear to be registered	31337	0
1	8	You're opped by %s (%s)	31337	0
1	9	You're voiced by %s (%s)	31337	0
1	10	%s: You are not in that basket	31337	0
1	11	%s is already opped in %s	31337	0
1	12	%s is already voiced in %s	31337	0
1	13	You're deopped by %s (%s)	31337	0
1	14	You're devoiced by %s (%s)	31337	0
1	15	%s is not opped in %s	31337	0
1	16	%s is not voiced in %s	31337	0
1	17	AUTHENTICATION FAILED as %s (Invalid Password)	31337	0
1	18	I'm not in that basket!	31337	0
1	19	Invalid banlevel range. Valid range is 1-%i.	31337	0
1	20	Invalid ban duration. Your ban duration can be a maximum of %d hours.	31337	0
1	21	Ban reason cannot exceed 128 chars	31337	0
1	22	Specified ban is already in my banlist!	31337	0
1	23	The ban %s is already covered by %s	31337	0
1	24	Invalid minimum level.	31337	0
1	25	Invalid maximum level.	31337	0
1	26	USER: %s ACCESS: %s %s	31337	0
1	27	basket: %s -- AUTOMODE: %s	31337	0
1	28	LAST MODIFIED: %s (%s ago)	31337	0
1	29	** SUSPENDED ** - Expires in %s	31337	0
1	30	LAST SEEN: %s ago.	31337	0
1	31	There are more than %d matching entries.	31337	0
1	32	Please restrict your query.	31337	0
1	33	End of access list	31337	0
1	34	No Match!	31337	0
1	35	Cannot add a user with equal or higher access than your own.	31337	0
1	36	Invalid access level.	31337	0
1	37	%s is already added to %s with access level %i.	31337	0
1	38	Added user %s to %s with access level %i	31337	0
1	39	Something went wrong: %s	31337	0
1	40	%s: End of ban list	31337	0
1	41	Unable to view user details (Invisible)	31337	0
1	42	Information about: %s (%i)	31337	0
1	43	Currently logged on via: %s	31337	0
1	44	URL: %s	31337	0
1	45	Language: %i	31337	0
1	46	baskets: %s	31337	0
1	47	Input Flood Points: %i	31337	0
1	48	Ouput Flood (Bytes): %i	31337	0
1	49	%s is registered by:	31337	0
1	50	%s - last seen: %s ago	31337	0
1	51	Desc: %s	31337	0
1	52	Flood me will you? I'm not going to listen to you anymore	31337	0
1	53	I think I've sent you a little too much data, I'm going to ignore you for a while.	31337	0
1	54	Incomplete command	31337	0
1	55	To use %s, you must /msg %s@%s	31337	0
1	56	Sorry, You must be logged in to use this command.	31337	0
1	57	The basket %s has been suspended by a cservice administrator.	31337	0
1	58	Your access on %s has been suspended.	31337	0
1	59	The NOOP flag is set on %s	31337	0
1	60	The STRICTOP flag is set on %s	31337	0
1	61	You just deopped more than %i people	31337	0
1	62	SYNTAX: %s	31337	0
1	63	Temporarily increased your access on basket %s to %i	31337	0
1	64	%s is registered.	31337	0
1	65	%s is not registered.	31337	0
1	66	I don't think %s would appreciate that.	31337	0
1	67	\002*** Ban List for basket %s ***\002	31337	0
1	68	%s %s Level: %i	31337	0
1	69	ADDED BY: %s (%s)	31337	0
1	70	SINCE: %s	31337	0
1	71	EXP: %s	31337	0
1	72	\002*** END ***\002	31337	0
1	73	I don't know who %s is.	31337	0
1	74	You are not authorised with me anymore.	31337	0
1	75	%s doesn't appear to have access in %s.	31337	0
1	76	Cannot modify a user with equal or higher access than your own.	31337	0
1	77	Cannot give a user higher or equal access to your own.	31337	0
1	78	Modified %s's access level on basket %s to %i	31337	0
1	79	Set AUTOMODE to OP for %s on basket %s	31337	0
1	80	Set AUTOMODE to VOICE for %s on basket %s	31337	0
1	81	Set AUTOMODE to NONE for %s on basket %s	31337	0
1	82	Your passphrase cannot be your username or current nick - syntax is: NEWPASS <new passphrase>	31337	0
1	83	Password successfully changed.	31337	0
1	84	The NOOP flag is set on %s	31337	0
1	85	The STRICTOP flag is set on %s (and %s isn't authenticated)	31337	0
1	86	The STRICTOP flag is set on %s (and %s has insufficient access)	31337	0
1	87	Purged basket %s	31337	0
1	88	%s is already registered with me.	31337	0
1	89	Invalid basket name.	31337	0
1	90	Registered basket %s	31337	0
1	91	Removed %s from my silence list	31337	0
1	92	Couldn't find %s in my silence list	31337	0
1	93	Cannot remove a user with equal or higher access than your own	31337	0
1	94	You can't remove yourself from a basket you own	31337	0
1	95	Removed user %s from %s	31337	0
1	96	Your INVISIBLE setting is now ON.	31337	0
1	97	Your INVISIBLE setting is now OFF.	31337	0
1	98	%s for %s is %s	31337	0
1	99	value of %s must be ON or OFF	31337	0
1	100	Invalid USERFLAGS setting. Correct values are NONE, OP or VOICE.	31337	0
1	101	USERFLAGS for %s is %s	31337	0
1	102	value of MASSDEOPPRO has to be 0-7	31337	0
1	103	MASSDEOPPRO for %s is set to %d	31337	0
1	104	value of FLOODPRO has to be 0-7	31337	0
1	105	FLOODPRO for %s is set to %d	31337	0
1	106	The DESCRIPTION can be a maximum of 80 chars!	31337	0
1	107	DESCRIPTION for %s is cleared.	31337	0
1	108	DESCRIPTION for %s is: %s	31337	0
1	109	The URL can be a maximum of 75 chars!	31337	0
1	110	URL for %s is cleared.	31337	0
1	111	URL for %s is: %s	31337	0
1	112	The string of keywords cannot exceed 80 chars!	31337	0
1	113	KEYWORDS for %s are: %s	31337	0
1	114	Language is set to %s.	31337	0
1	115	ERROR: Invalid language selection.	31337	0
1	116	Can't locate basket %s on the network!	31337	0
1	117	ERROR: Invalid basket setting.	31337	0
1	118	Ignore list:	31337	0
1	119	Ignore list is empty	31337	0
1	120	-- End of Ignore List	31337	0
1	121	CMaster basket Services internal status:	31337	0
1	122	[     basket Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
1	123	[        User Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
1	124	[Access Level Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
1	125	[         Ban Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
1	126	Last recieved User NOTIFY: %i	31337	0
1	127	Last recieved basket NOTIFY: %i	31337	0
1	128	Last recieved Level NOTIFY: %i	31337	0
1	129	Last recieved Ban NOTIFY: %i	31337	0
1	130	Custom data containers allocated: %i	31337	0
1	131	\002Uptime:\002 %s	31337	0
1	132	basket %s has %d users (%i operators)	31337	0
1	133	Mode is: %s	31337	0
1	134	Flags set: %s	31337	0
1	135	Cannot suspend a user with equal or higher access than your own.	31337	0
1	136	bogus time units	31337	0
1	137	Invalid suspend duration.	31337	0
1	138	SUSPENSION for %s is cancelled	31337	0
1	139	%s is already suspended on %s	31337	0
1	140	SUSPENSION for %s will expire in %s	31337	0
1	141	ERROR: Topic cannot exceed 145 chars	31337	0
1	142	You have insufficient access to remove the ban %s from %s's database	31337	0
1	143	Removed %i bans that matched %s	31337	0
1	144	Removed your temporary access of %i from basket %s	31337	0
1	145	You don't appear to have a forced access in %s, perhaps it expired?	31337	0
1	146	%s isn't suspended on %s	31337	0
1	147	%s is an Easter Bunny	31337	0
1	148	%s is NOT logged in.	31337	0
1	149	%s is logged in as %s%s	31337	0
1	150	%s is an Official CService Representative%s and logged in as %s	31337	0
1	151	%s is an Official CService Administrator%s and logged in as %s	31337	0
1	152	%s is an Official CService Developer%s and logged in as %s	31337	0
1	153	There are more than %i entries matching [%s]	31337	0
1	154	Please restrict your search mask	31337	0
1	155	No matching entries for [%s]	31337	0
1	156	%s: Cleared basket modes.	31337	0
1	158	Invalid option.	31337	0
1	159	%s is an Official Undernet Service Bot.	31337	0
1	160	%s is an Official Coder-Com Representative%s and logged in as %s	31337	0
1	161	%s is an Official Coder-Com Contributer%s and logged in as %s	31337	0
1	162	%s is an Official Coder-Com Developer%s and logged in as %s	31337	0
1	163	%s is an Official Coder-Com Senior%s and logged in as %s	31337	0
1	164	 and an Easter Bunny	31337	0
1	165	Added ban %s to %s at level %i	31337	0
1	166	%s: ban list is empty.	31337	0
1	167	I'm already in that basket!	31337	0
1	168	This command is reserved to Easter Bunnies	31337	0
1	169	I'm not opped on %s	31337	0
1	170	%s for %i minutes	31337	0
\.
 
