-- Language Definition Table for CMaster Undernet channel services.
-- English language definition.
-- 27/12/00 - Greg Sikorski <gte@atomicrevs.demon.co.uk>.
-- 01/03/01 - Daniel Simard <svr@undernet.org>.

DELETE FROM languages;
COPY "languages" FROM stdin;
1	English	31337
2	French	31337
3	Danish	31337
4	Dutch	31337
5	German	31337
6	Portuguese	31337
7	Italian	31337
8	Romanian	31337
\.

DELETE FROM translations;
COPY "translations" FROM stdin;
1	1	Sorry, You are already authenticated as %s	31337
1	2	AUTHENTICATION SUCCESSFUL as %s	31337
1	3	Sorry, you have insufficient access to perform that command	31337
1	4	Sorry, the channel %s is empty	31337
1	5	I don't see %s anywhere	31337
1	6	I can't find %s on channel %s	31337
1	7	The channel %s doesn't appear to be registered	31337
1	8	You're opped by %s (%s)	31337
1	9	You're voiced by %s (%s)	31337
1	10	%s: You are not in that channel	31337
1	11	%s is already opped in %s	31337
1	12	%s is already voiced in %s	31337
1	13	You're deopped by %s (%s)	31337
1	14	You're devoiced by %s (%s)	31337
1	15	%s is not opped in %s	31337
1	16	%s is not voiced in %s	31337
1	17	AUTHENTICATION FAILED as %s (Invalid Password)	31337
1	18	I'm not in that channel!	31337
1	19	Invalid banlevel range. Valid range is 1-%i.	31337
1	20	Invalid ban duration. Your ban duration can be a maximum of 336 hours.	31337
1	21	Ban reason cannot exceed 128 chars	31337
1	22	Specified ban is already in my banlist!	31337
1	23	The ban %s is already covered by %s	31337
1	24	Invalid minimum level.	31337
1	25	Invalid maximum level.	31337
1	26	USER: %s ACCESS: %s %s	31337
1	27	CHANNEL: %s -- AUTOMODE: %s	31337
1	28	LAST MODIFIED: %s (%s ago)	31337
1	29	** SUSPENDED ** - Expires in %s	31337
1	30	LAST SEEN: %s ago.	31337
1	31	There are more than 15 matching entries.	31337
1	32	Please restrict your query.	31337
1	33	End of access list	31337
1	34	No Match!	31337
1	35	Cannot add a user with equal or higher access than your own.	31337
1	36	Invalid access level.	31337
1	37	%s is already added to %s with access level %i.	31337
1	38	Added user %s to %s with access level %i	31337
1	39	Something went wrong: %s	31337
1	40	%s: End of ban list	31337
1	41	Unable to view user details (Invisible)	31337
1	42	Information about: %s (%i)	31337
1	43	Currently logged on via: %s	31337
1	44	URL: %s	31337
1	45	Language: %i	31337
1	46	Channels: %s	31337
1	47	Input Flood Points: %i	31337
1	48	Ouput Flood (Bytes): %i	31337
1	49	%s is registered by:	31337
1	50	%s - last seen: %s ago	31337
1	51	Desc: %s	31337
1	52	Flood me will you? I'm not going to listen to you anymore	31337
1	53	I think I've sent you a little too much data, I'm going to ignore you for a while.	31337
1	54	Incomplete command	31337
1	55	To use %s, you must /msg %s@%s	31337
1	56	Sorry, You must be logged in to use this command.	31337
1	57	The channel %s has been suspended by a cservice administrator.	31337
1	58	Your access on %s has been suspended.	31337
1	59	The NOOP flag is set on %s	31337
1	60	The STRICTOP flag is set on %s	31337
1	61	You just deopped more than %i people	31337
1	62	SYNTAX: %s	31337
1	63	Temporarily increased your access on channel %s to %i	31337
1	64	%s is registered.	31337
1	65	%s is not registered.	31337
1	66	I don't think %s would appreciate that.	31337
1	67	\002*** Ban List for channel %s ***\002	31337
1	68	%s %s Level: %i	31337
1	69	ADDED BY: %s (%s)	31337
1	70	SINCE: %i	31337
1	71	EXP: %s	31337
1	72	\002*** END ***\002	31337
1	73	Sorry, I don't know who %s is.	31337
1	74	Sorry, you are not authorized with me anymore.	31337
1	75	doesn't appear to have access in %s.	31337
1	76	Cannot modify a user with equal or higher access than your own.	31337
1	77	Cannot give a user higher or equal access to your own.	31337
1	78	Modified %s's access level on channel %s to %i	31337
1	79	Set AUTOMODE to OP for %s on channel %s	31337
1	80	Set AUTOMODE to VOICE for %s on channel %s	31337
1	81	Set AUTOMODE to NONE for %s on channel %s	31337	
1	82	Your passphrase cannot be your username or current nick - syntax is: NEWPASS <new passphrase>	31337
1	83	Password successfully changed.	31337
1	84	The NOOP flag is set on %s	31337
1	85	The STRICTOP flag is set on %s (and %s isn't authenticated)	31337
1	86	The STRICTOP flag is set on %s (and %s has insufficient access)	31337
1	87	Purged channel %s	31337
1	88	%s is already registered with me.	31337
1	89	Invalid channel name.	31337
1	90	Registered channel %s	31337
1	91	Removed %s from my silence list	31337
1	92	Couldn't find %s in my silence list	31337
1	93	Cannot remove a user with equal or higher access than your own	31337
1	94	You can't remove yourself from a channel you own	31337
1	95	Removed user %s from %s	31337
1	96	Your INVISIBLE setting is now ON.	31337
1	97	Your INVISIBLE setting is now OFF.	31337
1	98	%s for %s is %s	31337
1	99	value of %s must be ON or OFF	31337
1	100	Invalid USERFLAGS setting. Correct values are 0, 1, 2.	31337
1	101	USERFLAGS for %s is %i	31337
1	102	value of MASSDEOPPRO has to be 0-7	31337
1	103	MASSDEOPPRO for %s is set to %d	31337
1	104	value of FLOODPRO has to be 0-7	31337
1	105	FLOODPRO for %s is set to %d	31337
1	106	The DESCRIPTION can be a maximum of 80 chars!	31337
1	107	DESCRIPTION for %s is cleared.	31337
1	108	DESCRIPTION for %s is: %s	31337
1	109	The URL can be a maximum of 75 chars!	31337
1	110	URL for %s is cleared.	31337
1	111	URL for %s is: %s	31337
1	112	The string of keywords cannot exceed 80 chars!	31337
1	113	KEYWORDS for %s are: %s	31337
1	114	Language is set to %s.	31337
1	115	ERROR: Invalid language selection.	31337
1	116	Can't locate channel %s on the network!	31337
1	117	ERROR: Invalid channel setting.	31337
1	118	Ignore list:	31337
1	119	Ignore list is empty	31337
1	120	-- End of Ignore List	31337
1	121	CMaster Channel Services internal status:	31337
1	122	[     Channel Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337
1	123	[        User Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337
1	124	[Access Level Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337
1	125	[         Ban Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337
1	126	Last recieved User NOTIFY: %i	31337
1	127	Last recieved Channel NOTIFY: %i	31337
1	128	Last recieved Level NOTIFY: %i	31337
1	129	Last recieved Ban NOTIFY: %i	31337
1	130	Custom data containers allocated: %i	31337
1	131	\002Uptime:\002 %s	31337
1	132	Channel %s has %d users (%i operators)	31337
1	133	Mode is: %s	31337
1	134	Flags set: %s	31337
1	135	Cannot suspend a user with equal or higher access than your own.	31337
1	136	bogus time units	31337
1	137	Invalid suspend duration.	31337
1	138	SUSPENSION for %s is cancelled	31337
1	139	%s is already suspended on %s	31337
1	140	SUSPENSION for %s will expire in %s	31337
1	141	ERROR: Topic cannot exceed 145 chars	31337
1	142	You have insufficient access to remove the ban %s from %s's database	31337
1	143	Removed %i bans that matched %s	31337
1	144	Removed your temporary access of %i from channel %s	31337
1	145	You don't appear to have a forced access in %s, perhaps it expired?	31337
1	146	%s isn't suspended on %s	31337
1	147	%s is an IRC operator	31337
1	148	%s is NOT logged in.	31337
1	149	%s is logged in as %s%s	31337
1	150	%s is an Official CService Representative%s and logged in as %s	31337
1	151	%s is an Official CService Administrator%s and logged in as %s	31337
1	152	%s is an Official CService Developer%s and logged in as %s	31337
1	153	There are more than %i entries matching [%s]	31337
1	154	Please restrict your search mask	31337
1	155	No matching entries for [%s]	31337
1	156	%s: Cleared channel modes.	31337
1	158	Invalid option.	31337
1	159	%s is an Official Undernet Service Bot.	31337
1	160	%s is an Official Coder-Com Representative%s and logged in as %s	31337
1	161	%s is an Official Coder-Com Contributer%s and logged in as %s	31337
1	162	%s is an Official Coder-Com Developer%s and logged in as %s	31337
1	163	%s is an Official Coder-Com Senior%s and logged in as %s	31337
1	164	 and an IRC operator	31337
1	165	Added ban %s to %s at level %i	31337
1	166	%s: ban list is empty.		31337
1	167	I'm already in that channel!	31337
1	168	This command is reserved to IRC Operators	31337
1	169	I'm not opped on %s	31337
1	170	%s for %i minutes	31337
\.

-- French language definition.
-- 29/12/00 - Daniel Simard <svr@undernet.org>.
-- 01/03/01 - Daniel Simard <svr@undernet.org>.

COPY "translations" FROM stdin;
2	1	Désolé. Vous êtes déjà authentifié avec le nick %s	31337
2	2	AUTHENTICATION RÉUSSIE pour %s	31337
2	3	Désolé. Votre accès est insuffisant pour utiliser cette commande	31337
2	4	Désolé. Le canal %s est vide	31337
2	5	Je suis incapable de trouver %s	31337
2	6	Je ne vois pas %s sur le canal %s	31337
2	7	Le canal %s ne semble pas être enregistré	31337
2	8	Vous avez été op par %s (%s)	31337
2	9	Vous avez été voice par %s (%s)	31337
2	10	%s: Vous n'êtes pas dans ce canal	31337
2	11	%s est déjà op sur %s	31337
2	12	%s est déjà voice sur %s	31337
2	13	Vous avez été deop par %s (%s)	31337
2	14	Vous avez été devoice par %s (%s)	31337
2	15	%s n'est pas op sur %s	31337
2	16	%s n'est pas voice sur %s	31337
2	17	AUTHENTIFICATION REFUSÉE pour %s (Mot-de-passe Invalide)	31337
2	18	Je ne suis pas dans ce canal!!	31337
2	19	Niveau de ban invalide. Niveau valide: entre 1-%i.	31337
2	20	Duration du ban invalide. La duration maximale est de 336 heures.	31337
2	21	La raison d'un ban ne peut dépasser 128 caractères.	31337
2	22	Ce ban est déjà dans ma liste!	31337
2	23	Le ban %s est dejà affecté par %s	31337
2	24	Niveau minimum invalide.	31337
2	25	Niveau maximum invalide.	31337
2	26	USAGER: %s NIVEAU: %s %s	31337
2	27	CANAL: %s -- AUTOMODE: %s	31337
2	28	DERNIÈRE MODIFICATION: %s (%s avant)	31337
2	29	** SUSPENDU ** - Expire dans %s	31337
2	30	DERNIÈRE FOIS VU: %s avant.	31337
2	31	Il y a plus de 15 requêtes trouvées.	31337
2	32	SVP restreindre votre requête	31337
2	33	Fin de la liste des accès	31337
2	34	Aucune correspondace trouvée	31337
2	35	Vous ne pouvez additionner un usager au même niveau que vous ou plus élevé. 	31337
2	36	Niveau d'accès invalide	31337
2	37	%s a déjà été ajouté sur %s au niveau %i.	31337
2	38	Addition de l'usager %s sur %s au niveau %i	31337
2	39	Quelque chose semble incorrecte: %s	31337
2	40	%s: fin de la liste de ban		31337
2	41	Impossible de visualiser les détails de l'usager (Invisible).	31337
2	42	Information à propos de: %s (%i)	31337
2	43	Présentement connecté via: %s	31337
2	44	URL: %s	31337
2	45	Langue: %i	31337
2	46	Canaux: %s	31337
2	47	Entrée Flood: %i	31337
2	48	Sortie Flood (Bytes): %i	31337
2	49	%s a été enregistré par:	31337
2	50	%s - vu pour la dernière fois: %s avant	31337
2	51	Desc: %s	31337
2	52	Floodez moi voulez-vous? Je ne vais plus vous répondre dorénavant.	31337
2	53	Je crois que je vous ai envoyé beaucoup trop d'informations. Je vais vous ignorer un peu.	31337
2	54	Commande incomplète	31337
2	55	Pour utiliser %s, Vous devez /msg %s@%s	31337
2	56	Désolé. Vous devez être authentifié pour utiliser cette commande.	31337
2	57	Le canal %s a été suspendu par un administrateur de cservice.	31337
2	58	Votre access sur %s a été suspendu.	31337
2	59	Le mode NOOP est actif sur %s	31337
2	60	Le mode STRICTOP est actif sur %s	31337
2	61	Vous venez de deop plus de %i personnes	31337
2	62	SYNTAXE: %s	31337
2	63	Temporairement authentifié sur %s au niveau %i	31337
2	64	%s est enregistré.	31337
2	65	%s n'est pas enregistré.	31337
2	66	Je ne crois pas que %s apprécierait ça.	31337
2	67	\002*** Liste de Ban pour le canal %s ***\002	31337
2	68	%s %s Niveau: %i	31337
2	69	AJOUTÉ PAR: %s (%s)	31337
2	70	DEPUIS: %i	31337
2	71	EXP: %s	31337
2	72	\002*** FIN ***\002	31337
2	73	Désolé, Je ne sais pas qui est %s.	31337
2	74	Désolé, Vous n'êtes plus authentifié.	31337
2	75	Aucune accès correspondant sur %s.	31337
2	76	Vous ne pouvez pas modifier un usager au même niveau ou supérieur à vous.	31337
2	77	Vous ne pouvez pas donner un accès à un usager au même niveau ou supérieur à vous.	31337
2	78	Modification du niveau de %s sur le canal %s à %i	31337
2	79	Activation du MODE Automatique (OP) pour %s sur le canal %s	31337
2	80	Activation du MODE Automatique (VOICE) pour %s sur le canal %s	31337
2	81	Désactivation des MODES Automatiques pour %s sur le canal %s	31337	
2	82	Votre mot de passe ne peut être votre nom d'usager ni votre nick - La Syntaxe est: NEWPASS <nouveau mot-de-passe>	31337
2	83	Votre mot de passe a été changé avec succès.	31337
2	84	Le mode NOOP est actif sur %s	31337
2	85	Le mode STRICTOP est actif sur %s (et %s n'est pas authentifié)	31337
2	86	Le mode STRICTOP est actif sur %s (et l'accès de %s est insuffisant)	31337
2	87	%s a été purgé	31337
2	88	%s est déjà enregistré avec moi.	31337
2	89	Nom de canal invalide.	31337
2	90	%s a été enregistré	31337
2	91	Enlevé %s de ma liste silence/ignore	31337
2	92	Incapable de trouver %s dans ma liste silence/ignore	31337
2	93	Vous ne pouvez pas effacer un accès donc le niveau est égal ou plus élevé que le votre.	31337
2	94	Vous ne pouvez pas effacer votre accès sur un canal que vous possédez.	31337
2	95	L'accès de l'usager %s a été effacé sur %s	31337
2	96	Le mode INVISIBLE pour vos options confidentiels est maintenant à ON.	31337
2	97	Le mode INVISIBLE pour vos options confidentiels est maintenant à OFF.	31337
2	98	%s pour %s est %s	31337
2	99	La valeur de %s doit être ON ou OFF	31337
2	100	Configuration USERFLAGS invalide! Les valeurs correctes sont 0, 1, 2.	31337
2	101	Le USERFLAGS de %s est %i	31337
2	102	La valeur de MASSDEOPPRO doit être entre 0-7	31337
2	103	MASSDEOPPRO de %s a été configuré à %d	31337
2	104	La valeur de FLOODPRO doit être entre 0-7	31337
2	105	FLOODPRO de %s a été configuré à %d	31337
2	106	La DESCRIPTION ne doit pas dépasser 80 caractères.	31337
2	107	La DESCRIPTION de %s a été éffacé.	31337
2	108	La DESCRIPTION de %s est: %s	31337
2	109	L'URL ne doit pas dépasser 75 caractères.	31337
2	110	L'URL de %s a été effacé.	31337
2	111	L'URL de %s est: %s	31337
2	112	Les KEYWORDS (MotsClés) ne doivent pas dépasser 80 caractères.	31337
2	113	KEYWORDS de %s sont: %s	31337
2	114	Langue activé: %s.	31337
2	115	ERREUR: Sélection d'une langue invalide.	31337
2	116	Le canal %s est introuvable sur le réseau!	31337
2	117	ERREUR: Configuration de canal invalide.	31337
2	118	Liste d'Ignoré(s):	31337
2	119	La liste d'ignore est vide	31337
2	120	-- Fin de la liste d'ignore 	31337
2	121	Status interne du Service de Canaux CMaster:	31337
2	122	[          Statistiques du Canal] \002Entrées en cache:\002 %i    \002Requêtes BaseD:\002 %i    \002# de fois Cache touché:\002 %i    \002Efficacité:\002 %.2f%%	31337
2	123	[           Statistiques Usagers] \002Entrées en cache:\002 %i    \002Requêtes BaseD:\002 %i    \002# de fois Cache touché:\002 %i    \002Efficacité:\002 %.2f%%	31337
2	124	[   Statistiques niveaux d'accès] \002Entrées en cache:\002 %i    \002Requêtes BaseD:\002 %i    \002# de fois Cache touché:\002 %i    \002Efficacité:\002 %.2f%%	31337
2	125	[          Statistiques des bans] \002Entrées en cache:\002 %i    \002Requêtes BaseD:\002 %i    \002# de fois Cache touché:\002 %i    \002Efficacité:\002 %.2f%%	31337
2	126	Dernière réception NOTIFY d'usager: %i	31337
2	127	Dernière réception NOTIFY de canal: %i	31337
2	128	Dernière réception NOTIFY de niveau: %i	31337
2	129	Dernière réception NOTIFY de ban: %i	31337
2	130	Données assignées: %i	31337
2	131	\002En fonction depuis (uptime):\002 %s	31337
2	132	Le canal %s a %d usager(s) (%i opérateurs)	31337
2	133	Le(s) Mode(s) du canal: %s	31337
2	134	Flags activés: %s	31337
2	135	Vous ne pouvez pas suspendre un usager au même niveau que vous ni supérieur.	31337
2	136	Unité de temps invalide.	31337
2	137	Durée de suspension invalide.	31337
2	138	La SUSPENSION pour %s a été annulé.	31337
2	139	%s est déjà suspendu sur %s	31337
2	140	La SUSPENSION pour %s va expirer dans %s	31337
2	141	ERREUR: Le topic ne peut dépasser 145 caractères.	31337
2	142	votre accès est insuffisant pour enlever le ban %s de la base de données de %s	31337
2	143	Supprimé %i ban(s) qui figuraient %s	31337
2	144	Supprimé votre accès temporaire de niveau %i du canal %s	31337
2	145	Vous ne semblez pas d'avoir forcer l'accès à %s, Serait-elle expirée?	31337
2	146	%s n'est pas suspendu sur %s	31337
2	147	%s est un Opérateur IRC	31337
2	148	%s n'est pas authentifié.	31337
2	149	%s est authentifié via le userid %s%s	31337
2	150	%s est un représentant Officiel de CService%s et authentificé via le userid %s	31337
2	151	%s est un Administrateur Officiel de CService%s et authentificé via le userid %s	31337
2	152	%s est un Programmeur Officiel de CService%s et authentificé via le userid %s	31337
2	153	Il y a plus de %i entrées figurants [%s]	31337
2	154	Veuillez svp restreindre votre requête.	31337
2	155	Aucune entrée figurante pour votre requête. [%s]	31337
2	156	%s: Les modes du canal on été effacés.	31337
2	158	Option invalide.	31337
2	159	%s est un Service Officiel du réseau Undernet.	31337
2	160	Addition du ban %s sur %s au niveau %i	31337
2	161	%s: la liste des bans est vide.		31337
2	162	Je suis déjà sur ce canal!	31337
\.

-- Danish language definition.
-- 28/12/2000 - David Henriksen <david@itwebnet.dk>.

COPY "translations" FROM stdin;
3	1	Beklager, Du er allerede autoriseret som %s	31337
3	2	AUTORISATION LYKKEDES som %s	31337
3	3	Beklager, du har utilstrækkelig adgang til at udføre den kommando	31337
3	4	Beklager, kanalen %s er tom	31337
3	5	Jeg kan ikke se %s nogen steder	31337
3	6	Jeg kan ikke finde %s på kanalen %s	31337
3	7	Kanelen %s ser ikke ud til at være registreret	31337
3	8	Du er op'et af %s (%s)	31337
3	9	Du er voice'et af %s (%s)	31337
\.

-- Dutch language definition.
-- 30/12/2000 - Sebastian Serruys <sebastian.serruys@pandora.be>.

COPY "translations" FROM stdin;
4	1	Sorry, Je bent reeds geautoriseerd als %s	31337
4	2	AUTORISATIE SUCCESVOL als %s	31337
4	3	Sorry, je hebt onvoldoende toegang om dat commando uit te voeren	31337
4	4	Sorry, het kanaal %s is leeg	31337
4	5	Ik zie nergens %s	31337
4	6	Ik kan %s niet vinden op het kanaal %s	31337
4	7	Het kanaal %s ziet er niet geregistreerd uit	31337
4	8	Je bent geopt door %s (%s)	31337
4	9	Je bent gevoiced door %s (%s)	31337
4	10	%s: Je bent niet in dat kanaal	31337
\.

-- German language definition.
-- 03/01/2001 - Alexander Maassen <outsider@key2peace.org>.

COPY "translations" FROM stdin;
5	1	Sorry, Du bist schon authorisiert als %s	31337
5	2	AUTHORISATION als %s ACCEPTIERT	31337
5	3	Sorry, du hast zu wenig access fr dieses commando	31337
5	4	Sorry, Channel %s ist lehr	31337
5	5	Ich kann %s nirgendwo sehen	31337
5	6	Ich kann %s nicht im Channel %s sehen	31337
5	7	Channel %s is nicht registriert	31337
5	8	%s (%s) hat dich geopt	31337
5	9	%s (%s) hat dich gevoiced	31337
5	10	%s: Du bist nicht im Channel	31337
5	11	%s wurde bereits von %s geopt	31337
\.


-- Portuguese language definition.
-- 02/03/01 - Ed Cattuci <maniac@undernet.org>

COPY "translations" FROM stdin;
6	1	Sorry, You are already authenticated as %s	31337
6	2	AUTHENTICATION SUCCESSFUL as %s	31337
6	3	Sorry, you have insufficient access to perform that command	31337
6	4	Sorry, the channel %s is empty	31337
6	5	I don't see %s anywhere	31337
6	6	I can't find %s on channel %s	31337
6	7	The channel %s doesn't appear to be registered	31337
6	8	You're opped by %s (%s)	31337
6	9	You're voiced by %s (%s)	31337
6	10	%s: You are not in that channel	31337
6	11	%s is already opped in %s	31337
6	12	%s is already voiced in %s	31337
6	13	You're deopped by %s (%s)	31337
6	14	You're devoiced by %s (%s)	31337
6	15	%s is not opped in %s	31337
6	16	%s is not voiced in %s	31337
6	17	AUTHENTICATION FAILED as %s (Invalid Password)	31337
6	18	I'm not in that channel!	31337
6	19	Invalid banlevel range. Valid range is 1-%i.	31337
6	20	Invalid ban duration. Your ban duration can be a maximum of 336 hours.	31337
6	21	Ban reason cannot exceed 128 chars	31337
6	22	Specified ban is already in my banlist!	31337
6	23	The ban %s is already covered by %s	31337
6	24	Invalid minimum level.	31337
6	25	Invalid maximum level.	31337
6	26	USER: %s ACCESS: %s %s	31337
6	27	CHANNEL: %s -- AUTOMODE: %s	31337
6	28	LAST MODIFIED: %s (%s ago)	31337
6	29	** SUSPENDED ** - Expires in %s	31337
6	30	LAST SEEN: %s ago.	31337
6	31	There are more than 15 matching entries.	31337
6	32	Please restrict your query.	31337
6	33	End of access list	31337
6	34	No Match!	31337
6	35	Cannot add a user with equal or higher access than your own. 	31337
6	36	Invalid access level.	31337
6	37	%s is already added to %s with access level %i.	31337
6	38	Added user %s to %s with access level %i	31337
6	39	Something went wrong: %s	31337
6	40	%s: End of ban list		31337
6	41	Unable to view user details (Invisible)	31337
6	42	Information about: %s (%i)	31337
6	43	Currently logged on via: %s	31337
6	44	URL: %s	31337
6	45	Language: %i	31337
6	46	Channels: %s	31337
6	47	Input Flood Points: %i	31337
6	48	Ouput Flood (Bytes): %i	31337
6	49	%s is registered by:	31337
6	50	%s - last seen: %s ago	31337
6	51	Desc: %s	31337
6	52	Flood me will you? I'm not going to listen to you anymore	31337
6	53	I think I've sent you a little too much data, I'm going to ignore you for a while.	31337
6	54	Incomplete command	31337
6	55	To use %s, you must /msg %s@%s	31337
6	56	Sorry, You must be logged in to use this command.	31337
6	57	The channel %s has been suspended by a cservice administrator.	31337
6	58	Your access on %s has been suspended.	31337
6	59	The NOOP flag is set on %s	31337
6	60	The STRICTOP flag is set on %s	31337
6	61	You just deopped more than %i people	31337
6	62	SYNTAX: %s	31337
6	63	Temporarily increased your access on channel %s to %i	31337
6	64	%s is registered.	31337
6	65	%s is not registered.	31337
6	66	I don't think %s would appreciate that.	31337
6	67	\002*** Ban List for channel %s ***\002	31337
6	68	%s %s Level: %i	31337
6	69	ADDED BY: %s (%s)	31337
6	70	SINCE: %i	31337
6	71	EXP: %s	31337
6	72	\002*** END ***\002	31337
6	73	Sorry, I don't know who %s is.	31337
6	74	Sorry, you are not authorized with me anymore.	31337
6	75	doesn't appear to have access in %s.	31337
6	76	Cannot modify a user with equal or higher access than your own.	31337
6	77	Cannot give a user higher or equal access to your own.	31337
6	78	Modified %s's access level on channel %s to %i	31337
6	79	Set AUTOMODE to OP for %s on channel %s	31337
6	80	Set AUTOMODE to VOICE for %s on channel %s	31337
6	81	Set AUTOMODE to NONE for %s on channel %s	31337	
6	82	Your passphrase cannot be your username or current nick - syntax is: NEWPASS <new passphrase>	31337
6	83	Password successfully changed.	31337
6	84	The NOOP flag is set on %s	31337
6	85	The STRICTOP flag is set on %s (and %s isn't authenticated)	31337
6	86	The STRICTOP flag is set on %s (and %s has insufficient access)	31337
6	87	Purged channel %s	31337
6	88	%s is already registered with me.	31337
6	89	Invalid channel name.	31337
6	90	Registered channel %s	31337
6	91	Removed %s from my silence list	31337
6	92	Couldn't find %s in my silence list	31337
6	93	Cannot remove a user with equal or higher access than your own	31337
6	94	You can't remove yourself from a channel you own	31337
6	95	Removed user %s from %s	31337
6	96	Your INVISIBLE setting is now ON.	31337
6	97	Your INVISIBLE setting is now OFF.	31337
6	98	%s for %s is %s	31337
6	99	value of %s must be ON or OFF	31337
6	100	Invalid USERFLAGS setting. Correct values are 0, 1, 2.	31337
6	101	USERFLAGS for %s is %i	31337
6	102	value of MASSDEOPPRO has to be 0-7	31337
6	103	MASSDEOPPRO for %s is set to %d	31337
6	104	value of FLOODPRO has to be 0-7	31337
6	105	FLOODPRO for %s is set to %d	31337
6	106	The DESCRIPTION can be a maximum of 80 chars!	31337
6	107	DESCRIPTION for %s is cleared.	31337
6	108	DESCRIPTION for %s is: %s	31337
6	109	The URL can be a maximum of 75 chars!	31337
6	110	URL for %s is cleared.	31337
6	111	URL for %s is: %s	31337
6	112	The string of keywords cannot exceed 80 chars!	31337
6	113	KEYWORDS for %s are: %s	31337
6	114	Language is set to %s.	31337
6	115	ERROR: Invalid language selection.	31337
6	116	Can't locate channel %s on the network!	31337
6	117	ERROR: Invalid channel setting.	31337
6	118	Ignore list:	31337
6	119	Ignore list is empty	31337
6	120	-- End of Ignore List	31337
6	121	CMaster Channel Services internal status:	31337
6	122	[     Channel Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337
6	123	[        User Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337
6	124	[Access Level Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337
6	125	[         Ban Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337
6	126	Last recieved User NOTIFY: %i	31337
6	127	Last recieved Channel NOTIFY: %i	31337
6	128	Last recieved Level NOTIFY: %i	31337
6	129	Last recieved Ban NOTIFY: %i	31337
6	130	Custom data containers allocated: %i	31337
6	131	\002Uptime:\002 %s	31337
6	132	Channel %s has %d users (%i operators)	31337
6	133	Mode is: %s	31337
6	134	Flags set: %s	31337
6	135	Cannot suspend a user with equal or higher access than your own.	31337
6	136	bogus time units	31337
6	137	Invalid suspend duration.	31337
6	138	SUSPENSION for %s is cancelled	31337
6	139	%s is already suspended on %s	31337
6	140	SUSPENSION for %s will expire in %s	31337
6	141	ERROR: Topic cannot exceed 145 chars	31337
6	142	You have insufficient access to remove the ban %s from %s's database	31337
6	143	Removed %i bans that matched %s	31337
6	144	Removed your temporary access of %i from channel %s	31337
6	145	You don't appear to have a forced access in %s, perhaps it expired?	31337
6	146	%s isn't suspended on %s	31337
6	147	%s is an IRC operator	31337
6	148	%s is NOT logged in.	31337
6	149	%s is logged in as %s%s	31337
6	150	%s is an Official CService Representative%s and logged in as %s	31337
6	151	%s is an Official CService Administrator%s and logged in as %s	31337
6	152	%s is an Official CService Developer%s and logged in as %s	31337
6	153	There are more than %i entries matching [%s]	31337
6	154	Please restrict your search mask	31337
6	155	No matching entries for [%s]	31337
6	156	%s: Cleared channel modes.	31337
6	158	Invalid option.	31337
6	159	%s is an Official Undernet Service Bot.	31337
6	160	Added ban %s to %s at level %i	31337
6	161	%s: ban list is empty.		31337
6	162	I'm already in that channel!	31337
\.

-- Italian language definition.
-- 03/03/07 - Orlando Bassotto <future@break.net>.
-- 04/03/07 - Ombretta Gorini <ombra.g@galactica.it>.

COPY "translations" FROM stdin;
7	1	Spiacente, sei già autenticato come %s	31337
7	2	AUTENTICAZIONE RIUSCITA come %s	31337
7	3	Spiacente, non hai un accesso sufficiente per eseguire quel comando	31337
7	4	Spiacente, il canale %s è vuoto	31337
7	5	Non vedo nessun %s	31337
7	6	Non trovo %s nel canale %s	31337
7	7	Il canale %s non risulta essere registrato	31337
7	8	Ti è stato dato l'op da %s (%s)	31337
7	9	Ti è stato dato il voice da %s (%s)	31337
7	10	%s: Non sei in quel canale	31337
7	11	%s ha già l'op in %s	31337
7	12	%s ha già il voice in %s	31337
7	13	Ti è stato tolto l'op da %s (%s)	31337
7	14	Ti è stato tolto il voice da %s (%s)	31337
7	15	%s non ha l'op in %s	31337
7	16	%s non ha il voice in %s	31337
7	17	AUTENTICAZIONE FALLITA come %s (Password non valida)	31337
7	18	Non mi trovo in quel canale!	31337
7	19	Range del livello del ban non valido. Il range valido è compreso tra 1 e %i.	31337
7	20	Durata del ban non valida. La durata massima di un ban può essere di 336 ore.	31337
7	21	Il motivo del ban non può superare i 128 caratteri	31337
7	22	Il ban specificato è già presente nella mia lista dei ban!	31337
7	23	Il ban %s è già coperto da %s	31337
7	24	Livello minimo non valido.	31337
7	25	Livello massimo non valido.	31337
7	26	UTENTE: %s ACCESSO: %s %s	31337
7	27	CANALE: %s -- AUTOMODE: %s	31337
7	28	ULTIMA MODIFICA: %s (%s fà)	31337
7	29	** SOSPESO ** - Scade il %s	31337
7	30	VISTO L'ULTIMA VOLTA: %s.	31337
7	31	Sono presenti più di 15 corrispondenze.	31337
7	32	Per favore, restringi il campo di ricerca dell'interrogazione.	31337
7	33	Fine della lista degli accessi	31337
7	34	Nessuna Corrispondenza!	31337
7	35	Non puoi aggiungere un utente con un livello di accesso uguale o superiore al tuo.	31337
7	36	Livello di accesso non valido.	31337
7	37	%s è già stato aggiunto in %s con livello di accesso %i.	31337
7	38	Aggiunto l'utente %s a %s con livello di accesso %i	31337
7	39	Qualcosa non va: %s	31337
7	40	%s: Fine della lista dei ban	31337
7	41	Impossibile visualizzare i dettagli dell'utente (Invisibile)	31337
7	42	Informazioni su: %s (%i)	31337
7	43	Attualmente autenticato attraverso: %s	31337
7	44	URL: %s	31337
7	45	Lingua: %i	31337
7	46	Canali: %s	31337
7	47	Input Flood Points: %i	31337
7	48	Ouput Flood (Bytes): %i	31337
7	49	%s è registrato da:	31337
7	50	%s - visto l'ultima volta: %s 	31337
7	51	Desc: %s	31337
7	52	Vuoi floodarmi? Bene bene, allora mi sa che da adesso non ti ascolto più.	31337
7	53	Penso che abbia visto abbastanza, adesso ti ignoro per un po'.	31337
7	54	Comando non completo	31337
7	55	Per usare %s, devi scrivere /msg %s@%s	31337
7	56	Spiacente, devi autenticarti per usare questo comando.	31337
7	57	Il canale %s è stato sospeso da un amministratore di CService.	31337
7	58	Il tuo accesso su %s è stato sospeso.	31337
7	59	Il flag NOOP è impostato su %s	31337
7	60	Il flag STRICTOP è impostato su %s	31337
7	61	Hai appena tolto l'op a più di %i persone	31337
7	62	SINTASSI: %s	31337
7	63	Il tuo accesso è stato temporaneamente incrementato sul canale %s a %i	31337
7	64	%s è registrato.	31337
7	65	%s non è registrato.	31337
7	66	Non penso che %s possa apprezzarlo.	31337
7	67	\002*** Lista dei Ban per il canale %s ***\002	31337
7	68	%s %s Livello: %i	31337
7	69	AGGIUNTO DA: %s (%s)	31337
7	70	DAL: %i	31337
7	71	SCADENZA: %s	31337
7	72	\002*** FINE ***\002	31337
7	73	Spiacente, ma non conosco chi sia %s.	31337
7	74	Spiacente, non sei più autorizzato con me.	31337
7	75	non sembra avere accesso in %s.	31337
7	76	Non posso modificare un utente con un livello di accesso uguale o superiore al tuo.	31337
7	77	Non puoi dare un accesso uguale o superiore al tuo a un altro utente.	31337
7	78	Modificato il livello di accesso per %s sul canale %s a %i	31337
7	79	Impostato AUTOMODE a OP per %s sul canale %s	31337
7	80	Impostato AUTOMODE a VOICE per %s sul canale %s	31337
7	81	Impostato AUTOMODE a NONE per %s sul canale %s	31337	
7	82	La tua password non può essere il tuo username o il tuo attuale nick - la sintassi è: NEWPASS <nuova password>	31337
7	83	La password è stata cambiata con successo.	31337
7	84	Il flag NOOP è impostato su %s	31337
7	85	Il flag STRICTOP è impostato su %s (e %s non è autenticato)	31337
7	86	Il flag STRICTOP è impostato su %s (e %s non ha accesso sufficiente)	31337
7	87	Purgato il canale %s	31337
7	88	%s è già registrato da me.	31337
7	89	Nome del canale non valido.	31337
7	90	Registrato il canale %s	31337
7	91	%s è stato rimosso dalla lista dei silence	31337
7	92	Non trovo %s nella mia lista dei silence	31337
7	93	Non puoi rimuovere un utente con accesso uguale o superiore al tuo	31337
7	94	Non puoi rimuovere te stesso da un canale che possiedi	31337
7	95	Rimosso l'utente %s da %s	31337
7	96	L'impostazione INVISIBLE adesso è attiva (ON).	31337
7	97	L'impostazione INVISIBLE adesso non è attiva (OFF).	31337
7	98	%s per %s è %s	31337
7	99	il valore di %s dev'essere ON oppure OFF	31337
7	100	Impostazione USERFLAGS non valida. I valori corretti sono 0, 1 o 2.	31337
7	101	USERFLAGS per %s è %i	31337
7	102	il valore di MASSDEOPPRO dev'essere compreso tra 0 e 7 inclusi	31337
7	103	MASSDEOPPRO per %s è impostato a %d	31337
7	104	il valore di FLOODPRO dev'essere compreso tra 0 e 7 inclusi	31337
7	105	FLOODPRO per %s è impostato a %d	31337
7	106	La DESCRIPTION non può superare gli 80 caratteri.	31337
7	107	La DESCRIPTION per %s è stata cancellata.	31337
7	108	La DESCRIPTION per %s è: %s	31337
7	109	L'URL non può superare i 75 caratteri.	31337
7	110	L'URL per %s è stata cancellata.	31337
7	111	L'URL per %s è: %s	31337
7	112	La stringa contenente le parole chiave non può superare gli 80 caratteri.	31337
7	113	Le KEYWORDS (parole chiave) per %s sono: %s	31337
7	114	La lingua selezionata è %s.	31337
7	115	ERRORE: Selezione della lingua non valida.	31337
7	116	Non trovo il canale %s nella rete!	31337
7	117	ERRORE: Impostazione del canale non valida.	31337
7	118	Lista degli Ignore:	31337
7	119	La lista degli Ignore è vuota	31337
7	120	-- Fine della Lista degli Ignore	31337
7	121	Stato interno CMaster Channel Services:	31337
7	122	[     Channel Record Stats] \002Cached Entries:\002 %i    \002DB Richieste:\002 %i    \002Cache Hits:\002 %i    \002Efficienza:\002 %.2f%%	31337
7	123	[        User Record Stats] \002Cached Entries:\002 %i    \002DB Richieste:\002 %i    \002Cache Hits:\002 %i    \002Efficienza:\002 %.2f%%	31337
7	124	[Access Level Record Stats] \002Cached Entries:\002 %i    \002DB Richieste:\002 %i    \002Cache Hits:\002 %i    \002Efficienza:\002 %.2f%%	31337
7	125	[         Ban Record Stats] \002Cached Entries:\002 %i    \002DB Richieste:\002 %i    \002Cache Hits:\002 %i    \002Efficienza:\002 %.2f%%	31337
7	126	Last recieved User NOTIFY: %i	31337
7	127	Last recieved Channel NOTIFY: %i	31337
7	128	Last recieved Level NOTIFY: %i	31337
7	129	Last recieved Ban NOTIFY: %i	31337
7	130	Contenitori di dati proprietari allocati: %i	31337
7	131	\002Tempo di vita:\002 %s	31337
7	132	Il canale %s ha %d utenti (%i operatori)	31337
7	133	Mode è: %s	31337
7	134	Flags impostati: %s	31337
7	135	Non posso sospendere un utente con accesso uguale o superiore al tuo.	31337
7	136	unità di tempo sconosciuta	31337
7	137	Durata della sospensione non valida.	31337
7	138	La SOSPENSIONE per %s è stata annullata	31337
7	139	%s è già stato sospeso in %s	31337
7	140	La SOSPENSIONE per %s scadrà in %s	31337
7	141	ERRORE: Il topic non può superare i 145 caratteri	31337
7	142	Non hai accesso sufficiente per rimuovere il ban %s dal database di %s.	31337
7	143	Sono stati rimossi %i ban che corrispondono a %s	31337
7	144	Rimosso temporaneamente il tuo accesso di %i dal canale %s	31337
7	145	Non sembra che tu abbia un accesso forzato in %s, forse è scaduto?	31337
7	146	%s non è sospeso in %s	31337
7	147	%s è un operatore IRC	31337
7	148	%s NON è autenticato.	31337
7	149	%s è autenticato come %s%s	31337
7	150	%s è un Rappresentante Ufficiale di CService%s e autenticato come %s	31337
7	151	%s è un Ammistratore Ufficiale di CService%s e autenticato come %s	31337
7	152	%s è uno Sviluppatore Ufficiale di CService%s e autenticato come %s	31337
7	153	Ci sono più di %i corrispondenze di [%s]	31337
7	154	Per favore, restringi la tua maschera di ricerca	31337
7	155	Non ci sono corrispondenze per [%s]	31337
7	156	%s: Mode del canale annullati.	31337
7	158	Opzione non valida.	31337
7	159	%s è un Bot di Servizio Ufficiale di Undernet.	31337
7	160	Aggiunto il ban %s a %s con livello %i	31337
7	161	%s: lista dei ban vuota.		31337
7	162	Sono già in quel canale!	31337
\.

-- Romanian language definition.
-- 04/03/01 - Alex Badea <vampire@go.ro>

COPY "translations" FROM stdin;
8	1	Esti deja autentificat ca %s	31337
8	2	AUTENTIFICARE REUSITA ca %s	31337
8	3	Nu ai suficient acces pentru aceasta comanda	31337
8	4	Canalul %s e gol	31337
8	5	Nu-l vad pe %s nicaieri	31337
8	6	Nu-l gasesc pe %s pe canalul %s	31337
8	7	Canalul %s nu pare sa fie inregistrat	31337
8	8	Ai primit op de la %s (%s)	31337
8	9	Ai primit voce de la %s (%s)	31337
8	10	%s: Nu esti pe canal	31337
8	11	%s are deja op pe %s	31337
8	12	%s are deja voce pe %s	31337
8	13	Ai fost deopat de %s (%s)	31337
8	14	Ai fost de-voiced de %s (%s)	31337
8	15	%s nu are op pe %s	31337
8	16	%s nu are voce pe %s	31337
8	17	AUTENTIFICARE ESUATA ca %s (parola incorecta)	31337
8	18	Nu sunt pe canal!	31337
8	19	Nivelul de ban e incorect. Domeniul admis este 1-%i.	31337
8	20	Durata banului e incorecta. Poti da ban pe o durata maxima de 336 de ore.	31337
8	21	Motivul banului nu poate depasi 128 de caractere.	31337
8	22	Respectivul ban e deja pe lista!	31337
8	23	Banul pe %s e deja sub incidenta banului pe %s	31337
8	24	Nivel minim invalid.	31337
8	25	Nivel maxim invalid.	31337
8	26	USER: %s ACCES: %s %s	31337
8	27	CANAL: %s -- AUTOMODE: %s	31337
8	28	MODIFICAT: %s (in urma cu %s)	31337
8	29	** SUSPENDAT ** - Expira in %s	31337
8	30	VAZUT: in urma cu %s	31337
8	31	Sunt mai mult de 15 rezultate.	31337
8	32	Restrictioneaza-ti cererea.	31337
8	33	Sfarsit lista de accese	31337
8	34	N-am gasit nimic!	31337
8	35	Nu poti adauga un user cu acces mai mare sau egal cu al tau. 	31337
8	36	Nivel de acces incorect.	31337
8	37	%s e deja pe lista %s cu acces de %i.	31337
8	38	Am adaugat %s la %s cu acces de %i	31337
8	39	Ceva n-am mers cum trebuie: %s	31337
8	40	%s: Sfarsit lista de banuri		31337
8	41	Nu pot da detalii despre user (e invizibil)	31337
8	42	Informatii despre: %s (%i)	31337
8	43	In prezent logat prin: %s	31337
8	44	URL: %s	31337
8	45	Limba: %i	31337
8	46	Canale: %s	31337
8	47	Flood la intrare (puncte): %i	31337
8	48	Flood la iesire (bytes): %i	31337
8	49	%s e inregistrat de:	31337
8	50	%s - vazut in urma cu %s	31337
8	51	Desc: %s	31337
8	52	Ma floodezi, huh? N-ai decat sa vorbeshti in punga.	31337
8	53	Cred ca ti-am trimis cam multe date; o sa te ignor pentru o vreme.	31337
8	54	Comanda incompleta	31337
8	55	Pentru a folosi %s, trebuie sa /msg %s@%s	31337
8	56	Trebuie sa fii logat ca sa folosesti aceasta comanda.	31337
8	57	Canalul %s a fost suspendat de un administrator de la CService	31337
8	58	Accesul pe %s ti-a fost suspendat	31337
8	59	Flagul NOOP e activ pe %s	31337
8	60	Flagul STRICTOP e activ pe %s	31337
8	61	Ai deopat mau mult de %i persoane	31337
8	62	SINTAXA: %s	31337
8	63	Accesul pe %s ti-a fost marit temporar la %i	31337
8	64	%s e inregistrat.	31337
8	65	%s nu e inregistrat.	31337
8	66	Nu cred ca lui %s i-ar placea chestia asta.	31337
8	67	\002*** Ban List pentru canalul %s ***\002	31337
8	68	%s %s Nivel: %i	31337
8	69	DAT DE: %s (%s)	31337
8	70	DE LA: %i	31337
8	71	EXP: %s	31337
8	72	\002*** SFARSIT ***\002	31337
8	73	Nu stiu cine e %s.	31337
8	74	Nu mai esti autorizat.	31337
8	75	nu pare sa aiba acces pe %s.	31337
8	76	Nu poti modifica un user cu acces egal sau mai mare ca al tau.	31337
8	77	Nu poti da unui user acces mai mare sau egal cu al tau.	31337
8	78	Am modificat nivelul lui %s pe %s la %i	31337
8	79	Am setat AUTOMODE la OP pentru %s pe %s	31337
8	80	Am setat AUTOMODE la VOICE pentru %s pe %s	31337
8	81	Am setat AUTOMODE la NONE pentru %s pe %s	31337	
8	82	Parola nu poate fi username-ul sau nick-ul tau. - sintaxa e: NEWPASS <parola noua>	31337
8	83	Parola a fost schimbata.	31337
8	84	Flag-ul NOOP e activ pe %s	31337
8	85	Flag-ul STRICTOP e activ pe %s (si %s nu e logat)	31337
8	86	Flag-ul STRICTOP e activ pe %s (si %s nu are destul acces)	31337
8	87	Am purjat canalul %s	31337
8	88	%s e deja inregistrat.	31337
8	89	Nume de canal incorect.	31337
8	90	Am inregistrat canalul %s	31337
8	91	Am sters %s din lista de silence	31337
8	92	N-am gasit %s in lista de silence	31337
8	93	Nu poti sterge un user cu acces egal sau mai mare ca al tau	31337
8	94	Nu poti sa te stergi singur de pe un canal unde esti manager	31337
8	95	Am sters user-ul %s de pe %s	31337
8	96	Acum INVISIBLE este ON	31337
8	97	Acum INVISIBLE este OFF	31337
8	98	%s pentru %s e %s	31337
8	99	valoarea lui %s trebuie sa fie ON sau OFF	31337
8	100	Valoarea pentru USERFLAGS e incorecta. Valorile admise sunt 0, 1, 2.	31337
8	101	USERFLAGS pentru %s este %i	31337
8	102	valoarea pentru MASSDEOPPRO trebuie sa fie 0-7	31337
8	103	MASSDEOPPRO pentru %s este %d	31337
8	104	valoarea pentru FLOODPRO trebuie sa fie 0-7	31337
8	105	FLOODPRO pentru %s este %d	31337
8	106	DESCRIPTION poate sa aiba maxim 80 de caractere!	31337
8	107	Am sters DESCRIPTION pentru %s	31337
8	108	DESCRIPTION pentru %s este: %s	31337
8	109	URL poate sa aiba maxim 75 de caractere!	31337
8	110	Am sters URL pentru %s	31337
8	111	URL pentru %s este: %s	31337
8	112	Sirul de cuvinte cheie nu poate depasi 80 de caractere!	31337
8	113	KEYWORDS pentru %s sunt: %s	31337
8	114	Limba este acum %s.	31337
8	115	EROARE: Limba ceruta e incorecta.	31337
8	116	Nu gasesc canalul %s in retea!	31337
8	117	EROARE: Setare de canal incorecta.	31337
8	118	Lista de ignore:	31337
8	119	Lista de ignore e goala	31337
8	120	-- Sfarsit lista de ignore	31337
8	121	Stare interna pentru CMaster Channel Services:	31337
8	122	[         Canale] \002In cache:\002 %i    \002Cereri din DB:\002 %i    \002Hituri din cache:\002 %i    \002Eficienta:\002 %.2f%%	31337
8	123	[    Utilizatori] \002In cache:\002 %i    \002Cereri din DB:\002 %i    \002Hituri din cache:\002 %i    \002Eficienta:\002 %.2f%%	31337
8	124	[Nivele de acces] \002In cache:\002 %i    \002Cereri din DB:\002 %i    \002Hituri din cache:\002 %i    \002Eficienta:\002 %.2f%%	31337
8	125	[        Ban-uri] \002In cache:\002 %i    \002Cereri din DB:\002 %i    \002Hituri din cache:\002 %i    \002Eficienta:\002 %.2f%%	31337
8	126	Ultimul User NOTIFY primit: %i	31337
8	127	Ultimul Channel NOTIFY primit: %i	31337
8	128	Ultimul Level NOTIFY primit: %i	31337
8	129	Ultimul Ban NOTIFY primit: %i	31337
8	130	Containere de date custom alocate: %i	31337
8	131	\002Uptime:\002 %s	31337
8	132	Canalul %s are %d useri (%i operatori)	31337
8	133	Moduri: %s	31337
8	134	Flaguri: %s	31337
8	135	Nu poti suspenda un user cu acces egal sau mai mare ca al tau.	31337
8	136	Ce unitati de timp sunt alea?	31337
8	137	Durata de suspend incorecta.	31337
8	138	SUSPENDAREA pentru %s anulata	31337
8	139	%s e deja suspendat pe %s	31337
8	140	SUSPENDAREA pentru %s exipra in %s	31337
8	141	EROARE: Topicul nu poate depasi 145 de caractere	31337
8	142	Nu ai destul acces pentru a scoate banul %s de pe %s	31337
8	143	Am scos %i banuri care s-au potrivit cu %s	31337
8	144	Accesul temporar de %i pe %s a fost revocat	31337
8	145	Nu pari sa ai acces fortat pe %s, poate a expirat?	31337
8	146	%s nu e suspendat pe %s	31337
8	147	%s este un IRCop	31337
8	148	%s NU e logat	31337
8	149	%s e logat ca %s%s	31337
8	150	%s e un Reprezentant Oficial al CService%s si e logat ca %s	31337
8	151	%s e un Administrator Oficial al CService%s si e logat ca %s	31337
8	152	%s e un Programator Oficial al CService%s and logged in as %s	31337
8	153	Sunt mai mult de %i rezultate care se potrivesc cu [%s]	31337
8	154	Restrictioneaza-ti masca de cautare	31337
8	155	Nu sunt rezultate care sa se potriveasca cu [%s]	31337
8	156	%s: Am curatati modurile de canal.	31337
8	158	Optiune incorecta.	31337
8	159	%s este un Bot Oficial al Undernet.	31337
8	160	%s este un Reprezentant Oficial al Coder-Com%s si e logat ca %s	31337
8	161	%s este un Contributor Oficial al Coder-Com%s si e logat ca %s	31337
8	162	%s este un Programator Oficial al Coder-Com%s si e logat ca %s	31337
8	163	%s este un Reprezentant Senior al Coder-Com%s si e logat ca %s	31337
8	164	 si un IRCop	31337
8	165	Am adaugat ban pe %s pe %s la nivelul %i	31337
8	166	%s: lista de banuri e goala		31337
8	167	Sunt deja pe canal!	31337
8	168	Aceasta comanda este rezervata pentru IRCopi	31337
8	169	Nu sunt operator pe %s	31337
8	170	%s pentru %i minute	31337
\.
