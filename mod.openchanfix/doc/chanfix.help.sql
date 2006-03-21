-- Help information.
--
-- $Id: chanfix.help.sql,v 1.2 2006/03/21 23:12:38 buzlip01 Exp $

DELETE FROM help;

COPY "help" FROM stdin;
ADDFLAG	1	Adds the specified flag to the specified user. Possible flags:\na - Can manage users for local server/group\nb - Can block/unblock channels\nc - Can add/delete notes for channels\nf - Can perform manual chanfix\no - Can do anything\nu - Can manage users globally\nNote: You can only add one flag at a time.
ADDHOST	1	Adds the specified hostmask to the user's list of hostmasks.
ADDNOTE	1	Adds a note to the specified channel.
ADDUSER	1	Adds a new user, without flags, and optionally with the specified hostmask.
ALERT	1	Puts a channel on ALERT status\nChannels in alert status have information contained in notes that should be considered before fixing the channel. These channels also require the OVERRIDE flag to manually fix them.
BLOCK	1	Blocks a channel from being fixed, both automatically and manually.\nThe reason will be shown when doing INFO <channel>.
CHECK	1	Shows the number of ops and total clients in the specified channel.
CSCORE	1	Shows the same as SCORE, but in a compact output. See /msg C HELP SCORE.
CHANFIX	1	Performs a manual fix on the specified channel. Append OVERRIDE, YES, or an exclamation mark (!) to force a manual fix.
DELFLAG	1	Removes the specified flag from a user. See /msg C HELP ADDFLAG.
DELHOST	1	Deletes the specified hostmask from a user's list of hostmasks.
DELNOTE	1	Deletes a note matching the specified id from the specified channel. You can only delete the notes you have added.
DELUSER	1	Deletes the specified user.
QUOTE	1	This command should never be used.\nUsing it would be bad.\nHow bad?\nTry to imagine all life stopping instantaneously, and every molecule in your body exploding at the speed of light.
HELP	1	Shows help about the specified command. With no command specified, all commands that you have access to use are listed.
HISTORY	1	Shows the times that the specified channel has been manually fixed.
INFO	1	Shows all notes of the specified channel and whether the channel is actively blocked.
LISTHOSTS	1	With no username specified, it lists the hostmasks chanfix recognizes you by. Otherwise, it views the specified username's host list.\nNOTE: Server admins (+a) can only view host lists of users within their own server group. Users lower than server admin status can only view their own host lists.
OPLIST	1	Shows the top 10 op accounts and their scores for the specified channel. Append ALL or an exclamation mark (!) to list them all.
OPNICKS	1	Shows the nicknames currently opped on the specified channel.
REHASH	1	Reloads the translations, configuration, or help values based on the specified target.
RELOAD	1	Completely reloads chanfix.\nNOTE: This will subject chanfix to a reconnection which will also include a large netburst that takes time to process, so use with caution.
SCORE	1	Without extra arguments, shows the top scores of <channel>.\nOtherwise, it shows the score of either an <account> or a currently online client <=nick> for <channel>.
SET	1	Sets various configuration options to the specified values.\nBoolean settings: ENABLE_AUTOFIX, ENABLE_CHANFIX, ENABLE_CHANNEL_BLOCKING.\nInteger settings: NUM_SERVERS.
STATUS	1	Shows current statistics such as: uptime, percentage network presence, fix and block settings.
SUSPEND	1	Suspends the specified user.
UNALERT	1	Removes a channel from alert status.
UNBLOCK	1	Removes a channel from the blocklist causing it to be fixable again.
USET	1	Sets options on your account. Possible settings:\nNOTICE - receive notices, if no then PRIVMSG's will be sent.\nLANG - What language chanfix will communicate with you in; available languages: EN - English.
WHOIS	1	Shows account information for the specified user.
SETGROUP	1	Sets the group for the specified user.
SHUTDOWN	1	Saves the database and shuts GNUWorld down.
UNSUSPEND	1	Unsuspends the specified user.
WHOGROUP	1	Shows all users in the specified group along with their flags. If no argument is given, all groups are listed.
<INDEXOPER>	1	\002Oper Level\002: SCORE CSCORE CHECK HISTORY INFO OPLIST OPNICKS STATUS
<INDEXLOGGEDIN>	1	\002Logged In\002: LISTHOSTS USET WHOIS
<INDEXBLOCK>	1	\002Blocker (+b)\002: BLOCK UNBLOCK
<INDEXCOMMENT>	1	\002Commenter (+c)\002: ADDNOTE DELNOTE ALERT UNALERT
<INDEXCHANFIX>	1	\002Chanfixer (+f)\002: CHANFIX
<INDEXOWNER>	1	\002Owner (+o)\002: REHASH RELOAD SET SHUTDOWN
<INDEXSERVERADMIN>	1	\002Server Admin (+a)\002: ADDUSER DELUSER ADDHOST DELHOST ADDFLAG DELFLAG WHOGROUP SUSPEND UNSUSPEND
<INDEXUSERADMIN>	1	\002User Manager (+u)\002: ADDUSER DELUSER ADDHOST DELHOST ADDFLAG DELFLAG SETGROUP WHOGROUP SUSPEND UNSUSPEND
\.
