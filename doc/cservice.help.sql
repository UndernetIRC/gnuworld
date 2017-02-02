-- Help information copied from current live database.
--
-- Its not particularly in any nice order, but you can use the web interface
-- to change it all anyway.
--
-- $Id: cservice.help.sql,v 1.9 2008/01/09 22:46:54 kewlio Exp $

DELETE FROM help;

COPY "help" FROM stdin;
SET	1	/msg X set <#channel> <variable> <value>\n/msg X set <variable> <value>\nThis will set a channel or user account setting. \nUseraccount settings: INVISIBLE MAXLOGINS LANG\nChannel settings: AUTOJOIN AUTOTOPIC DESCRIPTION FLOODPRO KEYWORDS MASSDEOPPRO MODE NOOP OPLOG STRICTOP URL USERFLAGS\nFor more help type /msg X help SET <variable>
ACCESS	2	/msg X access <#canal> <*|chaine|utilisateur|=nickname> [-min <niveau>] [-max <niveau>] [-modif] [-none|-voice|-op]\r\nPeut afficher l'acc�s pour une certaine personne ou une chaine de caract�re. L'information indique le niveau d'acc�s,\r\nl'�tat de suspension et son niveau le cas �ch�ant, ainsi que la derni�re fois que l'utilisateur � �t� vu.\r\nSi il y a plus de 15 entr�es, allez sur http://cservice.undernet.org/live/ et logguez vous\r\npour voir la liste enti�re en cliquant sur "Channel Information" et en envoyant le nom du canal. \r\nVous pouvez aussi m�langer ces options au lieu d'un nom d'utilisateur ou d'une cha�ne de caract�re pour obtenir des r�sultats sp�cifiques.
SET AUTOJOIN	1	/msg X set <#channel> autojoin ON|OFF\nWhen the setting is ON, X will join the channel  and autorejoin after a netsplit.\nWhen the flag is OFF, X will part if it's on the channel, and it won't rejoin after a netsplit
SET AUTOTOPIC	1	/msg X set <#channel> AUTOTOPIC ON|OFF\nThis flag makes X reset the topic to the channel DESCRIPTION and URL every 30 minutes. \nNote that if the channel is idle, the topic will not be reset during that time.
OP	2	/msg X op <#canal> [nick] [nick2] ... [nickN]\r\nOppe une ou plusieurs personnes dans le canal sp�cifi�. Si aucun nickname n'est sp�cifi�, et que vous n'�tes pas d�j� opp�, X vous oppera.
BANLIST	2	/msg X banlist <#canal>\r\nMontre la liste des bans du canal. Cette commande marche que vous soyez ou non dans le canal.
OPERJOIN	2	/msg X operjoin <#canal>\r\nCeci fait joindre le canal sp�cifi� � X. Cette commande est r�serv�e aux Op�rateurs IRC et ne doit �tre utilis�e que pour r�soudre des d�synchronisations.
OPERPART	2	/msg X operpart <#canal>\r\nCeci fait partir X du canal sp�cifi�. Cette commande est r�serv�e aux Op�rateurs IRC et ne doit �tre utilis�e que pour r�soudre des d�synchronisations.
CHANINFO	12	/msg X chaninfo <#kanal>\r\nBir kanal�n kay�tl� olup olmad���n�, kay�tl� ise; kime ait oldu�unu, e�er varsa; mevcut DESCRIPTION ve URL de�erliklerini g�r�nt�ler.
PART	2	/msg X part <#canal>\r\nFait partir X du canal sp�cifi�. Si vous souhaitez que X parte un long moment, voyez la commande SET AUTOJOIN (niveau 500)
PURGE	2	/msg X purge <#canal> <raison>\r\nPurge le canal sp�cifi�. Cette commande est r�serv�e aux Administrateurs CService.
QUOTE	2	Pas toucher ;)
REGISTER	2	/msg X register <#canal> <utilisateur>\r\nCette commande est r�serv�e aux Administrateurs CService.
REHASH	2	/msg X rehash <table>\r\nCette commande est r�serv�e aux D�veloppeurs CService.
SET INVISIBLE	2	
UNBAN	2	/msg X unban <#canal> <nickname | *!*user@*.host>\r\nSupprime le *!*user@*.host sp�cifi� de la liste interne de X pour votre canal ainsi que du canal le cas �ch�ant.\r\nVous ne pouvez supprimer des bans de la liste interne de X que si vous avec un niveau sup�rieur ou �gal au niveau de ce ban.\r\nPour chercher dans cette liste interne, voyez la commande LBANLIST (niveau 0).
SET NOADDUSER	1	/msg X set NOADDUSER ON|OFF\nToggle noadduser on or off for your username. \nWhen this setting is on, another user cannot add you to any channels.
SET DESCRIPTION	1	/msg X set <#channel> DESCRIPTION <description here>\nSets the channel description. This value can be seen in the CHANINFO reply.
SET INVISIBLE	1	/msg X set INVISIBLE ON|OFF\nToggle invisible on or off for your username. \nWhen this setting is on, another user cannot see wether you are online or not.
SET KEYWORDS	1	/msg X set <#channel> <keywords here>\nSets keywords for your channel. You can use SEARCH to find channels with a certain keyword set.
SET FLOODPRO	1	/msg X set <#channel> FLOODPRO <defaults|kick|ban|off>\nThis channel flag (for level >=450 users) controls the behaviour of channel flood protection, to punish those flooding a channel via channel MESSAGE/NOTICE/CTCP/REPEAT\nEach floodtype can have a limit set which when breached within FLOODPERIOD time, the client(s) will be punished via KICK or KICKBAN (depending on the mode)\nie. To cause X to kickban users who send 5 messages to a channel within 2 seconds, one could do the following config commands:\n    1). /msg X set #chan FLOODPRO BAN\n    2). /msg X set #chan MSGFLOOD 5\n    3). /msg X set #chan FLOODPERIOD 2\nNOTE -- When BAN action is used the ban will be placed @ level 75 for 3 hours.\nCurrent settings can be viewed in the channel via: /msg X STATUS <chan>\nChannel operators and those with any access in X will not be punished by FLOODPRO.  Floods ocurring from floodnets will be acted upon accordingly and will result in X temporarily increasing the action from KICK->BAN for 5 minutes to help curb subsequent abuse.\nDefault settings (when first enabled, or when 'DEFAULTS' are set) are: MSGFLOOD:15, NOTICEFLOOD:5, CTCPFLOOD:5, REPEATFLOOD:5, FLOODPERIOD:15
SET MSGFLOOD	1	/msg X set <#channel> MSGFLOOD <0-255>\nClients exceeding this number of messages (any PRIVMSG or ACTION text) to a channel within FLOODPERIOD will result in the configured action (KICK or BAN) to be taken against a flooder when FLOODPRO is enabled on a channel.\nSetting a MSGFLOOD value of 0 will disable protection for this flood type.\nFor more help, try: /msg X HELP SET FLOODPRO
SET NOTICEFLOOD	1	/msg X set <#channel> NOTICEFLOOD <0-15>\nClients exceeding this number of notices (any /NOTICE text) to a channel within FLOODPERIOD will result in the configured action (KICK or BAN) to be taken against a flooder when FLOODPRO is enabled on a channel.\nSetting a NOTICEFLOOD value of 0 will disable protection for this flood type.\nFor more help, try: /msg X HELP SET FLOODPRO
SET CTCPFLOOD	1	/msg X set <#channel> CTCPFLOOD <0-15>\nClients exceeding this number of CTCPs to a channel within FLOODPERIOD will result in the configured action (KICK or BAN) to be taken against a flooder when FLOODPRO is enabled on a channel.\nSetting a CTCPFLOOD value of 0 will disable protection for this flood type.\nFor more help, try: /msg X HELP SET FLOODPRO
SET REPEATFLOOD	1	/msg X set <#channel> REPEATFLOOD <0|2-15>\nClients exceeding this number of line repeats to a channel (PRIVMSG, ACTION, /NOTICE, or /PART reason) within FLOODPERIOD will result in the configured action (KICK or BAN) to be taken against a flooder when FLOODPRO is enabled on a channel.\nSetting a REPEATFLOOD value of 0 will disable protection for this flood type.\nFor more help, try: /msg X HELP SET FLOODPRO
SET FLOODPERIOD	1	/msg X set <#channel> FLOODPERIOD <0-15>\nThe amount of time one of the MSGFLOOD/REPEATFLOOD/NOTICEFLOOD/CTCPFLOOD counters must be met within, for the FLOODPRO action (KICK or BAN) to be taken against a flooder.\nSetting a value of 0 will disable flood protection.\nFor more help, try: /msg X HELP SET FLOODPRO
SET NOTAKE	1	/msg X set <#channel> NOTAKE <on|off>\nThis channel flag (for channel managers) controls whether X will punish users for attempting to takeover the channel by banning all clients.\nThe punish action can be set via TAKEREVENGE.  For more information use: /msg X help SET TAKEREVENGE
SET TAKEREVENGE	1	/msg X set <#channel> TAKEREVENGE <none|ban|suspend>\nConfigure the action that X will take when a user triggers NOTAKE prevention (if enabled), whereby:\n    NONE = prevent the ban but take no other action\n    BAN = kickban the user\n    SUSPEND = suspend the user's channel access *and* kickban the user\nNOTE -- Channel BAN or SUSPEND actions will be placed @ level 500 for 7 days
SET OPLOG	1	/msg X set <#channel> OPLOG <on|off>\nWhen enabled by channel manager, the following commands will result in channel opnotices being sent: OP, DEOP, VOICE, DEVOICE, UNBAN, CLEARMODE\nThis will provide an audit trail for anyone abusing X access on a channel which previously had no accountability
MOTD	1	/msg X motd\nDisplays X message of the day.
PART	1	/msg X part <#channel>\nMakes X part your channel. If you want to part X for a long time refer to the SET AUTOJOIN command (Level 500)
ACCESS	3	
ADDCOMMENT	3	
ADDUSER	3	
BAN	3	
BANLIST	3	
CLEARMODE	3	
DEOP	3	
DEVOICE	3	
FORCE	3	
HELP	3	
INFO	3	
INVITE	3	
ISREG	3	
JOIN	3	
KICK	3	
LBANLIST	3	
LOGIN	3	
MODINFO	3	
MOTD	3	
NEWPASS	3	
OP	3	
OPERJOIN	3	
OPERPART	3	
PART	3	
PURGE	3	
QUOTE	3	
REGISTER	3	
REHASH	3	
REMOVEALL	3	
REMUSER	3	
SAY	3	
SEARCH	3	
SERVNOTICE	3	
SET	3	
SHOWCOMMANDS	3	
SHOWIGNORE	3	
STATS	3	
STATUS	3	
SUPPORT	3	
SUSPEND	3	
TOPIC	3	
UNBAN	3	
UNFORCE	3	
UNSUSPEND	3	
VERIFY	3	
VOICE	3	
ACCESS	4	
ADDCOMMENT	4	
ADDUSER	4	
BAN	4	
BANLIST	4	
CLEARMODE	4	
DEOP	4	
DEVOICE	4	
FORCE	4	
HELP	4	
INFO	4	
INVITE	4	
ISREG	4	
JOIN	4	
KICK	4	
LBANLIST	4	
LOGIN	4	
MODINFO	4	
MOTD	4	
NEWPASS	4	
OP	4	
OPERJOIN	4	
OPERPART	4	
PART	4	
PURGE	4	
QUOTE	4	
REGISTER	4	
REHASH	4	
REMOVEALL	4	
REMUSER	4	
SAY	4	
SEARCH	4	
SERVNOTICE	4	
SET	4	
SHOWCOMMANDS	4	
SHOWIGNORE	4	
STATS	4	
STATUS	4	
SUPPORT	4	
SUSPEND	4	
TOPIC	4	
UNBAN	4	
UNFORCE	4	
UNSUSPEND	4	
VERIFY	4	
VOICE	4	
ACCESS	5	
ADDCOMMENT	5	
ADDUSER	5	
BAN	5	
BANLIST	5	
CLEARMODE	5	
DEOP	5	
DEVOICE	5	
FORCE	5	
HELP	5	
INFO	5	
INVITE	5	
ISREG	5	
JOIN	5	
KICK	5	
LBANLIST	5	
LOGIN	5	
MODINFO	5	
MOTD	5	
NEWPASS	5	
OP	5	
OPERJOIN	5	
OPERPART	5	
PART	5	
PURGE	5	
QUOTE	5	
REGISTER	5	
REHASH	5	
REMOVEALL	5	
REMUSER	5	
SAY	5	
SEARCH	5	
SERVNOTICE	5	
SET	5	
SHOWCOMMANDS	5	
SHOWIGNORE	5	
STATS	5	
STATUS	5	
SUPPORT	5	
SUSPEND	5	
TOPIC	5	
UNBAN	5	
UNFORCE	5	
UNSUSPEND	5	
VERIFY	5	
VOICE	5	
ACCESS	6	
ADDCOMMENT	6	
ADDUSER	6	
BAN	6	
BANLIST	6	
CLEARMODE	6	
DEOP	6	
DEVOICE	6	
FORCE	6	
HELP	6	
INFO	6	
INVITE	6	
ISREG	6	
JOIN	6	
KICK	6	
LBANLIST	6	
LOGIN	6	
MODINFO	6	
MOTD	6	
NEWPASS	6	
OP	6	
OPERJOIN	6	
OPERPART	6	
PART	6	
PURGE	6	
QUOTE	6	
REGISTER	6	
REHASH	6	
REMOVEALL	6	
REMUSER	6	
SAY	6	
SEARCH	6	
SERVNOTICE	6	
SET	6	
SHOWCOMMANDS	6	
SHOWIGNORE	6	
STATS	6	
STATUS	6	
SUPPORT	6	
SUSPEND	6	
TOPIC	6	
UNBAN	6	
UNFORCE	6	
UNSUSPEND	6	
VERIFY	6	
VOICE	6	
ACCESS	7	
ADDCOMMENT	7	
ADDUSER	7	
BAN	7	
BANLIST	7	
CLEARMODE	7	
DEOP	7	
DEVOICE	7	
FORCE	7	
HELP	7	
INFO	7	
INVITE	7	
ISREG	7	
JOIN	7	
KICK	7	
LBANLIST	7	
LOGIN	7	
MODINFO	7	
MOTD	7	
NEWPASS	7	
OP	7	
OPERJOIN	7	
OPERPART	7	
PART	7	
PURGE	7	
QUOTE	7	
REGISTER	7	
REHASH	7	
REMOVEALL	7	
REMUSER	7	
SAY	7	
SEARCH	7	
SERVNOTICE	7	
SET	7	
SHOWCOMMANDS	7	
SHOWIGNORE	7	
STATS	7	
STATUS	7	
SUPPORT	7	
SUSPEND	7	
TOPIC	7	
UNBAN	7	
UNFORCE	7	
UNSUSPEND	7	
VERIFY	7	
VOICE	7	
SET USERFLAGS	8	/msg X set <#canal> USERFLAGS [0-2]\nDetermina daca da sau nu OP sau VOICE va fi setat automat AUTOMODE cand un nou user este adaugat la lista de accese.\nValoarea "default" este 0 (None). Variabile: 0: None  1: Op  2: Voice
SHOWCOMMANDS	8	/msg X showcommands <#caanal>\nArata comenzile disponibile pentru tine pentru un canal specificat.
SHOWIGNORE	8	/msg X showignore\nArata lista de ignore a lui X. X va da ignore unui user cand acesta il flood-eaza intentionat sau nu. \nDurata ignore-ului este de 60 de minute.\nNota: X nu iti va mai raspunde daca esti ignorat. Daca nu mai primesti reply-uri de la X, intreaba un prieten ca sa verifice ignorelist-ul pentru tine.
STATS	8	/msg X stats\nAceasta comanda este doar pentru Adminii CService.
STATUS	8	/msg X status <#canal>\nceasta comanda face acelasi lucru ca si comanda de nivel 200 STATUS, cu exceptia ca nu va raporta modurile canalului (+nt etc.) la oricine care are access 200 sau mai mic.
SUPPORT	8	/msg X support <#canal> <yes/no>\nO metoda alternativa care iti permite sa confirmi ca esti de acord sau nu pentru o aplicatie a unui canal, daca te alfi in lista suporterilor. Dupa autentificare, X iti va trimite o notificare despre orice aplicatie a unui canal in care esti trecut ca un suporter si deasemenea iti va trimite o notificare referitor la comanda SUPPORT. \nAlternativ, poti sa vizualizezi aplicatia din pagina web pentru a vedea mai multe informatii despre acest lucru, pune o obiectie, si confirma sau nu suportul pentru acel canal de acolo. Comanda nu va fi valabila daca nu esti trecut in lista celor care sunt suporteri a acelui canal, sau un canal nu va avea o aplicatie de "Pending" in web.
SUSPEND	8	/msg X suspend <#canal> <username> <durata> <M|O|Z> [nivel]\nSuspenda accesul unui user la X pe canalul respectiv, pe o perioada specificata.\nDaca nu este specificat level-ul, level-ul suspendului va fi din default accesul celui care a folosit comanda.\nPoti SUSPENDA accesul cuiva doar daca este mai mic decat al tau. Durata maxima pentru SUSPEND este de 372 de zile (8928 ore, 535680 minute).
ACCESS	9	
ADDCOMMENT	9	
ADDUSER	9	
BAN	9	
BANLIST	9	
CLEARMODE	9	
DEOP	9	
DEVOICE	9	
FORCE	9	
HELP	9	
INFO	9	
INVITE	9	
ISREG	9	
JOIN	9	
KICK	9	
LBANLIST	9	
LOGIN	9	
MODINFO	9	
MOTD	9	
NEWPASS	9	
OP	9	
OPERJOIN	9	
OPERPART	9	
PART	9	
PURGE	9	
QUOTE	9	
REGISTER	9	
REHASH	9	
REMOVEALL	9	
REMUSER	9	
SAY	9	
SEARCH	9	
SERVNOTICE	9	
SET	9	
SHOWCOMMANDS	9	
SHOWIGNORE	9	
STATS	9	
STATUS	9	
SUPPORT	9	
SUSPEND	9	
TOPIC	9	
UNBAN	9	
UNFORCE	9	
UNSUSPEND	9	
VERIFY	9	
VOICE	9	
ACCESS	10	
ADDCOMMENT	10	
ADDUSER	10	
BAN	10	
BANLIST	10	
CLEARMODE	10	
DEOP	10	
DEVOICE	10	
FORCE	10	
HELP	10	
INFO	10	
INVITE	10	
ISREG	10	
JOIN	10	
KICK	10	
LBANLIST	10	
LOGIN	10	
MODINFO	10	
MOTD	10	
NEWPASS	10	
OP	10	
OPERJOIN	10	
OPERPART	10	
PART	10	
PURGE	10	
QUOTE	10	
REGISTER	10	
REHASH	10	
REMOVEALL	10	
REMUSER	10	
SAY	10	
SEARCH	10	
SERVNOTICE	10	
SET	10	
SHOWCOMMANDS	10	
SHOWIGNORE	10	
STATS	10	
STATUS	10	
SUPPORT	10	
SUSPEND	10	
TOPIC	10	
UNBAN	10	
UNFORCE	10	
UNSUSPEND	10	
VERIFY	10	
VOICE	10	
UNFORCE	2	/msg X unforce <#canal>\r\nCette commande est r�serv�e aux Administrateurs CService.
SUSPENDME	1	Type /msg X@channels.undernet.org SUSPENDME <password> \nThis command is to be used in the situation where you believe your account has been compromised, and wish to immediately suspend all your access.\nThere must be more than one person logged into your account to issue this command.\nAfter issuing this command, you cannot perform any actions with your access until your account is dealt with by a CService representative.\nTo have the suspension removed send an email to object@undernet.org
SET MASSDEOPPRO	1	/msg X set <#channel> MASSDEOPPRO <value 0-7>\nThe maximum number of deops X will allow in a 15 sec period.\nA person exceeding this number will be deop'd and, if on the User List, suspended for 5 minutes. They will also have a level 25 Ban put on them and need to unban themselves after the suspend ends.
SET MAXLOGINS	1	/msg X set MAXLOGINS <value> (value must be between 1 and 3. Default value is 1).\nThis command defines the maximum number of times you may log into your account simultaneously.\nAs you login to an account for a 2nd and 3rd time, a warning is sent to the existing authenticated user(s) giving details of who has just logged into the account.\nThis is an additional security measure, and can give you an early warning sign that someone has your password and is (ab)using your account.
SET NOOP	1	/msg X set <#channel> NOOP ON|OFF\nWhen this flag is ON, noone except X can be an operator.
SET STRICTOP	1	/msg X set <#channel> STRICTOP ON|OFF\nWhen this setting is ON X allows only Level 100+ users who are authenticated to be opped.
SET URL	1	/msg X set <#channel> URL <http://url>\nYou can state your channel's homepage address(es) with this option (maximum of 75 characters). \nThe value can be seen in the CHANINFO response.
SET USERFLAGS	1	/msg X set <#channel> USERFLAGS [0-2]\nDetermines whether or not OP or VOICE will automatically be set for AUTOMODE when a new user is added to the userlist. \nDefault value is 0 (None). values: 0: none  1: Op  2: Voice
SHOWCOMMANDS	1	/msg X showcommands <#channel>\nShows the commands available to you for a given channel.
SHOWIGNORE	1	/msg X showignore\nShows X's ignore list. Ignores happen when someone intentionally or accidentally floods X. \nThe time length is 60 minutes. \nNote: X will not reply to you if you are being ignored. If you don't receive replies from X,\nask a friend to check the ignorelist for you.
STATS	1	/msg X stats\nThis command is for CService Admins only.
STATUS	1	/msg X status <#channel>\nThis does the same thing as level 200 STATUS, except it will not report channel modes (+nt etc.)\nto anyone with access below 200.
SET AUTOJOIN	2	
SET MASSDEOPPRO	2	
SUPPORT	1	/msg X support <#channel> <yes/no>\nAn alternate method allowing you to confirm or deny support for a channel application which you \nare listed as a supporter on. Upon authenticating, X will notify you of any channel applications that \nyou are listed as a supporter on and will also notify you of this SUPPORT command. \nAlternatively, you can choose to view the application on the web to see more information about it, \npost an objection, and confirm or deny your support from there. The command will not be effective \nif you are not listed on a channel as a supporter, or a channel does not have a pending application on the web.
HELP	12	/msg X help <komut>\r\nVerilen komut i�in yard�m bilgilerini g�r�nt�ler.
SET LANG	1	/msg X set lang <language code>\nWith this command you can specify in what language X should reply to you. \nAvailable languages: AR - Arabic, CA - Catalan, CH - Christmas, DK - Danish, NL - Dutch, EA - Easter, EN - English, FR - Fran�ais, DE - German, IL - Hebrew, HU - Hungarian, IT - Italian, NO - Norsk, PT - Portuguese, RO - Romanian, ES - Spanish, TR - Turkce,
UNSUSPEND	2	/msg X unsuspend <#canal> <utilisateur>\r\nSupprime la suspension � X d'un utilisateur sur votre canal,\r\nlui permettant donc d'acc�der de nouveau � X.\r\nVous ne pouvez UNSUSPEND que des gens ayant un acc�s inf�rieur au votre,\r\ndu moment que le niveau de suspension est �galement inf�rieur ou �gal � votre propre niveau.
PURGE	1	/msg X purge <#channel> [-noop] <reason>\nPurges a channel. This command is for CService Admins only.\n-noop flag causes no reops during purge
VOICE	2	/msg X voice <#canal> [nick] [nick2] ... [nickN]\r\nVoice (+v) un ou plusieurs nickname dans votre canal.
ADDCOMMENT	1	/msg X addcomment <username> <the comment>\nsets a usercomment for a given username. This command is for Cservice Admins only
NEWPASS	1	From now on all passwords will be done via the webpage. Please visit http://cservice.undernet.org/ and click on "web interface"\nThen login with your OLD password. When the page is loaded click on the "newpass" link on the left hand column and follow the instructions.
ADDUSER	1	/msg X adduser <#channel> <username> <access>\nAdds a user to the channel's userlist. You cannot ADDUSER someone with equal or higher access than your own.
BANLIST	1	/msg X banlist <#channel>\nShows the channel banlist. This command works whether you are on the channel or not.
DEOP	1	/msg X deop <#channel> [nick] [nick2] ... [nickN]\nDeops one or more persons in your channel. If no nick is specified, and you are opped on the channel, X will deop you. \nNOTE: be mindful of triggering the limit that may be in effect with MASSDEOPPRO (see SET MASSDEOPPRO - level 500).
DEVOICE	1	/msg X devoice <#channel> [nick] [nick2] ... [nickN]\nDevoices one or more users in your channel.
HELP	1	/msg X help <command>\nDisplays help information on a given command.
QUOTE	1	Don't touch :p
REGISTER	1	/msg X register <#channel> <username>\nThis command is for CService Admins only.
REHASH	1	/msg X rehash <table>\nThis command is for CService Coders only.
SAY	1	/msg X say <#channel> <text>\nThis command is for CService Admins only.
TOPIC	1	/msg X topic <#channel> <topic>\nMakes X change the topic in your channel (maximum of 145 characters) and includes your username, \nenclosed in parentheses, at the beginning of the topic. \nNOTE: If a DESCRIPTION or URL has been set and AUTOTOPIC is ON,\n the channel topic will be overwritten in 30 minutes, unless the channel is idle.
UNBAN	1	/msg X unban <#channel> <nick | *!*user@*.host>\nRemoves a *!*user@*.host from the X banlist of your channel and any matching bans   from the channel banlist also. \nYou can only remove bans from X's banlist that are equal to or lower than your own access. \nTo search X's banlist, refer to the LBANLIST command (level 0).
UNFORCE	1	/msg X unforce <#channel>\nThis command is for CService Admins only.
UNSUSPEND	1	/msg X unsuspend <#channel> <username>\nUnsuspends a user's access to X on your channel's userlist. \nYou can only UNSUSPEND someone with access lower than your own,\nprovided that the suspension level is also less than or equal to your own access level.
VERIFY	1	/msg X verify <nick>\nDisplays whether or not a person is logged in, and if so displays their username\nAnd also indicates whether or not they are an Official CService Administrator or Representative, and/or an IRC Operator.
VOICE	1	/msg X voice <#channel> [nick] [nick2] ... [nickN]\nVoices one or more users in your channel.
ACCESS	1	/msg X access <#channel> <*|pattern|username|=nickname> [-min <level>] [-max <level>] [-modif] [-none|-voice|-op]\nCan look up the access for a certain person or a string. The information indicates level,\nsuspension status and level if suspended, and when the user was last seen.\nIf there are more than 15 entries, go to http://cservice.undernet.org/live/ and login\nto see the entire list by clicking on the "Channel Information" link and submitting the channel name. \nYou can also use and mix these options instead of a username or string to get specific results.
BAN	1	/msg X ban <#channel> <nick|*!*user@*.host> [duration] [level] [reason]\nAdds a specific *!*user@*.host to the X banlist of your channel.\nYou may place a ban on a nick if the person is online or ban their *!*user@*.host if the person is not online. \nBans that cover a broader range than previously set bans will replace previous bans.\nAny user in the channel that matches the added ban will be kicked out if the ban level is 75+. \nThe duration can be specified in the following format: 400s, 300m, 200h, 100d (secs,mins,hours,days)\nThe ban level can range from 1 to your own level. The ban reason can be a maximum of 128 characters long. \nIf no duration or level is specified, the default duration will be 3 hours, and the level will be 75.\nBan Levels: 1-74 = Prevents the user from having ops (+o) on the channel. \nBan Levels: 75-500 = Prevents the user from being in the channel at all.
ADDUSER	2	/msg X adduser <#canal> <utilisateur> <niveau>\r\nAjoute un utilisateur au canal. Vous ne pouvez pas ajouter quelqu'un avec un niveau d'acc�s �gal ou sup�rieur au votre.
CHANINFO	1	/msg X chaninfo <#channel>\nDisplays whether or not a channel is registered, to whom, and the currently set DESCRIPTION and URL, if any.
CLEARMODE	1	/msg X clearmode <#channel>\nClears all channel modes. Can be used when your channel has been locked up (set +i or +k without your knowledge).\nNOTE: This is not necessarily the best method to gain entry to your channel,\nbecause other people or bots may be opped in the channel that may reset the modes as you clear them.\nRefer to the STATUS command (level 200) for related information
FORCE	1	/msg X force <#channel>\nLogs you in to a given channel using an administrative access. This is a CService Admin command only.
INFO	1	/msg X info <username>\nDisplays information about a username such as their nick and user@host if they are currently online, \ntheir language and invisible settings, last seen information, and whether they may have been suspended by a CService Administrator. \nIf used on yourself, it also lists all channels which you have access on (only CService and you can see this information, not other users). \nIf you wish to make this information hidden to the public, refer to the SET INVISIBLE command (level 0).
BAN	2	/msg X ban <#canal> <nickname|*!*user@*.host> [dur�e] [niveau] [raison]\r\nAjoute un *!*user@*.host sp�cifique � la liste des bans de X de votre canal.\r\nVous pourrez placer un ban en utilisant le nickname si la personne est pr�sente, ou sur le *!*user@*.host si elle ne l'est pas.\r\nLes bans couvrant un plus large masque que des bans mis pr�c�demment remplaceront ces pr�c�dents bans.\r\nN'importe quel utilisateur dans le canal qui correspond au ban sera kick� hors du canal si le niveau du ban est sup�rieur a 75.\r\nLe niveau de ban peut aller de 1 � votre niveau. La raison du ban doit comporter un maximum de 128 caract�res.\r\nSi aucune dur�e n'est sp�cifi�e, la dur�e par defaut sera 3 heures, et le niveau du ban par defaut sera 75.\r\nBan niveau: 1-74 = Emp�che l'utilisateur d'�tre opp� (+o) sur le canal.\r\nBan niveau: 75-500 = Emp�che l'utilisateur d'�tre dans le canal tout court.
INVITE	1	/msg X invite <#channel>\nMakes X invite you to your channel.
ISREG	1	/msg X isreg <#channel>\nCheck if a given channel is registered.
CHANINFO	2	/msg X chaninfo <#canal>\r\nMontre si un canal est enregistr� ou non, � qui, et les DESCRIPTION et URL courantes le cas �ch�ant.
REMOVEALL	2	/msg X removeall <#canal>\r\nSupprime tous les acc�s d'un canal. Cette commande est r�serv�e aux Administrateurs CService.
JOIN	1	/msg X join <#channel>\nMakes X join a channel. If you want X to be in the channel permanetly please refer to the SET AUTOJOIN command (Level 500).
KICK	1	/msg X kick <#channel> <nick|pattern> [reason]\nMakes X kick someone from your channel. If your access level is 200 or higher you can also kick by pattern.
LBANLIST	1	/msg X lbanlist <#channel> <* | *!*user@*.host>\nSearches X's banlist for a certain string. Use * to see the whole list, or specify a *!*user@*.host to find a specific ban. \nIf there are more than 15 entries, go to http://cservice.undernet.org/live/ and login to see the entire list\nby clicking on the "Channel Information" link and submitting the channel name.
LOGIN	1	/msg x@channels.undernet.org login <username> <password>\nThis loads your access listing into memory and authenticates you on ALL channels which you have access on simultaneously.
MODINFO	1	/msg X modinfo <#channel> <access|automode> <username> <value>\nThe following commands can be used to make changes to existing users in your channel's userlist.\nYou cannot modify someone with an equal or higher level.\nWith the variable access you can change a users accesslevel. The range is 1 to your own level - 1.\nThe automode variable allows you to set mode none, voice or op.\nBased on this setting a user can be voiced, oped when he enters the channel.
DEVOICE	12	/msg X devoice <#kanal> [rumuz] [rumuz2] ... [rumuzN]\r\nKanal�n�zda bir ya da daha fazla ki�iden s�z hakk�n� al�r.
CLEARMODE	2	/msg X clearmode <#canal>\r\nSupprime tous les modes du canal. Peut �tre utilis� lorsque votre canal � �t� bloqu� (mis +i ou +k sans votre consentement).\r\nNOTE: Ceci n'est pas forc�ment la meilleur m�thode pour entrer dans votre canal,\r\ncar d'autres personnes ou robots peuvent �tre opp� dans le canal et peuvent remettre les modes d�s que vous les supprimez.\r\nVoyez la commande STATUS (niveau 200) pour d'autres informations
ACCESS	12	
DEOP	2	/msg X deop <#canal> [nick] [nick2] ... [nickN]\r\nD�oppe une ou plusieurs personnes dans votre canal. Si aucun nickname n'est sp�cifi� X vous d�opera vous.\r\nNOTE: T�chez de ne pas d�passer la limite MASSDEOPPRO (voir SET MASSDEOPPRO - niveau 500).
DEVOICE	2	/msg X devoice <#canal> [nick] [nick2] ... [nickN]\r\nD�voice (-v) un ou plusieur utilisateurs dans votre canal.
FORCE	2	/msg X force <#canal>\r\nVous loggue dans le canal sp�cifi� en utilisant un acc�s administrateur. Cette commande est reserv�e aux Administrateurs CService.
ADDCOMMENT	2	/msg X addcomment <utilisateur> <le commentaire>\r\nD�fini un commentaire utilisateur pour l'utilisateur sp�cifi�. Cette commande est r�serv�e aux Administrateurs CService.
HELP	2	/msg X help <commande>\r\nAffiche l'aide pour la commande sp�cifi�e.
INFO	2	/msg X info <utilisateur>\r\nAffiche des informations sur un utilisateur CService, telles que son nom nickname et user@host si il est en ligne,\r\nson language et savoir si il est invisible, l'heure de la derni�re fois ou l'utilisateur � �t� vu, et si il a �t� suspendu par un Administrateur CService.\r\nSi vous l'utilisez sur vous-m�me, cel� listera �galement les canaux auquels vous avez acc�s (seulement CService et vous m�me pouvez voir cette information, pas les autres utilisateurs).\r\nSi vous souhaitez cacher cette information au public, voir � la commande SET INVISIBLE (niveau 0).
INVITE	2	/msg X invite <#canal>\r\nDemandes � X de vous inviter sur le canal sp�cifi�.
ISREG	2	/msg X isreg <#canal>\r\nV�rifie si le canal sp�cifi� est enregistr�.
JOIN	2	/msg X join <#canal>\r\nDemandes � X de joindre un canal. Si vous souhaitez que X reste en permanence dans votre canal, voyez la commande SET AUTOJOIN (niveau 500).
SAY	2	/msg X say <#canal> <texte>\r\nCette commande est r�serv�e aux Administrateurs CService.
SEARCH	2	/msg X search <param�tre_de_recherche>\r\nLances une recherche parmis les canaux de X o� le param�tre de recherche sp�cifi� correspond � un des mots cl�s d'un canal.\r\nL'utilisation de wildcards (*) n'est pas requis.
SERVNOTICE	2	/msg X servnotice <#canal> <texte>\r\nCette commande est r�serv�e aux Administrateurs CService.
KICK	2	/msg X kick <#canal> <nickname|chaine> [raison]\r\nDemandes � X d'�jecter quelqu'un du canal sp�cifi�.\r\nSi vous avez un niveau 200 ou sup�rieur vous pouvez �galement �jecter selon une chaine de caract�re contenu dans le user@host.
SHOWCOMMANDS	2	/msg X showcommands <#canal>\r\nMontre les commandes disponibles pour vous sur le canal sp�cifi�.
SHOWIGNORE	2	/msg X showignore\r\nMontre la liste des user@host ignor�s pas X. X ignore lorsque quelqu'un expr�s ou par accident le flood.\r\nLa dur�e de l'ignore est de 60 minutes.\r\nNote: X ne vous r�pondra pas si vous �tes ignor�. Si vous ne recevez pas de r�ponses de X,\r\ndemandez � un ami de v�rifier la liste des user@host ignor�s pour vous.
LBANLIST	2	/msg X lbanlist <#canal> <* | *!*user@*.host>\r\nEffectue une recherche dans la liste des BANs de X sur une certaine chaine de caract�res.\r\nUtilisez * pour voir la liste compl�te, ou bien sp�cifiez un *!*user@*.host pour trouver un BAN sp�cifique.\r\nSi il y a plus de 15 r�sultats, allez sur http://cservice.undernet.org/live/ et logguez vous pour voir la liste compl�te\r\nen cliquant sur le lien "Channel Information" et en soumettant le nom du canal.
LOGIN	2	/msg X@channels.undernet.org login <utilisateur> <mot_de_passe>\r\nCeci charge votre acc�s dans la m�moire de X et vous authentifie sur tous les canaux auquels vous avez acc�s.
MODINFO	2	/msg X modinfo <#canal> <ACCESS|AUTOMODE> <utilisateur> <valeur>\r\nCette commande peut �tre utilis�e pour effectuer des changement sur des utilisateurs ayant un acc�s sur le canal sp�cifi�.\r\nVous ne pouvez pas modifier quelqu'un ayant un niveau sup�rieur ou �gal au votre.\r\nAvec la variable ACCESS vous pouvez changer le niveau d'un utilisateur. De 1 � ( votre_niveau - 1 ).\r\nLa variable AUTOMODE permets de d�finir NONE, VOICE or OP comme valeurs.\r\nBas� sur ces r�glages, un utilisateur peut �tre voic� ou opp� lorsqu'il entre dans le canal.
MOTD	2	/msg X motd\r\nAffiche le message du jour de X.
STATS	2	/msg X stats\r\nCette commande est r�serv�e aux Administrateurs CService.
REMIGNORE	1	/msg X remignore <*!*host@mask>\nremoves a user@host from the ignore list. This is a CService Admin command only.
STATUS	2	/msg X status <#canal>\r\nDonnes des informations sur les utilisateurs loggu�s sur le canal, etc.\r\nX ne donnera pas les modes (+nt etc.) a quelqu'un avec un niveau d'acc�s inf�rieur � 200.
VERIFY	2	/msg X verify <nickname>\r\nAffiche si la personne est loggu�e ou non, et si oui, sous quel nom d'utilisateur\r\net indique aussi si la personne est un Administrateur CService, ou un Repr�sentant, et/ou un Op�rateur IRC.
REMUSER	2	/msg X remuser <#canal> <utilisateur>\r\nSupprime l'utilisateur sp�cifi� du canal sp�cifi�. Vous devez avoir un acc�s sup�rieur � l'acc�s de l'utilisateur que vous souhaitez effacer.\r\nVous pouvez �galement vous enlever de n'importequel canal,\r\ntant que vous n'en �tes pas le propri�taire (niveau 500 sur ce canal).\r\nLes propri�taires qui veulent se supprimer de leur canal doivent purger leur canal.
PANIC	2	Cette commande ne devrait jamais �tre utilis�e.\r\nL'utiliser serait mal.\r\nMal comment ?\r\nEssayez d'imaginer la vie s'arr�tant instantan�ment, et chaque mol�cule de votre corps explosant � la vitesse de la lumi�re.
DEOP	12	/msg X deop <#kanal> [rumuz] [rumuz2] ... [rumuzN]\r\nKanal�zdaki bir ya da daha fazla ki�inin operat�rl���n� al�r.\r\nE�er hi�bir rumuz belirtilmezse, ve siz kanalda operat�r iseniz, X sizin operat�rl���n�z� al�r.\r\nNOT: MASSDEOPPRO �zelli�indeki limiti a�maman�z konusunda dikkatli olun.
INVITE	12	/msg X invite <#kanal>\r\nX'in sizi kanal�n�za davet etmesini sa�lar.
REMOVEALL	1	/msg X removeall <#channel>\nRemoves all accesses from a channel. This command is for CService Admins only.
REMUSER	1	/msg X remuser <#channel> <username>\nRemoves a user from the channel database. You must have higher access than the user you want to remove. \nYou can also remove yourself from any channel,\nas long as you are not the channel manager (500 access level in that channel).\nManagers wishing to remove themselves will need to purge their channel.
SUSPEND	12	/msg X suspend <#kanal> <kullan�c�_ad�> <zaman_aral���(M|H|D)> [seviye]\r\nBelirtilen s�re boyunca, ki�inin, kanal�n�zda X'e olan eri�imini ask�ya al�r.\r\nE�er hi�bir seviye belirtilmezse belirtilmezse, standart eri�im ask�ya alma seviyesi komutu kullanan ki�inin seviyesi kadard�r.\r\nYaln�zca kendi eri�im seviyenizden daha d���k olan ki�ilere uygulayabilirsiniz.\r\nMaksimum ask�ya alma s�resi 372 g�n (8928 saat, 535680 dakika)d�rr.
INFO	12	/msg x INFO <kullan�c�_ad�>\r\nKullan�c� ad� i�in, e�er IRC'ye ba�l� ise,rumuzunu ve user@host'unu, lisan se�ene�ini,\r\nson g�r�lme bilgilerini ve e�er bir CService Y�neticisi taraf�ndan yetkisinin ask�ya al�n�p al�nmad���n�(suspend) bildirir.\r\nE�er bu komutu kendi kendiniz �zerinde kullan�rsan�z ayn� zamanda yetkiniz olan kanallar�da listeler(Bu bilgi sadece siz ve CService taraf�ndan g�r�lebilir, Di�er kullan�c�lar taraf�ndan de�il.)\r\nE�er bu bilgilerin herkese a��k olmas�n� istemiyorsan�z; INVISIBLE (seviye 0) komutuna ba�vurunuz.\r\n
BANLIST	12	/msg X banlist <#kanal>\r\nKanal yasakl�lar listesini g�sterir. Bu komut kanalda olsan�z da olmasan�z da �al���r.\r\n
FORCE	12	/msg X force <#kanal>\r\nBelirtilen bir kanala sizi y�netici eri�im ile tan�mlar. Bu komut sadece CService y�neticilerine aittir.
KICK	12	/msg x kick <#kanal> <rumuz> [sebep]\r\nKanal�n�zdan bir ki�iyi atman�z� sa�lar; sebep opsiyoneldir. Rumuzunuz sebep i�erisine  eklenecektir.\r\n
OP	12	/msg x op <#kanal> [rumuz] [rumuz2] ... [rumuzN]\r\nKanal�n�zdaki bir veya daha fazla ki�iye operat�rl�k verir. \r\nE�er hi�bir rumuz yaz�lmaz ve sizde kanalda operat�r de�ilseniz, X size operat�rl�k verir.
MODINFO	12	
SET	12	
SET AUTOJOIN	12	
SET INVISIBLE	12	
SET MASSDEOPPRO	12	
ACCESS	11	
ADDCOMMENT	11	
ADDUSER	11	
BAN	11	
BANLIST	11	
CHANINFO	11	
CLEARMODE	11	
DEOP	11	
DEVOICE	11	
FORCE	11	
HELP	11	
INFO	11	
INVITE	11	
ISREG	11	
JOIN	11	
KICK	11	
LBANLIST	11	
LOGIN	11	
MODINFO	11	
MOTD	11	
NEWPASS	11	
OP	11	
OPERJOIN	11	
OPERPART	11	
PANIC	11	
PART	11	
PURGE	11	
QUOTE	11	
REGISTER	11	
REHASH	11	
REMIGNORE	11	
REMOVEALL	11	
REMUSER	11	
SAY	11	
SEARCH	11	
SERVNOTICE	11	
SET	11	
SET AUTOJOIN	11	
SET INVISIBLE	11	
SET MASSDEOPPRO	11	
SHOWCOMMANDS	11	
SHOWIGNORE	11	
STATS	11	
STATUS	11	
SUPPORT	11	
SUSPEND	11	
TOPIC	11	
UNBAN	11	
UNFORCE	11	
UNSUSPEND	11	
VERIFY	11	
VOICE	11	
ACCESS	13	
ADDCOMMENT	13	
ADDUSER	13	
BAN	13	
BANLIST	13	
CHANINFO	13	
CLEARMODE	13	
DEOP	13	
DEVOICE	13	
FORCE	13	
HELP	13	
INFO	13	
INVITE	13	
ISREG	13	
JOIN	13	
KICK	13	
LBANLIST	13	
LOGIN	13	
MODINFO	13	
MOTD	13	
NEWPASS	13	
OP	13	
OPERJOIN	13	
OPERPART	13	
PANIC	13	
PART	13	
PURGE	13	
QUOTE	13	
REGISTER	13	
REHASH	13	
REMIGNORE	13	
REMOVEALL	13	
REMUSER	13	
SAY	13	
SEARCH	13	
SERVNOTICE	13	
SET	13	
SET AUTOJOIN	13	
SET INVISIBLE	13	
SET MASSDEOPPRO	13	
SHOWCOMMANDS	13	
SHOWIGNORE	13	
STATS	13	
STATUS	13	
SUPPORT	13	
SUSPEND	13	
TOPIC	13	
UNBAN	13	
UNFORCE	13	
UNSUSPEND	13	
VERIFY	13	
VOICE	13	
ACCESS	14	
ADDCOMMENT	14	
ADDUSER	14	
BAN	14	
BANLIST	14	
CHANINFO	14	
CLEARMODE	14	
DEOP	14	
DEVOICE	14	
FORCE	14	
HELP	14	
INFO	14	
INVITE	14	
ISREG	14	
JOIN	14	
KICK	14	
LBANLIST	14	
LOGIN	14	
MODINFO	14	
MOTD	14	
NEWPASS	14	
OP	14	
OPERJOIN	14	
OPERPART	14	
PANIC	14	
PART	14	
PURGE	14	
QUOTE	14	
REGISTER	14	
REHASH	14	
REMIGNORE	14	
REMOVEALL	14	
REMUSER	14	
SAY	14	
SEARCH	14	
SERVNOTICE	14	
SET	14	
SET AUTOJOIN	14	
SET INVISIBLE	14	
SET MASSDEOPPRO	14	
SHOWCOMMANDS	14	
SHOWIGNORE	14	
STATS	14	
STATUS	14	
SUPPORT	14	
SUSPEND	14	
TOPIC	14	
UNBAN	14	
UNFORCE	14	
UNSUSPEND	14	
VERIFY	14	
VOICE	14	
JOIN	12	/msg x join <#kanal>\r\nKanal�n�za X'in kat�lmas�n� sa�lar.
VERIFY	12	/msg X verify <rumuz>\r\nBir ki�inin, resmi bir CService y�neticisi ya da temsilcisi olup olmad���n�\r\nveya IRC operat�rl�k durumunu, e�er tan�mlanm��sa, kullan�c� ad�n� bildirir.
VOICE	12	/msg X voice <#kanal> [rumuz] [rumuz2] ... [rumuzN]\r\nKanal�n�zda bir ya da daha fazla ki�iye s�z hakk� verir.
ADDCOMMENT	12	/msg X addcomment <kullan�c�_ad�> <yorum>\r\nVerilen bir kullan�c� ad�na ek bilgi(yorum)\r\neklemek i�in sadece CService Y�neticileri taraf�ndan kullan�l�r.
ISREG	12	Bu komut ile ilgili bir yard�m bulunmamaktad�r.
ADDUSER	12	/msg X adduser <#kanal> <kullan�c�_ad�> <eri�im>\r\nKanal eri�im listesine yeni bir kullan�c� ekler.\r\nKendi eri�imi seviyenize e�it ya da b�y�k bir de�erle ekleme YAPAMAZSINIZ.
UNSUSPEND	12	/msg X unsuspend <#kanal> <kullan�c�_ad�>\r\nKanal�zda eri�im listesinde bulunan ki�inin, eri�iminin ask�ya al�nma durumunu iptal eder.\r\nKendininzden d���k eri�imdeki kimseye uygulayabilirsiniz. \r\nAyr�ca ask�ya alma seviyesininde sizin seviyenize e�it yada daha az olmas� gerekmektedir.
LBANLIST	12	/msg x lbanlist <#kanal> <* | *!*user@*.host>\r\nBelirli dizin i�in X'in yasakl�lar listesini ara�t�r�r.\r\nT�m listeyi g�rmek i�in * i�aretini kullanabilir, ya da belirli bir yasakl�y� g�rmek i�in *!*user@*.host kullanabilirsiniz.\r\n
PANIC	12	Bu komut ile ilgili bir yard�m bulunmamaktad�r.
BAN	12	/msg X ban <#kanal> <rumuz| *!*user@*.host> [zaman] [seviye] [sebep]\r\nKanal�n�z�n yasakl�lar listesine belirli bir *!*user@*.host ekler.\r\nKi�i IRC'ye ba�l�ysa rumuzunu, ba�l� de�ilse *!*user@*.host bilgisini yazman�z gerekir.\r\nkoyulan bir yasaklama eski yasaklamalar� i�eriyorsa, yenisiyle de�i�tirilirler.\r\nolarak belirlenebilir.\r\nYasaklama seviyesi 1'den kendi seviyenize kadar de�i�ebilir. Ban sebebi en fazla 128 karakter olabilir.\r\nE�er hi�bir de�erlik belirtilmezse zaman 3 saat, seviye ise 75 olarak belirlenir.\r\nYasaklama seviyeleri:  1-74 = kanalda operat�rl�k (+o) almas�n� engeller.\r\nYasaklama seviyeleri:  75+ = Ki�inin kanalda bulunmas�n� engeller.\r\n
SUPPORT	12	/msg X support <#kanal> <yes|no>\r\nDestekleyici olarak listelendi�iniz bir kanal�n ba�vurusunda, kanala verdi�iniz deste�i do�rulamak ya da reddetmek i�in alternatif bir y�ntemdir.\r\nTan�mlanman�zla birlikte, X sizi destekleyici olarak listelenmi� oldu�unuz kanal hakk�nda uyaracak ve ayr�ca SUPPORT komutunu kullanman�z� isteyecektir. \r\nAlternatif olarak, kanal ba�vurusu hakk�nda daha fazla bilgi almak i�in siteye bakabilir, itiraz yollayabilir ve deste�inizi onaylayabilir ya da reddedebilirsiniz.\r\nE�er kanal i�in bir destekleyici olarak listelenmemi�seniz ya da kanal�n beklemede olan bir ba�vurusu yoksa komut etkili olmayacakt�r.
QUOTE	12	Bu komut ile ilgili bir yard�m bulunmamaktad�r.
LOGIN	12	/msg x@channels.undernet.org login <kullan�c�_ad�> <�ifre>\r\nEri�im listelenmenizi haf�zaya al�rken, e�zamanl� olarak, eri�imli oldu�unuz t�m kanallarda tan�mlanman�z� sa�lar.\r\n
MOTD	12	/msg x motd\r\nX'in g�n�n mesaj�n� g�r�nt�ler.
NEWPASS	12	/msg x@channels.undernet.org newpass <yeni �ifre>\r\nKullan�c� ad�n�z i�in �ifrenizi de�i�tirmenizi sa�lar. \r\nKomutun �ifrenizi de�i�tirmede ba�ar�l� olmas� i�in, eski �ifreniz ile tan�mlanm�� olman�z gereklidir.\r\n�ifreler durum hassasiyetli, yani b�y�k k���k karakter �nemlidir.\r\n�rne�in: "CService", "cservice" de�ildir.\r\n�ifreler, ke�fedilmelerinin zorla�mas� a��s�ndan, bir kelimeden uzun olabilirken, bir c�mle ya da deyim olmas� dahi m�mk�nd�r.\r\nSizi, bu �zelli�i kullanman�z i�in te�vik ediyoruz. En fazla uzunluk 50 karakterdir.\r\n�rne�in, bir �ifre �u olabilir: "Undernet Kanal Servisi"\r\n
REGISTER	12	/msg X register <#kanal> <kullan�c�_ad�>\r\nBu komut sadece CService Y�neticileri taraf�ndan kullan�l�r.
CLEARMODE	12	/msg X clearmode <#kanal>\r\nT�m kanal modlar�n� temizler. Kanal�n�z kilitlendi�i zaman kullanabilirsiniz\r\n(bilginiz olmadan +i ya da +k konulmas� gibi).\r\nNOT: Bu kanal�n�za giri�inizi kazanman�z i�in, uygulanmas� gereken en iyi ��z�m y�ntemi de�ildir.\r\n��nk� di�er kullan�c�lar ya da botlar, siz kodlar� sildik�e yeniliyor olabilirler.\r\n�lgili bilgilendirmeler i�in, STATUS komutuna (seviye 200) ba�vurunuz.
REHASH	12	/msg X rehash <table>\r\nBu komut sadece CService Geli�tiricileri taraf�ndan kullan�labilir.
SEARCH	1	/msg X search <search parameter>\nMakes X list channels where the given search parameters match set keywords on the channel, if any.\nThe use of wildcards are not required.
SET	2	/msg X set invisible <yes/no>\r\nChange le status invisible/visible pour votre nom d'utilisateur.\r\nQuand c'est ON (yes), un autre utilisateur ne peux pas voir\r\nsi vous �tes loggu� � X ou non, si il ne connais pas votre nickname.\r\n\r\n/msg X set lang <code de langue>\r\nAvec cette commande vous pouvez sp�cifier dans quel language vous souhaitez que X vous parle.\r\nLanguages disponibles :\r\ndk - Danois, en - Anglais, es - Espagnol, fr - Fran�ais, hu - Hongrois,\r\nit - Italien, no - Norvegien, ro - Roumain, tr - Turque...\r\n\r\n/msg X set <#canal> <parametre> <valeur> \r\nCette commande vous permet de d�finir les parametres de votre canal.\r\nAUTOJOIN [on/off] - Quand c'est "ON" X joindra tout le temps votre canal, quand c'est "OFF"\r\n                    ca le fera partir si il est sur le canal, et/ou ne pas revenir apr�s un split.\r\nMASSDEOPPRO [1-7] - Le nombre maximum de d�ops que X autorisera par p�riode de 15 secondes.\r\nNOOP [on/off] - Quand c'est "ON", seul X peut �tre opp�, il deoppera tous les autres.\r\nSTRICTOP [on/off] - Ceci � "ON" n'autorise que les niveaux 100 et + authentifi�s � �tre opp�.\r\nAUTOTOPIC [on/off] - Ceci � "ON" fait que X remette le topic avec DESCRIPTION et URL toutes les 30 minutes. \r\n                     Noter que si le canal est idle, le topic ne sera pas remis durant cette periode d'inactivit�.\r\nDESCRIPTION [description] - D�fini la DESCRIPTION par d�faut du cananl (autotopic) (maximum de 80 caract�res). \r\nKEYWORDS [keywords] - D�fini les mots cl�s qui permettrons de trouver votre canal avec la commande SEARCH (niveau 0).\r\nMODE - Ceci sauvegardera les modes (+spntmilk) de votre canal dans le mode par d�faut.\r\nURL [url] - Vous pouvez d�finir l'URL de la page d'accueil de votre canal avec ceci (maximum de 75 caract�res)\r\nUSERFLAGS [0-2] - Determine si OP ou VOICE seront automatiquement d�fini pour les AUTOMODE\r\n                  quand un nouveau utilisateur est ajout� � X. La valeur par d�faut est 0 (aucun).\r\n                  valeurs: 0: aucun  1: OP  2: VOICE
OP	1	/msg X op <#channel> [nick] [nick2] ... [nickN]\nOps one or more persons in your channel. If no nick is specified, and you are not opped on the channel, X will op you.
SUPPORT	2	/msg X support <#canal> <yes/no>\r\nUne autre m�thode pour vous permettre de supporter ou de refuser le support pour une nouvelle application de canal\r\ndans laquelle vous �tes list� comme supporter. Lors de votre authentification X vous pr�viendra de toute application\r\nou vous serez list� comme supporter et vous pr�viendra �galement pour cette commande SUPPORT.\r\nVous pouvez �galement souhaiter voir l'application pour avoir plus d'informations,\r\nposter une objection, et confirmer ou refuser votre support.\r\nLa commande ne fonctionnera pas si vous n'�tes pas list� comme supporter pour le canal,\r\nou que le canal n'a pas d'application en cours sur le web.
OPERJOIN	1	/msg X operjoin <#channel>\nThis makes X join a given channel. This is an IRC Operator command only and should only be used to fix desynches.
SUSPEND	2	/msg X suspend <#canal> <utilisateur> <dur�e(M|H|D)> [niveau]\r\nSuspend l'acc�s � X d'un utilisateur sur votre canal pour la dur�e sp�cifi�e, au niveau sp�cifi�.\r\nSi le niveau n'est pas sp�cifi�, le niveau de l'utilisateur envoyant la commande sera utilis� par defaut.\r\nVous ne pouvez suspendre que des gens ayant un niveau inf�rieur au votre. La dur�e maximale est de 372 days (8928 hours, 535680 minutes).
PANIC	1	This command should never be used.\nUsing it would be bad.\nHow bad?\nTry to imagine all life stopping instantaniously, and every molecule in your body exploding at the speed of light.
TOPIC	2	/msg X topic <#canal> <titre>\r\nFait que X change le topic (titre du canal) dans votre canal (maximum 145 caract�res)\r\net ajoute votre nom d'utilisateur, entre parenth�ses, au d�but du <titre> sp�cifi�.\r\nNOTE: Si une DESCRIPTION ou une URL ont �t� d�finies et que AUTOTOPIC est � ON,\r\nle <titre> de canal sera remis toutes les 30 minutes, � moins que le canal ne soit idle.
OPERPART	1	/msg X operpart <#channel>\nThis makes X part a given channel. This is an IRC Operator command only and should only be used to fix desynches.
TOPIC	12	/msg x topic <#kanal> <ba�l�k>\r\nKanal ba�l���n�, ba�l�k ba�lang�c�na, parantez i�erisinde kullan�c� ad�n�z� bar�nd�rarak de�i�tirecektir (en fazla 145 karakter).\r\nNOT: E�er DESCRIPTION ya da URL belirlenmi�se, kanala giri� ��k�� oldu�u s�rece, 30 dakika i�inde de�i�ecektir.
UNFORCE	12	/msg X unforce <#kanal>\r\nBu komut sadece CService Y�neticileri taraf�ndan kullan�labilir.
UNBAN	12	/msg x unban <#kanal> <rumuz | *!*user@*.host>\r\n*!*user@*.host de�erli�ini ve di�er benzer yasaklamalar�, kanal yasakl�lar listenizden ��kar�r.\r\nAncak kendi seviyenize e�it ya da daha d���k olan yasaklamalar�, X'in yasakl�lar listesinden ��karabilirsiniz.\r\nX'in yasakl�lar listesinde arama yapmak i�in, LBANLIST (seviye 0) komutuna ba�vurunuz.\r\n
STATUS	12	/msg x status #kanal\r\nKanal modlar�n�n (+tn vb.) 200 eri�im alt�ndakilere bildirilmemesinin haricinde,  seviye 200'deki STATUS komutuyla ayn� �eyi yapar.
PART	12	/msg x part <#kanal>\r\nX'in kanalda ayr�lmas�n� sa�lar. E�er X'in kanal�n�zdan uzun bir zaman i�in ayr�lmas�n� istiyorsan�z SET AUTOJOIN komutunu kullan�n�z.
REMIGNORE	12	/msg X remignore <*!*host@mask>\r\nBir user@host'u ald�rmama(ignore) listesinden silmek i�in sadece CService Y�neticileri taraf�ndan kullan�labilen bir komuttur.
REMOVEALL	12	/msg X removeall <#kanal>\r\nBir kanaldaki t�m yetkileri silmek i�in sadece CService Y�neticileri taraf�ndan kullan�labilen komuttur.
REMUSER	12	/msg x remuser <#kanal> <kullan�c�_ad�>\r\nBir kullan�c�y� yetki veri bankas�ndan silmeyi sa�lar. \r\nKullan�c�y� silmek i�in yetkinizin silmek istedi�iniz ki�inin yetkisinden daha fazla olmas� �artt�r.\r\nBu seviyede ve �zerinde, 500 eri�imli yani kanal sahibi olmad���n�z s�rece kendinizi herhangi bir kanaldan silebilirsiniz. \r\nKendilerini silmek isteyen kanal sahipleri i�in, kanallar�n� kapatt�rmalar� gereklidir.\r\n
SAY	12	/msg X say <#kanal> <yaz�>\r\nBu komut sadece CService Y�neticileri taraf�ndan kullan�labilir.
SEARCH	12	/msg x search <arama parametresi>\r\nVerdi�iniz ve bir kanal i�in nitelendirilmi� arama parametrelerinin, e�er varsa, bulundu�u kanallar� listeler.
SERVNOTICE	12	/msg X servnotice <#kanal> <yaz�>\r\nBu komut sadece CService Y�neticileri taraf�ndan kullan�labilir.
STATS	12	/msg X stats\r\nBu komut sadece CService Y�neticileri taraf�ndan kullan�labilir.
SHOWIGNORE	12	/msg X showignore\r\nX'in komutlar�na ald�rmad��� ki�ilerin listesini g�sterir. \r\nX'in bu "komutlara ald�rmama i�lemi" bir ki�inin isteyerek ya da kaza ile X'e a��r� komut g�nderimi yapmas�yla ger�ekle�ir.Zaman uzunlu�u 60 dakikad�r.\r\nNOT: X'in komutlar�na ald�rmad��� ki�iler listesinde oldu�unuz s�rece X size kesinlikle hi� bir yan�t vermeyecektir bu y�zden bu listede olup olmad���n�z� bir arkada��n�z yard�m� ile kontrol etmelisiniz.
SHOWCOMMANDS	12	/msg x showcommands [#kanal]\r\nEri�im seviyenize g�re, kanalda kullanabilce�iniz t�m komutlar� listeler.\r\nE�er hi�bir kanal belirtilmezse, sadece 0 eri�im komutlar� g�r�nt�lenir.\r\n
PURGE	12	/msg X purge <#kanal> <sebep>\r\nKanal kay�d�n� siler. Sadece CService Y�neticileri taraf�ndan kullan�labilir.
OPERJOIN	12	/msg X operjoin <#kanal>\r\nX'i belirtilen bir kanala getirir. Sadece IRC Operat�rleri taraf�ndan, desynch durumlar�n� d�zeltmek amac� ile kullan�l�r.
OPERPART	12	/msg X operjoin <#kanal>\r\nX'i belirtilen bir kanaldan ��kart�r. Sadece IRC Operat�rleri taraf�ndan, desynch durumlar�n� d�zeltmek amac� ile kullan�l�r.
SUSPENDME	8	Type /msg X@channels.undernet.org SUSPENDME <parola>\nAceasta comandaeste pentru a putea fi folosita in situatia in care tu crezi ca username-ul tau este compromis, si doresti suspendarea imediata a tuturor acceselor pe care le ai.\nTrebuie sa fie mai mult de o persoana logata cu username-ul tau pentru a putea fi folosita aceasta comanda.\nDupa folosirea comenzii, nu mai poti face nimic cu username-ul tau pana cand nu este verificat de un Reprezentant CService.\nPentru a cere UnSuspendarea username-ului scrie un email la Object@Undernet.Org
ACCESS	8	/msg X access <#canal> <*|masca|username|=nickname> [-min <nivel>] [-max <nivel>] [-modif] [-none|-voice|-op]\nSe poate verifica accesul unei persoane sau a unei masti. Informatia indica nivelul, suspendarea (daca este cazul), si cand a fost vazut ultima oara userul.\nDaca sunt mai mult de 15 accese, du-te la http://cservice.undernet.org/live/ si logheaza-te, ca sa vezi intreaga lista da click pe optiunea "Channel Information" si completeaza numele canalului. \nDe asemenea puteti sa folositi deodata optiunile il locul username-ului sau mastii pentru a afla un anume rezultat.
ADDCOMMENT	8	/msg X addcomment <username> <comentariu>\nSeteaza un comentariu la un username dat. Aceasta comanda este numai pentru Adminii CService.
ADDUSER	8	/msg X adduser <#canal> <username> <acces>\nAdauga un user la lista de accese a canalului. Nu poti folosi comanda ADDUSER la cineva care are access egal sau mai mare ca al tau. 
BAN	8	/msg X ban <#canal> <nick | *!*user@*.host> [perioada] [nivel] [motiv]\nAdauga un ban pe un *!*user@*.host specificat la lista de banuri a lui X. \nPoti pune un ban pe un nick, daca persoana este conectata, sau sa dai ban pe *!*user@*.host daca nu e online.\nBanuri care acopera un ban dat anterior va inlocui primul ban. \nOrice user de pe canal la care is se potriveste addresa banata va primi kick, daca nivelul pe care e dat banul e 75+.\nNivelul banului poate lua valori intre 1 si nivelul accesului tau pe canal. Motivul de ban poate avea maxim 128 de caractere. \nDaca nu este specificata durata sau motivul, durata defaul va fi de 3 ore, iar nivelul de ban este 75.\nNiveluri de Ban:   1-74 = Interzice unui user sa fie operator (+o) pe canal.\nNiveluri de Ban: 75-500 = Interzice unui user sa fie pe canal.
BANLIST	8	/msg X banlist <#canal>\nArata lista de banuri a canalului. Aceasta comanda merge chiar daca sunteti sau nu pe canal.
TOPIC	8	/msg X topic <#canal> <topic>\nX va schimba topicul (maxim 145 caractere) si va include username-ul tau intre paranteze, la inceputul topicului. \nNOTA: Daca DESCRIPTION sau URL-ul a fost setat si AUTOTOPIC este ON, topicul va fi resetat peste 30 de minute, daca canalul nu e idle.
CLEARMODE	8	/msg X clearmode <#canal>\nSterge toate modurile canalului. Poate fi folosita cand canalul a fost inchis (set +i sau +k fara cunostinta ta).\nNOTA: Aceasta nu este neaparat cea mai buna metoda de a intra pe canal, deoarece alti useri sau boti care sunt operatori pe canal pot reseta modurile dupa ce le-ai sters. \nVezi comanda STATUS (nivel 200) pentru alte informatii.
SERVNOTICE	1	/msg X servnotice <#channel> <text>\nThis command is for CService Admins only.
SET MODE	1	/msg X set <#channel> MODE\nThis will save the current channel modes as default.
SUSPEND	1	/msg X suspend <#channel> <username> <duration<M|H|D>> [level]\nSuspends a user's access to X on your channel's userlist for the specified period of time, at the specified level if given.\nIf a level is not specified, the default suspension level used will be the access level of the issuer. \nYou can only SUSPEND someone with access lower than your own. The maximum duration is 372 days (8928 hours, 535680 minutes).
CHANINFO	8	/msg X chaninfo <#canal>\nArata daca canalul este sau nu inregistat, userul managerului, si actuala DESCRIPTION si URL, daca exista.
NEWPASS	2	D�sormais, les changements de mot de passe devront �tre effectu�s en utilisant la page web.\nAllez � http://cservice.undernet.org/live/ et logguez vous avec votre ancien mot de passe.\nQuand la page est charg�e, cliquez sur le lien "newpass" dans le menu de gauche et suivez les instructions.
DEOP	8	/msg X deop <#canal> [nick] [nick2] ... [nickN]\nDeopeaza una sau mai multe persoane din canal. Daca nu e specificat nici un nick si esti operator pe canal, X te va deopa. \nNOTA: Fi atent la limita care este setata la MASSDEOPPRO (vezi SET MASSDEOPPRO - nivel 500).
DEVOICE	8	/msg X devoice <#canal> [nick] [nick2] ... [nickN]\nDa devoice unuia sau mai multor useri de pe canal.
FORCE	8	/msg X force <#canal>\nTe logheaza pe un canal dat si poti folosi un acces administrativ. Aceasta comanda este doar pentru Adminii CService.
HELP	8	/msg X help <comanda>\nArata informatia de ajutor pentru o anume comanda.
INFO	8	/msg X info <username>\nArata informatii despre un username cum ar fi nick-ul si user@host daca sunt conectati, limba aleasa, cand a fost vazut ultima data si setarile de invisible, si daca aveti suspend de la un CService Administrator. \nDaca este folosita pe userul personal, va arata deasemenea si e-mail-ul dumneavoastra inregistrat, ultimul hostmask in formatul (nick!userid@host) folosit cand username-ul a fost logat , va lista toate canalele pe care ai acces (doar CService si tu pot vedea aceasta informatie, nu si alti useri). \nDaca doresti sa ascunzi aceste informati, vezi comanda SET INVISIBLE (Nivel 0).
INVITE	8	/msg X invite <#canal>\nX te va invita pe canal.
ISREG	8	/msg X isreg <#canal>\nVerifica daca un canal dat este inregistrat.
JOIN	8	/msg X join <#canal>\nIl face pe X sa intre pe canal. Daca vrei ca X sa fie pe canal permanent atunci uite-te la comanda SET AUTOJOIN (Nivel 500).
KICK	8	/msg X kick <#canal> <nick|pattern> [motiv]\nIl face pe X sa ii dea kick cuiva de pe canalul tau. Daca nivel tau de acces este 200 sau mai mare poti da kick prin model.
LBANLIST	8	/msg X lbanlist <#canal> <* | *!*user@*.host>\nCauta in lista de banuri a lui X pentru o anumita masca. Folositi * pentru a vedea intreaga lista, sau specifica un *!*user@*.host pentru a gasi un ban anume. \nDaca sunt mai mult de 15 banuri du-te la http://cservice.undernet.org/live/ si logheaza-te, ca sa vezi intreaga lista da lick pe optiunea "Channel Information" si completeaza numele canalului.
LOGIN	8	/msg x@channels.undernet.org login <username> <parola>\nAceasta incarca accesele tale in memorie si te autentifica pe TOATE canalele pe care ai access.
MODINFO	8	/msg X modinfo <#canal> <access|automode> <username> <valuare>\nAceste comenzi pot fi folosite pentru a face schimbari userilor existenti in lista de accese a canalului.\nNu poti modifica pe cineva care are acces egal sau mai mare decat at tau.\nCu aceasta variabila poti schimba accesul cuiva. Poti da acces cu 1 mai putin nivelului tau.\nVariabila automode iti da dreptul sa setezi modul cuiva none, voice sau op.\nPe baza acestei setari un user poate avea voice, op cand intra pe canal.
MOTD	8	/msg X motd\nArata mesajul zilei a lui X.
NEWPASS	8	De acum inainte toate schimbarile de parola se vor face doar prin website. Viziteaza http://cservice.undernet.org/ si da click pe "web interface" , apoi logheaza-te cu VECHEA ta parola. Cand pagina s-a incarcat da click pe link-ul "newpass" din coloana din stanga si urmeaza instructiunile.
OP	8	/msg X op <#canal> [nick] [nick2] ... [nickN]\nDa op unei sau mai multor persoane de pe canal. Daca nu e specificat nici un nick si nu esti op pe canal, X iti va da op.
OPERJOIN	8	/msg X operjoin <#canal>\nAceasta comanda il face pe X sa intre pe un canal specificat. Aceasta comanda este o comanda doar pentru IRC Operatori si este folosita doar pentru rezolvarea desincronizarii.
OPERPART	8	/msg X operpart <#canal>\nAceasta comanda il face pe X sa plece pe un canal specificat. Aceasta comanda este o comanda doar pentru IRC Operatori si este folosita doar pentru rezolvarea desincronizarii.
UNBAN	8	/msg X unban <#canal> <nick | *!*user@*.host>\nSterge o masca *!*user@*.host din lista de banuri a lui X si orice ban care se potriveste cu adresa. \nPoti sterge banuri din lista lui X daca sunt egale sau mai mici ca  nivel cu nivelul tau de pe canal. \nPentru a cauta in lista de banuri a lui X vezi comanda LBANLIST (nivel 0).
PANIC	8	Aceasta comanda nu trebuie folosita.\nFolosind-o ar fi un lucru rau.\nCat de rau?\nIncearca sa-ti imaginezi toata viata oprindu-se instantaneu, si fiecare molecula din corpul tau explodand la viteza luminii.
PART	8	/msg X part <#canal>\nX va iesi de pe canalul tau. Daca vrei sa scoti X de pe canal pentru o mai lunga perioada de timp uite-te la comanda ET AUTOJOIN (Nivel 500).
PURGE	8	/msg X purge <#canal> <motiv>\nPurjeaza un canal. Aceasta comanda este doar pentru Adminii CService.
QUOTE	8	Nu te atinge :P
REGISTER	8	/msg X register <#canal> <username>\nAceasta comanda este doar pentru Adminii CService.
REHASH	8	/msg X rehash <tabel>\nAceasta comanda este doar pentru Programatorii CService.
REMOVEALL	8	/msg X removeall <#canal>\nSterge toate accesele de pe un canal. Aceasta comanda este doar pentru Adminii CService.
UNFORCE	8	/msg X unforce <#canal>\nAceasta comanda este doar pentru Adminii CService.
REMIGNORE	8	/msg X remignore <*!*host@mask>\nSterge un user@host din lista de ignore. Aceasta comanda este doar pentru Adminii CService.
REMUSER	8	/msg X remuser <#canal> <username>\nSterge un user din baza de date a canalului. Trebuie sa ai acces mai mare decat acel user. \nDeasemenea te poti sterge singur de pe orice canal, atat timp cat nu esti managerul de canal (nivel 500 pe acel canal).\nManagerii care vor sa se stearga singuri vor trebui sa ceara "Purge" la canal.
SAY	8	/msg X say <#canal> <text>\nAceasta comanda este doar pentru Adminii CService.
SEARCH	8	/msg X search <parametri de cautare>\nIl face pe X sa listeze canalele unde anumiti parametri de cautare se potrivesc cu keyword-ul canalului, daca exista. \nNu este necesara folosirea wildcard-urilor (*).
SERVNOTICE	8	/msg X servnotice <#canal> <text>\nAceasta comanda este doar pentru Adminii CService.
SET	8	/msg X set <#canal> <variabila> <valuare>\n/msg X set <variabila> <valuare>\nAceasta comanda va seta un canal sau un username.\nSetari pentru username: INVISIBLE MAXLOGINS LANG\nSetari pentru canal: AUTOJOIN AUTOTOPIC DESCRIPTION KEYWORDS MASSDEOPPRO MODE NOOP STRICTOP URL USERFLAGS\nPentru mai multe informatii scrie: /msg X help SET <variabila> 
UNSUSPEND	8	/msg X unsuspend <#canal> <username>\nScoate suspendul de pe accesul unui user la X. \nPoti folosi comanda UNSUSPEND doar pe un user cu access mai mic ca al tau, iar level-ul suspendului trebuie sa fie mai mic sau egal cu propriul tau acces.
SET AUTOJOIN	8	/msg X set <#canal> autojoin ON|OFF\nCand este setat ON, il face pe X sa intre intotdeauna pe canal. \nCand este setat OFF, il va face pe X sa iasa de pe canal si sa nu reintre dupa un netsplit.
SET AUTOTOPIC	8	/msg X set <#canal> AUTOTOPIC ON|OFF\nX va schimba topic-ul canalului in URL-ul si DESCRIPTION canalului odata la 30 de minute.\nNota: Daca, canalul este idle, topicul nu va fi resetat in acest timp.
SET DESCRIPTION	8	/msg X set <#canal> DESCRIPTION <textul aici>\nSeteaza descrierea unul canal. Aceasta setare poate fi vazuta in reply-ul de la CHANINFO.
SET INVISIBLE	8	/msg X set INVISIBLE ON|OFF\nSeteaza invisible on sau off pentru username-ul tau.\nCand aceasta setare este on, alt user nu poate vedea chiar daca esti online sau nu.
SET KEYWORDS	8	/msg X set <#canal> <text>\nSeteaza cuvintele cheie ale canalului. Poti folosi comanda SEARCH pentru a gasi canalele cu un cuvant cheie setat.
SET LANG	8	/msg X set lang <cod de limba>\nCu aceasta comanda poti specifica limbajul cu care X iti raspunde.\nLimbi disponibile: AR - Arabic, CA - Catalan, CH - Christmas, DK - Danish, NL - Dutch, EA - Easter, EN - English, FR - Fran�ais, DE - German, IL - Hebrew, HU - Hungarian, IT - Italian, NO - Norsk, PT - Portuguese, RO - Romanian, ES - Spanish, TR - Turkce
SET MASSDEOPPRO	8	/msg X set <#canal> MASSDEOPPRO <valuare 0-7>\nNumarul maxim de deopari pe care X  le va accepta intr-o perioada de 15 sec.\nOrice user care depaseste va primi depop, si daca e in lista de accese si un suspend pentru 5 minute. Deasemenea ei vor avea ban de nivel 25 pentru a preveni reoparea, dupa ce suspendul expira ei trebuie sa isi scoata singuri ban-ul.
SET MAXLOGINS	8	/msg X set MAXLOGINS <valuare> (valuarea trebuie sa fie intre 1 si 3. Valoarea din default(initiala) este 1).\nAceasta optiune personala si te lasa sa determini numarul de logari curente al username-ului tau.\nDupa ce te loghezi la username pentru a 2-a si a 3-a oara, un avertisment este trimis la cei care sunt deja autentificati, dand detalii cine tocmai a s-a logat cu acel username.\nAceasta este o masura de protectie, si poate sa te avertizeze mai devreme ca cineva are parola ta si abuzeaza/foloseste username-ul tau.
SET MODE	8	/msg X set <#canal> MODE\nAceasta optiune va salva actualele moduri ale canalului ca default.
SET NOOP	8	/msg X set <#canal> NOOP ON|OFF\nCand aceasta setare este ON, inseamna ca nimeni nu poate fi operator de canal cu exceptia lui X.
SET STRICTOP	8	/msg X set <#canal> STRICTOP ON|OFF\nCand aceasta setare este ON, va lasa doar user-ii autentificati de Nivel 100+ ca sa fie opati.
SET URL	8	/msg X set <#canal> URL <http://url>\nPoti face publica adresa (adresele) canalului cu aceasta optiune (maxim of 75 de charactere).\nOptiunea poate fi vazuta in reply-ul de la comanda CHANINFO.
VERIFY	8	/msg X verify <nick>\nArata daca persoana este sau nu logata, si daca este asa va arata username-ul.\nSi de asemenea indica daca acesta este un Administrator sau Reprezentant Oficial de la CService si/sau este un IRC Operator.
VOICE	8	/msg X voice <#canal> [nick] [nick2] ... [nickN]\nDa voice unuia sau mai multor useri de pe canal.
\.
