-- $Id: language_halloween.sql,v 1.8 2008/01/16 19:19:21 kewlio Exp $
-- Muhauahuuahha.
-- Halloween language definition.
-- 30/10/01 - Gte (Greg Sikorski) <gte@atomicrevs.demon.co.uk>.
-- 31/10/01 - Isomer (Perry Lorier) <isomer@coders.net>. (various updates)
-- 31/10/02 - R33D33R (Reed Loden) <reed@reedloden.com>. (removed INSERT line, added chan name, added ending 0, and edited replies)

-- Run this once.

DELETE FROM translations WHERE language_id = 18;

COPY "translations" FROM stdin;
18	1	You're feeble disguise doesn't work. You are still %s	31337	0
18	2	Welcome to the Spooky UnderWorld, %s. MWAHAHAHAHHA!	31337	0
18	3	Muhauhauhauha, you can't do that :P	31337	0
18	4	Sorry, the casket %s is empty	31337	0
18	5	I don't see %s anywhere	31337	0
18	6	I can't find %s on casket %s	31337	0
18	7	The casket %s doesn't appear to be registered	31337	0
18	8	You're slaughtered by %s (%s) on %s	31337	0
18	9	You're stabbed by %s (%s) on %s	31337	0
18	10	%s: You are not in that casket	31337	0
18	11	%s is already opped in %s	31337	0
18	12	%s is already voiced in %s	31337	0
18	13	You're scared by %s (%s)	31337	0
18	14	You're frightened by %s (%s)	31337	0
18	15	%s is not scared in %s	31337	0
18	16	%s is not frightened in %s	31337	0
18	17	%s is denied entry into the Spooky UnderWorld	31337	0
18	18	I'm not in that casket!	31337	0
18	19	Invalid pitchfork-level range. Valid range is 1-%i.	31337	0
18	20	Invalid pitchfork duration. Your pitchfork duration can be a maximum of %d hours.	31337	0
18	21	Pitchfork reason cannot exceed 128 chars	31337	0
18	22	Specified pitchfork is already in my pitchfork-list!	31337	0
18	23	The pitchfork %s is already covered by %s	31337	0
18	24	Invalid minimum level.	31337	0
18	25	Invalid maximum level.	31337	0
18	26	PUMPKIN: %s GENERATION: %s %s	31337	0
18	27	CASKET: %s -- AUTOMODE: %s	31337	0
18	28	LAST MODIFIED: %s (%s ago)	31337	0
18	29	** BANISHED ** - Expires in %s (Level %i)	31337	0
18	30	LAST SEEN: %s ago.	31337	0
18	31	There are more than %d matching entries.	31337	0
18	32	Please restrict your query.	31337	0
18	33	End of generation list	31337	0
18	34	No Match!	31337	0
18	35	Cannot add a pumpkin that has an equal or higher generation than your own.	 31337	0
18	36	Invalid generation level.	31337	0
18	37	%s is already added to %s with generation level %i.	31337	0
18	38	Added pumpkin %s to %s with generation level %i	31337	0
18	39	Something went wrong: %s	31337	0
18	40	%s: End of pitchfork list	31337	0
18	41	That pumpkin is impersonating the invisible man.	31337	0
18	42	Information about: %s (%i)	31337	0
18	43	Currently disguised as: %s	31337	0
18	44	URL: %s	31337	0
18	45	Language: %i	31337	0
18	46	Caskets: %s	31337	0
18	47	Input Flood Points: %i	31337	0
18	48	Ouput Flood (Bytes): %i	31337	0
18	49	%s is registered by:	31337	0
18	50	%s - last seen: %s ago	31337	0
18	51	Desc: %s	31337	0
18	52	Flood me will you? I'm spoiled so I'm not going to listen to you anymore! :P	 31337	0
18	53	I think I've sent you a little too much data, I'm going to ignore you for a while. ;)	31337	0
18	54	Incomplete command	31337	0
18	55	To use %s, you must /msg %s@%s	31337	0
18	56	Sorry, you must chant the secret incantation to use this command.	31337	0
18	57	The casket %s has been suspended by a CService Administrator.	31337	0
18	58	Your generation on %s has been suspended.	31337	0
18	59	The NOOP flag is set on %s	31337	0
18	60	The STRICTOP flag is set on %s	31337	0
18	61	You just scared more than %i people	31337	0
18	62	SYNTAX: %s	31337	0
18	63	Temporarily increased your generation on casket %s to %i	31337	0
18	64	%s is present in the cemetary.	31337	0
18	65	%s is not present in the cemetary.	31337	0
18	66	I don't think %s would appreciate that.	31337	0
18	67	\002*** PITCHFORK List for Casket %s ***\002	31337	0
18	68	%s %s Level: %i	31337	0
18	69	ADDED BY: %s (%s)	31337	0
18	70	SINCE: %s	31337	0
18	71	EXP: %s	31337	0
18	72	\002*** END ***\002	31337	0
18	73	I don't know who %s is.	31337	0
18	74	You are no longer in disguise.	31337	0
18	75	%s doesn't appear to have generation in %s.	31337	0
18	76	Cannot modify a pumpkin that has an equal or higher generation than your own.	31337	0
18	77	Cannot give a pumpkin equal or higher generation than your own.	 31337	0
18	78	Modified %s's generation level on casket %s to %i	31337	0
18	79	Set AUTOMODE to OP for %s on casket %s	31337	0
18	80	Set AUTOMODE to VOICE for %s on casket %s	31337	0
18	81	Set AUTOMODE to NONE for %s on casket %s	31337	0
18	82	Your secret chant cannot be your pumpkinname or current nick - syntax is: NEWPASS <new passphrase>	31337	0
18	83	Password successfully changed.	31337	0
18	84	The NOOP flag is set on %s	31337	0
18	85	The STRICTOP flag is set on %s (and %s isn't authenticated)	31337	0
18	86	The STRICTOP flag is set on %s (and %s has insufficient generation)	 31337	0
18	87	%s crumbles into dust, forever banished from the UnderWorld	31337	0
18	88	%s is already registered with me.	31337	0
18	89	Invalid casket name.	31337	0
18	90	Registered casket %s	31337	0
18	91	Removed %s from my silence list	31337	0
18	92	Couldn't find %s in my silence list	31337	0
18	93	Cannot remove a pumpkin that has an equal or higher generation than your own	31337	0
18	94	You can't remove yourself from a casket you own	31337	0
18	95	Removed pumpkin %s from %s	31337	0
18	96	Your INVISIBLE setting is now ON.	31337	0
18	97	Your INVISIBLE setting is now OFF.	31337	0
18	98	%s for %s is %s	31337	0
18	99	Value of %s must be ON or OFF	31337	0
18	100	Invalid PUMPKINFLAGS setting. Correct values are NONE, OP, VOICE.	31337	0
18	101	PUMPKINFLAGS for %s is %s	31337	0
18	102	value of MASSDEOPPRO has to be 0-7	31337	0
18	103	MASSDEOPPRO for %s is set to %d	31337	0
18	104	value of FLOODPRO has to be 0-7	31337	0
18	105	FLOODPRO for %s is set to %d	31337	0
18	106	The DESCRIPTION can be a maximum of 80 chars!	31337	0
18	107	DESCRIPTION for %s is cleared.	31337	0
18	108	DESCRIPTION for %s is: %s	31337	0
18	109	The URL can be a maximum of 75 chars!	31337	0
18	110	URL for %s is cleared.	31337	0
18	111	URL for %s is: %s	31337	0
18	112	The string of keywords cannot exceed 80 chars!	31337	0
18	113	KEYWORDS for %s are: %s	31337	0
18	114	Language is set to %s.	31337	0
18	115	ERROR: Invalid language selection.	31337	0
18	116	Can't locate casket %s on the network!	31337	0
18	117	ERROR: Invalid casket setting.	31337	0
18	118	Ignore list:	31337	0
18	119	Ignore list is empty	31337	0
18	120	-- End of Ignore List	31337	0
18	121	CMaster Casket Services Internal Status:	31337	0
18	122	[Casket Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
18	123	[Pumpkin Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
18	124	[Generation Level Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
18	125	[Pitchfork Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
18	126	Last received Pumpkin NOTIFY: %i	31337	0
18	127	Last received Casket NOTIFY: %i	31337	0
18	128	Last received Level NOTIFY: %i	31337	0
18	129	Last received Pitchfork NOTIFY: %i	31337	0
18	130	Custom data containers allocated: %i	31337	0
18	131	\002Uptime:\002 %s	31337	0
18	132	casket %s has %d pumpkins (%i operators)	31337	0
18	133	Mode is: %s	31337	0
18	134	Flags set: %s	31337	0
18	135	Cannot suspend a pumpkin that has an equal or higher generation than your own.	31337	0
18	136	Bogus time units!	31337	0
18	137	Invalid suspend duration.	31337	0
18	138	SUSPENSION for %s is cancelled	31337	0
18	139	%s is already suspended on %s	31337	0
18	140	SUSPENSION for %s will expire in %s	31337	0
18	141	ERROR: Topic cannot exceed 145 chars	31337	0
18	142	You have insufficient generation to remove the pitchfork %s from %s's database	31337	0
18	143	Removed %i pitchforks that matched %s	31337	0
18	144	Removed your temporary generation of %i from casket %s	31337	0
18	145	You don't appear to have a forced generation in %s, perhaps it expired? :)	31337	0
18	146	%s isn't suspended on %s	31337	0
18	147	%s is a vampire scr1ptk1dd13 slayer	31337	0
18	148	%s is not in costume.	31337	0
18	149	%s is disguised as %s%s	31337	0
18	150	%s is an Official CService Trick-or-Treater%s and is disguised as %s	31337	0
18	151	%s is an Official CService Scary Trick-or-Treater%s and is disguised as %s	31337	0
18	152	%s is getting their ass kicked by Buffy%s and is disguised as %s	 31337	0
18	153	There are more than %i entries matching [%s]	31337	0
18	154	Please restrict your search mask	31337	0
18	155	No matching entries for [%s]	31337	0
18	156	%s: Cleared casket modes.	31337	0
18	158	Invalid option.	31337	0
18	159	%s is an Official UnderNet Ghost.	31337	0
18	160	%s is an Official Coder-Com Representative%s and is disguised as %s	31337	0
18	161	%s is an Official Coder-Com Contributer%s and is disguised as %s	31337	0
18	162	%s is an Official Coder-Com Developer%s and is disguised as %s	31337	0
18	163	%s is an Official Coder-Com Senior%s and is disguised as %s	 31337	0
18	164	 and is a vampire scr1ptk1dd13 slayer	31337	0
18	165	Added pitchfork %s to %s at level %i	31337	0
18	166	%s: pitchfork list is empty.	31337	0
18	167	I'm already in that casket!	31337	0
18	168	This command is reserved to vampire scr1ptk1dd13 slayers	31337	0
18	169	I'm not opped on %s	31337	0
18	170	%s for %i minutes	31337	0
\.

