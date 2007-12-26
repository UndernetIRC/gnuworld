-- Merry Christmas!

-- Christmas language definition.
-- 24/12/01 - R33D33R <reed@redmagnet.com>. (miscellaneous updates)
-- 24/12/01 - nighty <nighty@undernet.org>. (fixed missing field and type of queries)
-- 19/12/01 - R33D33R <reed@redmagnet.com>.
-- Special thanks to Vampire-.

-- Run this once.

DELETE FROM languages WHERE id=21;

INSERT INTO languages VALUES(21,'CH','Christmas',31337);

DELETE FROM translations WHERE language_id = 21;

COPY "translations" FROM stdin;
21	1	We know choosing a present is difficult, but having an identity crisis is a no-no %s	31337	0
21	2	Welcome to the Winter Wonderland, %s	31337	0
21	3	Sorry, you don''t have the correct number of stars to cast that spell :)	31337	0
21	4	Sorry, the tree %s is empty	31337	0
21	5	I don't see %s anywhere	31337	0
21	6	I can't find %s on tree %s	31337	0
21	7	The tree %s doesn't appear to be in Santa's sled	31337	0
21	8	You're opped by %s (%s) on %s	31337	0
21	9	You're voiced by %s (%s) on %s	31337	0
21	10	%s: You are not in that tree	31337	0
21	11	%s is already opped in %s	31337	0
21	12	%s is already voiced in %s	31337	0
21	13	You're deoped by %s (%s)	31337	0
21	14	You're devoiced by %s (%s)	31337	0
21	15	%s is not opped in %s	31337	0
21	16	%s is not voiced in %s	31337	0
21	17	%s is snowed out of the Winter Wonderland (Invalid Ornament)	31337	0
21	18	I'm not in that tree!	31337	0
21	19	Invalid curse-level range. Valid range is 1-%i.	31337	0
21	20	Invalid curse duration. Your curse duration can be a maximum of %d hours.	31337	0
21	21	Curse reason cannot exceed 128 chars	31337	0
21	22	Specified curse is already in my curse-list!	31337	0
21	23	The curse %s is already covered by %s	31337	0
21	24	Invalid minimum level.	31337	0
21	25	Invalid maximum level.	31337	0
21	26	ELF: %s STARS: %s %s	31337	0
21	27	TREE: %s -- AUTOMODE: %s	31337	0
21	28	LAST MODIFIED: %s (%s ago)	31337	0
21	29	** CHARMED ** - Expires in %s (Level %i)	31337	0
21	30	LAST SEEN: %s ago.	31337	0
21	31	There are more than %d matching entries.	31337	0
21	32	Please restrict your query.	31337	0
21	33	End of stars list	31337	0
21	34	No Match!	31337	0
21	35	Cannot add an elf that will have equal or more stars than you have.	31337	0
21	36	Invalid stars level.	31337	0
21	37	%s is already added to %s with stars level %i.	31337	0
21	38	Added elf %s to %s with stars level %i	31337	0
21	39	Something went wrong: %s	31337	0
21	40	%s: End of curse list	31337	0
21	41	That elf is under an invisible spell, therefore, you cannot view elf details	31337	0
21	42	Information about: %s (%i)	31337	0
21	43	Currently listed in Santa's Book as: %s	31337	0
21	44	URL: %s	31337	0
21	45	Language: %i	31337	0
21	46	Trees: %s	31337	0
21	47	Input Flood Points: %i	31337	0
21	48	Ouput Flood (Bytes): %i	31337	0
21	49	%s is registered by:	31337	0
21	50	%s - last seen: %s ago	31337	0
21	51	Desc: %s	31337	0
21	52	Flood me will you? I'm spoiled so I'm not going to listen to you anymore! :p	31337	0
21	53	I think I've sent you a little too much data, I'm going to ignore you for a while.	31337	0
21	54	Incomplete command	31337	0
21	55	To use %s, you must /msg %s@%s	31337	0
21	56	Sorry, you must be in Santa's Book to use this command.	31337	0
21	57	The tree %s has been charmed by one of Santa's North Pole Helpers.	31337	0
21	58	Your stars on %s has been charmed.	31337	0
21	59	The NOOP flag is set on %s	31337	0
21	60	The STRICTOP flag is set on %s	31337	0
21	61	You just deoped more than %i people	31337	0
21	62	SYNTAX: %s	31337	0
21	63	Temporarily increased your stars on tree %s to %i	31337	0
21	64	%s is present in Santa's sled.	31337	0
21	65	%s is not present in Santa's sled.	31337	0
21	66	I don't think %s would appreciate that.	31337	0
21	67	\002*** Curse List for Tree %s ***\002	31337	0
21	68	%s %s Level: %i	31337	0
21	69	ADDED BY: %s (%s)	31337	0
21	70	SINCE: %s	31337	0
21	71	EXP: %s	31337	0
21	72	\002*** END ***\002	31337	0
21	73	I don't know who %s is.	31337	0
21	74	Someone has erased you from Santa's Book.	31337	0
21	75	%s doesn't appear to have stars in %s.	31337	0
21	76	Cannot modify an elf with equal or more stars than you have.	31337	0
21	77	Cannot give an elf more or equal stars than you have.	31337	0
21	78	Modified %s's stars level on tree %s to %i	31337	0
21	79	Set AUTOMODE to OP for %s on tree %s	31337	0
21	80	Set AUTOMODE to VOICE for %s on tree %s	31337	0
21	81	Set AUTOMODE to NONE for %s on tree %s	31337	0
21	82	Your secret ornament cannot be your elfname or current nick - syntax is: NEWPASS <new passphrase>	31337	0
21	83	Ornament successfully changed.	31337	0
21	84	The NOOP flag is set on %s	31337	0
21	85	The STRICTOP flag is set on %s (and %s isn't authenticated)	31337	0
21	86	The STRICTOP flag is set on %s (and %s has insufficient stars)	31337	0
21	87	%s is frozen in ice never to be thawed.	31337	0
21	88	%s is already registered with me.	31337	0
21	89	Invalid tree name.	31337	0
21	90	Registered tree %s	31337	0
21	91	Removed %s from my silence list	31337	0
21	92	Couldn't find %s in my silence list	31337	0
21	93	Cannot remove an elf with equal or more stars than you have	31337	0
21	94	You can't remove yourself from a tree you own	31337	0
21	95	Removed elf %s from %s	31337	0
21	96	You casted the INVISIBLE SPELL on yourself.	31337	0
21	97	You revoked the INVISIBLE SPELL on yourself.	31337	0
21	98	%s for %s is %s	31337	0
21	99	Value of %s must be ON or OFF	31337	0
21	100	Invalid ELFFLAGS setting. Correct values are NONE, OP, VOICE.	31337	0
21	101	ELFFLAGS for %s is %s	31337	0
21	102	Value of MASSDEOPPRO has to be 0-7	31337	0
21	103	MASSDEOPPRO for %s is set to %d	31337	0
21	104	Value of FLOODPRO has to be 0-7	31337	0
21	105	FLOODPRO for %s is set to %d	31337	0
21	106	The DESCRIPTION can be a maximum of 80 chars!	31337	0
21	107	DESCRIPTION for %s is cleared.	31337	0
21	108	DESCRIPTION for %s is: %s	31337	0
21	109	The URL can be a maximum of 75 chars!	31337	0
21	110	URL for %s is cleared.	31337	0
21	111	URL for %s is: %s	31337	0
21	112	The string of keywords cannot exceed 80 chars!	31337	0
21	113	KEYWORDS for %s are: %s	31337	0
21	114	Language is set to %s.	31337	0
21	115	ERROR: Invalid language selection.	31337	0
21	116	Can't locate tree %s on the network!	31337	0
21	117	ERROR: Invalid tree setting.	31337	0
21	118	Ignore list:	31337	0
21	119	Ignore list is empty	31337	0
21	120	-- End of Ignore List	31337	0
21	121	CMaster Tree Services internal status:	31337	0
21	122	[Tree Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
21	123	[Elf Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
21	124	[Stars Level Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
21	125	[Curse Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
21	126	Last received Elf NOTIFY: %i	31337	0
21	127	Last received Tree NOTIFY: %i	31337	0
21	128	Last received Level NOTIFY: %i	31337	0
21	129	Last received Curse NOTIFY: %i	31337	0
21	130	Custom data containers allocated: %i	31337	0
21	131	\002Uptime:\002 %s	31337	0
21	132	Tree %s has %d elfs (%i operators)	31337	0
21	133	Mode is: %s	31337	0
21	134	Flags set: %s	31337	0
21	135	Cannot charm an elf with equal or more stars than you have.	31337	0
21	136	Bogus Time Units	31337	0
21	137	Invalid charm duration.	31337	0
21	138	CHARM for %s is cancelled	31337	0
21	139	%s is already charmed on %s	31337	0
21	140	CHARM for %s will expire in %s	31337	0
21	141	ERROR: Topic cannot exceed 145 chars	31337	0
21	142	You have insufficient stars to remove the curse %s from %s's database	31337	0
21	143	Removed %i curses that matched %s	31337	0
21	144	Removed your temporary stars of %i from tree %s	31337	0
21	145	You don't appear to have forced stars in %s, perhaps it expired?	31337	0
21	146	%s isn't charmed on %s	31337	0
21	147	%s is one of Santa's reindeer	31337	0
21	148	%s does not believe in Santa Claus.	31337	0
21	149	%s is listed in Santa's Book as %s%s	31337	0
21	150	%s is an Official North Pole Helper%s and is listed in Santa's Book as %s	31337	0
21	151	%s is a Generous Official North Pole Helper%s and is listed in Santa's Book as %s	31337	0
21	152	%s is an Angel%s and is listed in Santa's Book as %s	31337	0
21	153	There are more than %i entries matching [%s]	31337	0
21	154	Please restrict your search mask	31337	0
21	155	No matching entries for [%s]	31337	0
21	156	%s: Cleared tree modes.	31337	0
21	158	Invalid option.	31337	0
21	159	%s is an Official UnderNet Red-Nosed Reindeer.	31337	0
21	160	%s is an Official Coder-Com Representative%s and is listed in Santa's Book as %s	31337	0
21	161	%s is an Official Coder-Com Contributer%s and is listed in Santa's Book as %s	31337	0
21	162	%s is an Official Coder-Com Developer%s and is listed in Santa's Book as %s	31337	0
21	163	%s is an Official Coder-Com Senior%s and is listed in Santa's Book as %s	31337	0
21	164	 and is one of Santa's reindeer	31337	0
21	165	Added curse %s to %s at level %i	31337	0
21	166	%s: curse list is empty.	31337	0
21	167	I'm already in that tree!	31337	0
21	168	This command is reserved to Santa's reindeer	31337	0
21	169	I'm not opped on %s	31337	0
21	170	%s for %i minutes	31337	0
\.
