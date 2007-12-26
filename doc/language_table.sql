-- Language Definition Table for CMaster Undernet channel services.
-- 24/12/01 - nighty <nighty@undernet.org>. (fixed missing field)


-- Zap everything!
DELETE FROM translations;


-- English language definition.
-- 27/12/00 - Greg Sikorski <gte@atomicrevs.demon.co.uk>.
-- 01/03/01 - Daniel Simard <svr@undernet.org>.

COPY "translations" FROM stdin;
1	1	Sorry, You are already authenticated as %s	31337	0
1	2	AUTHENTICATION SUCCESSFUL as %s	31337	0
1	3	Sorry, you have insufficient access to perform that command	31337	0
1	4	Sorry, the channel %s is empty	31337	0
1	5	I don't see %s anywhere	31337	0
1	6	I can't find %s on channel %s	31337	0
1	7	The channel %s doesn't appear to be registered	31337	0
1	8	You're opped by %s (%s) on %s	31337	0
1	9	You're voiced by %s (%s) on %s	31337	0
1	10	%s: You are not in that channel	31337	0
1	11	%s is already opped in %s	31337	0
1	12	%s is already voiced in %s	31337	0
1	13	You're deopped by %s (%s) on %s	31337	0
1	14	You're devoiced by %s (%s) on %s	31337	0
1	15	%s is not opped in %s	31337	0
1	16	%s is not voiced in %s	31337	0
1	17	AUTHENTICATION FAILED as %s (Invalid Password)	31337	0
1	18	I'm not in that channel!	31337	0
1	19	Invalid banlevel range. Valid range is 1-%i.	31337	0
1	20	Invalid ban duration. Your ban duration can be a maximum of %d hours.	31337	0
1	21	Ban reason cannot exceed 128 chars	31337	0
1	22	Specified ban is already in my banlist!	31337	0
1	23	The ban %s is already covered by %s	31337	0
1	24	Invalid minimum level.	31337	0
1	25	Invalid maximum level.	31337	0
1	26	USER: %s ACCESS: %s %s	31337	0
1	27	CHANNEL: %s -- AUTOMODE: %s	31337	0
1	28	LAST MODIFIED: %s (%s ago)	31337	0
1	29	** SUSPENDED ** - Expires in %s (Level %i)	31337	0
1	30	LAST SEEN: %s ago.	31337	0
1	31	There are more than %d matching entries.	31337	0
1	32	Please restrict your query.	31337	0
1	33	End of access list	31337	0
1	34	No Match!	31337	0
1	35	Cannot add a user with equal or higher access than your own.	31337	0
1	36	Invalid access level.	31337	0
1	37	%s is already added to %s with access level %i.	31337	0
1	38	Added user %s to %s with access level %i	31337	0
1	39	Something went wrong: %s	31337	0
1	40	%s: End of ban list	31337	0
1	41	Unable to view user details (Invisible)	31337	0
1	42	Information about: %s (%i)	31337	0
1	43	Currently logged on via: %s	31337	0
1	44	URL: %s	31337	0
1	45	Language: %i	31337	0
1	46	Channels: %s	31337	0
1	47	Input Flood Points: %i	31337	0
1	48	Ouput Flood (Bytes): %i	31337	0
1	49	%s is registered by:	31337	0
1	50	%s - last seen: %s ago	31337	0
1	51	Desc: %s	31337	0
1	52	Flood me will you? I'm not going to listen to you anymore	31337	0
1	53	I think I've sent you a little too much data, I'm going to ignore you for a while.	31337	0
1	54	Incomplete command	31337	0
1	55	To use %s, you must /msg %s@%s	31337	0
1	56	Sorry, You must be logged in to use this command.	31337	0
1	57	The channel %s has been suspended by a cservice administrator.	31337	0
1	58	Your access on %s has been suspended.	31337	0
1	59	The NOOP flag is set on %s	31337	0
1	60	The STRICTOP flag is set on %s	31337	0
1	61	You just deopped more than %i people	31337	0
1	62	SYNTAX: %s	31337	0
1	63	Temporarily increased your access on channel %s to %i	31337	0
1	64	%s is registered.	31337	0
1	65	%s is not registered.	31337	0
1	66	I don't think %s would appreciate that.	31337	0
1	67	\002*** Ban List for channel %s ***\002	31337	0
1	68	%s %s Level: %i	31337	0
1	69	ADDED BY: %s (%s)	31337	0
1	70	SINCE: %s	31337	0
1	71	EXP: %s	31337	0
1	72	\002*** END ***\002	31337	0
1	73	I don't know who %s is.	31337	0
1	74	You are not authorised with me anymore.	31337	0
1	75	%s doesn't appear to have access in %s.	31337	0
1	76	Cannot modify a user with equal or higher access than your own.	31337	0
1	77	Cannot give a user higher or equal access to your own.	31337	0
1	78	Modified %s's access level on channel %s to %i	31337	0
1	79	Set AUTOMODE to OP for %s on channel %s	31337	0
1	80	Set AUTOMODE to VOICE for %s on channel %s	31337	0
1	81	Set AUTOMODE to NONE for %s on channel %s	31337	0
1	82	Your passphrase cannot be your username or current nick - syntax is: NEWPASS <new passphrase>	31337	0
1	83	Password successfully changed.	31337	0
1	84	The NOOP flag is set on %s	31337	0
1	85	The STRICTOP flag is set on %s (and %s isn't authenticated)	31337	0
1	86	The STRICTOP flag is set on %s (and %s has insufficient access)	31337	0
1	87	Purged channel %s	31337	0
1	88	%s is already registered with me.	31337	0
1	89	Invalid channel name.	31337	0
1	90	Registered channel %s	31337	0
1	91	Removed %s from my silence list	31337	0
1	92	Couldn't find %s in my silence list	31337	0
1	93	Cannot remove a user with equal or higher access than your own	31337	0
1	94	You can't remove yourself from a channel you own	31337	0
1	95	Removed user %s from %s	31337	0
1	96	Your INVISIBLE setting is now ON.	31337	0
1	97	Your INVISIBLE setting is now OFF.	31337	0
1	98	%s for %s is %s	31337	0
1	99	value of %s must be ON or OFF	31337	0
1	100	Invalid USERFLAGS setting. Correct values are NONE, OP or VOICE.	31337	0
1	101	USERFLAGS for %s is %s	31337	0
1	102	value of MASSDEOPPRO has to be 0-7	31337	0
1	103	MASSDEOPPRO for %s is set to %d	31337	0
1	104	value of FLOODPRO has to be 0-7	31337	0
1	105	FLOODPRO for %s is set to %d	31337	0
1	106	The DESCRIPTION can be a maximum of 80 chars!	31337	0
1	107	DESCRIPTION for %s is cleared.	31337	0
1	108	DESCRIPTION for %s is: %s	31337	0
1	109	The URL can be a maximum of 75 chars!	31337	0
1	110	URL for %s is cleared.	31337	0
1	111	URL for %s is: %s	31337	0
1	112	The string of keywords cannot exceed 80 chars!	31337	0
1	113	KEYWORDS for %s are: %s	31337	0
1	114	Language is set to %s.	31337	0
1	115	ERROR: Invalid language selection.	31337	0
1	116	Can't locate channel %s on the network!	31337	0
1	117	ERROR: Invalid channel setting.	31337	0
1	118	Ignore list:	31337	0
1	119	Ignore list is empty	31337	0
1	120	-- End of Ignore List	31337	0
1	121	CMaster Channel Services internal status:	31337	0
1	122	[     Channel Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
1	123	[        User Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
1	124	[Access Level Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
1	125	[         Ban Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
1	126	Last received User NOTIFY: %i	31337	0
1	127	Last received Channel NOTIFY: %i	31337	0
1	128	Last received Level NOTIFY: %i	31337	0
1	129	Last received Ban NOTIFY: %i	31337	0
1	130	Custom data containers allocated: %i	31337	0
1	131	\002Uptime:\002 %s	31337	0
1	132	Channel %s has %d users (%i operators)	31337	0
1	133	Mode is: %s	31337	0
1	134	Flags set: %s	31337	0
1	135	Cannot suspend a user with equal or higher access than your own.	31337	0
1	136	bogus time units	31337	0
1	137	Invalid suspend duration.	31337	0
1	138	SUSPENSION for %s is cancelled	31337	0
1	139	%s is already suspended on %s	31337	0
1	140	SUSPENSION for %s will expire in %s	31337	0
1	141	ERROR: Topic cannot exceed 145 chars	31337	0
1	142	You have insufficient access to remove the ban %s from %s's database	31337	0
1	143	Removed %i bans that matched %s	31337	0
1	144	Removed your temporary access of %i from channel %s	31337	0
1	145	You don't appear to have a forced access in %s, perhaps it expired?	31337	0
1	146	%s isn't suspended on %s	31337	0
1	147	%s is an IRC operator	31337	0
1	148	%s is NOT logged in.	31337	0
1	149	%s is logged in as %s%s	31337	0
1	150	%s is an Official CService Representative%s and logged in as %s	31337	0
1	151	%s is an Official CService Administrator%s and logged in as %s	31337	0
1	152	%s is an Official CService Developer%s and logged in as %s	31337	0
1	153	There are more than %i entries matching [%s]	31337	0
1	154	Please restrict your search mask	31337	0
1	155	No matching entries for [%s]	31337	0
1	156	%s: Cleared channel modes.	31337	0
1	158	Invalid option.	31337	0
1	159	%s is an Official Undernet Service Bot.	31337	0
1	160	%s is an Official Coder-Com Representative%s and logged in as %s	31337	0
1	161	%s is an Official Coder-Com Contributer%s and logged in as %s	31337	0
1	162	%s is an Official Coder-Com Developer%s and logged in as %s	31337	0
1	163	%s is an Official Coder-Com Senior%s and logged in as %s	31337	0
1	164	 and an IRC operator	31337	0
1	165	Added ban %s to %s at level %i	31337	0
1	166	%s: ban list is empty.	31337	0
1	167	I'm already in that channel!	31337	0
1	168	This command is reserved to IRC Operators	31337	0
1	169	I'm not opped on %s	31337	0
1	170	%s for %i minutes	31337	0
1	171	I'm not opped in %s	31337	0
1	172	Keywords: %s	31337	0
1	173	%s does not wish to be added to channels at this time.	31337	0
1	174	Your NOADDUSER setting is now ON.	31337	0
1	175	Your NOADDUSER setting is now OFF.	31337	0
1	176	There were %d failed login attempts since your last successful login.	31337	0
1	177	AUTHENTICATION FAILED as %s (Exceeded maximum login failures for this session)	31337	0
\.

-- French language definition.
-- 29/12/00 - Daniel Simard <svr@undernet.org>.
-- 01/03/01 - Daniel Simard <svr@undernet.org>.
-- 03/09/01 - nighty <nighty@undernet.org>.

COPY "translations" FROM stdin;
2	1	Désolé. Vous êtes déjà authentifié sous le nom d'utilisateur %s	31337	0
2	2	AUTHENTIFICATION RÉUSSIE pour %s	31337	0
2	3	Désolé. Votre accès est insuffisant pour utiliser cette commande	31337	0
2	4	Désolé. Le canal %s est vide	31337	0
2	5	Je suis incapable de trouver %s	31337	0
2	6	Je ne vois pas %s sur le canal %s	31337	0
2	7	Le canal %s ne semble pas être enregistré	31337	0
2	8	Vous avez été op par %s (%s) sur %s	31337	0
2	9	Vous avez été voice par %s (%s) sur %s	31337	0
2	10	%s: Vous n'êtes pas dans ce canal	31337	0
2	11	%s est déjà op sur %s	31337	0
2	12	%s est déjà voice sur %s	31337	0
2	13	Vous avez été deop par %s (%s)	31337	0
2	14	Vous avez été devoice par %s (%s)	31337	0
2	15	%s n'est pas op sur %s	31337	0
2	16	%s n'est pas voice sur %s	31337	0
2	17	AUTHENTIFICATION REFUSÉE pour %s (Mot de passe Invalide)	31337	0
2	18	Je ne suis pas dans ce canal!!	31337	0
2	19	Niveau de ban invalide. Niveau valide: entre 1 et %i.	31337	0
2	20	Durée du ban invalide. La durée maximale est de %d heures.	31337	0
2	21	La raison d'un ban ne peut dépasser 128 caractères.	31337	0
2	22	Ce ban est déjà dans ma liste!	31337	0
2	23	Le ban %s est dejà couvert par %s	31337	0
2	24	Niveau minimum invalide.	31337	0
2	25	Niveau maximum invalide.	31337	0
2	26	UTILISATEUR: %s NIVEAU: %s %s	31337	0
2	27	CANAL: %s -- AUTOMODE: %s	31337	0
2	28	DERNIÈRE MODIFICATION: %s (depuis %s)	31337	0
2	29	** SUSPENDU ** - Expire dans %s  (Level %i)	31337	0
2	30	DERNIÈRE FOIS VU: il y a %s.	31337	0
2	31	Il y a plus de %d requêtes trouvées.	31337	0
2	32	SVP restreindre votre requête	31337	0
2	33	Fin de la liste des accès	31337	0
2	34	Aucune correspondance trouvée!	31337	0
2	35	Vous ne pouvez pas ajouter un utilisateur à un niveau égal ou supérieur au votre. 	31337	0
2	36	Niveau d'accès invalide	31337	0
2	37	%s a déjà été ajouté sur %s au niveau %i.	31337	0
2	38	Utilisateur %s ajouté sur %s au niveau %i	31337	0
2	39	Quelque chose semble incorrect: %s	31337	0
2	40	%s: fin de la liste des bans	31337	0
2	41	Impossible de voir les détails de l'utilisateur (Invisible).	31337	0
2	42	Informations sur: %s (%i)	31337	0
2	43	Connecté en ce moment via: %s	31337	0
2	44	URL: %s	31337	0
2	45	Langue: %i	31337	0
2	46	Canaux: %s	31337	0
2	47	Entrée Flood: %i	31337	0
2	48	Sortie Flood (Bytes): %i	31337	0
2	49	%s a été enregistré par:	31337	0
2	50	%s - vu pour la dernière fois: il y a %s	31337	0
2	51	Desc: %s	31337	0
2	52	Vous voulez me Flooder? Je ne vais plus vous répondre dorénavant.	31337	0
2	53	Je crois que je vous ai envoyé beaucoup trop d'informations. Je vais vous ignorer un peu.	31337	0
2	54	Commande incomplète	31337	0
2	55	Pour utiliser %s, Vous devez /msg %s@%s	31337	0
2	56	Désolé. Vous devez être authentifié pour utiliser cette commande.	31337	0
2	57	Le canal %s a été suspendu par un administrateur de cservice.	31337	0
2	58	Votre accès sur %s a été suspendu.	31337	0
2	59	Le mode NOOP est actif sur %s	31337	0
2	60	Le mode STRICTOP est actif sur %s	31337	0
2	61	Vous venez de deop plus de %i personnes	31337	0
2	62	SYNTAXE: %s	31337	0
2	63	Votre niveau sur %s à été temporairement augmenté au niveau %i	31337	0
2	64	%s est enregistré.	31337	0
2	65	%s n'est pas enregistré.	31337	0
2	66	Je ne crois pas que %s apprécierait ça.	31337	0
2	67	\002*** Liste des Bans pour le canal %s ***\002	31337	0
2	68	%s %s Niveau: %i	31337	0
2	69	AJOUTÉ PAR: %s (%s)	31337	0
2	70	DEPUIS: %s	31337	0
2	71	EXPIRATION: %s	31337	0
2	72	\002*** FIN ***\002	31337	0
2	73	Désolé, Je ne sais pas qui est %s.	31337	0
2	74	Vous avez été déloggué, vous n'êtes plus authentifié.	31337	0
2	75	%s n'apparaît pas comme ayant accès sur %s.	31337	0
2	76	Vous ne pouvez pas modifier un utilisateur ayant un niveau supérieur ou égal au votre.	31337	0
2	77	Vous ne pouvez pas donner un niveau d'accès supérieur ou égal au votre à un utilisateur.	31337	0
2	78	Modification du niveau de %s sur le canal %s à %i effectuée.	31337	0
2	79	Activation du MODE Automatique (OP) pour %s sur le canal %s	31337	0
2	80	Activation du MODE Automatique (VOICE) pour %s sur le canal %s	31337	0
2	81	Désactivation des MODES Automatiques pour %s sur le canal %s	31337	0
2	82	Votre mot de passe ne peut être ni votre nom d'utilisateur ni votre nick - La Syntaxe est: NEWPASS <nouveau mot de passe>	31337	0
2	83	Votre mot de passe a été changé avec succès.	31337	0
2	84	Le mode NOOP est actif sur %s	31337	0
2	85	Le mode STRICTOP est actif sur %s (et %s n'est pas authentifié)	31337	0
2	86	Le mode STRICTOP est actif sur %s (et l'accès de %s est insuffisant)	31337	0
2	87	%s a été purgé	31337	0
2	88	%s est déjà enregistré.	31337	0
2	89	Nom de canal invalide.	31337	0
2	90	%s a été enregistré	31337	0
2	91	%s à été enlevé de ma liste silence/ignore	31337	0
2	92	%s introuvable dans ma liste silence/ignore	31337	0
2	93	Vous ne pouvez pas effacer un utilisateur ayant un niveau égal ou supérieur au votre.	31337	0
2	94	Vous ne pouvez pas effacer votre accès sur un canal dont vous êtes le propriétaire.	31337	0
2	95	L'accès de l'utilisateur %s a été effacé sur %s	31337	0
2	96	Le mode INVISIBLE pour vos informations est maintenant à ON.	31337	0
2	97	Le mode INVISIBLE pour vos informations est maintenant à OFF.	31337	0
2	98	%s pour %s est %s	31337	0
2	99	La valeur de %s doit être ON ou OFF	31337	0
2	100	Configuration USERFLAGS invalide! Les valeurs correctes sont NONE, OP, VOICE.	31337	0
2	101	Le USERFLAGS de %s est %s	31337	0
2	102	La valeur de MASSDEOPPRO doit être comprise entre 0 et 7	31337	0
2	103	MASSDEOPPRO de %s a été configuré à %d	31337	0
2	104	La valeur de FLOODPRO doit être comprise entre 0 et 7	31337	0
2	105	FLOODPRO de %s a été configuré à %d	31337	0
2	106	La DESCRIPTION ne doit pas dépasser 80 caractères.	31337	0
2	107	La DESCRIPTION de %s a été éffacée.	31337	0
2	108	La DESCRIPTION de %s est: %s	31337	0
2	109	L'URL ne doit pas dépasser 75 caractères.	31337	0
2	110	L'URL de %s a été effacée.	31337	0
2	111	L'URL de %s est: %s	31337	0
2	112	Les KEYWORDS (Mots-Clés) ne doivent pas dépasser 80 caractères.	31337	0
2	113	KEYWORDS de %s sont: %s	31337	0
2	114	Langue activée: %s.	31337	0
2	115	ERREUR: Langue selectionnée invalide.	31337	0
2	116	Le canal %s est introuvable sur le réseau!	31337	0
2	117	ERREUR: Configuration de canal invalide.	31337	0
2	118	Liste de silence/ignore:	31337	0
2	119	La liste de silence/ignore est vide	31337	0
2	120	-- Fin de la liste de silence/ignore 	31337	0
2	121	Statut interne du Service de Canaux CMaster:	31337	0
2	122	[          Statistiques du Canal] \002Entrées en cache:\002 %i    \002Requêtes Base:\002 %i    \002# de fois Cache touché:\002 %i    \002Efficacité:\002 %.2f%%	31337	0
2	123	[     Statistiques Utilisateurs] \002Entrées en cache:\002 %i    \002Requêtes Base:\002 %i    \002# de fois Cache touché:\002 %i    \002Efficacité:\002 %.2f%%	31337	0
2	124	[   Statistiques Niveaux d'accès] \002Entrées en cache:\002 %i    \002Requêtes Base:\002 %i    \002# de fois Cache touché:\002 %i    \002Efficacité:\002 %.2f%%	31337	0
2	125	[          Statistiques des bans] \002Entrées en cache:\002 %i    \002Requêtes Base:\002 %i    \002# de fois Cache touché:\002 %i    \002Efficacité:\002 %.2f%%	31337	0
2	126	Dernier évènement NOTIFY d'utilisateur: %i	31337	0
2	127	Dernier évènement NOTIFY de canal: %i	31337	0
2	128	Dernier évènement NOTIFY de niveau: %i	31337	0
2	129	Dernier évènement NOTIFY de ban: %i	31337	0
2	130	Données assignées: %i	31337	0
2	131	\002En fonction depuis (uptime):\002 %s	31337	0
2	132	Le canal %s a %d utilisateur(s) (%i opérateur(s))	31337	0
2	133	Les Modes du canal sont: %s	31337	0
2	134	Flags actifs: %s	31337	0
2	135	Vous ne pouvez pas suspendre un utilisateur ayant un niveau égal ou supérieur au votre.	31337	0
2	136	Unité de temps invalide.	31337	0
2	137	Durée de suspension invalide.	31337	0
2	138	La SUSPENSION pour %s a été annulée.	31337	0
2	139	%s est déjà suspendu sur %s	31337	0
2	140	La SUSPENSION pour %s va expirer dans %s	31337	0
2	141	ERREUR: Le topic ne peut dépasser 145 caractères.	31337	0
2	142	Votre accès est insuffisant pour enlever le ban %s de la base de données de %s	31337	0
2	143	%i ban(s) correpondant à %s ont été supprimés.	31337	0
2	144	Votre accès temporaire au niveau %i sur le canal %s à été supprimé.	31337	0
2	145	Vous ne semblez pas avoir forcé l'accès à %s, Serait-il expiré?	31337	0
2	146	%s n'est pas suspendu sur %s	31337	0
2	147	%s est un Opérateur IRC	31337	0
2	148	%s n'est pas authentifié.	31337	0
2	149	%s est authentifié sous le nom d'utilisateur %s%s	31337	0
2	150	%s est un représentant Officiel de CService%s et authentifié sous le nom d'utilisateur %s	31337	0
2	151	%s est un Administrateur Officiel de CService%s et authentifié sous le nom d'utilisateur %s	31337	0
2	152	%s est un Développeur Officiel de CService%s et authentifié sous le nom d'utilisateur %s	31337	0
2	153	Il y a plus de %i entrées correspondantes [%s]	31337	0
2	154	Veuillez svp restreindre votre requête.	31337	0
2	155	Aucune entrée ne correspond à votre requête. [%s]	31337	0
2	156	%s: Les modes du canal on été remis à zéro.	31337	0
2	158	Option Invalide.	31337	0
2	159	%s est un Bot de Services Officiel d'Undernet.	31337	0
2	160	%s est un Représentant Officiel de Coder-Com%s et authentifié sous le nom d'utilisateur %s	31337	0
2	161	%s est un Participan Officiel de Coder-Com%s et authentifié sous le nom d'utilisateur %s	31337	0
2	162	%s est un Développeur Officie de Coder-Com%s et authentifié sous le nom d'utilisateur %s	31337	0
2	163	%s est un Sénior Officiel de Coder-Com%s et authentifié sous le nom d'utilisateur %s	31337	0
2	164	 et un Opérateur IRC	31337	0
2	165	Ban sur %s ajouté à %s au niveau %i	31337	0
2	166	%s: La liste des bans est vide.	31337	0
2	167	Je suis déjà dans ce canal!	31337	0
2	168	Cette commande est réservée aux Opérateurs IRC	31337	0
2	169	Je ne suis pas op sur %s	31337	0
2	170	%s pour %i minutes	31337	0
\.

-- Danish language definition.
-- 28/12/2000 - David Henriksen <david@itwebnet.dk>.

COPY "translations" FROM stdin;
3	1	Beklager, Du er allerede autoriseret som %s	31337	0
3	2	AUTORISATION LYKKEDES som %s	31337	0
3	3	Beklager, du har utilstrækkelig adgang til at udføre den kommando	31337	0
3	4	Beklager, kanalen %s er tom	31337	0
3	5	Jeg kan ikke se %s nogen steder	31337	0
3	6	Jeg kan ikke finde %s på kanalen %s	31337	0
3	7	Kanelen %s ser ikke ud til at være registreret	31337	0
3	8	Du er op'et af %s (%s) i %s	31337	0
3	9	Du er voice'et af %s (%s) i %s	31337	0
3	10	%s: Du er ikke i denne kanal	31337	0
3	11	%s er allerede op'et i %s	31337	0
3	12	%s er allerede voice'et i %s	31337	0
3	13	Du er deop'et af %s (%s)	31337	0
3	14	Du er devoice'et af %s (%s)	31337	0
3	15	%s er ikke op'et i %s	31337	0
3	16	%s er ikke voice'et i %s	31337	0
3	17	AUTORISATION MISLYKKEDES som %s (Ugyldigt Kodeord)	31337	0
3	18	Jeg er ikke i den kanal!	31337	0
3	19	Ugyldigt banlevel område. Gyldigt område er 1-%i.	31337	0
3	20	Ugyldigt ban varighed. Din ban varighed kan max bestå af %d timer.	31337	0
3	21	Ban grund kan ikke overtræde 128 karaktere	31337	0
3	22	Specifiseret ban er allerede i min banliste!	31337	0
3	23	Ban'et %s er allerede dækket af %s	31337	0
3	24	Ugyldigt minimums level.	31337	0
3	25	Ugyldigt maximums level.	31337	0
3	26	BRUGER: %s ADGANG: %s %s	31337	0
3	27	KANAL: %s - AUTOMODE: %s	31337	0
3	28	SIDST ÆNDRET: %s (%s siden)	31337	0
3	29	** SUSPENDERET ** - Udløber om %s (Level %i)	31337	0
3	30	SIDST SET: %s siden.	31337	0
3	31	Der er mere end %d matchende indtastninger.	31337	0
3	32	Forkort venligst din forspørgelse.	31337	0
3	33	Slut på adgangsliste.	31337	0
3	34	Ingen match!	31337	0
3	35	Kan ikke tilføje en bruger med ens eller højere adgang end din egen.	31337	0
3	36	Ugyldigt adgangs level.	31337	0
3	37	%s er allerede tilføjet til %s med adgangs level %i.	31337	0
3	38	Tilføjet bruget %s til %s med adgangs level %i	31337	0
3	39	Noget gik galt: %s	31337	0
3	40	%s: Slut af ban liste	31337	0
3	41	Kan ikke vise bruger detaljer (Usynlig)	31337	0
3	42	Information omkring: %s (%i)	31337	0
3	43	Nuværende logget på via: %s	31337	0
3	44	URL: %s	31337	0
3	45	Sprog: %i	31337	0
3	46	Kanaler: %s	31337	0
3	47	Input Flood Antal: %i	31337	0
3	48	Output Flood (Bytes): %i	31337	0
3	49	%s er registreret af:	31337	0
3	50	%s - sidst set: %s siden	31337	0
3	51	Desc: %s	31337	0
3	52	Floode mig vil du? Jeg vil ikke høre mere på dig	31337	0
3	53	Jeg tror jeg har sendt dig en lille smule for meget data, jeg vil ignorere dig i et stykke tid.	31337	0
3	54	Ukomplet kommando	31337	0
3	55	For at bruge %s, skal du /msg %s@%s	31337	0
3	56	Sorry, Du skal være logget ind for at bruge denne kommando.	31337	0
3	57	Kanalen %s er blevet suspenderet af en cservice administrator.	31337	0
3	58	Din adgang på %s er blevet suspenderet.	31337	0
3	59	NOOP flaget er sat på %s	31337	0
3	60	STRICTOP flaget flaget er sat på %s	31337	0
3	61	Du har lige deop'et flere end %i folk	31337	0
3	62	SYNTAKS: %s	31337	0
3	63	Midlertidigt forhøjet din adgang på kanal %s til %i	31337	0
3	64	%s er registreret.	31337	0
3	65	%s er ikke registreret.	31337	0
3	66	Jeg tror ikke %s ville værtsætte det.	31337	0
3	67	\002*** Ban Liste for kanal %s ***\002	31337	0
3	68	%s %s Level: %i	31337	0
3	69	TILFØJET AF: %s (%s)	31337	0
3	70	SIDEN: %s	31337	0
3	71	UDL: %s	31337	0
3	72	\002*** SLUT ***\002	31337	0
3	73	Sorry, Jeg ved ikke hvem %s er.	31337	0
3	74	Sorry, du er ikke længere autoriseret med mig.	31337	0
3	75	%s Ser ikke ud til at have adgang i %s.	31337	0
3	76	Kan ikke ændre en bruger med ens eller højere adgang end din egen.	31337	0
3	77	Kan ikke give en bruger højere eller ens adgang end din egen.	31337	0
3	78	Ændrede %s's adgangs level på kanal %s til %i	31337	0
3	79	Satte Automatisk MODE til OP for %s på kanal %s	31337	0
3	80	Satte Automatisk MODE til VOICE for %s på kanal %s	31337	0
3	81	Automatisk MODE for %s på kanal %s er nu slået fra	31337	0
3	82	Dit kodeord kan ikke være dit brugernavn eller nuværende nick - syntaks er: NEWPASS <nyt kodeord>	31337	0
3	83	Kodeordet er ændret med succes.	31337	0
3	84	NOOP flaget er sat på %s	31337	0
3	85	STRICTOP flaget er sat på %s (og %s er ikke autoriseret)	31337	0
3	86	STRICTOP flaget er sat på %s (og %s har ikke tilstrækkelig adgang)	31337	0
3	87	Slettet kanal %s	31337	0
3	88	%s er allerede registeret med mig.	31337	0
3	89	Ugyldigt kanal navn.	31337	0
3	90	Registrerede kanal %s	31337	0
3	91	Har fjernet %s fra min ignorerings liste	31337	0
3	92	Kunne ikke finde %s i min ignorerings liste	31337	0
3	93	Kan ikke fjerne en bruger med ens eller højere adgang end din egen	31337	0
3	94	Du kan ikke fjerne dig selv fra en kanal du ejer	31337	0
3	95	Har fjernet bruger %s fra %s	31337	0
3	96	Din USYNLIG indstilling er nu sat til ON.	31337	0
3	97	Din USYNLIG indstilling er nu sat til OFF.	31337	0
3	98	%s for %s er %s	31337	0
3	99	Værdig af %s skal være ON eller OFF	31337	0
3	100	Ugyldig USERFLAGS indstilling. Korrekte værdiger er NONE, OP, VOICE.	31337	0
3	101	USERFLAGS for %s er %s	31337	0
3	102	Værdi af MASSDEOPPRO skal være 0-7	31337	0
3	103	MASSDEOPPRO for %s er sat til %d	31337	0
3	104	Værdi af FLOODPRO skal være 0-7	31337	0
3	105	FLOODPRO for %s er sat til %d	31337	0
3	106	DESKRIPTION kan max have 80 karaktere!	31337	0
3	107	DESKRIPTION for %s er fjernet.	31337	0
3	108	DESKRIPTION for %s er: %s	31337	0
3	109	URL'en kan max have 75 karaktere!	31337	0
3	110	URL for %s er fjernet.	31337	0
3	111	URL for %s er: %s	31337	0
3	112	Strengen af nøgleord kan ikke overtræde 80 karaktere!	31337	0
3	113	NØGLEORD for %s er: %s	31337	0
3	114	Sprog er sat til %s.	31337	0
3	115	FEJL: Ugyldigt sprogvalg.	31337	0
3	116	Kan ikke finde kanal %s på netværket!	31337	0
3	117	FEJL: Ugyldig kanal indstilling.	31337	0
3	118	Ignore liste:	31337	0
3	119	Ignore listen er tom	31337	0
3	120	-- Slut på Ignore Liste	31337	0
3	121	CMaster Kanal Services intern status:	31337	0
3	122	[        Kanal Statistik] \002Cached Indtastninger:\002 %i    \002DB Forespørgsler:\002 %i    \002Cache Hits:\002 %i    \002Udnyttelse:\002 %.2f%%	31337	0
3	123	[       Bruger Statistik] \002Cached Indtastninger:\002 %i    \002DB Forespørgsler:\002 %i    \002Cache Hits:\002 %i    \002Udnyttelse:\002 %.2f%%	31337	0
3	124	[Adgangs Level Statistik] \002Cached Indtastninger:\002 %i    \002DB Forespørgsler:\002 %i    \002Cache Hits:\002 %i    \002Udnyttelse:\002 %.2f%%	31337	0
3	125	[          Ban Statistik] \002Cached Indtastninger:\002 %i    \002DB Forespørgsler:\002 %i    \002Cache Hits:\002 %i    \002Udnyttelse:\002 %.2f%%	31337	0
3	126	Sidst modtaget Bruger NOTIFY: %i	31337	0
3	127	Sidst modtaget Kanal NOTIFY: %i	31337	0
3	128	Sidst modtaget Level NOTIFY: %i	31337	0
3	129	Sidst modtaget Ban NOTIFY: %i	31337	0
3	130	Custom data beholdere allokeret: %i	31337	0
3	131	\002Uptime:\002 %s	31337	0
3	132	Kanal %s har %d brugere (%i operatører)	31337	0
3	133	Mode er: %s	31337	0
3	134	Flag sat: %s	31337	0
3	135	Kan ikke suspendere en bruger med ens eller højere adgang end din egen.	31337	0
3	136	Ugyldige tids enheder.	31337	0
3	137	Ugyldig suspenderings periode.	31337	0
3	138	SUSPENDERING for %s er slået fra.	31337	0
3	139	%s er allerede suspenderet på %s	31337	0
3	140	SUSPENDERING for %s vil udløbe i %s	31337	0
3	141	FEJL: Topic kan ikke overtræde 145 karaktere	31337	0
3	142	Du har utilstrækkelig adgang til at fjerne ban'et %s fra %s's database	31337	0
3	143	Fjernet %i bans der matcher %s	31337	0
3	144	Fjernet din midlertidige adgang af %i fra kanal %s	31337	0
3	145	Du ser ikke ud til at have en tvunget adgang i %s, måske er den udløbet?	31337	0
3	146	%s er ikke suspenderet på %s	31337	0
3	147	%s er en IRC operatør	31337	0
3	148	%s er IKKE logget ind.	31337	0
3	149	%s er logget ind som %s%s	31337	0
3	150	%s er en Officiel CService Repræsentativ%s og logget ind som %s	31337	0
3	151	%s er en Officiel CService Administrator%s og logget ind som %s	31337	0
3	152	%s er en Officiel CService Udvikler%s og logget ind som %s	31337	0
3	153	Der er flere end %i indtastninger der matcher [%s]	31337	0
3	154	Forkort venligst din søge maske	31337	0
3	155	Ingen matchende indtastninger for [%s]	31337	0
3	156	%s: Kanal modes fjernet.	31337	0
3	158	Ugyldig indstilling.	31337	0
3	159	%s er en Officiel Undernet Service bot.	31337	0
3	160	%s er en Officiel Coder-Com Repræsentativ%s og logget ind som %s	31337	0
3	161	%s er en Officiel Coder-Com Medarbejder%s og logget ind som %s	31337	0
3	162	%s er en Officiel Coder-Com Udvikler%s og logget ind som %s	31337	0
3	163	%s er en Officiel Coder-Com Senior%s og logget ind som %s	31337	0
3	164	 og en IRC operatør	31337	0
3	165	Tilføjede ban %s til %s på level %i	31337	0
3	166	%s: ban liste er tom.	31337	0
3	167	Jeg er allerede i den kanal!	31337	0
3	168	Denne kommando er reserveeret til IRC Operatører	31337	0
3	169	Jeg er ikke op'et på %s	31337	0
3	170	%s for %i minuter	31337	0
\.

-- Portuguese language definition.
-- 20/04/02 - Giancarlo Ottaviani - NRiDeR <nrider@virtualpoint.com.br>

COPY "translations" FROM stdin;
6	1	Desculpe, Você já está autenticado como %s	31337	0
6	2	SUCESSO NA AUTENTICAÇÃO como %s	31337	0
6	3	Desculpe, você não tem acesso suficiente para efetuar este comando	31337	0
6	4	Desculpe, o canal %s está vazio	31337	0
6	5	Eu não vejo %s em lugar algum	31337	0
6	6	Eu não posso encontrar %s no canal %s	31337	0
6	7	O canal %s não parece ser registrado	31337	0
6	8	Foi dado op a você por %s (%s) no %s	31337	0
6	9	Foi dado voice a você por %s (%s) no %s	31337	0
6	10	%s: Você não está neste canal	31337	0
6	11	%s já está com op no %s	31337	0
6	12	%s já está com voice no %s	31337	0
6	13	Foi retirado o op de você por %s (%s)	31337	0
6	14	Foi retirado o voice de você por %s (%s)	31337	0
6	15	%s não está com op no %s	31337	0
6	16	%s não está com voice no %s	31337	0
6	17	FALHA NA AUTENTICAÇÃO como %s (Senha inválida)	31337	0
6	18	Eu não estou neste canal!	31337	0
6	19	Escala inválida de nívelban. Escala válida é 1-%i.	31337	0
6	20	Duração de ban inválida. Sua duração de ban aplicada pode ser de no máximo %d horas.	31337	0
6	21	Motivo do ban não exceder 128 characteres	31337	0
6	22	O ban especificado já está na banlist!	31337	0
6	23	O ban %s já está coberto por %s	31337	0
6	24	Nível mínimo inválido.	31337	0
6	25	Nível máximo inválido.	31337	0
6	26	USUÁRIO: %s NÍVEL: %s %s	31337	0
6	27	CANAL: %s -- MODO AUTOMÁTICO: %s	31337	0
6	28	ÚLTIMA MODIFICAÇÃO: %s (%s atrás)	31337	0
6	29	** SUSPENSÃO ** - Expira em %s  (nível %i)	31337	0
6	30	ÚLTIMA VEZ: %s atrás.	31337	0
6	31	Há mais de %d entradas.	31337	0
6	32	Por favor, Restrinja sua pergunta.	31337	0
6	33	Final da lista de acesso	31337	0
6	34	Não Consta!	31337	0
6	35	Não pode adicionar um usuário com acesso igual ou mais elevado do que o seu. 	31337	0
6	36	Nível de acesso inválido.	31337	0
6	37	%s já está adicionado no %s com nível de acesso %i.	31337	0
6	38	Adicionado usuário %s no %s com nível de acesso %i	31337	0
6	39	Alguma coisa errada: %s	31337	0
6	40	%s: Final da lista de ban	31337	0
6	41	Incapaz de ver os detalhes do usuário (Invisível)	31337	0
6	42	Informação sobre: %s (%i)	31337	0
6	43	Atualmente logado sobre: %s	31337	0
6	44	URL: %s	31337	0
6	45	Idioma: %i	31337	0
6	46	Canais: %s	31337	0
6	47	Pontos de entrada de Flood: %i	31337	0
6	48	Saída de Flood (Bytes): %i	31337	0
6	49	%s é registrado por:	31337	0
6	50	%s - última vez: %s atrás	31337	0
6	51	Descrição: %s	31337	0
6	52	Me fazendo flood? Não irei mais escuta-lo!	31337	0
6	53	Eu acredito ter enviado poucos dados a mais, Estou ignorando você por enquanto.	31337	0
6	54	Comando incompleto	31337	0
6	55	Para usar %s, você precisa /msg %s@%s	31337	0
6	56	Desculpe, Você precisa estar logado para efetuar este comando.	31337	0
6	57	O canal %s foi suspendido por um administrador do CService.	31337	0
6	58	Seu acesso no %s foi suspendido.	31337	0
6	59	Função NOOP está ajustada no %s	31337	0
6	60	Função STRICTOP está ajustada no %s	31337	0
6	61	Você tirou o op de mais de %i pessoas	31337	0
6	62	SINTAXE: %s	31337	0
6	63	Aumentou temporariamente seu acesso no canal %s para %i	31337	0
6	64	%s é registrado.	31337	0
6	65	%s não é registrado.	31337	0
6	66	Eu acredito que %s não apreciaria isso.	31337	0
6	67	\002*** Lista de ban para o canal %s ***\002	31337	0
6	68	%s %s Nível: %i	31337	0
6	69	ADICIONADO POR: %s (%s)	31337	0
6	70	DESDE: %s	31337	0
6	71	EXP: %s	31337	0
6	72	\002*** FIM ***\002	31337	0
6	73	Desculpe, Eu não sei quem %s é.	31337	0
6	74	Desculpe, você não é mais autorizado comigo.	31337	0
6	75	%s não parece ter o acesso no %s.	31337	0
6	76	Não pode modificar um usuário com acesso igual ou mais elevado que seu próprio acesso.	31337	0
6	77	Não pode dar um acesso mais elevado ou igual o que tem.	31337	0
6	78	Acesso modificado de %s's no canal %s para %i	31337	0
6	79	Ajustado AUTOMODE em OP para %s no canal %s	31337	0
6	80	Ajustado AUTOMODE em VOICE para %s no canal %s	31337	0
6	81	Ajustado AUTOMODE em NONE para %s no canal %s	31337	0
6	82	Sua senha não poder seu username ou nick - sintaxe é: NEWPASS <nova senha>	31337	0
6	83	Senha modificada com sucesso.	31337	0
6	84	Função NOOP está ligada no %s	31337	0
6	85	The STRICTOP está ligada no %s (e %s não está autenticado)	31337	0
6	86	The STRICTOP está ligada no %s (e %s tem nível insuficiente)	31337	0
6	87	Canal Perdido %s	31337	0
6	88	%s já está registrado comigo.	31337	0
6	89	Nome de canal inválido.	31337	0
6	90	Canal registrado %s	31337	0
6	91	Removido %s de minha lista de silêncio	31337	0
6	92	Não posso encontrar %s em minha lista do silêncio	31337	0
6	93	Não pode remover um usuário com o acesso igual ou mais elevado que seu próprio	31337	0
6	94	Você não pode se remover de um canal que registrou	31337	0
6	95	Usuário %s removido do %s	31337	0
6	96	Seu ajuste INVISÍVEL está agora LIGADO.	31337	0
6	97	Seu ajuste INVISÍVEL está agora DESLIGADO.	31337	0
6	98	%s para %s é %s	31337	0
6	99	valor de %s precisa ser ON ou OFF	31337	0
6	100	Ajuste de USERFLAGS inválido. Os valores corretos são NONE, OP, VOICE.	31337	0
6	101	USERFLAGS para %s é %s	31337	0
6	102	o valor de MASSDEOPPRO tem que ser 0-7	31337	0
6	103	MASSDEOPPRO para %s é ajustado a %d	31337	0
6	104	o valor de FLOODPRO tem que ser de 0-7	31337	0
6	105	FLOODPRO para %s é ajustado a %d	31337	0
6	106	A DESCRIÇÃO pode ter no máximo 80 caracteres!	31337	0
6	107	DESCRIÇÃO para %s foi apagada.	31337	0
6	108	DESCRIÇÃO para %s é: %s	31337	0
6	109	A URL pode ter no máximo 75 caracteres!	31337	0
6	110	URL para %s foi apagada.	31337	0
6	111	URL para %s é: %s	31337	0
6	112	A linha de keywords não pode exceder 80 caracteres!	31337	0
6	113	KEYWORDS para %s são: %s	31337	0
6	114	O idioma foi ajustado para %s.	31337	0
6	115	ERRO: Seleção inválida de idioma.	31337	0
6	116	Não pode localizar o canal %s na rede!	31337	0
6	117	ERRO: Ajuste inválido de canal.	31337	0
6	118	Lista de ignorados:	31337	0
6	119	Lista de ignorados vazia	31337	0
6	120	-- Fina da lista de ignorados	31337	0
6	121	CMaster Channel Services internal status:	31337	0
6	122	[     Channel Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
6	123	[        User Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
6	124	[Access Level Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
6	125	[         Ban Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
6	126	Last recieved User NOTIFY: %i	31337	0
6	127	Last recieved Channel NOTIFY: %i	31337	0
6	128	Last recieved Level NOTIFY: %i	31337	0
6	129	Last recieved Ban NOTIFY: %i	31337	0
6	130	Recipientes feitos sob encomenda dos dados alocados: %i	31337	0
6	131	\002Uptime:\002 %s	31337	0
6	132	Canal %s tem %d usuários (%i operadores)	31337	0
6	133	Modo é: %s	31337	0
6	134	Flags são: %s	31337	0
6	135	Não pode suspender um usuário com acesso igual ou mais elevado que o seu próprio.	31337	0
6	136	unidades bogus do tempo	31337	0
6	137	Duração de suspensão invalida.	31337	0
6	138	SUSPENSÃO de %s foi cancelada	31337	0
6	139	%s já está suspendido no %s	31337	0
6	140	SUSPENSÃO de %s acabará em %s	31337	0
6	141	ERRO: Tópico não pode exceder 145 caracteres	31337	0
6	142	Você tem nível insuficiente para remover o ban %s de %s's database	31337	0
6	143	Removido %i bans combinado %s	31337	0
6	144	Removido temporariamente o acesso de %i no canal %s	31337	0
6	145	Você não parece ter um acesso forçado em %s, talvez expirou?	31337	0
6	146	%s não está suspenso no %s	31337	0
6	147	%s é um IRC operator	31337	0
6	148	%s NÃO está logado.	31337	0
6	149	%s está logado como %s%s	31337	0
6	150	%s é um Representante Oficial CService%s e está logado como %s	31337	0
6	151	%s é um Administrador Oficial CService%s e está logado como %s	31337	0
6	152	%s é um Desenvolvedor Oficial CService%s e está logado como %s	31337	0
6	153	Há mais do que %i entrada combinando [%s]	31337	0
6	154	Por favor, restrinja sua mascara de pesquisa	31337	0
6	155	Nenhuma entradas combinando para [%s]	31337	0
6	156	%s: Modos do canal Apagado.	31337	0
6	158	Opção inválida.	31337	0
6	159	%s é um Bot de Serviço Oficial da Undernet.	31337	0
6	160	%s é um Representante Oficial Coder-Com%s e está logado como %s	31337	0
6	161	%s é um Contribuidor Oficial Coder-Com%s e está logado como %s	31337	0
6	162	%s é um Programador Oficial Coder-Com%s e está logado como %s	31337	0
6	163	%s é um Sênior Oficial Coder-Com%s e está logado como %s	31337	0
6	164	 e é IRC operator	31337	0
6	165	Adicionado ban %s no %s com nível %i	31337	0
6	166	%s: lista de ban vazia.	31337	0
6	167	Eu já estou neste canal!	31337	0
6	168	Este comando é reservado para IRC Operators	31337	0
6	169	Eu não tenho op no %s	31337	0
6	170	%s para %i minutos	31337	0
\.

-- Italian language definition.
-- 03/03/07 - Orlando Bassotto <future@break.net>.
-- 04/03/07 - Ombretta Gorini <ombra.g@galactica.it>.

COPY "translations" FROM stdin;
7	1	Spiacente, sei già autenticato come %s	31337	0
7	2	AUTENTICAZIONE RIUSCITA come %s	31337	0
7	3	Spiacente, non hai un accesso sufficiente per eseguire quel comando	31337	0
7	4	Spiacente, il canale %s è vuoto	31337	0
7	5	Non vedo nessun %s	31337	0
7	6	Non trovo %s nel canale %s	31337	0
7	7	Il canale %s non risulta essere registrato	31337	0
7	8	Ti è stato dato l'op da %s (%s) in %s	31337	0
7	9	Ti è stato dato il voice da %s (%s) in %s	31337	0
7	10	%s: Non sei in quel canale	31337	0
7	11	%s ha già l'op in %s	31337	0
7	12	%s ha già il voice in %s	31337	0
7	13	Ti è stato tolto l'op da %s (%s)	31337	0
7	14	Ti è stato tolto il voice da %s (%s)	31337	0
7	15	%s non ha l'op in %s	31337	0
7	16	%s non ha il voice in %s	31337	0
7	17	AUTENTICAZIONE FALLITA come %s (Password non valida)	31337	0
7	18	Non mi trovo in quel canale!	31337	0
7	19	Range del livello del ban non valido. Il range valido è compreso tra 1 e %i.	31337	0
7	20	Durata del ban non valida. La durata massima di un ban può essere di %d ore.	31337	0
7	21	Il motivo del ban non può superare i 128 caratteri	31337	0
7	22	Il ban specificato è già presente nella mia lista dei ban!	31337	0
7	23	Il ban %s è già coperto da %s	31337	0
7	24	Livello minimo non valido.	31337	0
7	25	Livello massimo non valido.	31337	0
7	26	UTENTE: %s ACCESSO: %s %s	31337	0
7	27	CANALE: %s -- AUTOMODE: %s	31337	0
7	28	ULTIMA MODIFICA: %s (%s fà)	31337	0
7	29	** SOSPESO ** - Scade il %s (Level %i)	31337	0
7	30	VISTO L'ULTIMA VOLTA: %s.	31337	0
7	31	Sono presenti più di %d corrispondenze.	31337	0
7	32	Per favore, restringi il campo di ricerca dell'interrogazione.	31337	0
7	33	Fine della lista degli accessi	31337	0
7	34	Nessuna Corrispondenza!	31337	0
7	35	Non puoi aggiungere un utente con un livello di accesso uguale o superiore al tuo.	31337	0
7	36	Livello di accesso non valido.	31337	0
7	37	%s è già stato aggiunto in %s con livello di accesso %i.	31337	0
7	38	Aggiunto l'utente %s a %s con livello di accesso %i	31337	0
7	39	Qualcosa non va: %s	31337	0
7	40	%s: Fine della lista dei ban	31337	0
7	41	Impossibile visualizzare i dettagli dell'utente (Invisibile)	31337	0
7	42	Informazioni su: %s (%i)	31337	0
7	43	Attualmente autenticato attraverso: %s	31337	0
7	44	URL: %s	31337	0
7	45	Lingua: %i	31337	0
7	46	Canali: %s	31337	0
7	47	Input Flood Points: %i	31337	0
7	48	Ouput Flood (Bytes): %i	31337	0
7	49	%s è registrato da:	31337	0
7	50	%s - visto l'ultima volta: %s 	31337	0
7	51	Desc: %s	31337	0
7	52	Vuoi floodarmi? Bene bene, allora mi sa che da adesso non ti ascolto più.	31337	0
7	53	Penso che abbia visto abbastanza, adesso ti ignoro per un po'.	31337	0
7	54	Comando non completo	31337	0
7	55	Per usare %s, devi scrivere /msg %s@%s	31337	0
7	56	Spiacente, devi autenticarti per usare questo comando.	31337	0
7	57	Il canale %s è stato sospeso da un amministratore di CService.	31337	0
7	58	Il tuo accesso su %s è stato sospeso.	31337	0
7	59	Il flag NOOP è impostato su %s	31337	0
7	60	Il flag STRICTOP è impostato su %s	31337	0
7	61	Hai appena tolto l'op a più di %i persone	31337	0
7	62	SINTASSI: %s	31337	0
7	63	Il tuo accesso è stato temporaneamente incrementato sul canale %s a %i	31337	0
7	64	%s è registrato.	31337	0
7	65	%s non è registrato.	31337	0
7	66	Non penso che %s possa apprezzarlo.	31337	0
7	67	\002*** Lista dei Ban per il canale %s ***\002	31337	0
7	68	%s %s Livello: %i	31337	0
7	69	AGGIUNTO DA: %s (%s)	31337	0
7	70	DAL: %s	31337	0
7	71	SCADENZA: %s	31337	0
7	72	\002*** FINE ***\002	31337	0
7	73	Spiacente, ma non conosco chi sia %s.	31337	0
7	74	Spiacente, non sei più autorizzato con me.	31337	0
7	75	%s non sembra avere accesso in %s.	31337	0
7	76	Non posso modificare un utente con un livello di accesso uguale o superiore al tuo.	31337	0
7	77	Non puoi dare un accesso uguale o superiore al tuo a un altro utente.	31337	0
7	78	Modificato il livello di accesso per %s sul canale %s a %i	31337	0
7	79	Impostato AUTOMODE a OP per %s sul canale %s	31337	0
7	80	Impostato AUTOMODE a VOICE per %s sul canale %s	31337	0
7	81	Impostato AUTOMODE a NONE per %s sul canale %s	31337	0
7	82	La tua password non può essere il tuo username o il tuo attuale nick - la sintassi è: NEWPASS <nuova password>	31337	0
7	83	La password è stata cambiata con successo.	31337	0
7	84	Il flag NOOP è impostato su %s	31337	0
7	85	Il flag STRICTOP è impostato su %s (e %s non è autenticato)	31337	0
7	86	Il flag STRICTOP è impostato su %s (e %s non ha accesso sufficiente)	31337	0
7	87	Purgato il canale %s	31337	0
7	88	%s è già registrato da me.	31337	0
7	89	Nome del canale non valido.	31337	0
7	90	Registrato il canale %s	31337	0
7	91	%s è stato rimosso dalla lista dei silence	31337	0
7	92	Non trovo %s nella mia lista dei silence	31337	0
7	93	Non puoi rimuovere un utente con accesso uguale o superiore al tuo	31337	0
7	94	Non puoi rimuovere te stesso da un canale che possiedi	31337	0
7	95	Rimosso l'utente %s da %s	31337	0
7	96	L'impostazione INVISIBLE adesso è attiva (ON).	31337	0
7	97	L'impostazione INVISIBLE adesso non è attiva (OFF).	31337	0
7	98	%s per %s è %s	31337	0
7	99	il valore di %s dev'essere ON oppure OFF	31337	0
7	100	Impostazione USERFLAGS non valida. I valori corretti sono NONE, OP o VOICE.	31337	0
7	101	USERFLAGS per %s è %s	31337	0
7	102	il valore di MASSDEOPPRO dev'essere compreso tra 0 e 7 inclusi	31337	0
7	103	MASSDEOPPRO per %s è impostato a %d	31337	0
7	104	il valore di FLOODPRO dev'essere compreso tra 0 e 7 inclusi	31337	0
7	105	FLOODPRO per %s è impostato a %d	31337	0
7	106	La DESCRIPTION non può superare gli 80 caratteri.	31337	0
7	107	La DESCRIPTION per %s è stata cancellata.	31337	0
7	108	La DESCRIPTION per %s è: %s	31337	0
7	109	L'URL non può superare i 75 caratteri.	31337	0
7	110	L'URL per %s è stata cancellata.	31337	0
7	111	L'URL per %s è: %s	31337	0
7	112	La stringa contenente le parole chiave non può superare gli 80 caratteri.	31337	0
7	113	Le KEYWORDS (parole chiave) per %s sono: %s	31337	0
7	114	La lingua selezionata è %s.	31337	0
7	115	ERRORE: Selezione della lingua non valida.	31337	0
7	116	Non trovo il canale %s nella rete!	31337	0
7	117	ERRORE: Impostazione del canale non valida.	31337	0
7	118	Lista degli Ignore:	31337	0
7	119	La lista degli Ignore è vuota	31337	0
7	120	-- Fine della Lista degli Ignore	31337	0
7	121	Stato interno CMaster Channel Services:	31337	0
7	122	[     Channel Record Stats] \002Cached Entries:\002 %i    \002DB Richieste:\002 %i    \002Cache Hits:\002 %i    \002Efficienza:\002 %.2f%%	31337	0
7	123	[        User Record Stats] \002Cached Entries:\002 %i    \002DB Richieste:\002 %i    \002Cache Hits:\002 %i    \002Efficienza:\002 %.2f%%	31337	0
7	124	[Access Level Record Stats] \002Cached Entries:\002 %i    \002DB Richieste:\002 %i    \002Cache Hits:\002 %i    \002Efficienza:\002 %.2f%%	31337	0
7	125	[         Ban Record Stats] \002Cached Entries:\002 %i    \002DB Richieste:\002 %i    \002Cache Hits:\002 %i    \002Efficienza:\002 %.2f%%	31337	0
7	126	Last recieved User NOTIFY: %i	31337	0
7	127	Last recieved Channel NOTIFY: %i	31337	0
7	128	Last recieved Level NOTIFY: %i	31337	0
7	129	Last recieved Ban NOTIFY: %i	31337	0
7	130	Contenitori di dati proprietari allocati: %i	31337	0
7	131	\002Tempo di vita:\002 %s	31337	0
7	132	Il canale %s ha %d utenti (%i operatori)	31337	0
7	133	Mode è: %s	31337	0
7	134	Flags impostati: %s	31337	0
7	135	Non posso sospendere un utente con accesso uguale o superiore al tuo.	31337	0
7	136	unità di tempo sconosciuta	31337	0
7	137	Durata della sospensione non valida.	31337	0
7	138	La SOSPENSIONE per %s è stata annullata	31337	0
7	139	%s è già stato sospeso in %s	31337	0
7	140	La SOSPENSIONE per %s scadrà in %s	31337	0
7	141	ERRORE: Il topic non può superare i 145 caratteri	31337	0
7	142	Non hai accesso sufficiente per rimuovere il ban %s dal database di %s.	31337	0
7	143	Sono stati rimossi %i ban che corrispondono a %s	31337	0
7	144	Rimosso temporaneamente il tuo accesso di %i dal canale %s	31337	0
7	145	Non sembra che tu abbia un accesso forzato in %s, forse è scaduto?	31337	0
7	146	%s non è sospeso in %s	31337	0
7	147	%s è un operatore IRC	31337	0
7	148	%s NON è autenticato.	31337	0
7	149	%s è autenticato come %s%s	31337	0
7	150	%s è un Rappresentante Ufficiale di CService%s e autenticato come %s	31337	0
7	151	%s è un Ammistratore Ufficiale di CService%s e autenticato come %s	31337	0
7	152	%s è uno Sviluppatore Ufficiale di CService%s e autenticato come %s	31337	0
7	153	Ci sono più di %i corrispondenze di [%s]	31337	0
7	154	Per favore, restringi la tua maschera di ricerca	31337	0
7	155	Non ci sono corrispondenze per [%s]	31337	0
7	156	%s: Mode del canale annullati.	31337	0
7	158	Opzione non valida.	31337	0
7	159	%s è un Bot di Servizio Ufficiale di Undernet.	31337	0
7	160	%s is an Official Coder-Com Representative%s and logged in as %s	31337	0
7	161	%s is an Official Coder-Com Contributer%s and logged in as %s	31337	0
7	162	%s is an Official Coder-Com Developer%s and logged in as %s	31337	0
7	163	%s is an Official Coder-Com Senior%s and logged in as %s	31337	0
7	165	Aggiunto il ban %s a %s con livello %i	31337	0
7	166	%s: lista dei ban vuota.	31337	0
7	167	Sono già in quel canale!	31337	0
\.

-- Romanian language definition.
-- 04/03/01 - Alex Badea <vampire@go.ro>
-- 10/28/02 - Typo fixes by 'Diaboliq'.
-- 12/23/02 - Tyranus <Tyranus@undernet.org>

COPY "translations" FROM stdin;
8	1	Esti deja autentificat ca %s	31337	0
8	2	AUTENTIFICARE REUSITA ca %s	31337	0
8	3	Nu ai suficient acces pentru aceasta comanda	31337	0
8	4	Canalul %s e gol	31337	0
8	5	Nu-l vad pe %s nicaieri	31337	0
8	6	Nu-l gasesc pe %s pe canalul %s	31337	0
8	7	Canalul %s nu pare sa fie inregistrat	31337	0
8	8	Ai primit op de la %s (%s) pe %s	31337	0
8	9	Ai primit voce de la %s (%s) pe %s	31337	0
8	10	%s: Nu esti pe canal	31337	0
8	11	%s are deja op pe %s	31337	0
8	12	%s are deja voce pe %s	31337	0
8	13	Ai fost deopat de %s (%s)	31337	0
8	14	Ai fost de-voiced de %s (%s)	31337	0
8	15	%s nu are op pe %s	31337	0
8	16	%s nu are voce pe %s	31337	0
8	17	AUTENTIFICARE ESUATA ca %s (parola incorecta)	31337	0
8	18	Nu sunt pe canal!	31337	0
8	19	Nivelul de ban e incorect. Domeniul admis este 1-%i.	31337	0
8	20	Durata banului e incorecta. Poti da ban pe o durata maxima de %d de ore.	31337	0
8	21	Motivul banului nu poate depasi 128 de caractere.	31337	0
8	22	Respectivul ban e deja pe lista!	31337	0
8	23	Banul pe %s e deja sub incidenta banului pe %s	31337	0
8	24	Nivel minim invalid.	31337	0
8	25	Nivel maxim invalid.	31337	0
8	26	USER: %s ACCES: %s %s	31337	0
8	27	CANAL: %s -- AUTOMODE: %s	31337	0
8	28	MODIFICAT: %s (in urma cu %s)	31337	0
8	29	** SUSPENDAT ** - Expira in %s (Level %i)	31337	0
8	30	VAZUT: in urma cu %s	31337	0
8	31	Sunt mai mult de %d rezultate.	31337	0
8	32	Restrictioneaza-ti cererea.	31337	0
8	33	Sfarsit lista de accese	31337	0
8	34	N-am gasit nimic!	31337	0
8	35	Nu poti adauga un user cu acces mai mare sau egal cu al tau. 	31337	0
8	36	Nivel de acces incorect.	31337	0
8	37	%s e deja pe lista %s cu acces de %i.	31337	0
8	38	Am adaugat %s la %s cu acces de %i	31337	0
8	39	Ceva n-a mers cum trebuie: %s	31337	0
8	40	%s: Sfarsit lista de banuri	31337	0
8	41	Nu pot da detalii despre user (e invizibil)	31337	0
8	42	Informatii despre: %s (%i)	31337	0
8	43	In prezent logat cu: %s	31337	0
8	44	URL: %s	31337	0
8	45	Limba: %i	31337	0
8	46	Canale: %s	31337	0
8	47	Flood la intrare (puncte): %i	31337	0
8	48	Flood la iesire (bytes): %i	31337	0
8	49	%s e inregistrat de:	31337	0
8	50	%s - vazut in urma cu %s	31337	0
8	51	Desc: %s	31337	0
8	52	Ma floodezi, huh? Nu am sa te mai ascult de acuma.	31337	0
8	53	Cred ca ti-am trimis cam multe date; o sa te ignor pentru o vreme.	31337	0
8	54	Comanda incompleta	31337	0
8	55	Pentru a folosi %s, trebuie sa /msg %s@%s	31337	0
8	56	Trebuie sa fii logat ca sa folosesti aceasta comanda.	31337	0
8	57	Canalul %s a fost suspendat de un administrator de la CService	31337	0
8	58	Accesul pe %s ti-a fost suspendat	31337	0
8	59	Flagul NOOP e activ pe %s	31337	0
8	60	Flagul STRICTOP e activ pe %s	31337	0
8	61	Ai deopat mai mult de %i persoane	31337	0
8	62	SINTAXA: %s	31337	0
8	63	Accesul pe %s ti-a fost marit temporar la %i	31337	0
8	64	%s e inregistrat.	31337	0
8	65	%s nu e inregistrat.	31337	0
8	66	Nu cred ca lui %s i-ar placea chestia asta.	31337	0
8	67	\002*** Ban List pentru canalul %s ***\002	31337	0
8	68	%s %s Nivel: %i	31337	0
8	69	DAT DE: %s (%s)	31337	0
8	70	DE LA: %s	31337	0
8	71	EXP: %s	31337	0
8	72	\002*** SFARSIT ***\002	31337	0
8	73	Nu stiu cine e %s.	31337	0
8	74	Nu mai esti autorizat.	31337	0
8	75	%s nu pare sa aiba acces pe %s.	31337	0
8	76	Nu poti modifica un user cu acces egal sau mai mare ca al tau.	31337	0
8	77	Nu poti da unui user acces mai mare sau egal cu al tau.	31337	0
8	78	Am modificat nivelul lui %s pe %s la %i	31337	0
8	79	Am setat AUTOMODE la OP pentru %s pe %s	31337	0
8	80	Am setat AUTOMODE la VOICE pentru %s pe %s	31337	0
8	81	Am setat AUTOMODE la NONE pentru %s pe %s	31337	0
8	82	Parola nu poate fi username-ul sau nick-ul tau. - sintaxa e: NEWPASS <parola noua>	31337	0
8	83	Parola a fost schimbata.	31337	0
8	84	Flag-ul NOOP e activ pe %s	31337	0
8	85	Flag-ul STRICTOP e activ pe %s (si %s nu e logat)	31337	0
8	86	Flag-ul STRICTOP e activ pe %s (si %s nu are destul acces)	31337	0
8	87	Am desfintat canalul %s	31337	0
8	88	%s e deja inregistrat.	31337	0
8	89	Nume de canal incorect.	31337	0
8	90	Am inregistrat canalul %s	31337	0
8	91	Am sters %s din lista de silence	31337	0
8	92	N-am gasit %s in lista de silence	31337	0
8	93	Nu poti sterge un user cu acces egal sau mai mare ca al tau	31337	0
8	94	Nu poti sa te stergi singur de pe un canal unde esti manager	31337	0
8	95	Am sters user-ul %s de pe %s	31337	0
8	96	Acum INVISIBLE este ON	31337	0
8	97	Acum INVISIBLE este OFF	31337	0
8	98	%s pentru %s e %s	31337	0
8	99	valoarea lui %s trebuie sa fie ON sau OFF	31337	0
8	100	Valoarea pentru USERFLAGS e incorecta. Valorile admise sunt NONE, OP, VOICE.	31337	0
8	101	USERFLAGS pentru %s este %s	31337	0
8	102	valoarea pentru MASSDEOPPRO trebuie sa fie 0-7	31337	0
8	103	MASSDEOPPRO pentru %s este %d	31337	0
8	104	valoarea pentru FLOODPRO trebuie sa fie 0-7	31337	0
8	105	FLOODPRO pentru %s este %d	31337	0
8	106	DESCRIPTION poate sa aiba maxim 80 de caractere!	31337	0
8	107	Am sters DESCRIPTION pentru %s	31337	0
8	108	DESCRIPTION pentru %s este: %s	31337	0
8	109	URL poate sa aiba maxim 75 de caractere!	31337	0
8	110	Am sters URL pentru %s	31337	0
8	111	URL pentru %s este: %s	31337	0
8	112	Sirul de cuvinte cheie nu poate depasi 80 de caractere!	31337	0
8	113	KEYWORDS pentru %s sunt: %s	31337	0
8	114	Limba este acum %s.	31337	0
8	115	EROARE: Limba ceruta e incorecta.	31337	0
8	116	Nu gasesc canalul %s in retea!	31337	0
8	117	EROARE: Setare de canal incorecta.	31337	0
8	118	Lista de ignore:	31337	0
8	119	Lista de ignore e goala	31337	0
8	120	-- Sfarsit lista de ignore	31337	0
8	121	Stare interna pentru CMaster Channel Services:	31337	0
8	122	[         Canale] \002In cache:\002 %i    \002Cereri din DB:\002 %i    \002Hituri din cache:\002 %i    \002Eficienta:\002 %.2f%%	31337	0
8	123	[    Utilizatori] \002In cache:\002 %i    \002Cereri din DB:\002 %i    \002Hituri din cache:\002 %i    \002Eficienta:\002 %.2f%%	31337	0
8	124	[Nivele de acces] \002In cache:\002 %i    \002Cereri din DB:\002 %i    \002Hituri din cache:\002 %i    \002Eficienta:\002 %.2f%%	31337	0
8	125	[        Ban-uri] \002In cache:\002 %i    \002Cereri din DB:\002 %i    \002Hituri din cache:\002 %i    \002Eficienta:\002 %.2f%%	31337	0
8	126	Ultimul User NOTIFY primit: %i	31337	0
8	127	Ultimul Channel NOTIFY primit: %i	31337	0
8	128	Ultimul Level NOTIFY primit: %i	31337	0
8	129	Ultimul Ban NOTIFY primit: %i	31337	0
8	130	Containere de date custom alocate: %i	31337	0
8	131	\002Uptime:\002 %s	31337	0
8	132	Canalul %s are %d useri (%i operatori)	31337	0
8	133	Moduri: %s	31337	0
8	134	Flaguri: %s	31337	0
8	135	Nu poti suspenda un user cu acces egal sau mai mare ca al tau.	31337	0
8	136	Ce unitati de timp sunt alea?	31337	0
8	137	Durata de suspend incorecta.	31337	0
8	138	SUSPENDAREA pentru %s anulata	31337	0
8	139	%s e deja suspendat pe %s	31337	0
8	140	SUSPENDAREA pentru %s expira in %s	31337	0
8	141	EROARE: Topicul nu poate depasi 145 de caractere	31337	0
8	142	Nu ai destul acces pentru a scoate banul %s de pe %s	31337	0
8	143	Am scos %i banuri care s-au potrivit cu %s	31337	0
8	144	Accesul temporar de %i pe %s a fost revocat	31337	0
8	145	Nu pari sa ai acces fortat pe %s, poate a expirat?	31337	0
8	146	%s nu e suspendat pe %s	31337	0
8	147	%s este un IRCop	31337	0
8	148	%s NU e logat	31337	0
8	149	%s e logat ca %s%s	31337	0
8	150	%s e un Reprezentant Oficial al CService%s si e logat ca %s	31337	0
8	151	%s e un Administrator Oficial al CService%s si e logat ca %s	31337	0
8	152	%s e un Programator Oficial al CService%s and logged in as %s	31337	0
8	153	Sunt mai mult de %i rezultate care se potrivesc cu [%s]	31337	0
8	154	Restrictioneaza-ti masca de cautare	31337	0
8	155	Nu sunt rezultate care sa se potriveasca cu [%s]	31337	0
8	156	%s: Am curatat modurile de canal.	31337	0
8	158	Optiune incorecta.	31337	0
8	159	%s este un Bot Oficial al Undernet.	31337	0
8	160	%s este un Reprezentant Oficial al Coder-Com%s si e logat ca %s	31337	0
8	161	%s este un Contributor Oficial al Coder-Com%s si e logat ca %s	31337	0
8	162	%s este un Programator Oficial al Coder-Com%s si e logat ca %s	31337	0
8	163	%s este un Reprezentant Senior al Coder-Com%s si e logat ca %s	31337	0
8	164	 si un IRCop	31337	0
8	165	Am adaugat ban pe %s pe %s la nivelul %i	31337	0
8	166	%s: lista de banuri e goala	31337	0
8	167	Sunt deja pe canal!	31337	0
8	168	Aceasta comanda este rezervata pentru IRCopi	31337	0
8	169	Nu sunt operator pe %s	31337	0
8	170	%s pentru %i minute	31337	0
\.

-- Catalan language definition.
-- 09/03/2001 - Algol <algol@undernet.org>.

COPY "translations" FROM stdin;
9	1	Ho sento, ja estàs autentificat/da com a %s	31337	0
9	2	AUTENTICACIÓ REEIXIDA com a %s	31337	0
9	3	Ho sento, el teu accés és insuficient per efectuar aquesta comanda.	31337	0
9	4	Ho sento, el canal %s és buit.	31337	0
9	5	No veig cap %s enlloc.	31337	0
9	6	No trobo cap %s al canal %s	31337	0
9	7	El canal %s no consta com a enregistrat.	31337	0
9	8	Has rebut op de %s (%s) a %s.	31337	0
9	9	Has rebut veu de %s (%s) a %s.	31337	0
9	10	%s: No ets a aquest canal.	31337	0
9	11	%s ja té op a %s	31337	0
9	12	%s ja té veu a %s	31337	0
9	13	L'op t'ha estat llevat per %s (%s).	31337	0
9	14	La veu t'ha estat llevada per %s (%s).	31337	0
9	15	%s no té op a %s	31337	0
9	16	%s no té veu a %s	31337	0
9	17	AUTENTICACIÓ FALLIDA com a %s (Contrasenya no vàlida).	31337	0
9	18	No sóc pas a aquest canal!	31337	0
9	19	Nivell de bandeig no vàlid. Els nivells vàlids són entre 1-%i.	31337	0
9	20	Durada de bandeig no vàlida . La durada màxima del bandeig és de %d hores.	31337	0
9	21	La raó del ban no pot excedir de 128 caràcters.	31337	0
9	22	El bandeig especificat ja és a la meva llista!	31337	0
9	23	El bandeig %s ja queda cobert per %s	31337	0
9	24	Nivell mínim no vàlid.	31337	0
9	25	Nivell màxim no vàlid.	31337	0
9	26	USUARI/A: %s ACCÉS: %s %s	31337	0
9	27	CANAL: %s -- AUTOMODE: %s	31337	0
9	28	DARRERA MODIFICACIÓ: %s (fa %s)	31337	0
9	29	** SUSPÈS/A ** - Expira d'aquí a %s (Level %i)	31337	0
9	30	VIST/A PER DARRERA VEGADA: fa %s.	31337	0
9	31	Hi ha més de %d entrades coincidents.	31337	0
9	32	Si et plau, restringeix la teva cerca.	31337	0
9	33	Fi de la llista d'accessos.	31337	0
9	34	Cap Coincidència!	31337	0
9	35	No pots afegir cap usuari/a amb un nivell d'accés igual o superior al teu.	31337	0
9	36	Nivell d'accés no vàlid.	31337	0
9	37	%s ja té accés a %s amb nivell %i.	31337	0
9	38	Afegit l'usuari/a %s a %s amb nivell d'accés %i.	31337	0
9	39	Quelcom no ha anat bé: %s	31337	0
9	40	%s: Fi de la llista de bandeigs.	31337	0
9	41	Els detalls de l'usuari/a no es poden veure pas (Invisible).	31337	0
9	42	Informació sobre: %s (%i)	31337	0
9	43	Connexió actual autenticada: %s	31337	0
9	44	URL: %s	31337	0
9	45	Idioma: %i	31337	0
9	46	Canals: %s	31337	0
9	47	Punts de Flood d'Entrada: %i	31337	0
9	48	Flood de Sortida (Bytes): %i	31337	0
9	49	%s està enregistrat per:	31337	0
9	50	%s - vist/a per darrer cop: fa %s	31337	0
9	51	Desc: %s	31337	0
9	52	Em vols floodejar, oi? Doncs no t'escoltaré més.	31337	0
9	53	Em penso que ja m'has fet enviar-te massa dades, ara t'ignoraré una estona.	31337	0
9	54	Comanda incompleta.	31337	0
9	55	Per fer %s, has de fer /msg %s@%s	31337	0
9	56	Ho sento, t'has d'haver autenticat per utilitzar aquesta comanda.	31337	0
9	57	El canal %s ha estat suspès per una/a administrador/a de CService.	31337	0
9	58	El teu accés a %s ha estat suspès.	31337	0
9	59	L'opció NOOP està activa a %s	31337	0
9	60	L'opció STRICTOP està activa a %s	31337	0
9	61	Acabes de llevar l'op a més de %i usuari/es.	31337	0
9	62	SINTAXI: %s	31337	0
9	63	El teu accés al canal %s ha estat incrementat temporalment fins %i.	31337	0
9	64	%s està enregistrat.	31337	0
9	65	%s no està enregistrat.	31337	0
9	66	No crec que a %s li fes gràcia.	31337	0
9	67	\002*** Llista de Bandeigs al canal %s ***\002	31337	0
9	68	%s %s Nivell: %i	31337	0
9	69	AFEGIT PER: %s (%s)	31337	0
9	70	DES DE: %s	31337	0
9	71	EXP: %s	31337	0
9	72	\002*** FI ***\002	31337	0
9	73	Ho sento, no sé qui és %s.	31337	0
9	74	Ho sento, ja no estàs autenticat/da.	31337	0
9	75	%s no figura amb accés a %s	31337	0
9	76	No pots modificar cap usuari/a amb nivell d'accés igual o superior al teu.	31337	0
9	77	No pots donar a un/a usuari/a un accés de nivell igual o superior al teu.	31337	0
9	78	El nivell d'accés de %s al canal %s s'ha canviat a %i.	31337	0
9	79	AUTOMODE canviat a OP per a %s al canal %s	31337	0
9	80	AUTOMODE canviat a VOICE pera %s al canal %s	31337	0
9	81	AUTOMODE canviat a NONE per a %s al canal %s	31337	0
9	82	La teva contrasenya no pot pas ser el teu nom d'usuari/a ni el teu nick actual - la sintaxi és: NEWPASS <nova contrasenya>	31337	0
9	83	Contrasenya canviada correctament.	31337	0
9	84	L'opció NOOP està activada a %s	31337	0
9	85	L'opció STRICTOP està activada a %s (i %s no s'ha autenticat).	31337	0
9	86	L'opció STRICTOP està activada a %s (i %s té un accés insuficient).	31337	0
9	87	El canal %s ha estat purgat.	31337	0
9	88	%s ja està enregistrat.	31337	0
9	89	Nom de canal no vàlid.	31337	0
9	90	El canal %s ha estat enregistrat.	31337	0
9	91	%s ha estat esborrat de la meva llista de silencis.	31337	0
9	92	No he trobat %s a la meva llista de silencis.	31337	0
9	93	No pots suprimir cap usuari amb un accés igual o superior al teu.	31337	0
9	94	No pots suprimir el teu propi accés a un canal que et pertany.	31337	0
9	95	S'ha suprimit l'usuari/a %s a %s	31337	0
9	96	La teva opció INVISIBLE és ara activa (ON).	31337	0
9	97	La teva opció INVISIBLE és ara inactiva (OFF).	31337	0
9	98	%s a %s és %s	31337	0
9	99	el valor de %s ha d'ésser 'ON' o 'OFF'.	31337	0
9	100	Valor d'USERFLAGS no vàlid. Els valors correctes són NONE, OP, VOICE.	31337	0
9	101	USERFLAGS per %s és %s.	31337	0
9	102	el valor de MASSDEOPPRO ha d'estar entre 0-7.	31337	0
9	103	MASSDEOPPRO per %s s'ha fixat a %d.	31337	0
9	104	el valor de FLOODPRO ha d'estar entre 0-7.	31337	0
9	105	FLOODPRO per %s s'ha fixat a %d.	31337	0
9	106	DESCRIPTION pot tenir fins a 80 caràcters com a màxim!	31337	0
9	107	La DESCRIPTION de %s ha estat esborrada.	31337	0
9	108	DESCRIPTION de %s és: %s	31337	0
9	109	URL pot tenir fins a 75 caràcters com a màxim!	31337	0
9	110	La URL de %s ha estat esborrada.	31337	0
9	111	La URL de %s és: %s	31337	0
9	112	KEYWORDS pot tenir fins a 80 caràcters com a màxim!	31337	0
9	113	Les KEYWORDS de %s són: %s	31337	0
9	114	L'idioma s'ha fixat a %s.	31337	0
9	115	ERROR: Selecció d'idioma no vàlida.	31337	0
9	116	No puc trobar el canal %s a la xarxa!	31337	0
9	117	ERROR: Configuració de canal no vàlida.	31337	0
9	118	Llista d'ignoraments:	31337	0
9	119	La llista d'ignoraments és buida.	31337	0
9	120	-- Fi de la Llista d'Ignoraments.	31337	0
9	121	Estat intern dels Serveis de Canals CMaster:	31337	0
9	122	[         Estadístiques de Canals] \002Entrades en Cache:\002 %i    \002DB Peticions:\002 %i    \002Encerts de Cache:\002 %i    \002Eficiència:\002 %.2f%%	31337	0
9	123	[       Estadístiques d'Usuari/es] \002Entrades en Cache:\002 %i    \002DB Peticions:\002 %i    \002Encerts de Cache:\002 %i    \002Eficiència:\002 %.2f%%	31337	0
9	124	[Estadístiques de Nivells d'Accés] \002Entrades en Cache:\002 %i    \002DB Peticions:\002 %i    \002Encerts de Cache:\002 %i    \002Eficiència:\002 %.2f%%	31337	0
9	125	[       Estadístiques de Bandeigs] \002Entrades en Cache:\002 %i    \002DB Peticions:\002 %i    \002Encerts de Cache:\002 %i    \002Eficiència:\002 %.2f%%	31337	0
9	126	Darrera recepció de NOTIFY d'Usuari/a: %i	31337	0
9	127	Darrera recepció de NOTIFY de Canal: %i	31337	0
9	128	Darrera recepció de NOTIFY de Nivell: %i	31337	0
9	129	Darrera recepció de NOTIFY de Bandeig: %i	31337	0
9	130	Contenidors de dades pròpies assignats: %i	31337	0
9	131	\002En funcionament (Uptime):\002 %s	31337	0
9	132	El canal %s té %d usuari/es (%i operador/es).	31337	0
9	133	Mode(s): %s	31337	0
9	134	Opcions actives: %s	31337	0
9	135	No pots suspendre cap usuari/a amb nivell d'accés igual o superior al teu.	31337	0
9	136	unitats de temps no vàlides.	31337	0
9	137	Durada de suspensió no vàlida.	31337	0
9	138	La SUSPENSIÓ de %s ha estat cancel·lada.	31337	0
9	139	%s ja està suspès/a a %s	31337	0
9	140	La SUSPENSIÓ de %s espirarà en %s	31337	0
9	141	ERROR: TOPIC no pot superar els 145 caràcters.	31337	0
9	142	El teu accés és insuficient per suprimir el bandeig %s de la base de dades de %s	31337	0
9	143	Suprimits %i bandeigs coincidents amb %s	31337	0
9	144	Suprimit el teu accés temporal de nivell %i al canal %s	31337	0
9	145	No consta que tinguis cap accés forçat a %s, potser ha expirat?	31337	0
9	146	%s no està suspès/a a %s	31337	0
9	147	%s és un/a Operador/a d'IRC.	31337	0
9	148	%s NO està autenticat/da.	31337	0
9	149	%s està autenticat/da com a %s%s	31337	0
9	150	%s és un/a Representant Oficial de CService%s i està autenticat/da com a %s	31337	0
9	151	%s és un/a Administrador/a Oficial de CService%s i està autenticat/da com a %s	31337	0
9	152	%s és un/a Programador/a Oficial de CService%s i està autenticat/da com a %s	31337	0
9	153	Hi ha més de %i entrades coincidents amb [%s]	31337	0
9	154	Restringeix la màscara de cerca, si et plau.	31337	0
9	155	Cap entrada coincident amb [%s]	31337	0
9	156	%s: Els modes del canal han estat esborrats.	31337	0
9	158	Opció no vàlida.	31337	0
9	159	%s és un Bot de Servei Oficial d'Undernet.	31337	0
9	160	%s és un/a Representant Oficial de Coder-Com%s i està autenticat/da com a %s	31337	0
9	161	%s és un/a Contrbuïdor/a Oficial de Coder-Com%s i està autenticat/da com a %s	31337	0
9	162	%s és un/a Programador/a Oficial de Coder-Com%s i està autenticat/da com a %s	31337	0
9	163	%s és un/a Senior Oficial de Coder-Com%s i està autenticat/da com a %s	31337	0
9	164	 i un/a Operador/a d'IRC.	31337	0
9	165	S'ha afegit el bandeig %s a %s amb nivell %i	31337	0
9	166	%s: la llista de bandeigs és buida.	31337	0
9	167	Ja sóc a aquest canal!	31337	0
9	168	Aquesta comanda és reservada per a Operador/es d'IRC.	31337	0
9	169	No tinc pas op a %s	31337	0
9	170	%s durant %i minuts.	31337	0
\.

-- Spanish language definition.
-- 09/03/2001 - Algol <algol@undernet.org>.

COPY "translations" FROM stdin;
10	1	Lo siento, ya estás autentificado/a como %s	31337	0
10	2	AUTENTIFICACIÓN COMPLETADA como %s	31337	0
10	3	Lo siento, tu acceso es insuficiente para ejecutar esta orden.	31337	0
10	4	Lo siento, el canal %s está vacío.	31337	0
10	5	No veo a %s en ningún sitio.	31337	0
10	6	No encuentro a %s en el canal %s	31337	0
10	7	El canal %s no consta como registrado.	31337	0
10	8	Has recibido op de %s (%s) en %s	31337	0
10	9	Has recibido voz de %s (%s) en %s	31337	0
10	10	%s: No estás en ese canal.	31337	0
10	11	%s ya tiene op en %s	31337	0
10	12	%s ya tiene voz en %s	31337	0
10	13	Te ha quitado el op: %s (%s)	31337	0
10	14	Te ha quitado la voz: %s (%s)	31337	0
10	15	%s no tiene op en %s	31337	0
10	16	%s no tiene voz en %s	31337	0
10	17	AUTENTIFICACIÓN FALLIDA como %s (Contraseña no válida).	31337	0
10	18	No estoy en ese canal!	31337	0
10	19	Nivel de ban no válido. El intervalo válido es 1-%i.	31337	0
10	20	Duración de ban no válida. La duración de ban máxima es de %d horas.	31337	0
10	21	El motivo del ban no puede superar los 128 caracteres.	31337	0
10	22	El ban especificado ya está en mi lista de bans!	31337	0
10	23	El ban %s ya está cubierto por %s	31337	0
10	24	Nivel mínimo no válido.	31337	0
10	25	Nivel máximo no válido.	31337	0
10	26	USUARIO/A: %s ACCESO: %s %s	31337	0
10	27	CANAL: %s -- AUTOMODO: %s	31337	0
10	28	ÚLTIMA MODIFICACIÓN: %s (hace %s)	31337	0
10	29	** SUSPENDIDO/A ** - Expira dentro de %s (Level %i)	31337	0
10	30	VISTO/A POR ÚLTIMA VEZ: hace %s.	31337	0
10	31	Hay más de %d entradas coincidentes.	31337	0
10	32	Restringe tu búsqueda, por favor.	31337	0
10	33	Fin de la lista de accesos.	31337	0
10	34	No hay coincidencias!	31337	0
10	35	No puedes añadir un usuario/a con un nivel igual o superior al tuyo.	31337	0
10	36	Nivel de acceso no válido.	31337	0
10	37	%s ya está añadido a %s con nivel de acceso %i.	31337	0
10	38	Se ha añadido el/la usuario/a %s a %s con nivel de acceso %i	31337	0
10	39	Algo ha fallado: %s	31337	0
10	40	%s: Fin de la lista de bans.	31337	0
10	41	Imposible ver los detalles del/la usuario/a (Invisible).	31337	0
10	42	Información sobre: %s (%i)	31337	0
10	43	Conexión actual autentificada: %s	31337	0
10	44	URL: %s	31337	0
10	45	Idioma: %i	31337	0
10	46	Canales: %s	31337	0
10	47	Puntos de Flood de Entrada: %i	31337	0
10	48	Flood de Salida (Bytes): %i	31337	0
10	49	%s está registrado por:	31337	0
10	50	%s - visto/a por última vez: %s ago	31337	0
10	51	Desc: %s	31337	0
10	52	Pretendes floodearme? Pues no voy a hacerte más caso.	31337	0
10	53	Creo que ya te he enviado datos de sobra, ahora te ignoraré un rato.	31337	0
10	54	Orden incompleta	31337	0
10	55	Para usar %s, debes /msg %s@%s	31337	0
10	56	Lo siento, has de estar autentificado/a para utilizar esta orden.	31337	0
10	57	El canal %s ha sido suspendido por un/a administrador/a de CService.	31337	0
10	58	Tu acceso en %s ha sido suspendido.	31337	0
10	59	La opción NOOP está activa en %s	31337	0
10	60	La opción STRICTOP está activa en %s	31337	0
10	61	Acabas de quitar el op a más de %i usuarios/as.	31337	0
10	62	SINTAXIS: %s	31337	0
10	63	Tu acceso en el canal %s se ha incrementado temporalmente a %i.	31337	0
10	64	%s está registrado.	31337	0
10	65	%s no está registrado.	31337	0
10	66	Dudo que a %s le parezca bien.	31337	0
10	67	\002*** Lista de bans para el canal %s ***\002	31337	0
10	68	%s %s Nivel: %i	31337	0
10	69	AÑADIDO POR: %s (%s)	31337	0
10	70	DESDE: %s	31337	0
10	71	EXP: %s	31337	0
10	72	\002*** FIN ***\002	31337	0
10	73	Lo siento, no sé quién es %s.	31337	0
10	74	Lo siento, ya no estás autentificado/a.	31337	0
10	75	%s no consta con acceso en %s	31337	0
10	76	No puedes modificar los usuarios/as con nivel de acceso igual o superior al tuyo.	31337	0
10	77	No puedes dar a un/a usuario/a un acceso de nivel igual o superior al tuyo.	31337	0
10	78	Se ha cambiado el nivel de acceso de %s en el canal %s a %i.	31337	0
10	79	AUTOMODE cambiado a OP para %s en el canal %s	31337	0
10	80	AUTOMODE cambiado a VOICE para %s en el canal %s	31337	0
10	81	AUTOMODE cambiado a NONE para %s en el canal %s	31337	0
10	82	Tu contraseña no puede ser tu nombre de usuario/a ni tu nick actual - la sintaxis es: NEWPASS <nueva contraseña>	31337	0
10	83	Contraseña cambiada con éxito.	31337	0
10	84	La opción NOOP está acitavada en %s	31337	0
10	85	La opción STRICTOP está activada en %s (y %s no está autentificado/a).	31337	0
10	86	La opción STRICTOP está activada en %s (y %s tiene un acceso insuficiente).	31337	0
10	87	El canal %s ha sido purgado.	31337	0
10	88	%s ya está registrado.	31337	0
10	89	Nombre de canal no válido.	31337	0
10	90	El canal %s ha sido registrado.	31337	0
10	91	%s ha sido eliminado de mi lista de silencios.	31337	0
10	92	No he encontrado %s en mi lista de silencios.	31337	0
10	93	No puedes suprimir usuarios/as con nivel de acceso igual o superior al tuyo.	31337	0
10	94	No puedes suprimir tu propio acceso en un canal que es tuyo.	31337	0
10	95	Se ha suprimido el/la usuario/a %s de %s	31337	0
10	96	Tu opción INVISIBLE está ahora activa (ON).	31337	0
10	97	Your INVISIBLE setting está ahora inactiva (OFF).	31337	0
10	98	%s en %s está %s	31337	0
10	99	el valor de %s sólo puede ser 'ON' u 'OFF'.	31337	0
10	100	Valor de USERFLAGS no válido. Los valores válidos son NONE, OP, VOICE.	31337	0
10	101	USERFLAGS en %s es %s.	31337	0
10	102	el valor de MASSDEOPPRO tiene que estar entre 0-7.	31337	0
10	103	MASSDEOPPRO en %s se ha establecido en %d.	31337	0
10	104	el valor de FLOODPRO tiene que estar entre 0-7.	31337	0
10	105	FLOODPRO en %s se ha establecido en %d.	31337	0
10	106	DESCRIPTION puede contener un máximo de 80 caracteres!	31337	0
10	107	La DESCRIPTION de %s ha sido borrada.	31337	0
10	108	La DESCRIPTION de %s es: %s	31337	0
10	109	URL puede contener un máximo de 75 caracteres!	31337	0
10	110	la URL de %s ha sido borrada.	31337	0
10	111	La URL de %s es: %s	31337	0
10	112	la cadena de KEYWORDS puede contener un máximo de 80 caracteres!	31337	0
10	113	KEYWORDS de %s son: %s	31337	0
10	114	Se ha seleccionado el idioma %s.	31337	0
10	115	ERROR: Selección de idioma no válida.	31337	0
10	116	No puedo encontrar el canal %s en la red!	31337	0
10	117	ERROR: Configuración de canal incorrecta.	31337	0
10	118	Lista de ignores:	31337	0
10	119	La lista de ignores está vacía	31337	0
10	120	-- Fin de la Lista de Ignores	31337	0
10	121	Estado interno de los Servicios de Canales CMaster:	31337	0
10	122	[          Estadísticas de Registros de Canales] \002Entradas en Cache:\002 %i    \002DB Peticiones:\002 %i    \002Aciertos en Cache:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
10	123	[         Estadísticas de Registros de Usuarios] \002Entradas en Cache:\002 %i    \002DB Peticiones:\002 %i    \002Aciertos en Cache:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
10	124	[Estadísticas de Registros de Niveles de Acceso] \002Entradas en Cache:\002 %i    \002DB Peticiones:\002 %i    \002Aciertos en Cache:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
10	125	[             Estadísticas de Registros de Bans] \002Entradas en Cache:\002 %i    \002DB Peticiones:\002 %i    \002Aciertos en Cache:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
10	126	Última recepción de NOTIFY de Usuario/a: %i	31337	0
10	127	Última recepción de NOTIFY de Canal: %i	31337	0
10	128	Última recepción de NOTIFY de Nivel: %i	31337	0
10	129	Última recepción de NOTIFY de Ban: %i	31337	0
10	130	Contenedores de datos propios asignados: %i	31337	0
10	131	\002Funcionando (Uptime):\002 %s	31337	0
10	132	El canal %s tiene %d usuarios/as (%i operadores/as)	31337	0
10	133	Modo(s): %s	31337	0
10	134	Opciones: %s	31337	0
10	135	No puedes suspender usuarios/as con un nivel de acceso igual o superior al tuyo.	31337	0
10	136	unidades de tiempo erróneas.	31337	0
10	137	Duración de suspensión no válida.	31337	0
10	138	La SUSPENSIÓN de %s ha sido cancelada	31337	0
10	139	%s ya está suspendido/a en %s	31337	0
10	140	La SUSPENSIÓN de %s expirará dentro de %s.	31337	0
10	141	ERROR: TOPIC no puede contener más de 145 caracteres.	31337	0
10	142	Tu acceso es insuficiente para suprimir el ban %s de la base de datos de %s	31337	0
10	143	Suprimidos %i bans coincidentes con %s	31337	0
10	144	Se ha suprimido tu acceso temporal de nivel %i en el canal %s	31337	0
10	145	No consta que tengas un acceso forzado en %s, quizás ha expirado?	31337	0
10	146	%s no está suspendido/a en %s	31337	0
10	147	%s es un/a Operador/a de IRC	31337	0
10	148	%s NO está autentificado/a.	31337	0
10	149	%s está autentificado/a como %s%s	31337	0
10	150	%s es un/a Representante Oficial de CService%s y está autentificado/a como %s	31337	0
10	151	%s es un/a Administrador/a Oficial de %s y está autentificado/a como %s	31337	0
10	152	%s es un/a Programador/a Oficial de CService%s y está autentificado/a como %s	31337	0
10	153	Hay más de %i entradas coincidentes con [%s]	31337	0
10	154	Restringe la máscara de búsqueda, por favor.	31337	0
10	155	No hay entradas coincidentes con [%s]	31337	0
10	156	%s: Los modos del canal han sido borrados.	31337	0
10	158	Opción inválida.	31337	0
10	159	%s es un Bot de Servicio Oficial de Undernet.	31337	0
10	160	%s es un/a Representante Oficial de Coder-Com%s y está autentificado/a como %s	31337	0
10	161	%s es un/a Contribuidor Oficial de Coder-Com%s y está autentificado/a como %s	31337	0
10	162	%s es un/a Programador/a Oficial de Coder-Com%s y está autentificado/a como %s	31337	0
10	163	%s es un/a Senior Oficial de Coder-Com%s y está autentificado/a como %s	31337	0
10	164	 y un/a Operador/a de IRC.	31337	0
10	165	Se ha añadido el ban %s a %s con nivel %i.	31337	0
10	166	%s: la lista de bans está vacía.	31337	0
10	167	Ya estoy en ese canal!	31337	0
10	168	Esta orden está reservada para Operadores/as de IRC.	31337	0
10	169	No tengo op en %s	31337	0
10	170	%s durante %i minutos.	31337	0
\.

-- Hungarian language definition.
-- 13/07/2001 - Laccc <blaszlo@kabelkon.ro>
-- 09/04/2003 - Laccc <Laccc@cservice.undernet.org>, updates/fixes.

COPY "translations" FROM stdin;
11	1	Mar be vagy jelentkezve %s felhasznaloi nev alatt	31337	0
11	2	AZONOSITAS SIKERULT a(z) %s felhasznaloi nev alatt	31337	0
11	3	Nincs eleg jogod ahhoz, hogy hasznald ezt a parancsot	31337	0
11	4	A(z) %s csatorna ures	31337	0
11	5	Nem latok %s nevet sehol	31337	0
11	6	Nem kapok %s nevet a(z) %s csatornan	31337	0
11	7	A(z) %s csatorna nincs meg regisztralva	31337	0
11	8	%s Operatori jogot adott neked (%s)	31337	0
11	9	%s beszelesi jogot adott neked (%s)	31337	0
11	10	%s: Nem tartozkodsz ezen a csatornan	31337	0
11	11	%s mar jenelneg is Operator a(z) %s csatornan	31337	0
11	12	%s mar jelenleg is voice-olva van a(z) %s csatornan	31337	0
11	13	%s elvette az Operatori jogodat (%s)	31337	0
11	14	%s elvette a beszelesi jogodat (%s)	31337	0
11	15	%s felhasznalonak nincs Operatori jog adva a(z) %s csatornan	31337	0
11	16	%s felhasznalonak nincs beszelesi jog adva a(z) %s csatornan	31337	0
11	17	AZONOSITAS SIKERTELEN a(z) %s felhasznaloi nev alatt (Hibas Jelszo)	31337	0
11	18	Nem vagyok azon a csatornan!	31337	0
11	19	Ervenytelen ban szint, 1-%i kozott valaszthatsz.	31337	0
11	20	Ervenytelen ban idotartalom. Maximum %d orat hasznalhatsz.	31337	0
11	21	A Ban indokban maximum csak 128 betut addhatsz meg.	31337	0
11	22	A megjelolt ban mar benne van a banlistaban!	31337	0
11	23	A(z) %s bant mar a letezo %s ban magaba vonja!	31337	0
11	24	Ervenytelen minimum szint.	31337	0
11	25	Ervenytelen maximum szint.	31337	0
11	26	FELHASZNALO: %s JOG(ACCESS): %s %s	31337	0
11	27	CSATRONA: %s -- AUTOMOD: %s	31337	0
11	28	UTOLJARA MODOSITVA: %s (%s -el ezelott)	31337	0
11	29	** FELFUGGESZTETT ** - Veget er %s mulva 	31337	0
11	30	UTOLJARA VOLT: %s -el ezelott..	31337	0
11	31	15-nel tobb hasonlo adat van.	31337	0
11	32	Legyszives korlatold a keresed.	31337	0
11	33	Vege az access listanak	31337	0
11	34	Nem kaptam semmit!	31337	0
11	35	Nem rakhatsz fel naladnal nagyobb- vagy a te jogoddal egyenlo erteku felhasznalokat.	31337	0
11	36	Ervenytelen access szint.	31337	0
11	37	%s nevu felhasznalo mar jelen van a(z) %s csatorna felhasznaloi listajaban, %i joggal.	31337	0
11	38	%s nevu felhasznalot sikeresen hozzaadtam a(z) %s csatornahoz, %i joggal	31337	0
11	39	Valamit nem sikerult befejeznem: %s	31337	0
11	40	%s: Vege a banlistanak	31337	0
11	41	Nem lehet latni a felhasznalo adatait (Lathatatlan)	31337	0
11	42	%s -rol az Informacio: (%i)	31337	0
11	43	Jelenleg be van jelentkezve: %s	31337	0
11	44	URL: %s	31337	0
11	45	Nyelv: %i	31337	0
11	46	Csatornak: %s	31337	0
11	47	Bejovo Flood pontok: %i	31337	0
11	48	Kimeno Flood (Bytes): %i	31337	0
11	49	A(z) %s csatorna regisztralva van:	31337	0
11	50	%s altal - Utoljara %s -el ezelott volt	31337	0
11	51	Koruliras: %s	31337	0
11	52	Akarsz floodolni, nemde? Tobbet nem halgatok rad.	31337	0
11	53	Azt hiszem kicsit sok adatot kuldtem neked, ignoralni foglak egy darabig.	31337	0
11	54	Befejezetlen parancs	31337	0
11	55	Hogy hasznalhasd %s -t, ahhoz /msg %s@%s -t kell hasznalj	31337	0
11	56	Eloszor azonositsd magad, mielott hasznalnad ezt a parancsot.	31337	0
11	57	A(z) %s csatornat felfuggesztette egy CService Administrator.	31337	0
11	58	A(z) %s csatornan levo jogod fel van fuggesztve.	31337	0
11	59	Aktivaltam a NOOP-ot a(z) %s csatornan	31337	0
11	60	Aktivaltam a STRICTOP-ot a(z) %s csatornan	31337	0
11	61	Tobb mint %i embernek vetted el az Operatori jogat	31337	0
11	62	SZINTAXIS: %s	31337	0
11	63	Ideiglenesen a(z) %s csatornan %i -re nott a jogod.	31337	0
11	64	%s regisztralva van .	31337	0
11	65	%s nincs regisztralva.	31337	0
11	66	Nem hinnem, hogy %s ertekelne ezt.	31337	0
11	67	\002*** A %s csatorna Ban Listaja ***\002	31337	0
11	68	%s %s Szint: %i	31337	0
11	69	FELTETTE: %s (%s)	31337	0
11	70	OTA: %s	31337	0
11	71	LEJAR:: %s	31337	0
11	72	\002*** VEGE ***\002	31337	0
11	73	Nem tudom ki az a(z) %s.	31337	0
11	74	Mar nem vagy azonositva.	31337	0
11	75	%s felhasznalonak nincs joga a(z) %s csatornahoz.	31337	0
11	76	Nem tudod modositani egyenlo- vagy naladnal magasabb erteku joggal rendelkezo szemely adatait.	31337	0
11	77	Nem tudsz a te jogoddal egyenlo- vagy magasabb jogot adni.	31337	0
11	78	%s joga a(z) %s csatornan %i lett.	31337	0
11	79	%s AUTOMOD erteke most OP lett a(z) %s csatornan	31337	0
11	80	%s AUTOMOD erteke most VOICE lett a(z) %s csatornan	31337	0
11	81	%s AUTOMOD erteke most NONE lett a(z) %s csatornan	31337	0
11	82	A jelszo nem egyezhet meg a userneveddel sem a nickneveddel - szintaxis : NEWPASS <uj jelszo>	31337	0
11	83	Sikeresen megvaltoztattam a jelszavad.	31337	0
11	84	Kiszedtem a NOOP-ot a(z) %s csatornarol	31337	0
11	85	A(z) %s csatornan aktivalva van a STRICTOP (es %s nincs azonositva)	31337	0
11	86	A(z) %s csatornan aktivalva van a STRICTOP (es %s felhasznalonak nincs eleg joga)	31337	0
11	87	A(z) %s torolt csatorna.	31337	0
11	88	%s mar jelenleg is regisztralva van.	31337	0
11	89	Ervenytelen csatorna nev.	31337	0
11	90	A(z) %s regisztralt csatorna.	31337	0
11	91	Letoroltem %s -t a silence listamrol	31337	0
11	92	Nem kapom %s -t a silence listamban	31337	0
11	93	Nem tudsz letorolni veled egyerteku- vagy magasabb joggal rendelkezo felhasznalot. 	31337	0
11	94	Nem tudod letorolni magad olyan csatornarol, ami a tied.	31337	0
11	95	Leszedtem %s -t a(z) %s csatornarol	31337	0
11	96	Az informacioid most lathatatlanok (INVISIBLE ON).	31337	0
11	97	Az informacioid most lathatoak (INVISIBLE OFF).	31337	0
11	98	A(z) %s %s -nak most %s	31337	0
11	99	A(z) %s erteke ON vagy OFF kell legyen	31337	0
11	100	Ervenytelen USERFLAGS beallitas. Ervenyes ertekek: NONE, OP, VOICE.	31337	0
11	101	A(z) %s csatornanak az USERFLAGS erteke most %s	31337	0
11	102	A MASSDEOPPRO erteke 0-7 kozott kell legyen	31337	0
11	103	A(z) %s csatorna MASSDEOPPRO opciojanak az erteke %d	31337	0
11	104	A FLOODPRO erteke 0-7 kozott kell legyen	31337	0
11	105	A(z) %s csatorna FLOODPRO opciojanak erteke %d	31337	0
11	106	A KORULIRAS maximum 80 betus lehet!	31337	0
11	107	A(z) %s csatorna KORULIRASA letorolve.	31337	0
11	108	A(z) %s csatorna KORULIRASA: %s	31337	0
11	109	Az URL maximum 75 betus lehet!	31337	0
11	110	A(z) %s csatorna URL-je letorolve.	31337	0
11	111	A(z) %s csatorna URL-je: %s	31337	0
11	112	A csatorna kulcsszavainak hossza nem lehet tobb mint 80 betu!	31337	0
11	113	A(z) %s csatorna kulcsszavai: %s	31337	0
11	114	A nyelv mostantol %s.	31337	0
11	115	HIBA: Ervenytelen nyelv valasztas.	31337	0
11	116	Nem kapom a(z) %s csatornat a halozaton!	31337	0
11	117	HIBA: Ervenytelen csatorna beallitas.	31337	0
11	118	Ignore lista:	31337	0
11	119	Az ignore lista ures	31337	0
11	120	-- Vege az ignore listanak	31337	0
11	121	CMaster Channel Services belso helyzete:	31337	0
11	122	[     Csatorna Record Helyzet] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
11	123	[  Felhasznalo Record Helyzet] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
11	124	[ Access szint Record Helyzet] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
11	125	[          Ban Record Helyyet] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
11	126	Utoljara kapott felhasznalo NOTIFY: %i	31337	0
11	127	Utoljara kapott csatorna NOTIFY: %i	31337	0
11	128	Utoljara kapott szint NOTIFY: %i	31337	0
11	129	Utoljara kapott Ban NOTIFY: %i	31337	0
11	130	A szokasos adat tartalmazok kiutalva: %i	31337	0
11	131	\002Uptime:\002 %s	31337	0
11	132	A(z) %s csatornanak %d felhasznaloja van (%i Operator)	31337	0
11	133	A csatorna mod: %s	31337	0
11	134	Opicok: %s	31337	0
11	135	Nem tudsz felfuggeszteni veled egyerteku- vagy magasabb joggal rendelkezo felhasznalokat.	31337	0
11	136	Hibas idoegysegek	31337	0
11	137	Hibas felfuggesztesi idotartalom.	31337	0
11	138	%s FELFUGGESZTESE visszavonva	31337	0
11	139	%s mar jelenleg is fel van fuggesztve a(z) %s csatornan	31337	0
11	140	%s FELFUGGESZTESE %s ido mulva jar le	31337	0
11	141	HIBA: A topicban nem lehet tobb mint 145 betu	31337	0
11	142	Nincs eleg jogod ahhoz, hogy leszedjed a(z) %s bant a(z) %s csatornarol	31337	0
11	143	Leszedtem %i olyan bant, ami talalt %s -el.	31337	0
11	144	Leszedtem a(z) %i ideiglenes jogodat a(z) %s csatirol	31337	0
11	145	Nem ugy nez ki mintha nem lenne eroltetett jogod a(z) %s csatornan, lehet hogy lejart volna?	31337	0
11	146	%s nincs felfuggesztve a(z) %s csatornan	31337	0
11	147	%s egy IRC operator	31337	0
11	148	%s NINCS azonositva.	31337	0
11	149	%s azonositasa megtortent, felhasznaloi neve: %s%s	31337	0
11	150	%s egy Hivatalos CService Kepviselo%s es %s a userneve	31337	0
11	151	%s egy Hivatalos CService Adminisztrator%s es %s a userneve	31337	0
11	152	%s egy Hivatalos CService Fejleszto%s es %s a userneve	31337	0
11	153	Tobb mint %i informacio talal. [%s]	31337	0
11	154	Legyszives korlatold a keresesi maszkot.	31337	0
11	155	Nem kaptam semmit [%s] kulcsszora	31337	0
11	156	%s: Letoroltem a csatorna modokat.	31337	0
11	158	Ervenytelen opcio.	31337	0
11	159	%s egy Hivatalos Undernet Bot.	31337	0
11	160	%s egy Hivatalos Coder-Com Kepviselo%s es %s a userneve	31337	0
11	161	%s egy Hivatalos Coder-Com Hozzajarulo%s es %s a userneve	31337	0
11	162	%s egy Hivatalos Coder-Com Kepviselo%s es %s a userneve	31337	0
11	163	%s egy Hivatalos Coder-Com Senior%s es %s a userneve	31337	0
11	164	 es egy IRC operator	31337	0
11	165	Feltettem a(z) %s bant a(z) %s csatornara, %i -es szintre.	31337	0
11	166	%s: Ures a ban lista.	31337	0
11	167	Mar jelenleg is bent vagyok azon a csatornan!	31337	0
11	168	Ezt a parancsot csak IRC Operatorok hasznalhatjak.	31337	0
11	169	Nem vagyok op a(z) %s csatornan	31337	0
11	170	%s %i percre	31337	0
\.

-- Turkish language definition.
-- 21/08/01 - Mehmet Akçin <cavalry@ircdestek.org>.
-- 21/08/01 - Ozan Ferah <claymore@ircdestek.org>.

COPY "translations" FROM stdin;
12	1	Üzgünüm, %s olarak zaten tanımlısınız.	31337	0
12	2	%s olarak TANIMLAMA BAŞARILI	31337	0
12	3	Üzgünüm, o komutu uygulamak için yeterli erişiminiz yok	31337	0
12	4	Üzgünüm, %s kanalı boş	31337	0
12	5	%s hiçbir yerde görünmüyor	31337	0
12	6	%s rumuzunu %s kanalında göremiyorum	31337	0
12	7	%s kanalı kayıtlı olarak gözükmüyor	31337	0
12	8	%s size operatörlük yetkisi verdi (%s) / %s	31337	0
12	9	%s size konuşma yetkisi verdi (%s) / %s	31337	0
12	10	%s: Siz kanalda değilsiniz	31337	0
12	11	%s zaten %s kanalında operatör durumunda	31337	0
12	12	%s zaten %s kanalında konuşma durumunda	31337	0
12	13	%s tarafından operatörlük yetkiniz alındı (%s)	31337	0
12	14	%s tarafından konuşma yetkiniz alındı (%s)	31337	0
12	15	%s için %s kanalında operatörlük yetkisi yok	31337	0
12	16	%s için %s kanalında konuşma yetkisi yok	31337	0
12	17	%s kanalında TANIMLAMA BAŞARISIZ (Geçersiz Şifre)	31337	0
12	18	Ben o kanalda değilim!	31337	0
12	19	Geçersiz yasaklama-seviye dizisi. Geçerli dizi 1-%i olmalı.	31337	0
12	20	Geçersiz yasaklama süresi. Sizin yasaklama süreniz en fazla %d saat olabilir.	31337	0
12	21	Yasaklama sebebi 128 karakteri aşmamalıdır	31337	0
12	22	Belirtilen yasaklama zaten benim yasaklılar listemde!	31337	0
12	23	%s yasaklaması zaten %s ile kapsanmış durumda	31337	0
12	24	Geçersiz en az seviye.	31337	0
12	25	Geçersiz en fazla seviye.	31337	0
12	26	KULLANICI: %s ERİŞİM: %s %s	31337	0
12	27	KANAL: %s -- OTOMOD: %s	31337	0
12	28	SON DEĞİŞİM: %s (%s önce)	31337	0
12	29	** ASKIYA ALINMIŞ ** - %s içinde süresi dolacak (Seviye %i)	31337	0
12	30	SON GÖRÜŞ: %s önce.	31337	0
12	31	15'den fazla eşleşen giriş var.	31337	0
12	32	Lütfen soruyu sınırlandırın.	31337	0
12	33	Erişim listesi sonu	31337	0
12	34	Eşleşme Yok!	31337	0
12	35	Sizikine eşit ya da yüksek seviye ile kullanıcı ekleyemezsiniz.	31337	0
12	36	Geçersiz erişim seviyesi.	31337	0
12	37	%s zaten %s kanalında %i erişim seviyesi ile eklenmiş.	31337	0
12	38	%s kullanıcısı %s kanalında %i erişim seviyesi ile eklendi	31337	0
12	39	Birşeyde aksama oldu: %s	31337	0
12	40	%s: Yasaklılar listesi sonu	31337	0
12	41	Kullanıcı detayları listelenemedi (Görünmez)	31337	0
12	42	Hakkında bilgiler: %s (%i)	31337	0
12	43	Şu anda giriş yapmış olan: %s	31337	0
12	44	Site: %s	31337	0
12	45	Lisan: %i	31337	0
12	46	Kanallar: %s	31337	0
12	47	Giren Yüklenme Puanı: %i	31337	0
12	48	Çıkan Yüklenme (Bit): %i	31337	0
12	49	%s kanalını kayıt ettirmiş olan:	31337	0
12	50	%s - son görüş: %s önce	31337	0
12	51	Tanım: %s	31337	0
12	52	Bana yükleme yapmaya devam edecek misin? Bundan sonra seni dinlemeyeceğim	31337	0
12	53	Düşünüyorum da sana biraz fazla bilgi yolladım, seni bir süre önemsemeyeceğim.	31337	0
12	54	Noksan komut	31337	0
12	55	%s komutunu kullanmak için, yapmanız gereken /msg %s@%s	31337	0
12	56	Üzgünüm, bu komutu kullanmak için tanımlanmalısınız.	31337	0
12	57	%s kanalı bir cservice yöneticisi tarafından geçici olarak kapatılmış.	31337	0
12	58	%s kanalındaki erişiminiz askıya alınmış.	31337	0
12	59	%s kanalında NOOP özelliği aktif hale getirilmiştir	31337	0
12	60	%s kanalında STRICTOP özelliği aktif hale getirilmiştir	31337	0
12	61	Biraz önce %i kişiden fazlasından operatörlük yetkisi aldınız	31337	0
12	62	SÖZDİZİMİ: %s	31337	0
12	63	%s kanalında erişiminiz geçici olarak %i olarak yükseltildi	31337	0
12	64	%s kayıtlı.	31337	0
12	65	%s kayıtsız.	31337	0
12	66	%s kanalının bunu takdir edeceğini sanmıyorum.	31337	0
12	67	\002*** %s için Yasaklılar Listesi ***\002	31337	0
12	68	%s %s Seviye: %i	31337	0
12	69	EKLEYEN: %s (%s)	31337	0
12	70	BAŞLANGIÇ: %s	31337	0
12	71	SONA ERİŞ: %s	31337	0
12	72	\002*** SON ***\002	31337	0
12	73	%s kimdir bilmiyorum.	31337	0
12	74	Artık bende tanımlı değilsiniz.	31337	0
12	75	%s %s kanalında erişimli gözükmüyor.	31337	0
12	76	Sizinkine eşit ya da fazla erişimli kullanıcıda değişiklik yapamazsınız.	31337	0
12	77	Sizinkine eşit ya da fazla erişim veremezsiniz.	31337	0
12	78	%s için erişim seviyesi %s kanalında %i olarak değiştirildi	31337	0
12	79	%s kullanıcı adı için %s kanalında otomatik operatörlük verildi	31337	0
12	80	%s kullanıcı adı için %s kanalında otomatik konuşma verildi	31337	0
12	81	%s kullanıcı adı için %s kanalında otomatik seçeneği kaldırıldı	31337	0
12	82	Şifre ibareniz kullanıcı adınız ya da şuanki rumuzunuz olamaz - sözdizimi: NEWPASS <yeni ibare>	31337	0
12	83	Şifre değişimi başarılı.	31337	0
12	84	%s kanalında NOOP özelliği aktif hale getirilmiştir	31337	0
12	85	%s kanalında STRICTOP özelliği aktif hale getirilmiştir (ve %s tanımlanmamış)	31337	0
12	86	%s kanalında STRICTOP özelliği aktif hale getirilmiştir (ve %s yetersiz erişimli)	31337	0
12	87	%s kanalı kapatıldı	31337	0
12	88	%s zaten bende kayıtlı.	31337	0
12	89	Geçersiz kanal ismi.	31337	0
12	90	Kayıtlı kanal %s	31337	0
12	91	%s sessizlik listemden çıkarıldı	31337	0
12	92	%s sessizlik listemde bulunamadı	31337	0
12	93	Sizinkine eşit ya da fazla erişimli kullanıcıyı silemezsiniz	31337	0
12	94	Kendinizi, sahibi olduğunuz kanaldan silemezsiniz	31337	0
12	95	%s kullanıcısı %s kanalında silindi	31337	0
12	96	GÖRÜNMEZLİK ayarınız açıldı.	31337	0
12	97	GÖRÜNMEZLİK ayarınız kapatıldı.	31337	0
12	98	%s %s için %s	31337	0
12	99	%s için değer ON ya da OFF olmalı	31337	0
12	100	Geçersiz USERFLAGS ayarı. Doğru değerler NONE, OP, VOICE.	31337	0
12	101	%s için USERFLAGS ayarı %s	31337	0
12	102	MASSDEOPPRO için değer 0-7 olmalı	31337	0
12	103	MASSDEOPPRO %s kanalı için %d yapıldı	31337	0
12	104	FLOODPRO için değer 0-7 olmalı	31337	0
12	105	FLOODPRO %s kanalı için %d yapıldı	31337	0
12	106	KANAL TANIMI en fazla 80 karakter olabilir	31337	0
12	107	%s için KANAL TANIMI silindi.	31337	0
12	108	%s için KANAL TANIMI: %s	31337	0
12	109	Site adı en fazla 75 karakter olabilir!	31337	0
12	110	%s için site adı silindi.	31337	0
12	111	%s için site adı: %s	31337	0
12	112	Anahtar kelimeler için dizgi 80 karakteri aşamaz!	31337	0
12	113	%s için ANAHTAR KELİMELER: %s	31337	0
12	114	Lisan %s olarak belirlendi.	31337	0
12	115	HATA: Geçersiz lisan seçimi.	31337	0
12	116	Ağda %s kanalının yeri saptanamıyor!	31337	0
12	117	HATA: Geçersiz kanal ayarı.	31337	0
12	118	Önemsenmeyenler listesi:	31337	0
12	119	Önemsenmeyenler listesi boş	31337	0
12	120	-- Önemsenmeyenler listesi sonu	31337	0
12	121	CMaster Kanal Servisleri dahili statüsü:	31337	0
12	122	[     Kanal Kayıt İstatistikleri] \002Kaydedilmiş girişler:\002 %i    \002VT İstekleri:\002 %i    \002Kayıt hitleri:\002 %i    \002Verim:\002 %.2f%%	31337	0
12	123	[        Kullanıcı Kayıt İstatistikleri] \002Kaydedilmiş girişler:\002 %i    \002VT İstekleri:\002 %i    \002Kayıt hitleri:\002 %i    \002Verim:\002 %.2f%%	31337	0
12	124	[Erişim Kayıt İstatistikleri] \002Kaydedilmiş girişler:\002 %i    \002VT İstekleri:\002 %i    \002Kayıt hitleri:\002 %i    \002Verim:\002 %.2f%%	31337	0
12	125	[         Yasaklama Kayıt İstatistikleri] \002Kaydedilmiş girişler:\002 %i    \002VT İstekleri:\002 %i    \002Kayıt hitleri:\002 %i    \002Verim:\002 %.2f%%	31337	0
12	126	Alınan son Kullanıcı BİLDİRİMİ: %i	31337	0
12	127	Alınan son Kanal BİLDİRİMİ: %i	31337	0
12	128	Alınan son Seviye BİLDİRİMİ: %i	31337	0
12	129	Alınan son Yasaklama BİLDİRİMİ: %i	31337	0
12	130	Belirli bilgi kapları tahsis edildi: %i	31337	0
12	131	\002Açık kaldığı süre:\002 %s	31337	0
12	132	%s kanalında %d tane kullanıcı var (%i operatör)	31337	0
12	133	Mod: %s	31337	0
12	134	Bayraklar: %s	31337	0
12	135	Sizinkine eşit ya da fazla erişimi olan kişiyi askıya alamazsınız.	31337	0
12	136	sahte zaman üniteleri	31337	0
12	137	Geçersiz akıya alma süresi.	31337	0
12	138	%s için ASKIYA ALMA iptal edildi	31337	0
12	139	%s zaten %s kanalında askıya alınmış durumda	31337	0
12	140	%s için ASKIYA ALMA %s süre sonra dolacak	31337	0
12	141	HATA: Başlık 145 karakteri aşamaz	31337	0
12	142	%s yasaklamasını %s kanalının veri tabanından kaldırmak için yetersiz erişiminiz var	31337	0
12	143	%i tane %s ile eşleşen yasaklama silindi	31337	0
12	144	%i olan geçici erişiminiz %s kanalından silindi	31337	0
12	145	%s kanalında güçlendirilmiş erişimin yok gibi görünüyor, belki süresi dolmuştur?	31337	0
12	146	%s %s kanalında askıya alınmamış	31337	0
12	147	%s bir IRC operatörü	31337	0
12	148	%s tanımlanmamış durumda.	31337	0
12	149	%s %s%s olarak tanımlı	31337	0
12	150	%s Resmi bir CService Temsilcisi%s ve %s olarak tanımlı	31337	0
12	151	%s Resmi bir CService Yöneticisi%s ve %s olarak tanımlı	31337	0
12	152	%s Resmi bir CService Geliştiricisi%s ve %s olarak tanımlı	31337	0
12	153	%i taneden daha fazla eşleşme var [%s]	31337	0
12	154	Lütfen arama maskenizi sınırlandırın	31337	0
12	155	[%s] için geçerli eşleşme yok	31337	0
12	156	%s: Kanal modları temizlendi.	31337	0
12	158	Geçersiz seçenek.	31337	0
12	159	%s Resmi bir Undernet Servis Robotu.	31337	0
12	160	%s Resmi bir Coder-Com Temsilcisi%s ve %s olarak tanımlı	31337	0
12	161	%s Resmi bir Coder-Com Katkıda Bulunucusu%s ve %s olarak tanımlı	31337	0
12	162	%s Resmi bir Coder-Com Geliştiricisi%s ve %s olarak tanımlı	31337	0
12	163	%s Resmi bir Coder-Com Uzmanı%s ve %s olarak tanımlı	31337	0
12	164	 ve bir IRC operatörü	31337	0
12	165	%s yasaklaması %s kanalında %i seviye ile eklendi	31337	0
12	166	%s: yasaklılar listesi boş.	31337	0
12	167	Ben zaten o kanaldayım!	31337	0
12	168	Bu komut IRC operatörlere ait	31337	0
12	169	%s kanalında operatörlük yetkim yok	31337	0
12	170	%s için %i dakika	31337	0
\.

-- Norwegian Language Definition.
-- 21/08/01
-- Bjørn Osdal (Nick: Snatcher) <mrosdal@online.no>
-- Ronny Kvislavangen (Nick: KingGenie) <genie@undernet.org>
-- Past Contributions: Mr_|r0n, CISC.

COPY "translations" FROM stdin;
13	1	Beklager, men du er allerede logget inn som %s.	31337	0
13	2	INNLOGGING GODKJENT som %s.	31337	0
13	3	Beklager, men du har ikke nok aksess for å få utført den kommandoen.	31337	0
13	4	Beklager, men kanalen %s er tom.	31337	0
13	5	Jeg kan ikke finne %s noen steder	31337	0
13	6	Jeg kan ikke finne %s på %s 	31337	0
13	7	Kanalen %s ser ikke ut til å være registrert.	31337	0
13	8	Du har fått operatør status av %s (%s) i %s	31337	0
13	9	Du har fått voice status av %s (%s) i %s	31337	0
13	10	%s: Du er ikke i den kanalen	31337	0
13	11	%s er allerede operatør i %s	31337	0
13	12	%s har allerede voice i %s	31337	0
13	13	%s (%s) tok ifra deg operatør status	31337	0
13	14	%s (%s) tok ifra deg voice status	31337	0
13	15	%s er ikke operatør i %s	31337	0
13	16	%s har ikke voice i %s	31337	0
13	17	INNLOGGING FEILET som %s (Feil passord)	31337	0
13	18	Jeg er ikke i den kanalen!	31337	0
13	19	Ugyldig banlevel. Må være 1-%i.	31337	0
13	20	Ugyldig ban varighet. Den kan maksimum være %d timer.	31337	0
13	21	Ban grunn kan ikke overstige 128 tegn	31337	0
13	22	Den spesifiserte bannen er allerede i min banliste!	31337	0
13	23	Bannen %s er allerede dekket av %s	31337	0
13	24	Ikke korrekt minimums level	31337	0
13	25	Ikke korrekt maksimums level	31337	0
13	26	BRUKER: %s AKSESS: %s %s	31337	0
13	27	KANAL: %s -- AUTOMODE: %s	31337	0
13	28	SIST MODIFISERT: %s (%s siden)	31337	0
13	29	** SUSPENDERT ** - Utløper om %s (Level %i)	31337	0
13	30	Sist sett for: %s timer siden	31337	0
13	31	Det er mere enn %d treff som passer dine kriterier.	31337	0
13	32	Vennligst begrens ditt søk.	31337	0
13	33	Slutt på aksesslisten	31337	0
13	34	Ingen treff!	31337	0
13	35	Kan ikke legge til bruker med lik eller høyere aksess enn din egen.	31337	0
13	36	Ikke gyldig aksess level.	31337	0
13	37	%s er allerede lagt til i %s med aksess level %i.	31337	0
13	38	Lagt til bruker %s i %s med aksess level %i	31337	0
13	39	Noe gikk galt: %s	31337	0
13	40	%s: Slutt på banlisten	31337	0
13	41	Ikke mulig å se brukers detaljer (Usynlig)	31337	0
13	42	Informasjon om: %s (%i)	31337	0
13	43	Er akuratt nå logget på via: %s	31337	0
13	44	URL: %s	31337	0
13	45	Språk: %i	31337	0
13	46	Kanaler: %s	31337	0
13	47	Input Flood Points: %i	31337	0
13	48	Ouput Flood (Bytes): %i	31337	0
13	49	%s er registrert av:	31337	0
13	50	%s - sist sett: %s siden	31337	0
13	51	Beskrivelse: %s	31337	0
13	52	Bare flood meg du. Jeg kommer ikke til å høre på deg igjen	31337	0
13	53	Jeg tror jeg sendte deg litt for mye data. Jeg kommer til å ignorere deg for en liten stund.	31337	0
13	54	Ufullstendig kommando	31337	0
13	55	For å bruke %s, må du bruke /msg %s@%s	31337	0
13	56	Beklager, men du må være logget inn for å bruke denne kommandoen.	31337	0
13	57	Kanalen %s har blitt suspendert av en Cservice Administrator.	31337	0
13	58	Din aksess på %s er suspendert.	31337	0
13	59	NOOP flagget er satt på %s	31337	0
13	60	STRICTOP flagget er satt på %s	31337	0
13	61	Du deopet flere enn %i personer	31337	0
13	62	SYNTAKS: %s	31337	0
13	63	Midlertidig øket din aksess på kanal %s til %i	31337	0
13	64	%s er registrert.	31337	0
13	65	%s er ikke registrert.	31337	0
13	66	Jeg tror ikke at %s vil like det der.	31337	0
13	67	\002*** Banliste for %s ***\002	31337	0
13	68	%s %s Level: %i	31337	0
13	69	LAGT TIL AV: %s (%s)	31337	0
13	70	SIDEN: %s	31337	0
13	71	UTGÅR: %s	31337	0
13	72	\002*** SLUTT ***\002	31337	0
13	73	Jeg vet ikke hvem %s er.	31337	0
13	74	Du er ikke logget inn hos meg lenger.	31337	0
13	75	%s ser ikke ut til å ha aksess i %s.	31337	0
13	76	Kan ikke modifisere en bruker med aksess som er lik eller høyere enn din egen.	31337	0
13	77	Kan ikke gi en bruker aksess som er lik eller høyere enn din egen.	31337	0
13	78	Modifisert %s sin aksess på kanal %s til %i	31337	0
13	79	Satt AUTOMODE til OP for %s på kanal %s	31337	0
13	80	Satt AUTOMODE til VOICE for %s på kanal %s	31337	0
13	81	Satt AUTOMODE til IKKENOE for %s påp kanal %s	31337	0
13	82	Ditt passord kan ikke være ditt brukernavn eller ditt nåværende nick - syntaks er: NEWPASS <nytt passord>	31337	0
13	83	Passordet er byttet.	31337	0
13	84	NOOP flagget er satt på %s	31337	0
13	85	STRICTOP flagget er satt på %s (og %s er ikke logget inn)	31337	0
13	86	STRICTOP flagget er satt på %s (og %s har ikke nok aksess)	31337	0
13	87	Slettet kanal %s	31337	0
13	88	%s er allerede registrert hos meg.	31337	0
13	89	Ugyldig kanalnavn.	31337	0
13	90	Registrert kanal %s	31337	0
13	91	Slettet %s fra min ignoreliste	31337	0
13	92	Kunne ikke slette %s fra min ignoreliste	31337	0
13	93	Kan ikke slette en bruker med lik eller høyere aksess enn din egen	31337	0
13	94	Du kan ikke fjerne deg selv ifra en kanal du eier.	31337	0
13	95	Slettet bruker %s fra %s	31337	0
13	96	INVISIBLE(usynlig) er nå ON(på).	31337	0
13	97	INVISIBLE(usynlig) er nå OFF(av).	31337	0
13	98	%s for %s er %s	31337	0
13	99	%s MÅ være ON eller OFF	31337	0
13	100	Ugyldig USERFLAGS instilling. Korrekte verdier er: NONE, OP, VOICE.	31337	0
13	101	USERFLAGS for %s er %s	31337	0
13	102	Verdien for MASSDEOPPRO må være 0-7	31337	0
13	103	MASSDEOPPRO for %s er satt til %d	31337	0
13	104	Verdien til FLOODPRO må være 0-7	31337	0
13	105	FLOODPRO for %s er satt til %d	31337	0
13	106	BESKRIVELSE kan maksimalt være 80 tegn!	31337	0
13	107	BESKRIVELSE for %s er slettet.	31337	0
13	108	BESKRIVELSE for %s er: %s	31337	0
13	109	URL kan maksimum være 75 tegn!	31337	0
13	110	URL for %s er slettet.	31337	0
13	111	URL for %s er: %s	31337	0
13	112	KEYWORDS kan ikke overstige 80 tegn!	31337	0
13	113	KEYWORDS for %s er: %s	31337	0
13	114	Språk er satt til %s.	31337	0
13	115	ERROR: Ugyldig språk valg.	31337	0
13	116	Kan ikke finne kanal %s på nettverket!	31337	0
13	117	ERROR: Ugyldig kanal innstilling.	31337	0
13	118	Ignoreliste:	31337	0
13	119	Ignorelista er tom	31337	0
13	120	-- Slutt på ignorelista	31337	0
13	121	CMaster Channel Services intern status:	31337	0
13	122	[     Channel Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
13	123	[        User Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
13	124	[Access Level Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
13	125	[         Ban Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
13	126	Sist mottatt bruker NOTIFY: %i	31337	0
13	127	Sist mottatt kanal NOTIFY: %i	31337	0
13	128	Sist mottatt level NOTIFY: %i	31337	0
13	129	Sist mottatt ban NOTIFY: %i	31337	0
13	130	Custom data containers allocated: %i	31337	0
13	131	\002Oppetid:\002 %s	31337	0
13	132	Kanalen %s har %d brukere (%i operatører)	31337	0
13	133	Mode er: %s	31337	0
13	134	Flagg satt: %s	31337	0
13	135	Kan ikke suspendere en bruker med lik eller høyere aksess enn din egen.	31337	0
13	136	bogus tids enheter	31337	0
13	137	Ugyldig suspend varighet.	31337	0
13	138	SUSPENSION for %s er slettet	31337	0
13	139	%s er allerede suspendert på %s	31337	0
13	140	SUSPENSION for %s går ut om %s	31337	0
13	141	ERROR: Topic kan ikke overskride 145 tegn	31337	0
13	142	Du har ikke tilstrekkelig aksess for å slette bannen %s ifra %s's database	31337	0
13	143	Slettet %i bans som stemmer med %s	31337	0
13	144	Midlertidig slettet din aksess på %i ifra kanal %s	31337	0
13	145	Det ser ikke ut som at du har en tvunget aksess i %s, kanskje den har utløpt?	31337	0
13	146	%s er ikke suspendert på %s	31337	0
13	147	%s er en IRC operatør	31337	0
13	148	%s er IKKE logget inn.	31337	0
13	149	%s er logget inn som %s%s	31337	0
13	150	%s er en offisiell CService Representant%s og er logget inn som  %s	31337	0
13	151	%s er en offisiell CService Administrator%s og er logget inn som %s	31337	0
13	152	%s er en offisiell CService Utvikler%s og er logget inn som %s	31337	0
13	153	Det er flere enn %i muligheter som passer [%s]	31337	0
13	154	Vennligst begrens dine søkekriterie	31337	0
13	155	Ingen postering funner for [%s]	31337	0
13	156	%s: Slettet kanal modes.	31337	0
13	158	Ugyldig valg.	31337	0
13	159	%s er en offisiell Undernet Service Bot.	31337	0
13	160	%s er en offisiell Coder-Com Representant%s og er logget inn som %s	31337	0
13	161	%s er en offisiell Coder-Com Bidragsyter%s og er logget inn som %s	31337	0
13	162	%s er en offisiell Coder-Com Utvikler%s og er logget inn som %s	31337	0
13	163	%s er en offisiell Coder-Com Senior%s og er logget inn som %s	31337	0
13	164	 og en IRC operatør	31337	0
13	165	Lagt til ban %s til %s med level %i	31337	0
13	166	%s: banlisten er tom.	31337	0
13	167	Jeg er allerede på den kanalen!	31337	0
13	168	Denne kommandoen er reservert for IRC operatører	31337	0
13	169	Jeg er ikke operatør på %s	31337	0
13	170	%s for %i minutter	31337	0
\.

-- Arabic Language Definition.
-- 02/09/01
-- B_O_S_S - boss@boss.org

COPY "translations" FROM stdin;
14	1	Assif. anta mo3arafone sabi9ane %s	31337	0
14	2	ta3arof naji7li  %s	31337	0
14	3	Assif.	31337	0
14	4	Assif. Al9anatou %s farigha	31337	0
14	5	lam yatamakan li woujoud  %s	31337	0
14	6	Lam ajid %s fi 9anate %s	31337	0
14	7	al9anatou %s laysate mousajala	31337	0
14	8	la9ade asbahta op min tarafe %s (%s) fi %s	31337	0
14	9	la9ade asbahta voice min tarafe %s (%s) fi %s	31337	0
14	10	%s: anta lasta dakhila hadihi al9anate	31337	0
14	11	%s lahou sabi9ane op fi  %s	31337	0
14	12	%s lahou sabi9ane voice fi %s	31337	0
14	13	la9ade okhida laka al op min tarafe %s (%s)	31337	0
14	14	la9ade okhida laka voice min tarafe %s (%s)	31337	0
14	15	%s laissa op fi %s	31337	0
14	16	%s laissa voice fi %s	31337	0
14	17	TA3AROUFE GHAYR NAJI7 %s (KALIMATO ASIRE KHATI2A)	31337	0
14	18	Ana lastou dakhila hadidi al9anate!!	31337	0
14	19	Moustawa ban GHAYROU sahihe. Moustawa sahihe: bayna 1-%i.	31337	0
14	20	Moudatou ban ghayrou sahiha. Almoudatou alkouswa Hia %d heures.	31337	0
14	21	Sababou al ban layomkino an yata3adda 128 7arfe .	31337	0
14	22	Hada al ban Youjadou sabi9ane fe la2i7a!	31337	0
14	23	Al ban %s youjado sabi9ane %s	31337	0
14	24	Almousstawa al adna ghayrou sahi7e.	31337	0
14	25	Almousstawa al a9ssa ghayrou sahi7e.	31337	0
14	26	MOUSSTA3MILE%: %s MOUSSTAWA: %s %s	31337	0
14	27	AL9ANATE: %s -- AMRE: %s	31337	0
14	28	AKHIRE TAGHYIRE: %s (%s kabla)	31337	0
14	29	** MOUTAWA9IFE ** - Yantahi fi %s (moustawa %i)	31337	0
14	30	AKHIRE MARA CHOUHIDA : %s 9able.	31337	0
14	31	Youjado aktare min %d talabe .	31337	0
14	32	Min Fadlik 7addid talabake	31337	0
14	33	Nihayato lai7ati al mousstawayate	31337	0
14	34	lam youjade ayou chayeine youwafi9o talabakome 	31337	0
14	35	La youmkino zeyadata chakhs lahou moustawake awe aktare. 	31337	0
14	36	Mousstawa atadakhole ghayro sahi7e	31337	0
14	37	%s oudifa moussaba9ane %s fi mousstawa %i.	31337	0
14	38	ziyadatou moussta3mile %s ala %s fi mousstawa %i	31337	0
14	39	Youjado ghalate: %s	31337	0
14	40	%s: nihayatou lai7ati al ban	31337	0
14	41	la youmkinou idharou tafassile al moussta3mile.	31337	0
14	42	Ma3loumate 3ala: %s (%i)	31337	0
14	43	alane moutassile 3abra: %s	31337	0
14	44	URL: %s	31337	0
14	45	Lougha: %i	31337	0
14	46	9ANAWATE: %s	31337	0
14	47	Madkhale Flood: %i	31337	0
14	48	Makhraje Flood (Bytes): %i	31337	0
14	49	%s moussajaloune min tarafe:	31337	0
14	50	%s - chouhida lile mara akhira : %s 9able	31337	0
14	51	wassf: %s	31337	0
14	52	Touridouna flood ? lane arouda alaykoume ba3da hada.	31337	0
14	53	Adono anani arsaltou lakoume ma3loumatine katira. saatajahaloukome ba3da al wa9te.	31337	0
14	54	Amroune na9ise	31337	0
14	55	li issti3mali %s, alaykoume bi /msg %s@%s	31337	0
14	56	Assif. 3Alayka an takouna mo3arafe li sti3mali hada al amre.	31337	0
14	57	9anatou %s oughli9ate min tarafe moudire cservice.	31337	0
14	58	Tadakhouloukoume fi %s tawa9afa.	31337	0
14	59	Tari9ate NOOP mousta3mala biha fi %s	31337	0
14	60	Tari9ate STRICTOP mousta3mala fi %s	31337	0
14	61	La9ad fa3alta deop li aktare min %i chakhsse	31337	0
14	62	SYNTAXE: %s	31337	0
14	63	mou3arafe moua9atane fi %s bi mousstawa %i	31337	0
14	64	%s mousajala.	31337	0
14	65	%s ghayre moussajala.	31337	0
14	66	La adono ana %s you7iba hada.	31337	0
14	67	\002*** lai7ato al Ban li 9anate %s ***\002	31337	0
14	68	%s %s mousstawa: %i	31337	0
14	69	OUDIFA MIN Tarafe: %s (%s)	31337	0
14	70	MOUNDO: %s	31337	0
14	71	YANTAHI: %s	31337	0
14	72	\002*** NIHAYA ***\002	31337	0
14	73	Assif, La a3rifou man houa %s.	31337	0
14	74	Assif, Lasta mou3arafe.	31337	0
14	75	%s La youjado ayo tadakhoule mouwafi9e fi %s.	31337	0
14	76	La youmkinouka taghyire ayi moussata3mile lahou moustawakoume awe aktare. 	31337	0
14	77	La youmkino ane tane9asa tadakhoule lahou moustawake awe aktare .	31337	0
14	78	Taghyire mousstawa %s fi al9anate %s ila %i	31337	0
14	79	Tachghilou tari9ate (OP) atela9ai li %s fi 9anate %s	31337	0
14	80	Tachghilou tarikate (VOICE) atela9ai li %s fi 9anate %s	31337	0
14	81	Izalatou tari9ate atela9ai li %s fil 9anate %s	31337	0
14	82	ra9mouka asiri la younassibou isma lmousta3mile wa nick - al Syntaxe hiya: NEWPASS <nouveau mot-de-passe>	31337	0
14	83	ra9mouka asiri ghouyira bi naja7e.	31337	0
14	84	Tari9atou NOOP Mousta3mala fi %s	31337	0
14	85	Tari9atou STRICTOP mousta3mala fi %s (wa %s Layssa mou3arafe)	31337	0
14	86	Tari9atou STRICTOP mousta3mala fi %s (wa tadakhoulo %s ghayrou kafi)	31337	0
14	87	%s na9iye	31337	0
14	88	%s mousajale sabi9ane houna.	31337	0
14	89	Ismou al 9anate ghayrou sa7i7e.	31337	0
14	90	%s Soujilate	31337	0
14	91	Izalatou %s min lai7ate a soukoute/tajahoule	31337	0
14	92	Ghayrou 9adire 3ala ijadi %s fi lai7ati soukoute/tajahoule	31337	0
14	93	La youmkino izalate tadakhoule lahou moustawake awe aktare.	31337	0
14	94	La youmkino izalate tadakhoulaka fi 9anate tasta3miloha.	31337	0
14	95	Tadakhoulou al moussta3mile %s ounzila ila %s	31337	0
14	96	Tari9atou INVISIBLE li khiyaratikoume al khassa al ane ON.	31337	0
14	97	Tari9atou INVISIBLE li khiyaratikoume al khassa al ane off.	31337	0
14	98	%s li %s hia %s	31337	0
14	99	Kimatou %s yajibou ane takouna ON aw OFF	31337	0
14	100	Tahakoume USERFLAGS ghayrou sa7i7e! Al 9iyamou a sa7i7a hia NONE,OP,VOICE.	31337	0
14	101	Al USERFLAGS li %s houa %s	31337	0
14	102	9imatou  MASSDEOPPRO yajibou ana takouna bayne 0-7	31337	0
14	103	MASSDEOPPRO li %s ta7akama fihi ila %d	31337	0
14	104	9imatou  FLOODPRO yajibou ane takouna bayne 0-7	31337	0
14	105	FLOODPRO %s ta7akama fihi ila %d	31337	0
14	106	Al wasfou la yajibou ane yata3ada 80 7arfane.	31337	0
14	107	Wasaf %s ouzila.	31337	0
14	108	Wasf %s houa: %s	31337	0
14	109	Al URL la yajibou ana yata3ada 75 7arfane.	31337	0
14	110	Al URL li %s ouzila.	31337	0
14	111	Al URL li %s houa: %s	31337	0
14	112	Al KEYWORDS (Alkalimatou almiftahe) la yajibou ane tata3ada 80 7arfane.	31337	0
14	113	KEYWORDS li %s houme: %s	31337	0
14	114	loughato mousta3emala: %s.	31337	0
14	115	kHATA2: ikhtiyarou loughatine ghayre sa7iha.	31337	0
14	116	Al 9anate %s ghayrou mawjouda 3ala chabakati arabte!	31337	0
14	117	KHATA2: Ta7akoumou al 9anate ghayrou sa7i7e.	31337	0
14	118	Lai7atou al motajahaline:	31337	0
14	119	Lai7atou almoutajahaline farigha	31337	0
14	120	-- Nihayatou lai7ati al moutajahaline 	31337	0
14	121	Status interne du Service de Canaux CMaster:	31337	0
14	130	almoutayatou moutafa9a: %i	31337	0
14	131	\002file khidma moundou (uptime):\002 %s	31337	0
14	132	Al 9anate %s li %d moussta3mile (%i Amiloune)	31337	0
14	133	Touroukou al 9anate: %s	31337	0
14	134	al flags almoussta3male : %s	31337	0
14	135	La youmkino tawa9ofe mousta3mile lahou moustawake awe aktare.	31337	0
14	136	Wa7adate alwa9te ghayre sa7i7a.	31337	0
14	137	Moudatou al i9afe ghayre sa7i7a.	31337	0
14	138	i9afe %s oulghiya.	31337	0
14	139	%s mouwa9afe sabi9ane 3ala %s	31337	0
14	140	al tawa9ofe li %s sa ya3male fi %s	31337	0
14	141	KHATA2: al topic la yajibou ane yata3ada 145 7arfane.	31337	0
14	142	tadakhoulouke ghayre kafi li izalati al ban %s min la2i7ate al mou3tayate li %s	31337	0
14	143	7adfou %i ban(s) alati ta7tawi 3alayhe %s	31337	0
14	144	Izalatou tadakhoulouke al moua9ate fi al mousstawa %i fi 9anate %s	31337	0
14	145	La tadhare 9ade da3amata tadakhoulake ila %s, hale satantahi?	31337	0
14	146	%s lame youwa9afe 3ala %s	31337	0
14	147	%s huwa Opérateur IRC	31337	0
14	148	%s ghayre mou3arafe.	31337	0
14	149	%s mou3arafe 3abra userid %s%s	31337	0
14	150	%s moumatile rassmi li CService%s wa mou3arafe 3abra userid %s	31337	0
14	151	%s huwa moudire rassmi li CService%s wa mou3arafe 3abra userid %s	31337	0
14	152	%s huwa moubarmije rassmi li CService%s wa mou3arafe 3abra userid %s	31337	0
14	153	LA youjadou ayo %i doukhoule 7adire  [%s]	31337	0
14	154	Min fadlike 7adide talabake.	31337	0
14	155	La youjado doukhoule 7adire li talabouke. [%s]	31337	0
14	156	%s: Touroukou al 9anate 9ade ouzila.	31337	0
14	158	khiyare ghayrou sa7ihe.	31337	0
14	159	%s Undernet Service Bot Rassmi .	31337	0
14	160	%s is an Official Coder-Com Representative%s wa mou3arafe bi %s	31337	0
14	161	%s is an Official Coder-Com Contributer%s wa mou3arafe bi %s	31337	0
14	162	%s is an Official Coder-Com Developer%s wa mou3arafe bi %s	31337	0
14	163	%s is an Official Coder-Com Senior%s wa mou3arafe bi %s	31337	0
14	164	 wa IRC operator	31337	0
14	165	Tamate idafato ban %s li %s fi darajati %i	31337	0
14	166	%s: Lai7ato l ban farigha .	31337	0
14	167	Ana sabi9ane fi hadihi al 9anate!	31337	0
14	168	hada al amre moukhassasse li IRC Operators	31337	0
14	169	Ana lastou op fi %s	31337	0
14	170	%s ila %i da9i9a	31337	0
\.

-- Hebrew	 language definition.
-- 22/04/02 - Revital Abudi <revital@netart.co.il>.
-- 22/04/02 - Shaun Okeefe <simba@wildstar.net>.

COPY "translations" FROM stdin;
15	1	%sîöèòøéí, äğê îæåää ëáø áîòøëú áëéğåé  	31337	0
15	2	%säàéîåú òáø áäöìçä ë  	31337	0
15	3	âéùúê àéğğä îñôé÷ä òì îğú ìäùúîù áô÷åãä æå	31337	0
15	4	øé÷ %sîöèòøéí, òøåõ  	31337	0
15	5	ìà ğîöà áàéæåø %säîùúîù  	31337	0
15	6	%s àéğğå ğåëç áòøåõ %s äîùúîù	31337	0
15	7	ìà øùåí %s òøåõ 	31337	0
15	8	%s á (%s) %s ÷éáìú àåô î	31337	0
15	9	%s á (%s) %s ÷éáìú ÷åì î	31337	0
15	10	%s àéğê  áòøåõ	31337	0
15	11	%s ëáø àåô á %s	31337	0
15	12	%s ëáø ÷åì á %s	31337	0
15	13	äåøéã ìê àú äàåô %s (%s) on %s	31337	0
15	14	äåøéã ìê àú ä÷åì %s (%s) on %s	31337	0
15	15	%s àéğå  àåô á: %s	31337	0
15	16	%s àéğå  ÷åì á: %s	31337	0
15	17	àéğå àåùø %säîùúîù   	31337	0
15	18	àğé ìà ğîöà áòøåõ !	31337	0
15	19	1 øîú äáàï ìà çå÷éú,äèååç äåà áéï -%i	31337	0
15	20	îùê æîï äáàï ìà çå÷é, äæîï äîéøáé äåà %d ùòåú.	31337	0
15	21	.ñéáú äáàï ìà éëåìä ìäéåú îòáø ì 128 úååéí	31337	0
15	22	.áàï æä ëáø ğîöà áøùéîä	31337	0
15	23	%s îëåñä ò"é äáàï %s äáàï	31337	0
15	24	äøîä äîéğéîìéú îæòøéú.	31337	0
15	25	äøîä äî÷ñéîìéú îéøáéú.	31337	0
15	26	%s %s :âéùä %sùí îùúîù: 	31337	0
15	27	%s :îöá àåèåîèé %sòøåõ: 	31337	0
15	28	%s :ìôğé %s :ùåğä ìàçøåğä 	31337	0
15	29	%s òã  %i äâéùä îåùòú áøîä ùì	31337	0
15	30	%sğøàä ìàçøåğä ìôğé:	31337	0
15	31	éù éåúø î %d òøëéí îúàéîéí.	31337	0
15	32	äâáì àú á÷ùúê.	31337	0
15	33	.ñåó øùéîú äâéùåú	31337	0
15	34	àéï äúàîåú !	31337	0
15	35	ìà ğéúï ìäåñéó îùúîù áòì âéùä ùååä àå âáåää îæå ùìê.	31337	0
15	36	øîú äâéùä ìà çå÷éú.	31337	0
15	37	%s äåñó ì %s òí âéùä áøîä ùì %i 	31337	0
15	38	%s òí øîú âéùä ùì  %s  äåñó ëáø ì %i	31337	0
15	39	îùäå ùâåé %s	31337	0
15	40	%s: ñåó øùéîú äáàğéí	31337	0
15	41	.ìà ğéúï ìøàåú àú ôøèé äîùúîù (áìúé ğøàä)	31337	0
15	42	%s îéãò òì äîùúîù  (%i)	31337	0
15	43	%s :ëøâò îæåää ãøê	31337	0
15	44	%s :ëúåáú	31337	0
15	45	%i :ùôä	31337	0
15	46	%s òøåöéí:	31337	0
15	47	Input Flood Points: %i	31337	0
15	48	Ouput Flood (Bytes): %i	31337	0
15	49	%s øùåí ò"é	31337	0
15	50	%s - ğøàä ìàçøåğä ìôğé %s	31337	0
15	51	Desc: %s	31337	0
15	52	úöéó àåúé? àğé ìà îúëååï ìä÷ùéá ìê éåúø.	31337	0
15	53	àğé çåùá ùùìçúé ìê éåúø îéãé ğúåğéí, àğé àúòìí îîê ìæîï îä.	31337	0
15	54	ô÷åãä ìà îåùìîú.	31337	0
15	55	 /msg %s@%s òìéê ìáöò %s ëãé ìäùúîù á	31337	0
15	56	äğê öøéê ìäæãäåú  òì îğú ìäùúîù áô÷åãä æå.	31337	0
15	57	îåùòä ë"é ääğäìä %s äòøåõ	31337	0
15	58	%säâéùä ùìê îåùòú á 	31337	0
15	59	%s äåâãø á NOOP flag  	31337	0
15	60	%s äåâãø á STRICTOP flag 	31337	0
15	61	àğùéí %iäåøãú àåô ìéåúø î  	31337	0
15	62	%s úçáéø:	31337	0
15	63	%s áòøåõ %i äòìä àú âéùúê áàåôï æîğé ì	31337	0
15	64	øùåí %s	31337	0
15	65	àéğå øùåí %s	31337	0
15	66	éòøéê æàú %s àéğğé çåùá ù	31337	0
15	67	\002*** øùéîú äáàğéí ùì òøåõ %s ***\002	31337	0
15	68	%s %s  %iøîä 	31337	0
15	69	%s äåñó ò"é (%s)	31337	0
15	70	%sîàæ: 	31337	0
15	71	%s òã:	31337	0
15	72	\002*** END ***\002	31337	0
15	73	àéğå îåëø %s äîùúîù 	31337	0
15	74	.àéğê øùàé ìäùúîù áé éåúø	31337	0
15	75	%s àéğå îåôéò ëáòì âéùä á %s	31337	0
15	76	.àéğê éëåì ìùğåú øîú âéùä ìøîä ùååä àå âáåää ìæå ùìê	31337	0
15	77	ìà ğéúï ìúú âéùä ùååä àå âáåää îæå ùìê.	31337	0
15	78	%s ìîùúîù %s áòøåõ %i ùåğúä øîú äâéùä ì	31337	0
15	79	%s á %sáåöò, îöá àåô àåèåîèé ì  	31337	0
15	80	%s á %s áåöò, îöá ôìåñ àåèåîèé ì 	31337	0
15	81	%s áòøåõ %säåâãø îöá àåèåîèé ùì àó àçã 	31337	0
15	82	ìùéğåé NEWPASS <new password>äñéñîä àéğğä éëåìä ìäéåú äùí îùúîù ùìê àå äëéğåé äğåëçé, áöò: 	31337	0
15	83	äñéñîä ùåğúä áäöìçä.	31337	0
15	84	%s äåâãø á NOOP flag  	31337	0
15	85	(ìà àåùø %s å)   %s äåâãø á  STRICTOP flag 	31337	0
15	86	(àéğğå áòì âéùä îñôé÷ä %s) äåâãø á %s  STRICTOP flag 	31337	0
15	87	ğñâø  %s	31337	0
15	88	ëáø øùåí %s 	31337	0
15	89	ùí äòøåõ ùâåé.	31337	0
15	90	øùåí %s	31337	0
15	91	äåñø îäøùéîä äù÷èä %s	31337	0
15	92	àéğå ğîöà áøùéîä äù÷èä %s 	31337	0
15	93	àéğê éëåì ìäñéø îùúîù ùøîú äâéùä ùìå ùååä àå âáåää îùìê.	31337	0
15	94	àéğê éëåì ìäñéø àú òöîê îäòøåõ ùìê.	31337	0
15	95	%s îòøåõ %s äåñø äîùúîù	31337	0
15	96	äîöá äáìúé ğøàä ôòéì.	31337	0
15	97	äîöá äáìúé ğøàä àéğå ôòéì.	31337	0
15	98	%s äåà  %s ì %s 	31337	0
15	99	.öøéê ìäéåú ôòéì àå àéğå ôòéì %sòøê ùì 	31337	0
15	100	äâãøåú éåæøôìàâ ùâåéåú, äâãøåú ğëåğåú äï  NONE, OP, VOICE.	31337	0
15	101	%s ì %s éåæøôìàâ äåà	31337	0
15	102	.òøëéé îàñ-ãéàåô-ôøå öøéëéí ìäéåú áéï 7-0	31337	0
15	103	%s îåâãøéí ì %d îàñ-ãéàåô-ôøå	31337	0
15	104	òøëéé ôìåãôøå öøéëéí ìäéåú áéï 7-0	31337	0
15	105	%s îåâãøéí ì %d òøëéé ôìåãôøå	31337	0
15	106	äúéàåø àéğå éëåì ìäéåú îòáø ì 180 úååéí!	31337	0
15	107	äúéàåø ğîç÷ %s 	31337	0
15	108	%s äåà %s äúéàåø ì	31337	0
15	109	äëúåáú àéğä éëåìä ìäéåú îòáø ì 75 úååéí!	31337	0
15	110	ğîç÷ä %s äëúåáú ì	31337	0
15	111	%s äëúåáú ì %s	31337	0
15	112	îçøåæú äàåúéåú àéğğä éëåìä ìäéåú îòáø ì 80 úååéí!	31337	0
15	113	%s äï %s îéìåú äîôúç ì	31337	0
15	114	%s äâãøú äùôä äğáçøú äéà 	31337	0
15	115	ùâéàä: áçéøú äùôä àéğğä àôùøéú.	31337	0
15	116	òì äøùú %s ìà ğéúï ìîöåà àú äòøåõ 	31337	0
15	117	ùâéàä: äâãøåú äòøåõ ùâåéåú.	31337	0
15	118	øùéîú äúòìîåéåú:	31337	0
15	119	øùéîú ääúòìîåéåú øé÷ä.	31337	0
15	120	-- ñåó øùéîú ääúòìîåéåú	31337	0
15	121	CMaster Channel Services internal status:	31337	0
15	122	[     Channel Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
15	123	[        User Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
15	124	[Access Level Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
15	125	[         Ban Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
15	126	éãéòä àçøåğä ùäú÷áìä îäîùúîù: %i	31337	0
15	127	éãéòä àçøåğä ùäú÷áìä îäòøåõ: %i	31337	0
15	128	éãéòä àçøåğä ùäú÷áìä  òì øîä : %i	31337	0
15	129	äåãòä  àçøåğä ùäú÷áìä òì áàï : %i	31337	0
15	130	Custom data containers allocated: %i	31337	0
15	131	\002Uptime:\002 %s	31337	0
15	132	%s îùúîùéí áòøåõ %d àåôéí å %iéùğí	31337	0
15	133	%s äîöá äåà 	31337	0
15	134	Flags set: %s	31337	0
15	135	àéï àôùøåú ìäùòåú îùúîù áòì âéùä ùååä àå âáåää îæå ùìê.	31337	0
15	136	äæîï ùâåé.	31337	0
15	137	îùê æîï ääùòéä àéğå çå÷é.	31337	0
15	138	îáåèìú %s ääùòéä ùì	31337	0
15	139	%s ëáø îåùòä á %s	31337	0
15	140	%s úéâîø úåê %s ääùòéä ùì	31337	0
15	141	ùâéàä: äèåôé÷ àéğå éëåì ìäëéì éåúø î 145 úååéí.	31337	0
15	142	ğúåğéí %s î %s âéùúê àéğä îñôé÷ä òì îğú ìäñéø àú äáàï 	31337	0
15	143	áàğéí îúàéîéí %i %s äåñøå á	31337	0
15	144	%i áøîä ùì %s äåñøä âéùúê äæîğéú îòøåõ	31337	0
15	145	You don't appear to have a forced access in %s, perhaps it expired?	31337	0
15	146	%s àéğå îåùòä á %s	31337	0
15	147	IRC äåà îôòéì %s  	31337	0
15	148	àéğå îæåää %s 	31337	0
15	149	%s%s îæåää ë %s  	31337	0
15	150	%s øùîé, åîæåää ë CService Representative%s äåà %s	31337	0
15	151	%s øùîé, åîæåää ë CService Administrator%s äåà %s	31337	0
15	152	%s øùîé, åîæåää ë CService Developer%s äåà %s	31337	0
15	153	òøëéí %i éåúø î [%s]éù ì   	31337	0
15	154	úäéä éåúø ñôöéôé áçéôåù	31337	0
15	155	[%s] ìà äú÷áìå òøëéí îúàéîéí ì	31337	0
15	156	îöáé äòøåõ äğîç÷å :%s 	31337	0
15	158	äàôùøåú àéğä çå÷éú.	31337	0
15	159	äåà áåè øùîé ùì àğãøğè %s 	31337	0
15	160	%s øùîé, åîæåää ë Coder-Com Representative%s äåà %s	31337	0
15	161	%s øùîé, åîæåää ë Coder-Com Contributer%s  äåà %s	31337	0
15	162	%s øùîé, åîæåää ë Coder-Com Developer%s   äåà %s	31337	0
15	163	%s øùîé, åîæåää ë Coder-Com Senior%s    äåà %s	31337	0
15	164	IRC åîôòéì	31337	0
15	165	%s äáàï %s áòøåõ %i ğåñó áøîä ùì	31337	0
15	166	øùéîú äáàğéí øé÷ä :%s 	31337	0
15	167	àğé ëáø ğîöà áòøåõ !	31337	0
15	168	ø÷ îôòìé äùøú éëåìéí ìäùúîù áô÷åãä æå.	31337	0
15	169	%s àéï ìé àåô á	31337	0
15	170	%s ã÷åú ì %i	31337	0
\.

-- Macedonian language definition.
-- 01/04/03 - Marjan Lazarevski <acid@iunc.net>.

COPY "translations" FROM stdin;
16	1	Izvini, No veke si logiran kako %s	31337	0
16	2	USPESHNO LOGIRAN kako %s	31337	0
16	3	Izvini, nemash dovolno pristap za taa komanda	31337	0
16	4	Izvini, kanalot %s e prazen	31337	0
16	5	Ne go zabelezuvam %s nikade	31337	0
16	6	Nemozam da go lociram %s na kanal %s	31337	0
16	7	Kanalot %s ne e registriran	31337	0
16	8	Dobi op od %s (%s) na %s	31337	0
16	9	Dobi glas od %s (%s) na %s	31337	0
16	10	%s: ne si na toj kanal	31337	0
16	11	%s veke ima op na %s	31337	0
16	12	%s veke ima glas na %s	31337	0
16	13	Odzemen ti e opot od %s (%s) na %s	31337	0
16	14	Odzemen ti e glasot od %s (%s) na %s	31337	0
16	15	%s nema op na %s	31337	0
16	16	%s nema glas na %s	31337	0
16	17	NEUSPESHNO LOGIRANJE kako %s (Pogreshna Lozinka)	31337	0
16	18	Ne sum na toj kanal!	31337	0
16	19	Pogreshen ban dostrel. Dozvoleniot dostrel e 1-%i.	31337	0
16	20	Pogreshno vremetraenje na banot. Vashiot ban moze da bide najmnogu %d chasa.	31337	0
16	21	Ban porakata nemoze da bide poveke od 128 bukvi	31337	0
16	22	Odbraniot ban e veke na mojata lista!	31337	0
16	23	Banot %s e veke pokrien so %s	31337	0
16	24	Pogreshno minimalno nivo.	31337	0
16	25	Pogreshno maksimalno nivo.	31337	0
16	26	KORISNIK: %s PRISTAP: %s %s	31337	0
16	27	KANAL: %s -- AVTOMOD: %s	31337	0
16	28	POSLEDNO MODIFICIRAN: %s (%s pred)	31337	0
16	29	** SUSPENDIRAN ** - Istekuva za %s (Nivo %i)	31337	0
16	30	POSLEDNO VIDEN: %s pred.	31337	0
16	31	Ima poveke od %d isti.	31337	0
16	32	Ve molam namalete go izborot.	31337	0
16	33	Kraj na pristap listata	31337	0
16	34	Nema Takov!	31337	0
16	35	Nemozesh da dodadesh korisnik so ednakov ili pogolem pristap od tvojot.	31337	0
16	36	Pogreshno pristap nivo.	31337	0
16	37	%s e veke dodaden na %s so pristap nivo %i.	31337	0
16	38	Dodaden korisnikot %s na %s so pristap nivo %i	31337	0
16	39	Neshto trgna naopaku: %s	31337	0
16	40	%s: Kraj na ban listata	31337	0
16	41	Detalite nemozat da se vidat (Nevidliv)	31337	0
16	42	Informacija za: %s (%i)	31337	0
16	43	Momentalno logiran preku: %s	31337	0
16	44	WEB Strana: %s	31337	0
16	45	Jazik: %i	31337	0
16	46	Kanali: %s	31337	0
16	47	Vlezni povtoruvacki poeni: %i	31337	0
16	48	Izlezno povtoruvanje (Bajti): %i	31337	0
16	49	%s e registriran od:	31337	0
16	50	%s - posledno viden: %s pred	31337	0
16	51	Opis: %s	31337	0
16	52	Ke me poplavuvash od zborovi a? E ne te slusham veke!	31337	0
16	53	Mislam deka ti prativ malku poveke podatoci, odsega momentalno si ignoriran.	31337	0
16	54	Nekompletna komanda	31337	0
16	55	Za upotreba na %s, morash da pishesh /msg %s@%s	31337	0
16	56	Izvini, Mora da si logiran za da ja upotrebish taa komanda.	31337	0
16	57	Kanalot %s e suspendiran od cservice administrator.	31337	0
16	58	Vashiot pristap na %s e suspendiran.	31337	0
16	59	BEZOP flagot e postaven na %s	31337	0
16	60	STRICTOP flagot e postaven na %s	31337	0
16	61	Shtotuku odzede op na poveke od %i luge	31337	0
16	62	UPOTREBA: %s	31337	0
16	63	Momentalno go zgolemiv tvojot pristap na kanal %s na %i	31337	0
16	64	%s e registriran.	31337	0
16	65	%s ne e registriran.	31337	0
16	66	Mislam deka %s ne bi go sakal toa.	31337	0
16	67	\002*** Ban Lista za kanal %s ***\002	31337	0
16	68	%s %s Nivo: %i	31337	0
16	69	DODADEN OD: %s (%s)	31337	0
16	70	NA: %s	31337	0
16	71	ISTEKUVA: %s	31337	0
16	72	\002*** KRAJ ***\002	31337	0
16	73	Neznam koj %s e.	31337	0
16	74	Vekje ne se logiran na mene.	31337	0
16	75	%s nema pristap na %s.	31337	0
16	76	Nemozesh da modificirash korisnik so ednakov ili pogolem pristap od tvojot.	31337	0
16	77	Nemozesh da dadesh pogolem ili ednakov pristap na tvojot.	31337	0
16	78	Modificirano e pristap nivoto na %s na kanal %s na %i	31337	0
16	79	Postaven AVTOMOD za OP na %s na kanal %s	31337	0
16	80	Postaven AVTOMOD za GLAS na %s na kanal %s	31337	0
16	81	Nema AVTOMODE za %s na kanal %s	31337	0
16	82	Vashata lozinka nemoze da bide vasheto korisnicko ime ili nadimakot - upotreba: NEWPASS <nova lozinka>	31337	0
16	83	Lozinkata uspeshno smeneta.	31337	0
16	84	BEZOP flagot e postaven na %s	31337	0
16	85	BEZOP flagot e postaven na %s (a %s ne e logiran)	31337	0
16	86	BEZOP flagot e postaven na %s (a %s nema dovolen pristap)	31337	0
16	87	Izbrishan kanal %s	31337	0
16	88	%s e veke registriran so mene.	31337	0
16	89	Imeto na kanalot e invalidno.	31337	0
16	90	Kanalot %s e sega registriran	31337	0
16	91	Trgant %s od mojata molk lista	31337	0
16	92	Nemozam da go najdam %s vo mojata molk lista	31337	0
16	93	Nemozesh da odzemesh pristap na korisnik so ednakov ili pogolem pristap od tvojot	31337	0
16	94	Nemozesh da si go odzemesh pristapot od sopstvieniot kanal	31337	0
16	95	Odzemen pristap na korisnikot %s od %s	31337	0
16	96	Vashiot NEVIDLIV flag e sega VKLICEN.	31337	0
16	97	Vashiot NEVIDLIV flag e sega ISKLUCEN.	31337	0
16	98	%s za %s e %s	31337	0
16	99	podesuvanjeto na %s mora da bide VKLUCENO(ON) ili ISKLUCENO (OFF)	31337	0
16	100	Pogreshni KORISNICKI PODESUVANJA. Tocnite podesuvanja se NONE, OP, VOICE.	31337	0
16	101	KORISNICKITE PODESUVANJA za %s se %s	31337	0
16	102	podesuvanjeto za MASSDEOPPRO mora da bide 0-7	31337	0
16	103	MASSDEOPPRO za %s e postaven na %d	31337	0
16	104	podesuvanjeto za FLOODPRO mora da bide 0-7	31337	0
16	105	FLOODPRO za %s e postaven na %d	31337	0
16	106	OPISOT moze da bide maksimum 80 bukvi !	31337	0
16	107	OPISOT za %s e iscisten.	31337	0
16	108	OPISOT za %s e: %s	31337	0
16	109	WEB Stranata moze da bide maksimum 75 bukvi!	31337	0
16	110	WEB Stranata na %s e iscistena.	31337	0
16	111	WEB Stranata na %s e: %s	31337	0
16	112	Podesuvanjeto na klucnite zborovi nemoze da bide poveke od 80 bukvi!	31337	0
16	113	KLUCNITE ZBOROVI za %s se: %s	31337	0
16	114	Jazikot e sega postaven na %s.	31337	0
16	115	GRESHKA: Pogreshna selekcija na jazikot.	31337	0
16	116	Nemozam da go lociram kanalot %s na mrezata!	31337	0
16	117	GRESHKA: Pogreshno podesuvanje za kanalot.	31337	0
16	118	Ignore lista:	31337	0
16	119	Ignore listata e prazna	31337	0
16	120	-- Kraj na Ignore Listata	31337	0
16	121	CMaster status na servisot na kanali:	31337	0
16	122	[     Rekordna Statistika Na Kanalot] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
16	123	[        Rekordna Statistika Na Korisnici] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
16	124	[Rekordna Statistika Na Pristap] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
16	125	[         Rekordna Statistika Na Ban] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
16	126	Posledno Primeno Korsnicko Izvestuvanje: %i	31337	0
16	127	Posledno Primeno Izvestuvanje Za Kanalot: %i	31337	0
16	128	Posledno Primeno Izvestuvanje Za Nivo: %i	31337	0
16	129	Posledno Primeno Izvestuvanje Za Ban: %i	31337	0
16	130	Custom data containers allocated: %i	31337	0
16	131	\002Uptime:\002 %s	31337	0
16	132	Kanalot %s ima %d korisnici (%i operatori)	31337	0
16	133	Modot e: %s	31337	0
16	134	Postaveni flagovi: %s	31337	0
16	135	Nemozesh da suspendirash korisnik so ednakov ili pogolem pristap od tvojot.	31337	0
16	136	pogreshno vreme	31337	0
16	137	Pogreshno vremetraenje na suspenzija.	31337	0
16	138	SUSPENZIJATA na %s ponishtena	31337	0
16	139	%s e veke suspendiran na %s	31337	0
16	140	SUSPENZIJATA na %s ke istece za %s	31337	0
16	141	GRESHKA: Naslovot nemoze da bide pogolem od 145 bukvi	31337	0
16	142	Nemash dovolno pristap za da go trgnesh banot %s od bazata na %s	31337	0
16	143	Trgnati %i banovi shto go pokrivaat %s	31337	0
16	144	Odzemen e vashiot privremen pristap od %i na kanal %s	31337	0
16	145	Izgleda deka nemash prinuden pristap na %s, mozebi istekol?	31337	0
16	146	%s ne e suspendiran na %s	31337	0
16	147	%s e IRC operator	31337	0
16	148	%s NE e logiran.	31337	0
16	149	%s e logiran kako %s%s	31337	0
16	150	%s e Oficijalen CService Pretstavnik%s i logiran kako %s	31337	0
16	151	%s e Oficijalen CService Administrator%s i logiran kako %s	31337	0
16	152	%s e Oficijalen CService Developer%s i logiran kako %s	31337	0
16	153	Ima povekje od %i vlezovi koi se sovpagaat. [%s]	31337	0
16	154	Ve molam odredete na baranata maska	31337	0
16	155	Nema takvi koi se sovpagaat [%s]	31337	0
16	156	%s: Modovite na kanalot se iscisteni.	31337	0
16	158	Pogreshna opcija.	31337	0
16	159	%s e Oficijalen Undernet Servisen Bot.	31337	0
16	160	%s e Oficijalen Coder-Com Pretstavnik%s i logiran kako %s	31337	0
16	161	%s e Oficijalen Coder-Com Pridonesnik%s i logiran kako %s	31337	0
16	162	%s e Oficijalen Coder-Com Developer%s i logiran kako %s	31337	0
16	163	%s e Oficijalen Coder-Com Senior%s i logiran kako %s	31337	0
16	164	 i IRC operator	31337	0
16	165	Dodaden ban %s na %s so nivo %i	31337	0
16	166	%s: ban listata e prazna.	31337	0
16	167	Veke sum vo toj kanal!	31337	0
16	168	Taa komanda e rezervirana za IRC Operatori	31337	0
16	169	Nemam op na %s	31337	0
16	170	%s za %i minuti	31337	0
\.
