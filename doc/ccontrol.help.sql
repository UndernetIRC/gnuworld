-- 2001-03-02 |MrBean| "$Id: ccontrol.help.sql,v 1.7 2002/01/02 22:22:21 mrbean_ Exp $"
-- Created the file and added all help topics to the database

-- 2201-03-11 |MrBean|
-- Added OP,DEOP,MODERATE,UNMODERATE,CLEARCHAN
-- Now the script first deletes previous help , and then reinsert all of the topics

-- Changed some commands. Also usage replaced with Syntax in the commands it was missing FWAP

-- Delete the current help entries

DELETE FROM help;
COPY "help" FROM stdin; 
ACCESS		1	****** ACCESS COMMAND ******
ACCESS		2	Shows the bot access list
ACCESS		3	Syntax: /msg $BOT$ access
ACCESS		4	Note: the access is shown as bit mask of commands
ADDCOMMAND		1	****** ADDCOMMAND COMMAND ******
ADDCOMMAND		2	Gives a permission to oper to use specified command
ADDCOMMAND		3	Syntax: /msg $BOT$ addcommand [-fr] <oper> <command>
ADDCOMMAND		4	-fr can only be specified by smt+ and cause the command to be added
ADDCOMMAND		5	<oper> - the oper handle you wand to give permission
ADDCOMMAND		6	<command> - the command you want to add
ADDUSER		1	****** ADDUSER COMMAND ******
ADDUSER		2	Adds new oper to the bot access list
ADDUSER		3	Syntax: /msg $BOT$ adduser <handle> <oper level> <server> <password>
ADDUSER		4	<handle> - The handle the bot will recognize the oper under
ADDUSER		5	<oper level> - Sets the initial oper commands , options are : oper , admin , smt , coder
ADDUSER		6	<password> - the password of the new oper
ADDOPERCHAN		1	****** ADDOPERCHAN COMMAND ******
ADDOPERCHAN		2	Marks a channel as ircoperators only
ADDOPERCHAN		3	Syntax: /msg $BOT$ addoperchan <channel>
ADDOPERCHAN		4	thous making the bot kick and ban everyone who is not an ircop
CHANINFO		1	****** CHANINFO COMMAND ******
CHANINFO		2	Gives you all sort of information about a channel
CHANINFO		3	Syntax: /msg $BOT$ chaninfo <channel>
CLEARCHAN		1	****** CLEARCHAN COMMAND ******
CLEARCHAN		2	Clears channel modes
CLEARCHAN		3	Syntax: /msg $BOT$ clearchan <#channel> [mode string]
CLEARCHAN		4	<#channel> -  the channel to clear modes from
CLEARCHAN		5	<mode string> - can be ALL (for all channel modes) or any combination of ilpskmntob
CLEARCHAN		6	o = remove all channel ops
CLEARCHAN		7	b = remove all bans
CLEARCHAN		8	*NOTE* - if you dont specify a mode string the bot will remove modes obikl
DEAUTH		1	****** DEAUTH COMMAND ******
DEAUTH		2	Deauthenticates you with the bot
DEAUTH		3	Syntax: /msg $BOT$ deauth
DEOP		1	****** DEOP COMMAND ******
DEOP		2	Deops users on a specific channel
DEOP		3	Syntax: /msg $BOT$ deop <#chan> <nick1> [nick2] [nick3] ..
REMCOMMAND		1	****** DELCOMMAND COMMAND ******
REMCOMMAND		2	Removes an oper access to a certain command
REMCOMMAND		3	Syntax: /msg $BOT$ remcommand <command> <handle>
REMCOMMAND		4	<command> - the command to delete
REMCOMMAND		5	<handle> - the handle of the oper you want to remove the command to
GLINE		1	****** GLINE COMMAND ******
GLINE		2	Glines a certain host
GLINE		3	Syntax: /msg $BOT$ gline <user@host> [duration] <reason>
GLINE		4	<duration> - the duriation in second to gline the host
GLINE		5	<user@host> - the host to gline
GLINE		6	<reason> - the reason for the gline
HELP		1	****** HELP COMMAND ******
HELP		2	Shows help on commands
HELP		3	Syntax: /msg $BOT$ help [command]
INVITE		1	------ INVITE COMMAND ------
INVITE		2	Make the bot invite you to an invite only channel
INVITE		3	Syntax: /msg $BOT$ invite <chan>
JUPE		1	****** JUPE COMMAND ******
JUPE		2	Jupe a server
JUPE		3	Syntax: /msg $BOT$ jupe <servername> <reason>
JUPE		4	<servername> - the name of the server to jupe
JUPE		5	<reason> - reason for the jupe
KICK		1	****** KICK COMMAND ******
KICK		2	Makes  the bot kick a user from a channel
KICK		3	Syntax: /msg $BOT$ kick <#channel> <nick> <reason>
KICK		4	<#channel> - the channel to kick the user from
KICK		5	<nick> - the nick of the user you want to kick
KICK		6	<reason> - reason for the kick
LISTOPERCHANS		1	****** LISTOPERCHANS COMMANDS ******
LISTOPERCHANS		2	Shows a list of the channels that are marked as ircops only
LISTOPERCHANS		3	Syntax: /msg $BOT$ listoperchans
LOGIN		1	****** LOGIN COMMAND ******
LOGIN		2	Authenticates you with the bot
LOGIN		3	Syntax: /msg $BOT$ login <handle> <pass>
LOGIN		4	<handle> - your handle on the bot
LOGIN		5	<password> - your password
MODE		1	****** MODE COMMAND ******
MODE		2	Makes the bot change channel modes
MODE		3	Syntax: /msg $BOT$ mode <#channel> <mode string>
MODE		4	<#channel> -  the channel to change the mode in
MODE		5	<mode string> - the string that contains the mode
MODE		6	Ex : /msg k mode #coder-com +o |MrBean|
MODERATE		1	****** MODERATE COMMAND ******
MODERATE		2	Moderates a channel
MODERATE		3	Syntax: /msg $BOT$ moderate <#channel>
MODERATE		4	<#channel> -  the channel to moderate
MODUSER		1	****** MODUSER COMMAND ******
MODUSER		2	Modify an existing user
MODUSER		3	Syntax: /msg $BOT$ moduser <handle> <-ah new host] <-dh host> <-s server> <-p password> <-gl on/off> <-op on/off> <-ua> <-uf new flags> <-e email> <-mt m/n>
MODUSER		4	-ah adds a new host for the user
MODUSER		5	-dh removes a host for a user
MODUSER		6	-s associate a user with the server;
MODUSER		7	-p changes the user password
MODUSER		8	-gl toggeles wheather the logs will be messaged to the user
MODUSER		9	-op toggeles if a user must be opered up to user the needop commands
MODUSER		10	-ua resets the user access acording to his flags
MODUSER		11	-uf updates the oper flags to new one (OPER/ADMIN/SMT/CODER)
MODUSER		12	-e updates the user email
MODUSER		13	-mt change the way the bot communicates (m/message/n/notice)
NEWPASS		1	****** NEWPASS COMMAND ******
NEWPASS		2	Changes your password
NEWPASS		2	Syntax: /msg $BOT$ newpass <newpass>
OP		1	****** OP COMMAND ******
OP		2	Op users on a specific channel
OP		3	Syntax: /msg $BOT$ op <#chan> <nick1> [nick2] [nick3] ..
REMGLINE		1	****** REMGLINE COMMAND ******
REMGLINE		2	Removes a gline on a host
REMGLINE		3	Syntax: /msg $BOT$ remgline <user@host>
REMGLINE		4	<user@host> - the host to remove the gline for
REMOPERCHAN		1	****** REMOPERCHAN COMMAND ******
REMOPERCHAN		2	Unmarks a channel as ircop only
REMOPERCHAN		3	Syntax: /msg $BOT$ remoperchan <#channel>
REMOPERCHAN		4	<#channel> - the channel you want to unmark as operchan
REMUSER		1	****** REMOVEOPER COMMAND ******
REMUSER		2	Removes an oper from the bot access list
REMUSER		3	Syntax: /msg $BOT$ remuser <handle>
REMUSER		4	<handle> - handle of the oper you want to remove
SCANGLINE		1	****** SCANGLINE COMMAND ******
SCANGLINE		2	Scans the gline list for a gline matching a certain host
SCANGLINE		3	Syntax: /msg $BOT$ scangline <host mask>
SUSPEND		1	****** SUSPEND COMMAND ******
SUSPEND		2	Suspend an oper for a certain amount of time
SUSPEND		3	Syntax: /msg $BOT$ suspend <oper> <duration>[s/m/h/d] [-l level] <time units>
SUSPEND		4	<oper> - oper handle to suspend
SUSPEND		5	<duration> - the duration of the suspend
SUSPEND		6	<time units> - the unit in which you specify duriation
SUSPEND		7	time units can be : s - seconds , m - minutes , h - hours , d- days
TRANSLATE		1	****** TRANSLATE COMMAND ******
TRANSLATE		2	Translates a numeric into a nick
TRANSLATE		3	Syntax: /msg $BOT$ translate <numeric>
TRANSLATE		4	the numeric you want to translate
UNMODERATE		1	****** UNMODERATE COMMAND ******
UNMODERATE		2	UnModerates a channel
UNMODERATE		3	Syntax: /msg $BOT$ unmoderate <#channel>
UNMODERATE		4	<#channel> -  the channel to moderate
UNSUSPEND		1	****** UNSUSPEND COMMAND ******
UNSUSPEND		2	UNSuspend an oper
UNSUSPEND		3	Syntax: /msg $BOT$ unsuspend <oper>
UNSUSPEND		4	<oper> - oper handle to unsuspend
WHOIS		1	****** WHOIS COMMAND ******
WHOIS		2	Gives information about a user
WHOIS		3	Syntax: /msg $BOT$ whois <nick>
WHOIS		4	<nick> - the person you want to get info about
LISTHOSTS		1	****** LISTHOSTS COMMAND ******
LISTHOSTS		2	Lists all the hosts that an oper is allowed to login from
LISTHOSTS		3	Syntax: /msg $BOT$ listhosts <handle>
CHECKNET		1	****** CHECKNET COMMAND ******
CHECKNET		2	Lists all the missing servers
CHECKNET		3	Syntax: /msg $BOT$ checknet
LISTIGNORES		1	****** LISTIGNORES COMMAND ******
LISTIGNORES		2	Shows the current ignore list of the bot
LISTIGNORES		3	Syntax: /msg $BOT$ listignores
REMIGNORE		1	****** REMIGNORE COMMAND ******
REMIGNORE		2	Removes a host from the bot ignore list
REMIGNORE		3	Syntax: /msg $BOT$ remignore <host>
COMMANDS		1	****** COMMANDS COMMAND ******
COMMANDS		2	Changes command options'
COMMANDS		3	Syntax: /msg $BOT$ commands <-ds/-en/-no/-nl/-na/-ml/> <command> <option>
COMMANDS		4	-ds - disable the command
COMMANDS		5	-en - enable the command
COMMANDS		6	-no - toggles the need to be operd up to use the command
COMMANDS		7	-nl - toggles if the command should not be logged in the lastcom report
COMMANDS		8	-na - changes the name of the command
COMMANDS		9	-ml - set the minimum level a oper must have for this command to be added to
GCHAN		1	****** GCHAN COMMAND ******
GCHAN		2	Sets a bad channel gline on a channel
GCHAN		3	Syntax: /msg $BOT$ Gchan <#channel> <duration/-per> <reason>');
GCHAN		4	if a -per is specified as a duration, the gline will be permanent
REMCHAN		1	****** REMGCHAN COMMAND ******
REMCHAN		2	Removes a gchan gline
REMCHAN		3	Syntax: /msg $BOT$ remgchan <#channel>
LIST		1	****** LIST COMMAND ******
LIST		2	Types can be either Glines, suspended or servers
LIST		3	Syntax: /msg $BOT$ list <list type>
USERINFO		1	****** USERINFO COMMAND ******
USERINFO		2	Shows information about all the opers that their server/handle match the supplied mask
USERINFO		3	Syntax: /msg $BOT$ userinfo <user*/servers*>
LEARNNET		1	****** LEARNNET COMMAND ******
LEARNNET		2	Add all the unknown server which currently are connected to the network to the database
LEARNNET		3	Syntax: /msg $BOT$ learnnet
REMSERVER		1	****** REMSERVER COMMAND ******
REMSERVER		2	Removes a server from the bot database
REMSERVER		3	Syntax: /msg $BOT$ remserver <server name>
LASTCOM		1	****** LASTCOM COMMAND ******
LASTCOM		2	will paste all the last commands which where issued to the bot
LASTCOM		3	Syntax: /msg $BOT$ lastcom [num of commands ] [from days ago - d]
LASTCOM		4	the default value is 20 if no num of commands is specified
LASTCOM		5	a user can also specify the number of days ago the commands where issued
FORCEGLINE		1	****** FORCEGLINE COMMAND ******
FORCEGLINE		2	Glines a host for a certain amount of time
FORCEGLINE		3	Syntax: /msg $BOT$ forcegline [-fu] <host> <duration[s/m/h/d] reason
FORCEGLINE		4	-fu flag can only be specified by smt+
FORCEGLINE		5	This type of gline is used to gline for more than 14 days and for a gline which affects more than 255 users.
EXCEPTIONS		1	****** EXCEPTIONS COMMAND ******
EXCEPTIONS		2	Excpetions = AGL in uworld, meaning a user can have more than a certain
EXCEPTIONS		3	Syntax: /msg $BOT$ Exceptions <add/del/list> [host mask] <connections>
EXCEPTIONS		4	amount of connections from a single host than the default one which is specified
EXCEPTIONS		5	in the bot conf file, before he gets automatically glined for clones.
ADDSERVER		1	****** ADDSERVER COMMAND ******
ADDSERVER		2	Adds a new server to the bot database
ADDSERVER		3	Syntax: /msg $BOT$ addserver <server>
ADDSERVER		4	*note* The server need to be linked in order for this command to work
STATUS		1	****** STATUS COMMAND ******
STATUS		2	Show all sorts of debug information
STATUS		3	Syntax: /msg $BOT$ status
