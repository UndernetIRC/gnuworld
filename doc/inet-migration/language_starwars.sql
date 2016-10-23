-- Star Wars language definition.
-- 13/02/2013 	- LD <ld@cservice.undernet.org>		-	initial release
-- 19/05/2015	- Empus <empus@undernet.org>		-	modifications

DELETE FROM translations WHERE language_id = 22;

INSERT INTO languages VALUES(22,'SW','StarWars',31337);

COPY "translations" FROM stdin;
22	1	You must unlearn what you have learnt. You are already dark sided as %s	31337	0
22	2	AUTHENTICATION SUCCESSFUL as %s. May the force be with you.	31337	0
22	3	The force in you is not strong enough to perform that command	31337	0
22	4	Sorry, the deathstar %s is empty	31337	0
22	5	I cannot locate %s anywhere in this galaxy	31337	0
22	6	I cannot locate %s on deathstar %s	31337	0
22	7	The deathstar %s doesn't appear to be registered	31337	0
22	8	You're recruited to the dark side by %s (%s) on %s	31337	0
22	9	You're promoted to storm trooper by %s (%s) on %s	31337	0
22	10	%s: You are not aboard that deathstar	31337	0
22	11	%s is already a dark sider in %s	31337	0
22	12	%s is already a storm trooper in %s	31337	0
22	13	The power of the dark side you know not, therefore you're annihilated  by %s (%s) on %	31337	0
22	14	I find your lack of faith disturbing, you're demoted from storm trooper by %s (%s) on %s	31337	0
22	15	%s is not dark sided in %s	31337	0
22	16	%s is not a storm trooper in %s	31337	0
22	17	Acces to the force as %s is denied (Invalid Force Code)	31337	0
22	18	I'm not in that deathstar!	31337	0
22	19	Invalid destruction level range. Valid range is 1-%i.	31337	0
22	20	Invalid destruction duration. The destruction duration can be a maximum of %d days.	31337	0
22	21	Destruction reason cannot exceed 128 chars	31337	0
22	22	Specified destructor is already in my war path	31337	0
22	23	The destructor %s is already covered by %s	31337	0
22	24	Invalid minimum level.	31337	0
22	25	Invalid maximum level.	31337	0
22	26	FORCEKEEPER: %s FORCE LEVEL: %s %s	31337	0
22	27	DEATHSTAR: %s -- AUTOMODE: %s	31337	0
22	28	LAST MODIFIED: %s (%s ago)	31337	0
22	29	** EXTERMINATED ** - Returns in %s (Level %i)	31337	0
22	30	LAST SEEN: %s ago.	31337	0
22	31	There are more than %d matching entries.	31337	0
22	32	Please restrict your query.	31337	0
22	33	End of the force list	31337	0
22	34	No match!	31337	0
22	35	Cannot add a forcekeeper with equal or higher force level than your own.	31337	0
22	36	Invalid force level.	31337	0
22	37	%s is already a forcekeeper on %s with force Level %i	31337	0
22	38	Added forcekeeper %s to %s at force level %i	31337	0
22	39	Something went wrong. Learn the ways of the force you must!: %s	31337	0
22	40	%s: End of destruction list	31337	0
22	41	Unable to view forcekeeper details (Invisible)	31337	0
22	42	Forcekeeper Information: %s (%i)	31337	0
22	43	Currently dark sided as: %s	31337	0  
22	44	URL: %	31337	0
22	45	Language: %i	31337	0
22	46	Deathstars: %s	31337	0
22	47	Input Flood Points: %i	31337	0
22	48	Ouput Flood (Bytes): %i	31337	0
22	49	%s is Ruled by:	31337	0
22	50	%s - last seen: %s ago	31337	0
22	51	Desc: %s	31337	0
22	52	Flood me will you? Listen to you not, ignore you I shall!	31337	0
22	53	I think I've sent you a little too much data, I'm going to ignore you for a while.	31337	0
22	54	Incomplete command	31337	0
22	55	To use %s, you must /msg %s@%s	31337	0
22	56	Dark sided you must be, to use this command.	31337	0
22	57	The deathstar %s has been terminated by a Jedi Knight!.	31337	0
22	58	Your access on %s has been terminated.	31337	0
22	59	The NOOP flag is set on %s	31337	0
22	60	The STRICTOP flag is set on %s	31337	0
22	61	You just annihalated more than %i people from the dark side	31337	0
22	62	SYNTAX: %s	31337	0
22	63	Temporarily increased your force level on deathstar %s to %i	31337	0
22	64	%s is registered.	31337	0
22	65	%s is not registered.	31337	0
22	66	Your powers are weak old man %s. Do or do not, there is no try	31337	0
22	67	\002*** Destruction List for channel %s ***\002	31337	0
22	68	%s %s Level: %i	31337	0
22	69	TERMINATED BY: %s (%s)	31337	0
22	70	SINCE: %s	31337	0
22	71	TERMINATED: %s	31337	0
22	72	\002*** END ***\002	31337	0
22	73	I don't know who %s is	31337	0
22	74	You are not dark sided with me anymore	31337	0
22	75	%s doesn't appear to have access in %s	31337	0
22	76	Cannot modify a forcekeeper with equal or more power than you	31337	0
22	77	Cannot give a forcekeeper stronger or equal power to your own.	31337	0
22	78	Modified %s's access level on deathstar %s to %i	31337	0
22	79	Set AUTOMODE to Dark-Sider for %s on deathstar %s	31337	0
22	80	Set AUTOMODE to Storm Trooper for %s on deathstar %s	31337	0
22	81	Set AUTOMODE to Padawan for %s on deathstar %s	31337	0
22	82	Your passphrase cannot be your username or current nick - syntax is: NEWPASS <new passphrase>	31337	0
22	83	Password successfully changed.	31337	0
22	84	The NOOP flag is set on %s	31337	0
22	85	The STRICTOP flag is set on %s (and %s isn't dark sided)	31337	0
22	86	The STRICTOP flag is set on %s (and %s the force isn't strong enough in this warrior)	31337	0
22	87	Pulverised deathstar %s	31337	0
22	88	%s is already registered with me.	31337	0
22	89	Invalid deathstar name.	31337	0
22	90	Constructed deathstar %s	31337	0
22	91	Removed %s from my stronghold list	31337	0
22	92	Couldn't find %s in my stronghold list	31337	0
22	93	Cannot destroy a lightsaber with equal or a stronger force than your own	31337	0
22	94	You can't remove yourself from a deathstar that you rule	31337	0
22	95	Removed forcekeeper %s from %s	31337	0
22	96	Your CLOAK setting is now ON.	31337	0
22	97	Your CLOAK setting is now OFF.	31337	0
22	98	%s for %s is %s	31337	0
22	99	Value of %s must be ON or OFF	31337	0
22	100	Invalid FORCEKEEPERFLAGS setting. Correct values are NONE, OP or VOICE.	31337	0
22	101	FORCEKEEPERFLAGS for %s is %s	31337	0
22	102	Value of MASSDEOPPRO has to be 0-7	31337	0
22	103	MASSDEOPPRO for %s is set to %d	31337	0
22	104	Value of FLOODPRO has to be 0-7	31337	0
22	105	FLOODPRO for %s is set to %d	31337	0
22	106	The DESCRIPTION can be a maximum of 80 chars!	31337	0
22	107	DESCRIPTION for %s is cleared.	31337	0
22	108	DESCRIPTION for %s is: %s	31337	0
22	109	The URL can be a maximum of 75 chars!	31337	0
22	110	URL for %s is cleared.	31337	0
22	111	URL for %s is: %s	31337	0
22	112	The string of keywords cannot exceed 80 chars!	31337	0
22	113	KEYWORDS for %s are: %s	31337	0
22	114	LANGUAGE is set to %s.	31337	0
22	115	ERROR: Invalid LANGUAGE selection.	31337	0
22	116	Cannot locate deathstar %s anywhere in the galaxy	31337	0
22	117	ERROR: Invalid deathstar setting	31337	0
22	118	Stronghold list:	31337	0
22	119	Stronghold list is empty	31337	0
22	120	-- End of Stronghold List	31337	0
22	121	CMaster Deathstar Services internal status:	31337	0
22	122	[  Deathstar Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
22	123	[Forcekeeper Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
22	124	[Force Level Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
22	125	[        Destruction Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
22	126	Last received Forcekeeper notify: %i	31337	0
22	127	Last received Deathstar notify: %i	31337	0
22	128	Last received Force Level notify: %i	31337	0
22	129	Last received Destruction notify: %i	31337	0
22	130	Custom data containers allocated: %i	31337	0
22	131	\002Uptime:\002 %s	31337	0
22	132	Deathstar %s has %d users (%i operators)	31337	0
22	133	Mode is: %s	31337	0
22	134	Flags set: %s	31337	0
22	135	Cannot destroy a forcekeeper with an equal or stronger force than your own.	31337	0
22	136	bogus time units	31337	0
22	137	Invalid destruction duration.	31337	0
22	138	A glorious day for you it is %s, your termination is cancelled	31337	0
22	139	%s is already destroyed on %s	31337	0
22	140	**ATTENTION!** Destruction of forcekeeper %s will expire in %s	31337	0
22	141	ERROR: Mission statement cannot exceed 145 chars	31337	0
22	142	You have insufficient force level to remove the destructor %s from %s's database	31337	0
22	143	Removed %i destructors that matched %s	31337	0
22	144	Removed your temporary access of %i from deathstar %s	31337	0
22	145	You don't appear to have a forced access in %s, perhaps it expired?	31337	0
22	146	%s isn't destroyed on %s	31337	0
22	147	%s is an Enemy Bounty Hunter	31337	0
22	148	%s is not dark sided, they must learn the ways of the force!.	31337	0
22	149	%s is dark sided as %s%s	31337	0
22	150	%s is an Accomplished Jedi Apprentice%s and dark sided as %s	31337	0
22	151	%s is a Deadly and Feared Jedi Knight%s and dark sided as %s	31337	0
22	152	%s is a Jedi Force Creator%s and dark sided as %s	31337	0
22	153	There are more than %i entries matching [%s]	31337	0
22	154	Please restrict your search mask	31337	0
22	155	No matching entries for [%s]	31337	0
22	156	%s: Cleared deathstar modes.	31337	0
22	158	Invalid option	31337	0
22	159	%s is an Official Undernet Sith Lord	31337	0
22	160	%s is an Official Coder-Com Jedi-Force Apprentice%s and logged in as %s	31337	0
22	161	%s is an Official Coder-Com Jedi-Force Contributer%s and logged in as %s	31337	0
22	162	%s is an Official Coder-Com Jedi-Force Creator%s and logged in as %s	31337	0
22	163	%s is an Official Coder-Com Master Jedi-Force Creator%s and logged in as %s	31337	0
22	164	 and an Enemy Bounty Hunter	31337	0
22	165	Added destructor %s to %s at level %i	31337	0
22	166	%s: destruction list is empty.	31337	0
22	167	I'm already in that deathstar!	31337	0
22	168	This command is reserved to Enemy Bounty Hunters	31337	0
22	169	I'm not opped in %s	31337	0
22	170	%s for %i minutes	31337	0
22	171	I'm not a dark-sider in %s	31337	0
22	172	Keywords: %s	31337	0
22	173	%s does not wish to board deathstars at this time.	31337	0
22	174	Your FORCEBLOCKER defense shield is now ON.	31337	0
22	175	Your FORCEBLOCKER defense shield is now OFF.	31337	0
22	176	There were %d failed attempts to join the dark side since your last successful crossover.	31337	0
22	177	Learn the ways of the force you must %s (Allies, you already have)	31337	0
22	178	Set INVITE on login %s for deathstar %s	31337	0
22	179	AUTHENTICATION FAILED as %s. (Invalid Token)	31337	0
22	180	The following clients are currently dark sided as %s:	31337	0
22	181	If any of these clients are not you, your account may have been compromised. If you wish to suspend all your access as a precautionary measure, type '/msg X@channels.undernet.org suspendme <password>'	31337	0
22	182	*** NOTE: You will NOT be able to use your account after you issue this command, until you contact a CService Administrator to resolve the problem: /join #usernames	31337	0
\.
