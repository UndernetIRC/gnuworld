-- 2001-03-02 |MrBean| "$Id: ccontrol.help.sql,v 1.20 2009/07/25 18:12:33 hidden1 Exp $"
-- Created the file and added all help topics to the database

-- 2201-03-11 |MrBean|
-- Added OP,DEOP,MODERATE,UNMODERATE,CLEARCHAN
-- Now the script first deletes previous help , and then reinsert all of the topics

-- Changed some commands. Also usage replaced with Syntax in the commands it was missing FWAP

-- Restructure help into alphabetical order, remove obselete commands, add new commands

-- Delete the current help entries

DELETE FROM help;
COPY "help" FROM stdin; 
ADDCOMMAND		1	****** ADDCOMMAND COMMAND ******
ADDCOMMAND		2	Gives permission to an oper to use a specified command
ADDCOMMAND		3	Syntax: /msg $BOT$ addcommand [-fr] <oper> <command>
ADDCOMMAND		4	-fr can only be specified by SMT+ to force addition
ADDCOMMAND		5	<oper> - the oper username you want to give the command to
ADDCOMMAND		6	<command> - the command you want to add
ADDCOMMAND		7	CODERS only: you can use /msg $BOT$ addcommand [-fr] <-allopers|-alladmins|-allsmts|-allcoders> <command>
ADDSERVER		1	****** ADDSERVER COMMAND ******
ADDSERVER		2	Adds a new server to the bot database
ADDSERVER		3	Syntax: /msg $BOT$ addserver <server>
ADDSERVER		4	*note* The server has to be linked in order for this command to work
ADDUSER		1	****** ADDUSER COMMAND ******
ADDUSER		2	Adds new oper to the bot access list
ADDUSER		3	Syntax: /msg $BOT$ adduser <username> <oper level> [server] <password>
ADDUSER		4	<username> - The username the bot will recognize the oper under
ADDUSER		5	<oper level> - Sets the initial oper commands, options are: oper, admin, smt, coder
ADDUSER		6	[server] - add the oper to the specified server, defaults to your home server
ADDUSER		7	<password> - the password of the new oper
ADDOPERCHAN		1	****** ADDOPERCHAN COMMAND ******
ADDOPERCHAN		2	Marks a channel as ircoperators only
ADDOPERCHAN		3	Syntax: /msg $BOT$ addoperchan <channel>
ADDOPERCHAN		4	thous making the bot kick and ban everyone who is not an ircop
CHANINFO		1	****** CHANINFO COMMAND ******
CHANINFO		2	Gives you all sort of information about a channel
CHANINFO		3	Syntax: /msg $BOT$ chaninfo <#channel>
CHANINFO		4	<#channel> - the channel you would like information about
CHECKNET		1	****** CHECKNET COMMAND ******
CHECKNET		2	Lists all current missing servers
CHECKNET		3	Syntax: /msg $BOT$ checknet
CLEARCHAN		1	****** CLEARCHAN COMMAND ******
CLEARCHAN		2	Clears a channel's modes
CLEARCHAN		3	Syntax: /msg $BOT$ clearchan <#channel> [mode string]
CLEARCHAN		4	<#channel> -  the channel to clear modes in
CLEARCHAN		5	[mode string] - can be ALL (for all channel modes) or any combination of ilpskmntDob
CLEARCHAN		6	o = remove all channel ops
CLEARCHAN		7	b = remove all bans
CLEARCHAN		8	*NOTE* - if you dont specify a mode string the bot will remove modes obikl
COMMANDS		1	****** COMMANDS COMMAND ****** 
COMMANDS		2	Changes a command's options
COMMANDS		3	Syntax: /msg $BOT$ commands <-ds|-en|-no|-nl|-na|-ml> <command> <option>
COMMANDS		4	-ds - disable the command
COMMANDS		5	-en - enable the command
COMMANDS		6	-no - toggles the need to be oper'd up to use the command
COMMANDS		7	-nl - toggles if the command should not be logged in the lastcom report
COMMANDS		8	-na - changes the name of the command
COMMANDS		9	-ml - set the minimum level a oper must have for this command to be added to
CONFIG		1	****** CONFIG COMMAND ******
CONFIG		2	Sets internal configuration options within the bot
CONFIG		3	Syntax: /msg $BOT$ config <option> <value>
CONFIG		4	<option> may be:
CONFIG		5	-GTime <duration> - default gline time (accepts friendly times e.g. 900s, 30m, 3h, 1d)
CONFIG		6	-VClones <amount> - threshold for warning about virtual (realname) clones
CONFIG		7	-Clones <amount> - threshold for clones on a single IP
CONFIG		8	-CClones <amount> - threshold for CIDR clones (see below)
CONFIG		9	-CClonesCIDR <size> - CIDR bit length for clones (0-32)
CONFIG		10	-CClonesGline <Yes|No> - auto-gline for CIDR clones flag
CONFIG		11	-CClonesGTime <duration> - gline duration for CIDR clones (accepts friendly times)
CONFIG		12	-IClones <amount> - threshold for CIDR ident clones
CONFIG		13	-IClonesGline <Yes|No> - auto-gline for CIDR ident clones flag
CONFIG		14	-CClonesTime <duration> - time between clone warnings to msglog (accepts friendly times)
CONFIG		15	-GBCount <count> - number of glines to set at once
CONFIG		16	-GBInterval <duration> - time between setting gline bursts (accepts friendly times)
CONFIG		17	-SGline <Yes|No> - Save glines flag
DEAUTH		1	****** DEAUTH COMMAND ******
DEAUTH		2	Deauthenticates you from the bot
DEAUTH		3	Syntax: /msg $BOT$ deauth
DEOP		1	****** DEOP COMMAND ******
DEOP		2	Deops one or more users on a specific channel
DEOP		3	Syntax: /msg $BOT$ deop <#chan> <nick1> [nick2 nick3...]
DEOP		4	<#chan> - the channel to deop the users on
DEOP		5	<nick1, nick2..> - nicks to deop
EXCEPTIONS		1	****** EXCEPTIONS COMMAND ******
EXCEPTIONS		2	Excepts users from the various glines/warnings within CONFIG
EXCEPTIONS		3	Syntax: /msg $BOT$ exceptions <add|del|list> <host mask> <connections>
EXCEPTIONS		4	<host mask> - user@host mask to except
EXCEPTIONS		5	<connections> - number of connections allowed before gline/warning
FORCECHANGLINE		1	****** FORCECHANGLINE COMMAND ******
FORCECHANGLINE		2	Glines all users on the given channel
FORCECHANGLINE		3	Syntax: /msg $BOT$ forcechangline [-u] <#channel> [duration] <reason>
FORCECHANGLINE		4	<#channel> - the channel name to gline
FORCECHANGLINE		5	[duration] - Duration of the gline, may have s,m,h,d for secs,mins,hours,days
FORCECHANGLINE		6	<reason> - reason for the gline
FORCECHANGLINE		7	[-u] - only glines the unidented clients found on the channel with ~*@IP glines
FORCECHANGLINE		8	This type of chan gline is limited for 24 hours and cannot be issued on channels with opers or channels which are marked as nomode channels
FORCEGLINE		1	****** FORCEGLINE COMMAND ******
FORCEGLINE		2	Glines a host for a certain amount of time
FORCEGLINE		3	Syntax: /msg $BOT$ forcegline [-fu] <nick|user@host> <duration[s/m/h/d]> <reason>
FORCEGLINE		4	-fu flag can only be specified by SMT+ (used to force)
FORCEGLINE		5	<nick|user@host> - the nick or user@host to gline (user@host supports CIDR)
FORCEGLINE		6	<duration> - duration of the gline (supports s,m,h,d for secs,mins,hours,days)
FORCEGLINE		7	<reason> - the reason for the gline
FORCEGLINE		8	This type of gline is used to gline for more than 14 days and/or for a gline which affects more than 255 users.
GCHAN		1	****** GCHAN COMMAND ******
GCHAN		2	Sets a bad channel gline on a channel
GCHAN		3	Syntax: /msg $BOT$ gchan <#channel> <duration|-per> <reason>
GCHAN		4	<#channel> - the channel to badchannel gline
GCHAN		5	<duration> - duration of the gline (-per for permanent)
GCHAN		6	<reason> - reason for the badchannel gline
GLINE		1	****** GLINE COMMAND ******
GLINE		2	glines a certain host from the network
GLINE		3	Syntax: /msg $BOT$ gline <user@host> [duration] <reason>
GLINE		4	<user@host> - the host to gline (CIDR not supported)
GLINE		5	<duration> - the duration of the gline (supports s,m,h,d for secs,mins,hours,days)
GLINE		6	<reason> - the reason for the gline
HELP		1	****** HELP COMMAND ******
HELP		2	Shows help on commands (like this one!)
HELP		3	Syntax: /msg $BOT$ help [command]
HELP		4	If command is empty, this command shows the commands you have access to.
INVITE		1	****** INVITE COMMAND ******
INVITE		2	Makes the bot invite you to an invite only channel
INVITE		3	Syntax: /msg $BOT$ invite <#channel>
INVITE		4	<#channel> - the channel to invite you to
JUPE		1	****** JUPE COMMAND ******
JUPE		2	Jupe a server (prevent it connecting to the network)
JUPE		3	Syntax: /msg $BOT$ jupe <servername> <reason>
JUPE		4	<servername> - the name of the server to jupe
JUPE		5	<reason> - reason for the jupe
KICK		1	****** KICK COMMAND ******
KICK		2	Makes the bot kick a user from a channel
KICK		3	Syntax: /msg $BOT$ kick <#channel> <nick> <reason>
KICK		4	<#channel> - the channel to kick the user from
KICK		5	<nick> - the nick of the user you want to kick
KICK		6	<reason> - reason for the kick
LASTCOM		1	****** LASTCOM COMMAND ******
LASTCOM		2	This command will paste all the last commands which where issued to the bot
LASTCOM		3	Syntax: /msg $BOT$ lastcom [numcommands] [from days ago - d]
LASTCOM		4	[numcommands] - The number of commands to show (default: 20)
LASTCOM		5	You can also specify the number of days ago
LASTCOMM		1	See /msg $BOT$ help lastcom
LEARNNET		1	****** LEARNNET COMMAND ******
LEARNNET		2	Adds all the unknown servers which are currently connected to the network to the bot database
LEARNNET		3	Syntax: /msg $BOT$ learnnet
LIST		1	****** LIST COMMAND ******
LIST		2	This command lists various internal lists
LIST		3	Syntax: /msg $BOT$ list <type>
LIST		4	<type> - glines, servers, badchannels, exceptions, channels, shells
LIST		5	 (see /msg $BOT$ help list channels for more info on channels)
LIST	CHANNELS	1	****** LIST CHANNELS COMMAND ******
LIST	CHANNELS	2	This command lists channels based on various criteria
LIST	CHANNELS	3	Syntax: /msg $BOT$ list channels <type>
LIST	CHANNELS	4	<type> may be:
LIST	CHANNELS	5	key <key> - list all channels with the specified key (supports wildcards)
LIST	CHANNELS	6	modes <modes> - list all channels with the modes, you may explicitly specify + or - modes
LIST	CHANNELS	7	topic <topic> - list all channels with a topic matching the specified topic (supports wildcards)
LIST	CHANNELS	8	topicby <nick!user@host> - list all channels with topics set by nick!user@host (supports wildcards)
LISTHOSTS		1	****** LISTHOSTS COMMAND ******
LISTHOSTS		2	Lists all the hosts that an oper is allowed to login from
LISTHOSTS		3	Syntax: /msg $BOT$ listhosts <username>
LISTHOSTS		4	<username> - the username to lookup
LISTIGNORES		1	****** LISTIGNORES COMMAND ******
LISTIGNORES		2	Shows the current ignore list of the bot
LISTIGNORES		3	Syntax: /msg $BOT$ listignores
LISTOPERCHANS		1	****** LISTOPERCHANS COMMANDS ******
LISTOPERCHANS		2	Shows a list of the channels that are marked as ircops only
LISTOPERCHANS		3	Syntax: /msg $BOT$ listoperchans
LOGIN		1	****** LOGIN COMMAND ******
LOGIN		2	Authenticates you with the bot
LOGIN		3	Syntax: /msg $BOT$ login <username> <password>
LOGIN		4	<handle> - your username on the bot
LOGIN		5	<password> - your password
LOGOUT		1	See /msg $BOT$ help deauth
MAXUSERS		1	****** MAXUSERS COMMANDS ******
MAXUSERS		2	Shows the peak user level as seen by the bot
MAXUSERS		3	Syntax: /msg $BOT$ maxusers
MODE		1	****** MODE COMMAND ******
MODE		2	Makes the bot change a channel's modes
MODE		3	Syntax: /msg $BOT$ mode <#channel> <mode string>
MODE		4	<#channel> -  the channel to change the mode in
MODE		5	<mode string> - the string that contains the mode
MODERATE		1	****** MODERATE COMMAND ******
MODERATE		2	Moderates a channel
MODERATE		3	Syntax: /msg $BOT$ moderate <#channel>
MODERATE		4	<#channel> - the channel to moderate
MODUSER		1	****** MODUSER COMMAND ******
MODUSER		2	Modify an existing bot user
MODUSER		3	Syntax: /msg $BOT$ moduser <username> [-ah new host] [-dh host] [-s server] [-p password] [-gl on|off]
MODUSER		4	  [-op on|off] [-ua] [-uf flags] [-e email] [-mt m|n]
MODUSER		5	<username> - username to modify
MODUSER		6	[-ah host] - adds a new host for the user
MODUSER		7	[-dh host] - removes a host for a user
MODUSER		8	[-s server] - associate the user with a server
MODUSER		9	[-p password] - changes the user's password
MODUSER		10	[-gl on|off] - toggles whether the logs will be messaged to the user
MODUSER		11	[-op on|off] - toggles if a user must be oper'd up to use the needop commands
MODUSER		12	[-ua] - resets the user access acording to his flags
MODUSER		13	[-uf flags] - updates the oper flags to new one (OPER|ADMIN|SMT|CODER)
MODUSER		14	[-e email] - updates the user's email
MODUSER		15	[-mt m|n] - change the way the bot communicates to the user (m=message,n=notice)
MODUSER		16	[-glag on|off] - toggles whether the server lag reports will be messaged to the user
NEWPASS		1	****** NEWPASS COMMAND ******
NEWPASS		2	Changes your password for the bot
NEWPASS		3	Syntax: /msg $BOT$ newpass <newpass>
NOMODE		1	****** NOMODE COMMAND ******
NOMODE		2	Managed the nomode list
NOMODE		3	Syntax: /msg $BOT$ nomode <#channel> <reason>
NOMODE		4	<#channel> - channel to ban modes from
NOMODE		5	<reason> - reason to give when someone tries
OP		1	****** OP COMMAND ******
OP		2	Op users on a specific channel
OP		3	Syntax: /msg $BOT$ op <#channel> <nick1> [nick2 nickN...]
OP		4	<#channel> - channel to op the user(s) on
OP		5	<nick1 nick2...> - users to op
REMCOMMAND		1	****** DELCOMMAND COMMAND ******
REMCOMMAND		2	Removes an oper's access to a certain command
REMCOMMAND		3	Syntax: /msg $BOT$ remcommand <command> <username>
REMCOMMAND		4	<command> - the command to remove access to
REMCOMMAND		5	<username> - the username of the oper you want to remove the command from
REMCOMMAND		6	CODERS only: you can use /msg $BOT$ remcommand <-allopers|-alladmins|-allsmts|-allcoders> <command>
REMGCHAN		1	****** REMGCHAN COMMAND ******
REMGCHAN		2	Removes a gchan badchannel gline
REMGCHAN		3	Syntax: /msg $BOT$ remgchan <#channel>
REMGCHAN		4	<#channel> - channel to remove from badchannel gline list
REMGLINE		1	****** REMGLINE COMMAND ******
REMGLINE		2	Removes a gline on a user@host
REMGLINE		3	Syntax: /msg $BOT$ remgline <user@host>
REMGLINE		4	<user@host> - the host to remove the gline for
REMIGNORE		1	****** REMIGNORE COMMAND ******
REMIGNORE		2	Removes a host from the bot's ignore list
REMIGNORE		3	Syntax: /msg $BOT$ remignore <host>
REMIGNORE		4	<host> - the host to remove from the ignore list
REMOPERCHAN		1	****** REMOPERCHAN COMMAND ******
REMOPERCHAN		2	Unmarks a channel as ircop only
REMOPERCHAN		3	Syntax: /msg $BOT$ remoperchan <#channel>
REMOPERCHAN		4	<#channel> - the channel you want to unmark as operchan
REMSERVER		1	****** REMSERVER COMMAND ******
REMSERVER		2	Removes a server from the bot's database
REMSERVER		3	Syntax: /msg $BOT$ remserver <server name>
REMSERVER		4	NOTE: use with caution!
REMSGLINE		1	****** REMSGLINE COMMAND ******
REMSGLINE		2	Removes an SGLINE from the network
REMSGLINE		3	Syntax: /msg $BOT$ remsgline <user@host>
REMSGLINE		4	<user@host> - the user@host to remove from the sgline list
REMUSER		1	****** REMOVEOPER COMMAND ******
REMUSER		2	Removes an oper from the bot's access list
REMUSER		3	Syntax: /msg $BOT$ remuser <username>
REMUSER		4	<username> - username of the oper you want to remove
REOP		1	****** REOP COMMAND ******
REOP		2	Removes all channel ops from a channel and ops the specified nick
REOP		3	Syntax: /msg $BOT$ reop <#channel> <nick>
REOP		4	<#channel> - channel to perform reop in
REOP		5	<nick> - nick to be opped once channel is deopped
SAY		1	****** SAY COMMAND ******
SAY		2	Force the bot (or server) to talk in a channel or to a nick
SAY		3	Syntax: /msg $BOT$ say <-s|-b> <#channel|nick> <message>
SAY		4	<-s> - Message will originate from the server
SAY		5	<-b> - Message will originate from the bot ($BOT$)
SAY		6	<#channel|nick> - the channel or nick to receive the message
SAY		7	<message> - the message to send
SCAN		1	****** SCAN COMMAND ******
SCAN		2	Scans the network for users matching the required criteria
SCAN		3	Syntax: /msg $BOT$ scan <-fh user@host | -h user@host> [-v] [-i]
SCAN		4	Syntax: /msg $BOT$ scan <-n realname> [-v] [-i]
SCAN		5	<-h user@host> - match user@host/host/IP mask (supports CIDR and wildcards)
SCAN		6	<-fh user@host> - match user@host (masked host only) (supports wildcards)
SCAN		7	<-n realname> - match the specified realname (supports wildcards)
SCAN		8	[-v] - display the first 15 matching users (rather than just a count)
SCAN		9	[-i] - perform an ident report (listing number of unique idents found within the match)
SCANGLINE		1	****** SCANGLINE COMMAND ******
SCANGLINE		2	Scans the gline list for a gline matching a certain host
SCANGLINE		3	Syntax: /msg $BOT$ scangline <host mask>
SCANGLINE		4	<host mask> - the host mask to lookup
SCHANGLINE		1	****** SCHANGLINE COMMAND ******
SCHANGLINE		2	Glines all users on the given channel
SCHANGLINE		3	Syntax: /msg $BOT$ schangline [-u] <#channel> [duration] <reason>
SCHANGLINE		4	<#channel> - the channel name to gline
SCHANGLINE		5	[duration] - Duration of the gline, may have s,m,h,d for secs,mins,hours,days
SCHANGLINE		6	<reason> - reason for the gline
SCHANGLINE		7	[-u] - only glines the unidented clients found on the channel with ~*@IP glines
SGLINE		1	****** SGLINE COMMAND ******
SGLINE		2	gline a user@host with relaxed restrictions
SGLINE		3	Syntax: /msg $BOT$ sgline <user@host> [duration] <reason>
SGLINE		4	<user@host> - the user@host to gline from the network
SGLINE		5	[duration] - duration of the gline, supports s,m,h,d for secs,mins,hours,days
SGLINE		6	<reason> - reason for the gline
SHELLS		1	****** SHELLS COMMAND ******
SHELLS		2	Add/remove/modify shells limits/exceptions
SHELLS		3	Syntax: /msg $BOT$ SHELLS <addcompany|addnetblock|delcompany|delnetblock|list|chlimit|chname|clearall> <Shell company|Netblock> <max connections>
SHELLS		4	<Shell company>: The name of the shell company
SHELLS		5	<Netblock>: CIDR netblock
SHELLS		6	<max connections>: The number of connections allowed before gline
SHUTDOWN		1	****** SHUTDOWN COMMAND ******
SHUTDOWN		2	Shutdown the service and exit
SHUTDOWN		3	Syntax: /msg $BOT$ shutdown <reason>
SHUTDOWN		4	<reason> - the reason to use in the /squit message of the server
STATUS		1	****** STATUS COMMAND ******
STATUS		2	Show all sorts of debug and status information about the bot and server
STATUS		3	Syntax: /msg $BOT$ status
SUSPEND		1	****** SUSPEND COMMAND ******
SUSPEND		2	Suspends an oper from the bot for a certain amount of time
SUSPEND		3	Syntax: /msg $BOT$ suspend <username> <duration> [-l level] <reason>
SUSPEND		4	<username> - username to suspend
SUSPEND		5	<duration> - the duration of the suspend (supports s,m,h,d for secs,mins,hours,days)
SUSPEND		6	[-l level] - level at which the user is suspended (required minimum to unsuspend)
SUSPEND		7	<reason> - reason for suspension
TRANSLATE		1	****** TRANSLATE COMMAND ******
TRANSLATE		2	Translates a numeric into a nick
TRANSLATE		3	Syntax: /msg $BOT$ translate <numeric>
TRANSLATE		4	<numeric> - the numeric you want to translate
UNJUPE		1	****** UNJUPE COMMAND ******
UNJUPE		2	Removes a server jupe previously set by the jupe command
UNJUPE		3	Syntax: /msg $BOT$ unjupe <server>
UNJUPE		4	<server> - server name to unjupe
UNMODERATE		1	****** UNMODERATE COMMAND ******
UNMODERATE		2	UnModerates a channel previously moderated with 'moderate'
UNMODERATE		3	Syntax: /msg $BOT$ unmoderate <#channel>
UNMODERATE		4	<#channel> - the channel to stop moderating
UNSUSPEND		1	****** UNSUSPEND COMMAND ******
UNSUSPEND		2	UNSuspend an oper previously suspended
UNSUSPEND		3	Syntax: /msg $BOT$ unsuspend <username>
UNSUSPEND		4	<username> - username to unsuspend
USERINFO		1	****** USERINFO COMMAND ******
USERINFO		2	Shows information about all the opers that their server/handle match the supplied mask
USERINFO		3	Syntax: /msg $BOT$ userinfo <server|username> [-cl]
USERINFO		4	<server|username> - the server or username to lookup (supports wildcards)
USERINFO		5	[-cl] - If this is specified, the command will also list all commands available to each user matched
WHOIS		1	****** WHOIS COMMAND ******
WHOIS		2	Gives information about a user on the network
WHOIS		3	Syntax: /msg $BOT$ whois <nick>
WHOIS		4	<nick> - the person you want to get info about
