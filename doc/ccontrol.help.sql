
-- 2001-03-02 |MrBean| "$Id: ccontrol.help.sql,v 1.3 2001/03/11 21:40:10 mrbean_ Exp $"
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
INSERT INTO help (command,line,help) VALUES ('ADDCOMMAND',3,'Usage : /msg $BOT$ addcommand <oper> <command>');
INSERT INTO help (command,line,help) VALUES ('ADDCOMMAND',4,'<oper> - the oper handle you wand to give permission');
INSERT INTO help (command,line,help) VALUES ('ADDCOMMAND',5,'<command> - the command you want to add');

-- ADDNEWOPER

INSERT INTO help (command,line,help) VALUES ('ADDNEWOPER',1,'------ ADDNEWOPER COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('ADDNEWOPER',2,'Adds new oper to the bot access list');
INSERT INTO help (command,line,help) VALUES ('ADDNEWOPER',3,'Usage : /msg $BOT$ addnewoper <handle> <oper level> <password>');
INSERT INTO help (command,line,help) VALUES ('ADDNEWOPER',4,'<handle> - The handle the bot will recognize the oper under');
INSERT INTO help (command,line,help) VALUES ('ADDNEWOPER',5,'<oper level> - Sets the initial oper commands , options are : oper , admin , coder');
INSERT INTO help (command,line,help) VALUES ('ADDNEWOPER',6,'<password> - the password of the new oper');

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


-- MODOPER

INSERT INTO help (command,line,help) VALUES ('MODOPER',1,'------ MODOPER COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('MODOPER',2,'Modify an existing oper ');
INSERT INTO help (command,line,help) VALUES ('MODOPER',3,'Usage : /msg $BOT$ modoper <oper> <option> <new value>');
INSERT INTO help (command,line,help) VALUES ('MODOPER',4,'<oper> - the oper handle to modify');
INSERT INTO help (command,line,help) VALUES ('MODOPER',5,'<option> - the option to change , can be newpass , addhost , delhost');
INSERT INTO help (command,line,help) VALUES ('MODOPER',6,'<new value> - the new value to set');
INSERT INTO help (command,line,help) VALUES ('MODOPER',7,'Note : /msg $BOT$ help <option> for more help on a specific command');

-- MODOPER NEWPASS

INSERT INTO help (command,subcommand,line,help) VALUES ('MODOPER','NEWPASS',1,'Changes the password for an oper');
INSERT INTO help (command,subcommand,line,help) VALUES ('MODOPER','NEWPASS',2,'Usage : /msg $BOT$ modoper <oper> newpass <new password>');

-- MODOPER ADDHOST

INSERT INTO help (command,subcommand,line,help) VALUES ('MODOPER','ADDHOST',1,'Add a new host to an oper access list');
INSERT INTO help (command,subcommand,line,help) VALUES ('MODOPER','ADDHOST',2,'Usage : /msg $BOT$ modoper <oper> addhost <new host>');

-- MODOPER DELHOST

INSERT INTO help (command,subcommand,line,help) VALUES ('MODOPER','DELHOST',1,'Delete a new host to an oper access list');
INSERT INTO help (command,subcommand,line,help) VALUES ('MODOPER','DELHOST',2,'Usage : /msg $BOT$ modoper <oper> delhost <new host>');

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

INSERT INTO help (command,line,help) VALUES ('REMOVEOPER',1,'------ REMOVEOPER COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('REMOVEOPER',2,'Removes an oper from the bot access list');
INSERT INTO help (command,line,help) VALUES ('REMOVEOPER',3,'Usage : /msg $BOT$ removeoper <handle>');
INSERT INTO help (command,line,help) VALUES ('REMOVEOPER',4,'<handle> - handle of the oper you want to remove');

-- SCANGLINE

INSERT INTO help (command,line,help) VALUES ('SCANGLINE',1,'------ SCANGLINE COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('SCANGLINE',2,'Scans the gline list for a gline matching a certain host');
INSERT INTO help (command,line,help) VALUES ('SCANGLINE',3,'Usage : /msg $BOT$ scangline <host mask>');

-- SUSPEND

INSERT INTO help (command,line,help) VALUES ('SUSPEND',1,'------ SUSPEND COMMAND ------');
INSERT INTO help (command,line,help) VALUES ('SUSPEND',2,'Suspend an oper for a certain amount of time');
INSERT INTO help (command,line,help) VALUES ('SUSPEND',3,'Usage : /msg $BOT$ suspend <oper> <duration> <time units>');
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



