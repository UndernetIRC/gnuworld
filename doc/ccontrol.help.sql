-- 2001-03-02 |MrBean| "$Id: ccontrol.help.sql,v 1.5 2001/12/27 09:02:58 mrbean_ Exp $"
-- Created the file and added all help topics to the database

-- 2201-03-11 |MrBean|
-- Added OP,DEOP,MODERATE,UNMODERATE,CLEARCHAN
-- Now the script first deletes previous help , and then reinsert all of the topics

-- Delete the current help entries

Delete FROM help;

-- ACCESS 

INSERT INTO help (command,line,help) VALUES ('ACCESS',1,'------ ACCESS COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('ACCESS',2,'Shows the bot access list');
INSERT INTO help (command,line,help) VALUES ('ACCESS',3,'Note: the access is shown as bit mask of commands');
INSERT INTO help (command,line,help) VALUES ('ACCESS',4,'Usage : /msg $BOT$ access');

-- ADDCOMMAND 

INSERT INTO help (command,line,help) VALUES ('ADDCOMMAND',1,'------ ADDCOMMAND COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('ADDCOMMAND',2,'Gives a permission to oper to use specified command');
INSERT INTO help (command,line,help) VALUES ('ADDCOMMAND',3,'Usage : /msg $BOT$ addcommand [-fr] <oper> <command>');
INSERT INTO help (command,line,help) VALUES ('ADDCOMMAND',4,'-fr can only be specified by smt+ and cause the command to be added');
INSERT INTO help (command,line,help) VALUES ('ADDCOMMAND',5,'<oper> - the oper handle you wand to give permission');
INSERT INTO help (command,line,help) VALUES ('ADDCOMMAND',6,'<command> - the command you want to add');

-- ADDNEWOPER

INSERT INTO help (command,line,help) VALUES ('ADDOPER',1,'------ ADDNEWOPER COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('ADDOPER',2,'Adds new oper to the bot access list');
INSERT INTO help (command,line,help) VALUES ('ADDOPER',3,'Usage : /msg $BOT$ addnewoper <handle> <oper level> <password>');
INSERT INTO help (command,line,help) VALUES ('ADDOPER',4,'<handle> - The handle the bot will recognize the oper under');
INSERT INTO help (command,line,help) VALUES ('ADDOPER',5,'<oper level> - Sets the initial oper commands , options are : oper , admin , coder');
INSERT INTO help (command,line,help) VALUES ('ADDOPER',6,'<password> - the password of the new oper');

-- ADDOPERCHAN

INSERT INTO help (command,line,help) VALUES ('ADDOPERCHAN',1,'------ ADDOPERCHAN COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('ADDOPERCHAN',2,'Marks a channel as ircoperators only');
INSERT INTO help (command,line,help) VALUES ('ADDOPERCHAN',3,'thous making the bot kick and ban everyone who is not an ircop');
INSERT INTO help (command,line,help) VALUES ('ADDOPERCHAN',4,'Usage : /msg $BOT$ addoperchan <channel>');

-- CHANINFO

INSERT INTO help (command,line,help) VALUES ('CHANINFO',1,'------ CHANINFO COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('CHANINFO',2,'Gives you all sort of information about a channel');
INSERT INTO help (command,line,help) VALUES ('CHANINFO',3,'Usage : /msg $BOT$ chaninfo <channel>');

-- CLEARCHAN

INSERT INTO help (command,line,help) VALUES ('CLEARCHAN',1,'------ CLEARCHAN COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('CLEARCHAN',2,'Clears channel modes ');
INSERT INTO help (command,line,help) VALUES ('CLEARCHAN',3,'Usage : /msg $BOT$ clearchan <#channel> [mode string]');
INSERT INTO help (command,line,help) VALUES ('CLEARCHAN',4,'<#channel> -  the channel to moderate');
INSERT INTO help (command,line,help) VALUES ('CLEARCHAN',5,'<mode string> - can be ALL (for all channel modes) or any combination of ilpskmntob');
INSERT INTO help (command,line,help) VALUES ('CLEARCHAN',6,'o = remove all channel ops');
INSERT INTO help (command,line,help) VALUES ('CLEARCHAN',7,'b = remove all bans');
INSERT INTO help (command,line,help) VALUES ('CLEARCHAN',8,'*NOTE* - if you dont specify a mode string the bot will remove modes obikl');


-- DEAUTH 

INSERT INTO help (command,line,help) VALUES ('DEAUTH',1,'------ DEAUTH COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('DEAUTH',2,'Deauthenticates you with the bot');
INSERT INTO help (command,line,help) VALUES ('DEAUTH',3,'Usage : /msg $BOT$ deauth');

-- DEOP

INSERT INTO help (command,line,help) VALUES ('DEOP',1,'------ DEOP COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('DEOP',2,'Deops users on a specific channel');
INSERT INTO help (command,line,help) VALUES ('DEOP',3,'Usage : /msg $BOT$ deop <#chan> <nick1> [nick2] [nick3] .. ');

-- DELCOMMAND

INSERT INTO help (command,line,help) VALUES ('DELCOMMAND',1,'------ DELCOMMAND COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('DELCOMMAND',2,'Removes an oper access to a certain command');
INSERT INTO help (command,line,help) VALUES ('DELCOMMAND',3,'Usage : /msg $BOT$ delcommand <command> <handle>');
INSERT INTO help (command,line,help) VALUES ('DELCOMMAND',4,'<command> - the command to delete');
INSERT INTO help (command,line,help) VALUES ('DELCOMMAND',5,'<handle> - the handle of the oper you want to remove the command to');

-- GLINE

INSERT INTO help (command,line,help) VALUES ('GLINE',1,'------ GLINE COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('GLINE',2,'Glines a certain host');
INSERT INTO help (command,line,help) VALUES ('GLINE',3,'Usage : /msg $BOT$ [duration (sec)] <user@host> <reason>');
INSERT INTO help (command,line,help) VALUES ('GLINE',4,'<duration> - the duriation in second to gline the host');
INSERT INTO help (command,line,help) VALUES ('GLINE',5,'<user@host> - the host to gline');
INSERT INTO help (command,line,help) VALUES ('GLINE',6,'<reason> - the reason for the gline');

-- HELP

INSERT INTO help (command,line,help) VALUES ('HELP',1,'------ HELP COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('HELP',2,'Shows help on commands');
INSERT INTO help (command,line,help) VALUES ('HELP',3,'Usage : /msg $BOT$ help [command]');

-- INVITE

INSERT INTO help (command,line,help) VALUES ('INVITE',1,'------ INVITE COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('INVITE',2,'Make the bot invite you to an invite only channel');
INSERT INTO help (command,line,help) VALUES ('INVITE',3,'Usage : /msg $BOT$ invite <chan>');

-- JUPE

INSERT INTO help (command,line,help) VALUES ('JUPE',1,'------ JUPE COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('JUPE',2,'Jupe a server');
INSERT INTO help (command,line,help) VALUES ('JUPE',3,'Usage : /msg $BOT$ jupe <servername> <reason>');
INSERT INTO help (command,line,help) VALUES ('JUPE',4,'<servername> - the name of the server to jupe');
INSERT INTO help (command,line,help) VALUES ('JUPE',5,'<reason> - reason for the jupe');


-- KICK 

INSERT INTO help (command,line,help) VALUES ('KICK',1,'------ KICK COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('KICK',2,'Makes  the bot kick a user from a channel');
INSERT INTO help (command,line,help) VALUES ('KICK',3,'Usage : /msg $BOT$ kick <#channel> <nick> <reason>');
INSERT INTO help (command,line,help) VALUES ('KICK',4,'<#channel> - the channel to kick the user from');
INSERT INTO help (command,line,help) VALUES ('KICK',5,'<nick> - the nick of the user you want to kick');
INSERT INTO help (command,line,help) VALUES ('KICK',6,'<reason> - reason for the kick');

-- LISTOPERCHANS

INSERT INTO help (command,line,help) VALUES ('LISTOPERCHANS',1,'------ LISTOPERCHANS COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('LISTOPERCHANS',2,'Shows a list of the channels that are marked as ircops only');

-- LOGIN

INSERT INTO help (command,line,help) VALUES ('LOGIN',1,'------ LOGIN COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('LOGIN',2,'Authenticates you with the bot');
INSERT INTO help (command,line,help) VALUES ('LOGIN',3,'Usage : /msg $BOT$ login <handle> <pass>');
INSERT INTO help (command,line,help) VALUES ('LOGIN',4,'<handle> - your handle on the bot');
INSERT INTO help (command,line,help) VALUES ('LOGIN',5,'<password> - your password');

-- MODE

INSERT INTO help (command,line,help) VALUES ('MODE',1,'------ MODE COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('MODE',2,'Makes the bot change channel modes');
INSERT INTO help (command,line,help) VALUES ('MODE',3,'Usage : /msg $BOT$ mode <#channel> <mode string>');
INSERT INTO help (command,line,help) VALUES ('MODE',4,'<#channel> -  the channel to change the mode in');
INSERT INTO help (command,line,help) VALUES ('MODE',5,'<mode string> - the string that contains the mode');
INSERT INTO help (command,line,help) VALUES ('MODE',6,'Ex : /msg k mode #coder-com +o |MrBean|');

-- MODERATE

INSERT INTO help (command,line,help) VALUES ('MODERATE',1,'------ MODERATE COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('MODERATE',2,'Moderates a channel ');
INSERT INTO help (command,line,help) VALUES ('MODERATE',3,'Usage : /msg $BOT$ moderate <#channel> ');
INSERT INTO help (command,line,help) VALUES ('MODERATE',4,'<#channel> -  the channel to moderate');


-- MODUSER

INSERT INTO help (command,line,help) VALUES ('MODUSER',1,'------ MODUSER COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('MODUSER',2,'Modify an existing user');
INSERT INTO help (command,line,help) VALUES ('MODUSER',3,'Usage: /msg $BOT$ moduser <handle> <-ah new host] <-dh host> <-s server> <-p password> <-gl on/off> <-op on/off> <-ua> <-uf new flags> <-e email>');
INSERT INTO help (command,line,help) VALUES ('MODUSER',4,'-ah adds a new host for the user');
INSERT INTO help (command,line,help) VALUES ('MODUSER',5,'-dh removes a host for a user');
INSERT INTO help (command,line,help) VALUES ('MODUSER',6,'-s associate a user with the server');
INSERT INTO help (command,line,help) VALUES ('MODUSER',7,'-p changes the user password');
INSERT INTO help (command,line,help) VALUES ('MODUSER',8,'-gl toggeles wheather the logs will be messaged to the user');
INSERT INTO help (command,line,help) VALUES ('MODUSER',9,'-op toggeles if a user must be opered up to user the needop commands');
INSERT INTO help (command,line,help) VALUES ('MODUSER',10,'-ua resets the user access acording to his flags');
INSERT INTO help (command,line,help) VALUES ('MODUSER',11,'-uf updates the oper flags to new one (OPER/ADMIN/SMT/CODER)');
INSERT INTO help (command,line,help) VALUES ('MODUSER',12,'-e updates the user email');

-- NEWPASS

INSERT INTO help (command,line,help) VALUES ('NEWPASS',1,'------ NEWPASS COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('NEWPASS',2,'Changes your password');

-- OP

INSERT INTO help (command,line,help) VALUES ('OP',1,'------ OP COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('OP',2,'Op users on a specific channel');
INSERT INTO help (command,line,help) VALUES ('OP',3,'Usage : /msg $BOT$ op <#chan> <nick1> [nick2] [nick3] .. ');

-- REMGLINE

INSERT INTO help (command,line,help) VALUES ('REMGLINE',1,'------ REMGLINE COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('REMGLINE',2,'Removes a gline on a host');
INSERT INTO help (command,line,help) VALUES ('REMGLINE',3,'Usage : /msg $BOT$ remgline <user@host>');
INSERT INTO help (command,line,help) VALUES ('REMGLINE',4,'<user@host> - the host to remove the gline for');

-- REMOPERCHAN

INSERT INTO help (command,line,help) VALUES ('REMOPERCHAN',1,'------ REMOPERCHAN COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('REMOPERCHAN',2,'Unmarks a channel as ircop only');
INSERT INTO help (command,line,help) VALUES ('REMOPERCHAN',3,'Usage : /msg $BOT$ remoperchan <#channel>');
INSERT INTO help (command,line,help) VALUES ('REMOPERCHAN',4,'<#channel> - the channel you want to unmark');

-- REMOVEOPER

INSERT INTO help (command,line,help) VALUES ('REMOPER',1,'------ REMOVEOPER COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('REMOPER',2,'Removes an oper from the bot access list');
INSERT INTO help (command,line,help) VALUES ('REMOPER',3,'Usage : /msg $BOT$ removeoper <handle>');
INSERT INTO help (command,line,help) VALUES ('REMOPER',4,'<handle> - handle of the oper you want to remove');

-- SCANGLINE

INSERT INTO help (command,line,help) VALUES ('SCANGLINE',1,'------ SCANGLINE COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('SCANGLINE',2,'Scans the gline list for a gline matching a certain host');
INSERT INTO help (command,line,help) VALUES ('SCANGLINE',3,'Usage : /msg $BOT$ scangline <host mask>');

-- SUSPEND

INSERT INTO help (command,line,help) VALUES ('SUSPEND',1,'------ SUSPEND COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('SUSPEND',2,'Suspend an oper for a certain amount of time');
INSERT INTO help (command,line,help) VALUES ('SUSPEND',3,'Usage : /msg $BOT$ suspend <oper> <duration>[s/m/h/d] [-l level] <time units>');
INSERT INTO help (command,line,help) VALUES ('SUSPEND',4,'<oper> - oper handle to suspend');
INSERT INTO help (command,line,help) VALUES ('SUSPEND',5,'<duration> - the duration of the suspend');
INSERT INTO help (command,line,help) VALUES ('SUSPEND',6,'<time units> - the unit in which you specify duriation');
INSERT INTO help (command,line,help) VALUES ('SUSPEND',7,'time units can be : s - seconds , m - minutes , h - hours , d- days');


-- TRANSLATE 

INSERT INTO help (command,line,help) VALUES ('TRANSLATE',1,'------ TRANSLATE COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('TRANSLATE',2,'Translates a numeric into a nick');
INSERT INTO help (command,line,help) VALUES ('TRANSLATE',3,'Usage : /msg $BOT$ transalte <numeric>');
INSERT INTO help (command,line,help) VALUES ('TRANSLATE',4,'the numeric you want to translate');

-- UNMODERATE

INSERT INTO help (command,line,help) VALUES ('UNMODERATE',1,'------ UNMODERATE COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('UNMODERATE',2,'UnModerates a channel ');
INSERT INTO help (command,line,help) VALUES ('UNMODERATE',3,'Usage : /msg $BOT$ unmoderate <#channel> ');
INSERT INTO help (command,line,help) VALUES ('UNMODERATE',4,'<#channel> -  the channel to moderate');

-- UNSUSPEND 

INSERT INTO help (command,line,help) VALUES ('UNSUSPEND',1,'------ UNSUSPEND COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('UNSUSPEND',2,'UNSuspend an oper');
INSERT INTO help (command,line,help) VALUES ('UNSUSPEND',3,'Usage : /msg $BOT$ unsuspend <oper> ');
INSERT INTO help (command,line,help) VALUES ('UNSUSPEND',4,'<oper> - oper handle to unsuspend');

-- WHOIS

INSERT INTO help (command,line,help) VALUES ('WHOIS',1,'------ WHOIS COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('WHOIS',2,'Gives information about a user');
INSERT INTO help (command,line,help) VALUES ('WHOIS',3,'Usage : /msg $BOT$ whois <nick>');
INSERT INTO help (command,line,help) VALUES ('WHOIS',4,'<nick> - the person you want to get info about');

-- LISTHOSTS

INSERT INTO help (command,line,help) VALUES ('LISTHOSTS',1,'------ LISTHOSTS COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('LISTHOSTS',2,'Syntax: /msg $BOT$ listhosts <handle>');
INSERT INTO help (command,line,help) VALUES ('LISTHOSTS',3,'Lists all the hosts that an oper is allowed to login from');

-- CHECKNET

INSERT INTO help (command,line,help) VALUES ('CHECKNET',1,'------ CHECKNET COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('CHECKNET',2,'Syntax: /msg $BOT$ checknet');
INSERT INTO help (command,line,help) VALUES ('CHECKNET',3,'Lists all the missing servers');

-- LISTIGNORES

INSERT INTO help (command,line,help) VALUES ('LISTIGNORES',1,'------ LISTIGNORES COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('LISTIGNORES',2,'Syntax: /msg $BOT$ LISTIGNORES');
INSERT INTO help (command,line,help) VALUES ('LISTIGNORES',3,'Shows the current ignore list of the bot');

-- REMIGNORE

INSERT INTO help (command,line,help) VALUES ('REMIGNORE',1,'------ REMIGNORE COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('REMIGNORE',1,'Syntax: /msg bot Remignore <host>');
INSERT INTO help (command,line,help) VALUES ('REMIGNORE',3,'Removes a host from the bot ignore list');

-- COMMANDS

INSERT INTO help (command,line,help) VALUES ('COMMANDS',1,'------ COMMANDS COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('COMMANDS',2,'Syntax: /msg $BOT$ commands <-ds/-en/-no/-nl/-na/-ml/> <command> <option>');
INSERT INTO help (command,line,help) VALUES ('COMMANDS',3,'Changes command options');
INSERT INTO help (command,line,help) VALUES ('COMMANDS',4,'-ds - disable the command');
INSERT INTO help (command,line,help) VALUES ('COMMANDS',5,'-en - enable the command');
INSERT INTO help (command,line,help) VALUES ('COMMANDS',6,'-no - toggles the need to be operd up to use the command');
INSERT INTO help (command,line,help) VALUES ('COMMANDS',7,'-nl - toggles if the command should not be logged in the lastcom report');
INSERT INTO help (command,line,help) VALUES ('COMMANDS',9,'-na - changes the name of the command');
INSERT INTO help (command,line,help) VALUES ('COMMANDS',10,'-ml - set the minimum level a oper must have for this command to be added to');

-- GCHAN

INSERT INTO help (command,line,help) VALUES ('GCHAN',1,'------ GCHAN COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('GCHAN',2,'Syntax: /msg $BOT$ Gchan <#channel> <duration/-per> <reason>');
INSERT INTO help (command,line,help) VALUES ('GCHAN',3,'Sets a bad channel gline on a channel');
INSERT INTO help (command,line,help) VALUES ('GCHAN',4,'if a -per is specified as a duration, the gline will be permanent');

-- REMGCHAN

INSERT INTO help (command,line,help) VALUES ('REMCHAN',1,'------ REMGCHAN COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('REMCHAN',2,'Syntax: /msg $BOT$ remgchan <#channel>');
INSERT INTO help (command,line,help) VALUES ('REMCHAN',3,'Removes a gchan gline');

-- LIST

INSERT INTO help (command,line,help) VALUES ('LIST',1,'------ LIST COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('LIST',2,'Syntax: /msg $BOT$ list <list type>');
INSERT INTO help (command,line,help) VALUES ('LIST',3,'Types can be either Glines or suspended');

-- USERINFO

INSERT INTO help (command,line,help) VALUES ('LIST',1,'------ USERINFO COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('LIST',2,'Syntax: /msg $BOT$ userinfo <user*/servers*>');
INSERT INTO help (command,line,help) VALUES ('LIST',3,'Shows information about all the opers that their server/handle match the supplied mask');

-- LEARNNET 

INSERT INTO help (command,line,help) VALUES ('LEARNNET',1,'------ LEARNNET COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('LEARNNET',2,'Syntax: /msg $BOT$ learnnet [-r]');
INSERT INTO help (command,line,help) VALUES ('LEARNNET',3,'Add all the unknown server which currently are connected to the network to the database');
INSERT INTO help (command,line,help) VALUES ('LEARNNET',4,'-r switch can be added only by CODERS, which will cause the bot to delete all the known servers and relearn the network.

-- REMSERVER

INSERT INTO help (command,line,help) VALUES ('REMSERVER',1,'------ REMSERVER COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('REMSERVER',2,'Syntax: /msg $BOT$ RemServer [server name]');
INSERT INTO help (command,line,help) VALUES ('REMSERVER',3,'Removes a server from the bot database');

-- LASTCOM

INSERT INTO help (command,line,help) VALUES ('LASTCOM',1,'------ LASTCOM COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('LASTCOM',2,'Syntax: /msg $BOT$ lastcom [num of commands ] [from days ago - d]');
INSERT INTO help (command,line,help) VALUES ('LASTCOM',3,'will paste all the last commands which where issued to the bot');
INSERT INTO help (command,line,help) VALUES ('LASTCOM',4,'the default value is 20 if no num of commands is specified');
INSERT INTO help (command,line,help) VALUES ('LASTCOM',5,'a user can also specify the number of days ago the commands where issued');

-- FORCEGLINE

INSERT INTO help (command,line,help) VALUES ('FORCEGLINE',1,'------ FORCEGLINE COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('FORCEGLINE',2,'Syntax: /msg $BOT$ ForceGline [-fu] <host> <duration[s/m/h/d] reason');
INSERT INTO help (command,line,help) VALUES ('FORCEGLINE',3,'Glines a host for a certain amount of time');
INSERT INTO help (command,line,help) VALUES ('FORCEGLINE',4,'-fu flag can only be specified by smt+');
INSERT INTO help (command,line,help) VALUES ('FORCEGLINE',5,'This type of gline is used to gline for more than 14 days and for a gline which affects more than 255 users.');

-- EXCEPTIONS

INSERT INTO help (command,line,help) VALUES ('EXCEPTIONS',1,'------ EXCEPTIONS COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('EXCEPTIONS',2,'Syntax: /msg $BOT$ Exceptions <add/del/list> [host mask]');
INSERT INTO help (command,line,help) VALUES ('EXCEPTIONS',3,'Excpetions = AGL in uworld, meaning a user can have more than a certain');
INSERT INTO help (command,line,help) VALUES ('EXCEPTIONS',4,'amount of connections from a single host than the default one which is specified');
INSERT INTO help (command,line,help) VALUES ('EXCEPTIONS',5,'in the bot conf file, before he gets automatically glined for clones.');

-- ADDSERVER 

INSERT INTO help (command,line,help) VALUES ('ADDSERVER',1,'------ ADDSERVER COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('ADDSERVER',2,'Syntax: /msg $BOT$ addserver <server>');
INSERT INTO help (command,line,help) VALUES ('ADDSERVER',3,'Adds a new server to the bot database');

