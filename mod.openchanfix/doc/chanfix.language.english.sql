-- Language Definition Table for chanfix.
--
-- Language: English (1)
-- Author: evilnet development <http://evilnet.sourceforge.net>
--
-- $Id: chanfix.language.english.sql,v 1.7 2007/02/01 14:11:17 buzlip01 Exp $

			
DELETE FROM translations;
	
COPY "translations" FROM stdin;
1	1	You may only add one flag per ADDFLAG command.	31337	0
1	2	No such user %s.	31337	0
1	3	Only an owner can add the owner flag.	31337	0
1	4	Only an owner can add the user management flag.	31337	0
1	5	You cannot add a flag to a user in a different group.	31337	0
1	6	You cannot add a block flag.	31337	0
1	7	You cannot add a serveradmin flag.	31337	0
1	8	User %s already has flag '%c'.	31337	0
1	9	Added flag '%c' to user %s.	31337	0
1	10	You cannot add a host to a user in a different group.	31337	0
1	11	User %s already has hostmask %s.	31337	0
1	12	Failed adding hostmask %s to user %s.	31337	0
1	13	Added hostmask %s to user %s.	31337	0
1	14	%s is an invalid channel name.	31337	0
1	15	Note recorded for channel %s.	31337	0
1	16	User %s already exists.	31337	0
1	17	Created user %s (%s).	31337	0
1	18	Created user %s.	31337	0
1	19	Error creating user %s. (Insertion failed)	31337	0
1	20	The channel %s already has the ALERT flag.	31337	0
1	21	ALERT flag added to channel %s	31337	0
1	22	Channel blocking is disabled.	31337	0
1	23	The channel %s is already blocked.	31337	0
1	24	The channel %s has been blocked.	31337	0
1	25	Sorry, manual chanfixes are currently disabled.	31337	0
1	26	Sorry, chanfix is currently disabled because not enough servers are linked.	31337	0
1	27	No such channel %s.	31337	0
1	28	%s cannot be chanfixed as it uses oplevels (+A/+U). If this channel has been taken over and needs to be returned to the original owners, use \002OPLIST %s\002 to see the real owners and then use an appropriate oper service to fix the channel manually.	31337	0
1	29	%s is a registered channel.	31337	0
1	30	There are no scores in the database for %s.	31337	0
1	31	The channel %s is already being manually fixed.	31337	0
1	32	The highscore in channel %s is %d which is lower than the minimum score required (%.2f * %d = %d).	31337	0
1	33	The channel %s is being automatically fixed. Append the OVERRIDE flag to force a manual fix.	31337	0
1	34	The channel %s is BLOCKED.	31337	0
1	35	Alert: The channel %s has notes. Use \002INFO %s\002 to read them. Append the OVERRIDE flag to force a manual fix.	31337	0
1	36	Manual chanfix acknowledged for %s	31337	0
1	37	I see %d opped out of %d total clients in %s.	31337	0
1	38	You may only remove one flag per DELFLAG command.	31337	0
1	39	Only an owner can delete the owner flag.	31337	0
1	40	Only an owner can delete the user management flag.	31337	0
1	41	You cannot delete a flag from a user in a different group.	31337	0
1	42	You cannot remove a block flag.	31337	0
1	43	You cannot remove a serveradmin flag.	31337	0
1	44	User %s does not have flag '%c'.	31337	0
1	45	Deleted flag '%c' from user %s.	31337	0
1	46	You cannot delete a host from a user in a different group.	31337	0
1	47	User %s doesn't have hostmask %s.	31337	0
1	48	Failed deleting hostmask %s from user %s.	31337	0
1	49	Deleted hostmask %s from user %s.	31337	0
1	50	There is no entry in the database for %s.	31337	0
1	51	The channel %s does not have any notes.	31337	0
1	52	An unknown error occured while checking the note id.	31337	0
1	53	There is no such note with that note_id.	31337	0
1	54	No such note #%d for channel %s.	31337	0
1	55	Note #%d for channel %s was not added by you. You can only delete notes that you added.	31337	0
1	56	Note #%d for channel %s is not a manually added note. You can only delete notes that were manually added.	31337	0
1	57	Note #%d for channel %s deleted.	31337	0
1	58	You cannot delete an owner unless you're an owner.	31337	0
1	59	You cannot delete a user manager unless you're an owner.	31337	0
1	60	You cannot delete a user in a different group.	31337	0
1	61	Deleted user %s.	31337	0
1	62	Error deleting user %s.	31337	0
1	63	Channel %s has never been manually fixed.	31337	0
1	64	An unknown error occurred while reading this channel's notes.	31337	0
1	65	Channel %s has been manually fixed on:	31337	0
1	67	End of list.	31337	0
1	68	No information on %s in the database.	31337	0
1	69	Information on %s:	31337	0
1	70	%s is BLOCKED.	31337	0
1	71	%s is being chanfixed.	31337	0
1	72	%s is being autofixed.	31337	0
1	73	Notes (%d):	31337	0
1	74	End of information.	31337	0
1	75	%s is ALERTED.	31337	0
1	76	[%d:%s] %s \002%s\002%s%s	31337	0
1	77	Could not find channel %s on the network.	31337	0
1	78	I am not in %s.	31337	0
1	79	You are already in %s!	31337	0
1	80	Top %d unique op accounts in channel %s:	31337	0
1	81	Found %d unique op accounts in channel %s:	31337	0
1	82	Rank Score Account -- Time first opped / Time last opped / Nick	31337	0
1	83	Opped clients on channel %s:	31337	0
1	84	I see 1 opped client in %s.	31337	0
1	85	I see %u opped clients in %s.	31337	0
1	86	Successfully reloaded help tables. %i entries in table.	31337	0
1	87	Successfully rehashed configuration file.	31337	0
1	88	Reloading client...see you on the flip side	31337	0
1	89	Score for %s (%s) in channel %s: %u. (Ranked #%u of %d)	31337	0
1	90	~U %s no@such.nick 0	31337	0
1	91	No such nick %s.	31337	0
1	92	None.	31337	0
1	93	Top %u scores for channel %s in the database:	31337	0
1	94	Top %u scores for current ops in %s in the database:	31337	0
1	95	Top %u scores for current non-ops in %s in the database:	31337	0
1	96	Please use SET NUM_SERVERS <integer number>.	31337	0
1	97	NUM_SERVERS is now %d.	31337	0
1	98	Enabled autofix.	31337	0
1	99	Disabled autofix.	31337	0
1	100	Please use SET ENABLE_AUTOFIX <on/off>.	31337	0
1	101	Enabled manual chanfix.	31337	0
1	102	Disabled manual chanfix.	31337	0
1	103	Please use SET ENABLE_CHANFIX <on/off>.	31337	0
1	104	Enabled channel blocking.	31337	0
1	105	Disabled channel blocking.	31337	0
1	106	Please use SET ENABLE_CHANNEL_BLOCKING <on/off>.	31337	0
1	107	This setting does not exist.	31337	0
1	108	User %s is already in group %s.	31337	0
1	109	Set group %s for user %s.	31337	0
1	110	Shutting down the server as per your request.	31337	0
1	111	Uptime: \002%s\002	31337	0
1	112	Automatic fixing is: \002%s\002	31337	0
1	113	Manual fixing is: \002%s\002	31337	0
1	114	Channel blocking is: \002%s\002	31337	0
1	115	Required amount of servers linked is %u%% of %u, which is a minimum of %u servers.	31337	0
1	116	Splitmode enabled: only %i servers linked.	31337	0
1	117	Splitmode disabled. There are %i servers linked.	31337	0
1	118	You cannot suspend a user in a different group.	31337	0
1	119	User %s is already suspended.	31337	0
1	120	Suspended user %s indefinitely.	31337	0
1	121	The channel %s does not have the ALERT flag.	31337	0
1	122	ALERT flag removed from channel %s	31337	0
1	123	The channel %s is not blocked.	31337	0
1	124	Channel %s has been unblocked.	31337	0
1	125	You cannot unsuspend a user in a different group.	31337	0
1	126	User %s is not currently suspended.	31337	0
1	127	Unsuspended user %s.	31337	0
1	128	Language is set to %s.	31337	0
1	129	I will now send you notices.	31337	0
1	130	I will now send you privmsgs.	31337	0
1	131	Please use USET NOTICE <on/off>.	31337	0
1	134	You can only WHOGROUP on your group.	31337	0
1	135	Users with group %s [username (flags)]:	31337	0
1	136	Number of users: %d.	31337	0
1	137	List of all users:	31337	0
1	138	User: %s, Flags: %s, Group: %s	31337	0
1	140	--SUSPENDED--	31337	0
1	142	Group: %s	31337	0
1	143	Flags: none.	31337	0
1	144	Flags: +%s	31337	0
1	145	Sorry, I do not accept commands during a burst.	31337	0
1	146	Unknown command.	31337	0
1	147	Your access to this service is suspended.	31337	0
1	148	Your current host does not match any registered hosts for your username.	31337	0
1	149	You need to authenticate to use this command.	31337	0
1	150	This command requires flag '%c'.	31337	0
1	151	This command requires one of these flags: \"%s\".	31337	0
1	152	This command requires authentication and flag '%c'.	31337	0
1	153	This command requires authentication and one of these flags: \"%s\".	31337	0
1	154	SYNTAX: 	31337	0
1	155	Language: %s (%s)	31337	0
1	156	~U %s no@score.for.account 0	31337	0
1	157	No score for account %s on channel %s.	31337	0
1	158	Score for account %s in channel %s: %u. (Ranked #%u of %d)	31337	0
1	159	Suspending yourself is not a very wise thing to do.	31337	0
1	160	Last modified: %s (%s ago)	31337	0
1	161	NeedOper: Yes	31337	0
1	162	NeedOper: No	31337	0
1	163	You cannot view hosts of a user in a different group.	31337	0
1	164	Host list for %s:	31337	0
1	165	End of host list.	31337	0
1	166	No score for %s on %s.	31337	0
1	167	~U %s no@score.for.nick 0	31337	0
1	168	List of all groups:	31337	0
1	169	Number of groups: %d.	31337	0
1	170	Removed the default hostmask of *!*@* from user %s.	31337	0
1	171	Channel service linked. New channels will be scored.	31337	0
1	172	Channel service not linked. New channels will not be scored.	31337	0
1	173	This command cannot proceed while an update is in progress. Please try again later.	31337	0
1	174	Total number of channels recorded: %u	31337	0
1	175	Total number of channel ops recorded: %u	31337	0
1	176	Last used the service: %s ago	31337	0
1	177	Last used the service: Never	31337	0
1	178	WARNING: Channel %s is being manually fixed; aborting fix as per BLOCK.	31337	0
1	179	WARNING: Channel %s is being automatically fixed; aborting fix as per BLOCK.	31337	0
1	180	You cannot suspend an owner unless you're an owner.	31337	0
1	181	You cannot suspend a user manager unless you're an owner.	31337	0
1	182	You cannot unsuspend an owner unless you're an owner.	31337	0
1	183	You cannot unsuspend a user manager unless you're an owner.	31337	0
1	184	You cannot delete a host from an owner unless you're an owner.	31337	0
1	185	You cannot delete a host from a user manager unless you're an owner.	31337	0
1	186	You cannot add a host to an owner unless you're an owner.	31337	0
1	187	You cannot add a host to a user manager unless you're an owner.	31337	0
1	188	%s is temporarily blocked. (Use OVERRIDE to bypass)	31337	0
1	189	%s is temporarily blocked due to oper intervention via EUWorld within the last hour. Please append the OVERRIDE flag to force a manual fix.	31337	0
1	190	List of all blocked channels:	31337	0
1	191	%d channels blocked.	31337	0
1	192	Current fix has been running for %s	31337	0
1	193	Current fix is on hold (waiting for ops to join)	31337	0
1	194	Channel Score -- Time first opped / Time last opped	31337	0
1	195	Account %s doesn't have any scores in the database.	31337	0
1	196	You score for %s is not high enough to issue a fix.	31337	0
1	197	Accounts who can issue fixes in channel %s:	31337	0
1	198	Account -- Time first opped / Time last opped	31337	0
1	199	You do not have a high enough score for %s to check canfix list.	31337	0
1	200	Sorry, chanfix cannot fix channels at the moment, please try again soon.	31337	0
1	201	The channel %s is being automatically fixed. You cannot issue another fix at the moment.	31337	0
1	202	The channel %s cannot be fixed, please contact a help channel for further information.	31337	0
1	203	Sorry, I'm too busy at the moment. Please try again soon.	31337	0
1	204	Sorry, I cant fix channels at the moment.	31337	0
1	205	Fixing %s, please wait.	31337	0
1	206	You must wait %s seconds before %s can be fixed again with this command, or contact a help channel.	31337	0
1	207	End of OPLIST for %s	31337	0
1	208	You cannot see logs more than one year ago.	31337	0
1	209	Listing last %d messages from day %d.	31337	0
1	210	End of LASTCOM report.	31337	0
1	211	The channel %s is TEMPBLOCKED. To fix it, please append the OVERRIDE flag.	31337	0
1	214	%s is TEMPBLOCKED. (Use OVERRIDE to bypass)	31337	0
1	221	Simulate complete for %s	31337	0
1	222	(%s) Opping: %s (%d Clients)	31337	0
1	223	(%s) Channel modes have been removed.	31337	0
1	224	Simulate for %s (%s) starting at next fixing round (Current C time %s).	31337	0
1	225	NOTE: This is only an estimate, if ops with points join or part it could affect who gets opped.	31337	0
1	226	You may only WHOFLAG one flag.	31337	0
\.
