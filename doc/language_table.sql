-- Language Definition Table for CMaster Undernet channel services.
 
-- Zap everything! 
DELETE FROM translations;

-- English language definition.
-- 27/12/00 - Greg Sikorski <gte@atomicrevs.demon.co.uk>.
-- 01/03/01 - Daniel Simard <svr@undernet.org>.

COPY "translations" FROM stdin;
1	1	Sorry, You are already authenticated as %s	31337
1	2	AUTHENTICATION SUCCESSFUL as %s	31337
1	3	Sorry, you have insufficient access to perform that command	31337
1	4	Sorry, the channel %s is empty	31337
1	5	I don't see %s anywhere	31337
1	6	I can't find %s on channel %s	31337
1	7	The channel %s doesn't appear to be registered	31337
1	8	You're opped by %s (%s) on %s	31337
1	9	You're voiced by %s (%s) on %s	31337
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
1	29	** SUSPENDED ** - Expires in %s (Level %i)	31337
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
1	70	SINCE: %s	31337
1	71	EXP: %s	31337
1	72	\002*** END ***\002	31337
1	73	I don't know who %s is.	31337
1	74	You are not authorised with me anymore.	31337
1	75	%s doesn't appear to have access in %s.	31337
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
1	126	Last received User NOTIFY: %i	31337
1	127	Last received Channel NOTIFY: %i	31337
1	128	Last received Level NOTIFY: %i	31337
1	129	Last received Ban NOTIFY: %i	31337
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
-- 03/09/01 - nighty <nighty@undernet.org>.

COPY "translations" FROM stdin;
2	1	D�sol�. Vous �tes d�j� authentifi� sous le nom d'utilisateur %s	31337
2	2	AUTHENTIFICATION R�USSIE pour %s	31337
2	3	D�sol�. Votre acc�s est insuffisant pour utiliser cette commande	31337
2	4	D�sol�. Le canal %s est vide	31337
2	5	Je suis incapable de trouver %s	31337
2	6	Je ne vois pas %s sur le canal %s	31337
2	7	Le canal %s ne semble pas �tre enregistr�	31337
2	8	Vous avez �t� op par %s (%s)	31337
2	9	Vous avez �t� voice par %s (%s)	31337
2	10	%s: Vous n'�tes pas dans ce canal	31337
2	11	%s est d�j� op sur %s	31337
2	12	%s est d�j� voice sur %s	31337
2	13	Vous avez �t� deop par %s (%s)	31337
2	14	Vous avez �t� devoice par %s (%s)	31337
2	15	%s n'est pas op sur %s	31337
2	16	%s n'est pas voice sur %s	31337
2	17	AUTHENTIFICATION REFUS�E pour %s (Mot de passe Invalide)	31337
2	18	Je ne suis pas dans ce canal!!	31337
2	19	Niveau de ban invalide. Niveau valide: entre 1 et %i.	31337
2	20	Dur�e du ban invalide. La dur�e maximale est de 336 heures.	31337
2	21	La raison d'un ban ne peut d�passer 128 caract�res.	31337
2	22	Ce ban est d�j� dans ma liste!	31337
2	23	Le ban %s est dej� couvert par %s	31337
2	24	Niveau minimum invalide.	31337
2	25	Niveau maximum invalide.	31337
2	26	UTILISATEUR: %s NIVEAU: %s %s	31337
2	27	CANAL: %s -- AUTOMODE: %s	31337
2	28	DERNI�RE MODIFICATION: %s (depuis %s)	31337
2	29	** SUSPENDU ** - Expire dans %s  (Level %i)	31337
2	30	DERNI�RE FOIS VU: il y a %s.	31337
2	31	Il y a plus de 15 requ�tes trouv�es.	31337
2	32	SVP restreindre votre requ�te	31337
2	33	Fin de la liste des acc�s	31337
2	34	Aucune correspondance trouv�e!	31337
2	35	Vous ne pouvez pas ajouter un utilisateur � un niveau �gal ou sup�rieur au votre. 	31337
2	36	Niveau d'acc�s invalide	31337
2	37	%s a d�j� �t� ajout� sur %s au niveau %i.	31337
2	38	Utilisateur %s ajout� sur %s au niveau %i	31337
2	39	Quelque chose semble incorrect: %s	31337
2	40	%s: fin de la liste des bans		31337
2	41	Impossible de voir les d�tails de l'utilisateur (Invisible).	31337
2	42	Informations sur: %s (%i)	31337
2	43	Connect� en ce moment via: %s	31337
2	44	URL: %s	31337
2	45	Langue: %i	31337
2	46	Canaux: %s	31337
2	47	Entr�e Flood: %i	31337
2	48	Sortie Flood (Bytes): %i	31337
2	49	%s a �t� enregistr� par:	31337
2	50	%s - vu pour la derni�re fois: il y a %s	31337
2	51	Desc: %s	31337
2	52	Vous voulez me Flooder? Je ne vais plus vous r�pondre dor�navant.	31337
2	53	Je crois que je vous ai envoy� beaucoup trop d'informations. Je vais vous ignorer un peu.	31337
2	54	Commande incompl�te	31337
2	55	Pour utiliser %s, Vous devez /msg %s@%s	31337
2	56	D�sol�. Vous devez �tre authentifi� pour utiliser cette commande.	31337
2	57	Le canal %s a �t� suspendu par un administrateur de cservice.	31337
2	58	Votre access sur %s a �t� suspendu.	31337
2	59	Le mode NOOP est actif sur %s	31337
2	60	Le mode STRICTOP est actif sur %s	31337
2	61	Vous venez de deop plus de %i personnes	31337
2	62	SYNTAXE: %s	31337
2	63	Votre niveau sur %s � �t� temporairement augment� au niveau %i	31337
2	64	%s est enregistr�.	31337
2	65	%s n'est pas enregistr�.	31337
2	66	Je ne crois pas que %s appr�cierait �a.	31337
2	67	\002*** Liste des Bans pour le canal %s ***\002	31337
2	68	%s %s Niveau: %i	31337
2	69	AJOUT� PAR: %s (%s)	31337
2	70	DEPUIS: %s	31337
2	71	EXPIRATION: %s	31337
2	72	\002*** FIN ***\002	31337
2	73	D�sol�, Je ne sais pas qui est %s.	31337
2	74	Vous avez �t� d�loggu�, vous n'�tes plus authentifi�.	31337
2	75	%s n'appara�t pas comme ayant acc�s sur %s.	31337
2	76	Vous ne pouvez pas modifier un utilisateur ayant un niveau sup�rieur ou �gal au votre.	31337
2	77	Vous ne pouvez pas donner un niveau d'acc�s sup�rieur ou �gal au votre � un utilisateur.	31337
2	78	Modification du niveau de %s sur le canal %s � %i effectu�e.	31337
2	79	Activation du MODE Automatique (OP) pour %s sur le canal %s	31337
2	80	Activation du MODE Automatique (VOICE) pour %s sur le canal %s	31337
2	81	D�sactivation des MODES Automatiques pour %s sur le canal %s	31337	
2	82	Votre mot de passe ne peut �tre ni votre nom d'utilisateur ni votre nick - La Syntaxe est: NEWPASS <nouveau mot de passe>	31337
2	83	Votre mot de passe a �t� chang� avec succ�s.	31337
2	84	Le mode NOOP est actif sur %s	31337
2	85	Le mode STRICTOP est actif sur %s (et %s n'est pas authentifi�)	31337
2	86	Le mode STRICTOP est actif sur %s (et l'acc�s de %s est insuffisant)	31337
2	87	%s a �t� purg�	31337
2	88	%s est d�j� enregistr�.	31337
2	89	Nom de canal invalide.	31337
2	90	%s a �t� enregistr�	31337
2	91	%s � �t� enlev� de ma liste silence/ignore	31337
2	92	%s introuvable dans ma liste silence/ignore	31337
2	93	Vous ne pouvez pas effacer un utilisateur ayant un niveau �gal ou sup�rieur au votre.	31337
2	94	Vous ne pouvez pas effacer votre acc�s sur un canal dont vous �tes le propri�taire.	31337
2	95	L'acc�s de l'utilisateur %s a �t� effac� sur %s	31337
2	96	Le mode INVISIBLE pour vos informations est maintenant � ON.	31337
2	97	Le mode INVISIBLE pour vos informations est maintenant � OFF.	31337
2	98	%s pour %s est %s	31337
2	99	La valeur de %s doit �tre ON ou OFF	31337
2	100	Configuration USERFLAGS invalide! Les valeurs correctes sont 0, 1, 2.	31337
2	101	Le USERFLAGS de %s est %i	31337
2	102	La valeur de MASSDEOPPRO doit �tre comprise entre 0 et 7	31337
2	103	MASSDEOPPRO de %s a �t� configur� � %d	31337
2	104	La valeur de FLOODPRO doit �tre comprise entre 0 et 7	31337
2	105	FLOODPRO de %s a �t� configur� � %d	31337
2	106	La DESCRIPTION ne doit pas d�passer 80 caract�res.	31337
2	107	La DESCRIPTION de %s a �t� �ffac�e.	31337
2	108	La DESCRIPTION de %s est: %s	31337
2	109	L'URL ne doit pas d�passer 75 caract�res.	31337
2	110	L'URL de %s a �t� effac�e.	31337
2	111	L'URL de %s est: %s	31337
2	112	Les KEYWORDS (Mots-Cl�s) ne doivent pas d�passer 80 caract�res.	31337
2	113	KEYWORDS de %s sont: %s	31337
2	114	Langue activ�e: %s.	31337
2	115	ERREUR: Langue selectionn�e invalide.	31337
2	116	Le canal %s est introuvable sur le r�seau!	31337
2	117	ERREUR: Configuration de canal invalide.	31337
2	118	Liste de silence/ignore:	31337
2	119	La liste de silence/ignore est vide	31337
2	120	-- Fin de la liste de silence/ignore 	31337
2	121	Statut interne du Service de Canaux CMaster:	31337
2	122	[          Statistiques du Canal] \002Entr�es en cache:\002 %i    \002Requ�tes Base:\002 %i    \002# de fois Cache touch�:\002 %i    \002Efficacit�:\002 %.2f%%	31337
2	123	[     Statistiques Utilisateurs] \002Entr�es en cache:\002 %i    \002Requ�tes Base:\002 %i    \002# de fois Cache touch�:\002 %i    \002Efficacit�:\002 %.2f%%	31337
2	124	[   Statistiques Niveaux d'acc�s] \002Entr�es en cache:\002 %i    \002Requ�tes Base:\002 %i    \002# de fois Cache touch�:\002 %i    \002Efficacit�:\002 %.2f%%	31337
2	125	[          Statistiques des bans] \002Entr�es en cache:\002 %i    \002Requ�tes Base:\002 %i    \002# de fois Cache touch�:\002 %i    \002Efficacit�:\002 %.2f%%	31337
2	126	Dernier �v�nement NOTIFY d'utilisateur: %i	31337
2	127	Dernier �v�nement NOTIFY de canal: %i	31337
2	128	Dernier �v�nement NOTIFY de niveau: %i	31337
2	129	Dernier �v�nement NOTIFY de ban: %i	31337
2	130	Donn�es assign�es: %i	31337
2	131	\002En fonction depuis (uptime):\002 %s	31337
2	132	Le canal %s � %d utilisateur(s) (%i op�rateur(s))	31337
2	133	Les Modes du canal sont: %s	31337
2	134	Flags actifs: %s	31337
2	135	Vous ne pouvez pas suspendre un utilisateur ayant un niveau �gal ou sup�rieur au votre.	31337
2	136	Unit� de temps invalide.	31337
2	137	Dur�e de suspension invalide.	31337
2	138	La SUSPENSION pour %s a �t� annul�e.	31337
2	139	%s est d�j� suspendu sur %s	31337
2	140	La SUSPENSION pour %s va expirer dans %s	31337
2	141	ERREUR: Le topic ne peut d�passer 145 caract�res.	31337
2	142	Votre acc�s est insuffisant pour enlever le ban %s de la base de donn�es de %s	31337
2	143	%i ban(s) correpondant � %s ont �t� supprim�s.	31337
2	144	Votre acc�s temporaire au niveau %i sur le canal %s � �t� supprim�.	31337
2	145	Vous ne semblez pas avoir forc� l'acc�s � %s, Serait-il expir�?	31337
2	146	%s n'est pas suspendu sur %s	31337
2	147	%s est un Op�rateur IRC	31337
2	148	%s n'est pas authentifi�.	31337
2	149	%s est authentifi� sous le nom d'utilisateur %s%s	31337
2	150	%s est un repr�sentant Officiel de CService%s et authentifi� sour le nom d'utilisateur %s	31337
2	151	%s est un Administrateur Officiel de CService%s et authentifi� sous le nom d'utilisateur %s	31337
2	152	%s est un D�veloppeur Officiel de CService%s et authentifi� sous le nom d'utilisateur %s	31337
2	153	Il y a plus de %i entr�es correspondantes [%s]	31337
2	154	Veuillez svp restreindre votre requ�te.	31337
2	155	Aucune entr�e ne correspond � votre requ�te. [%s]	31337
2	156	%s: Les modes du canal on �t� remis � z�ro.	31337
2	158	Option Invalide.	31337
2	159	%s est un Bot de Services Officiel d'Undernet.	31337
2	160	%s est un Repr�sentant Officiel de Coder-Com%s et authentifi� sous le nom d'utilisateur %s	31337
2	161	%s est un Participan Officiel de Coder-Com%s et authentifi� sous le nom d'utilisateur %s	31337
2	162	%s est un D�veloppeur Officie de Coder-Com%s et authentifi� sous le nom d'utilisateur %s	31337
2	163	%s est un S�nior Officiel de Coder-Com%s et authentifi� sous le nom d'utilisateur %s	31337
2	164	 et un Op�rateur IRC	31337
2	165	Ban sur %s ajout� � %s au niveau %i	31337
2	166	%s: La liste des bans est vide.		31337
2	167	Je suis d�j� dans ce canal!	31337
2	168	Cette commande est r�serv�e aux Op�rateurs IRC	31337
2	169	Je ne suis pas op sur %s	31337
2	170	%s pour %i minutes	31337
\.

-- Danish language definition.
-- 28/12/2000 - David Henriksen <david@itwebnet.dk>.

COPY "translations" FROM stdin;
3	1	Beklager, Du er allerede autoriseret som %s	31337
3	2	AUTORISATION LYKKEDES som %s	31337
3	3	Beklager, du har utilstr�kkelig adgang til at udf�re den kommando	31337
3	4	Beklager, kanalen %s er tom	31337
3	5	Jeg kan ikke se %s nogen steder	31337
3	6	Jeg kan ikke finde %s p� kanalen %s	31337
3	7	Kanelen %s ser ikke ud til at v�re registreret	31337
3	8	Du er op'et af %s (%s)	31337
3	9	Du er voice'et af %s (%s)	31337
3	10	%s: Du er ikke i denne kanal	31337
3	11	%s er allerede op'et i %s	31337
3	12	%s er allerede voice'et i %s	31337
3	13	Du er deop'et af %s (%s)	31337
3	14	Du er devoice'et af %s (%s)	31337
3	15	%s er ikke op'et i %s	31337
3	16	%s er ikke voice'et i %s	31337
3	17	AUTORISATION MISLYKKEDES som %s (Ugyldigt Kodeord)	31337
3	18	Jeg er ikke i den kanal!	31337
3	19	Ugyldigt banlevel omr�de. Gyldigt omr�de er 1-%i.	31337
3	20	Ugyldigt ban varighed. Din ban varighed kan max best� af 336 timer.	31337
3	21	Ban grund kan ikke overtr�de 128 karaktere	31337
3	22	Specifiseret ban er allerede i min banliste!	31337
3	23	Ban'et %s er allerede d�kket af %s		31337
3	24	Ugyldigt minimums level.	31337
3	25	Ugyldigt maximums level.	31337
3	26	BRUGER: %s ADGANG: %s %s	31337
3	27	KANAL: %s - AUTOMODE: %s	31337
3	28	SIDST �NDRET: %s (%s siden)	31337
3	29	** SUSPENDERET ** - Udl�ber om %s (Level %i)	31337
3	30	SIDST SET: %s siden.	31337
3	31	Der er mere end 15 matchende indtastninger.	31337
3	32	Forkort venligst din forsp�rgelse.	31337
3	33	Slut p� adgangsliste.	31337
3	34	Ingen match!	31337
3	35	Kan ikke tilf�je en bruger med ens eller h�jere adgang end din egen.	31337
3	36	Ugyldigt adgangs level.	31337
3	37	%s er allerede tilf�jet til %s med adgangs level %i.	31337
3	38	Tilf�jet bruget %s til %s med adgangs level %i	31337
3	39	Noget gik galt: %s	31337
3	40	%s: Slut af ban liste	31337
3	41	Kan ikke vise bruger detaljer (Usynlig)	31337
3	42	Information omkring: %s (%i)	31337
3	43	Nuv�rende logget p� via: %s	31337
3	44	URL: %s	31337
3	45	Sprog: %i	31337
3	46	Kanaler: %s	31337
3	47	Input Flood Antal: %i	31337
3	48	Output Flood (Bytes): %i	31337
3	49	%s er registreret af:	31337
3	50	%s - sidst set: %s siden	31337
3	51	Desc: %s	31337
3	52	Floode mig vil du? Jeg vil ikke h�re mere p� dig	31337
3	53	Jeg tror jeg har sendt dig en lille smule for meget data, jeg vil ignorere dig i et stykke tid.	31337
3	54	Ukomplet kommando	31337
3	55	For at bruge %s, skal du /msg %s@%s	31337
3	56	Sorry, Du skal v�re logget ind for at bruge denne kommando.	31337
3	57	Kanalen %s er blevet suspenderet af en cservice administrator.	31337
3	58	Din adgang p� %s er blevet suspenderet.	31337
3	59	NOOP flaget er sat p� %s	31337
3	60	STRICTOP flaget flaget er sat p� %s	31337
3	61	Du har lige deop'et flere end %i folk	31337
3	62	SYNTAKS: %s	31337
3	63	Midlertidigt forh�jet din adgang p� kanal %s til %i	31337
3	64	%s er registreret.	31337
3	65	%s er ikke registreret.	31337
3	66	Jeg tror ikke %s ville v�rts�tte det.	31337
3	67	\002*** Ban Liste for kanal %s ***\002	31337
3	68	%s %s Level: %i	31337
3	69	TILF�JET AF: %s (%s)	31337
3	70	SIDEN: %s	31337
3	71	UDL: %s	31337
3	72	\002*** SLUT ***\002	31337
3	73	Sorry, Jeg ved ikke hvem %s er.	31337
3	74	Sorry, du er ikke l�ngere autoriseret med mig.	31337
3	75	%s Ser ikke ud til at have adgang i %s.	31337
3	76	Kan ikke �ndre en bruger med ens eller h�jere adgang end din egen.	31337
3	77	Kan ikke give en bruger h�jere eller ens adgang end din egen.	31337
3	78	�ndrede %s's adgangs level p� kanal %s til %i	31337
3	79	Satte Automatisk MODE til OP for %s p� kanal %s	31337
3	80	Satte Automatisk MODE til VOICE for %s p� kanal %s	31337
3	81	Automatisk MODE for %s p� kanal %s er nu sl�et fra	31337
3	82	Dit kodeord kan ikke v�re dit brugernavn eller nuv�rende nick - syntaks er: NEWPASS <nyt kodeord>	31337
3	83	Kodeordet er �ndret med succes.	31337
3	84	NOOP flaget er sat p� %s	31337
3	85	STRICTOP flaget er sat p� %s (og %s er ikke autoriseret)	31337
3	86	STRICTOP flaget er sat p� %s (og %s har ikke tilstr�kkelig adgang)	31337
3	87	Slettet kanal %s	31337
3	88	%s er allerede registeret med mig.	31337
3	89	Ugyldigt kanal navn.	31337
3	90	Registrerede kanal %s	31337
3	91	Har fjernet %s fra min ignorerings liste	31337
3	92	Kunne ikke finde %s i min ignorerings liste	31337
3	93	Kan ikke fjerne en bruger med ens eller h�jere adgang end din egen	31337
3	94	Du kan ikke fjerne dig selv fra en kanal du ejer	31337
3	95	Har fjernet bruger %s fra %s	31337
3	96	Din USYNLIG indstilling er nu sat til ON.	31337
3	97	Din USYNLIG indstilling er nu sat til OFF.	31337
3	98	%s for %s er %s	31337
3	99	V�rdig af %s skal v�re ON eller OFF	31337
3	100	Ugyldig USERFLAGS indstilling. Korrekte v�rdiger er 0, 1, 2.	31337
3	101	USERFLAGS for %s er %i	31337
3	102	V�rdi af MASSDEOPPRO skal v�re 0-7	31337
3	103	MASSDEOPPRO for %s er sat til %d	31337
3	104	V�rdi af FLOODPRO skal v�re 0-7	31337
3	105	FLOODPRO for %s er sat til %d	31337
3	106	DESKRIPTION kan max have 80 karaktere!	31337
3	107	DESKRIPTION for %s er fjernet.	31337
3	108	DESKRIPTION for %s er: %s	31337
3	109	URL'en kan max have 75 karaktere!	31337
3	110	URL for %s er fjernet.	31337
3	111	URL for %s er: %s	31337
3	112	Strengen af n�gleord kan ikke overtr�de 80 karaktere!	31337
3	113	N�GLEORD for %s er: %s	31337
3	114	Sprog er sat til %s.	31337
3	115	FEJL: Ugyldigt sprogvalg.	31337
3	116	Kan ikke finde kanal %s p� netv�rket!	31337
3	117	FEJL: Ugyldig kanal indstilling.	31337
3	118	Ignore liste:	31337
3	119	Ignore listen er tom	31337
3	120	-- Slut p� Ignore Liste	31337
3	121	CMaster Kanal Services intern status:	31337
3	122	[        Kanal Statistik] \002Cached Indtastninger:\002 %i    \002DB Foresp�rgsler:\002 %i    \002Cache Hits:\002 %i    \002Udnyttelse:\002 %.2f%%	31337
3	123	[       Bruger Statistik] \002Cached Indtastninger:\002 %i    \002DB Foresp�rgsler:\002 %i    \002Cache Hits:\002 %i    \002Udnyttelse:\002 %.2f%%	31337
3	124	[Adgangs Level Statistik] \002Cached Indtastninger:\002 %i    \002DB Foresp�rgsler:\002 %i    \002Cache Hits:\002 %i    \002Udnyttelse:\002 %.2f%%	31337
3	125	[          Ban Statistik] \002Cached Indtastninger:\002 %i    \002DB Foresp�rgsler:\002 %i    \002Cache Hits:\002 %i    \002Udnyttelse:\002 %.2f%%	31337
3	126	Sidst modtaget Bruger NOTIFY: %i	31337
3	127	Sidst modtaget Kanal NOTIFY: %i	31337
3	128	Sidst modtaget Level NOTIFY: %i	31337
3	129	Sidst modtaget Ban NOTIFY: %i	31337
3	130	Custom data beholdere allokeret: %i	31337
3	131	\002Uptime:\002 %s	31337
3	132	Kanal %s har %d brugere (%i operat�rer)	31337
3	133	Mode er: %s	31337
3	134	Flag sat: %s	31337
3	135	Kan ikke suspendere en bruger med ens eller h�jere adgang end din egen.	31337
3	136	Ugyldige tids enheder.	31337
3	137	Ugyldig suspenderings periode.	31337
3	138	SUSPENDERING for %s er sl�et fra.	31337
3	139	%s er allerede suspenderet p� %s	31337
3	140	SUSPENDERING for %s vil udl�be i %s	31337
3	141	FEJL: Topic kan ikke overtr�de 145 karaktere	31337
3	142	Du har utilstr�kkelig adgang til at fjerne ban'et %s fra %s's database	31337
3	143	Fjernet %i bans der matcher %s	31337
3	144	Fjernet din midlertidige adgang af %i fra kanal %s	31337
3	145	Du ser ikke ud til at have en tvunget adgang i %s, m�ske er den udl�bet?	31337
3	146	%s er ikke suspenderet p� %s	31337
3	147	%s er en IRC operat�r	31337
3	148	%s er IKKE logget ind.	31337
3	149	%s er logget ind som %s%s	31337
3	150	%s er en Officiel CService Repr�sentativ%s og logget ind som %s	31337
3	151	%s er en Officiel CService Administrator%s og logget ind som %s	31337
3	152	%s er en Officiel CService Udvikler%s og logget ind som %s	31337
3	153	Der er flere end %i indtastninger der matcher [%s]	31337
3	154	Forkort venligst din s�ge maske	31337
3	155	Ingen matchende indtastninger for [%s]	31337
3	156	%s: Kanal modes fjernet.	31337
3	158	Ugyldig indstilling.	31337
3	159	%s er en Officiel Undernet Service bot.	31337
3	160	%s er en Officiel Coder-Com Repr�sentativ%s og logget ind som %s	31337
3	161	%s er en Officiel Coder-Com Medarbejder%s og logget ind som %s	31337
3	162	%s er en Officiel Coder-Com Udvikler%s og logget ind som %s	31337
3	163	%s er en Officiel Coder-Com Senior%s og logget ind som %s	31337
3	164	 og en IRC operat�r	31337
3	165	Tilf�jede ban %s til %s p� level %i	31337
3	166	%s: ban liste er tom.	31337
3	167	Jeg er allerede i den kanal!	31337
3	168	Denne kommando er reserveeret til IRC Operat�rer	31337
3	169	Jeg er ikke op'et p� %s	31337
3	170	%s for %i minuter	31337
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
6	29	** SUSPENDED ** - Expires in %s  (Level %i)	31337
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
6	70	SINCE: %s	31337
6	71	EXP: %s	31337
6	72	\002*** END ***\002	31337
6	73	Sorry, I don't know who %s is.	31337
6	74	Sorry, you are not authorized with me anymore.	31337
6	75	%s doesn't appear to have access in %s.	31337
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
6	160	%s is an Official Coder-Com Representative%s and logged in as %s	31337
6	161	%s is an Official Coder-Com Contributer%s and logged in as %s	31337
6	162	%s is an Official Coder-Com Developer%s and logged in as %s	31337
6	163	%s is an Official Coder-Com Senior%s and logged in as %s	31337
6	164	 and an IRC operator	31337
6	165	Added ban %s to %s at level %i	31337
6	166	%s: ban list is empty.		31337
6	167	I'm already in that channel!	31337
6	168	This command is reserved to IRC Operators	31337
6	169	I'm not opped on %s	31337
6	170	%s for %i minutes	31337
\.

-- Italian language definition.
-- 03/03/07 - Orlando Bassotto <future@break.net>.
-- 04/03/07 - Ombretta Gorini <ombra.g@galactica.it>.

COPY "translations" FROM stdin;
7	1	Spiacente, sei gi� autenticato come %s	31337
7	2	AUTENTICAZIONE RIUSCITA come %s	31337
7	3	Spiacente, non hai un accesso sufficiente per eseguire quel comando	31337
7	4	Spiacente, il canale %s � vuoto	31337
7	5	Non vedo nessun %s	31337
7	6	Non trovo %s nel canale %s	31337
7	7	Il canale %s non risulta essere registrato	31337
7	8	Ti � stato dato l'op da %s (%s)	31337
7	9	Ti � stato dato il voice da %s (%s)	31337
7	10	%s: Non sei in quel canale	31337
7	11	%s ha gi� l'op in %s	31337
7	12	%s ha gi� il voice in %s	31337
7	13	Ti � stato tolto l'op da %s (%s)	31337
7	14	Ti � stato tolto il voice da %s (%s)	31337
7	15	%s non ha l'op in %s	31337
7	16	%s non ha il voice in %s	31337
7	17	AUTENTICAZIONE FALLITA come %s (Password non valida)	31337
7	18	Non mi trovo in quel canale!	31337
7	19	Range del livello del ban non valido. Il range valido � compreso tra 1 e %i.	31337
7	20	Durata del ban non valida. La durata massima di un ban pu� essere di 336 ore.	31337
7	21	Il motivo del ban non pu� superare i 128 caratteri	31337
7	22	Il ban specificato � gi� presente nella mia lista dei ban!	31337
7	23	Il ban %s � gi� coperto da %s	31337
7	24	Livello minimo non valido.	31337
7	25	Livello massimo non valido.	31337
7	26	UTENTE: %s ACCESSO: %s %s	31337
7	27	CANALE: %s -- AUTOMODE: %s	31337
7	28	ULTIMA MODIFICA: %s (%s f�)	31337
7	29	** SOSPESO ** - Scade il %s (Level %i)	31337
7	30	VISTO L'ULTIMA VOLTA: %s.	31337
7	31	Sono presenti pi� di 15 corrispondenze.	31337
7	32	Per favore, restringi il campo di ricerca dell'interrogazione.	31337
7	33	Fine della lista degli accessi	31337
7	34	Nessuna Corrispondenza!	31337
7	35	Non puoi aggiungere un utente con un livello di accesso uguale o superiore al tuo.	31337
7	36	Livello di accesso non valido.	31337
7	37	%s � gi� stato aggiunto in %s con livello di accesso %i.	31337
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
7	49	%s � registrato da:	31337
7	50	%s - visto l'ultima volta: %s 	31337
7	51	Desc: %s	31337
7	52	Vuoi floodarmi? Bene bene, allora mi sa che da adesso non ti ascolto pi�.	31337
7	53	Penso che abbia visto abbastanza, adesso ti ignoro per un po'.	31337
7	54	Comando non completo	31337
7	55	Per usare %s, devi scrivere /msg %s@%s	31337
7	56	Spiacente, devi autenticarti per usare questo comando.	31337
7	57	Il canale %s � stato sospeso da un amministratore di CService.	31337
7	58	Il tuo accesso su %s � stato sospeso.	31337
7	59	Il flag NOOP � impostato su %s	31337
7	60	Il flag STRICTOP � impostato su %s	31337
7	61	Hai appena tolto l'op a pi� di %i persone	31337
7	62	SINTASSI: %s	31337
7	63	Il tuo accesso � stato temporaneamente incrementato sul canale %s a %i	31337
7	64	%s � registrato.	31337
7	65	%s non � registrato.	31337
7	66	Non penso che %s possa apprezzarlo.	31337
7	67	\002*** Lista dei Ban per il canale %s ***\002	31337
7	68	%s %s Livello: %i	31337
7	69	AGGIUNTO DA: %s (%s)	31337
7	70	DAL: %s	31337
7	71	SCADENZA: %s	31337
7	72	\002*** FINE ***\002	31337
7	73	Spiacente, ma non conosco chi sia %s.	31337
7	74	Spiacente, non sei pi� autorizzato con me.	31337
7	75	%s non sembra avere accesso in %s.	31337
7	76	Non posso modificare un utente con un livello di accesso uguale o superiore al tuo.	31337
7	77	Non puoi dare un accesso uguale o superiore al tuo a un altro utente.	31337
7	78	Modificato il livello di accesso per %s sul canale %s a %i	31337
7	79	Impostato AUTOMODE a OP per %s sul canale %s	31337
7	80	Impostato AUTOMODE a VOICE per %s sul canale %s	31337
7	81	Impostato AUTOMODE a NONE per %s sul canale %s	31337	
7	82	La tua password non pu� essere il tuo username o il tuo attuale nick - la sintassi �: NEWPASS <nuova password>	31337
7	83	La password � stata cambiata con successo.	31337
7	84	Il flag NOOP � impostato su %s	31337
7	85	Il flag STRICTOP � impostato su %s (e %s non � autenticato)	31337
7	86	Il flag STRICTOP � impostato su %s (e %s non ha accesso sufficiente)	31337
7	87	Purgato il canale %s	31337
7	88	%s � gi� registrato da me.	31337
7	89	Nome del canale non valido.	31337
7	90	Registrato il canale %s	31337
7	91	%s � stato rimosso dalla lista dei silence	31337
7	92	Non trovo %s nella mia lista dei silence	31337
7	93	Non puoi rimuovere un utente con accesso uguale o superiore al tuo	31337
7	94	Non puoi rimuovere te stesso da un canale che possiedi	31337
7	95	Rimosso l'utente %s da %s	31337
7	96	L'impostazione INVISIBLE adesso � attiva (ON).	31337
7	97	L'impostazione INVISIBLE adesso non � attiva (OFF).	31337
7	98	%s per %s � %s	31337
7	99	il valore di %s dev'essere ON oppure OFF	31337
7	100	Impostazione USERFLAGS non valida. I valori corretti sono 0, 1 o 2.	31337
7	101	USERFLAGS per %s � %i	31337
7	102	il valore di MASSDEOPPRO dev'essere compreso tra 0 e 7 inclusi	31337
7	103	MASSDEOPPRO per %s � impostato a %d	31337
7	104	il valore di FLOODPRO dev'essere compreso tra 0 e 7 inclusi	31337
7	105	FLOODPRO per %s � impostato a %d	31337
7	106	La DESCRIPTION non pu� superare gli 80 caratteri.	31337
7	107	La DESCRIPTION per %s � stata cancellata.	31337
7	108	La DESCRIPTION per %s �: %s	31337
7	109	L'URL non pu� superare i 75 caratteri.	31337
7	110	L'URL per %s � stata cancellata.	31337
7	111	L'URL per %s �: %s	31337
7	112	La stringa contenente le parole chiave non pu� superare gli 80 caratteri.	31337
7	113	Le KEYWORDS (parole chiave) per %s sono: %s	31337
7	114	La lingua selezionata � %s.	31337
7	115	ERRORE: Selezione della lingua non valida.	31337
7	116	Non trovo il canale %s nella rete!	31337
7	117	ERRORE: Impostazione del canale non valida.	31337
7	118	Lista degli Ignore:	31337
7	119	La lista degli Ignore � vuota	31337
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
7	133	Mode �: %s	31337
7	134	Flags impostati: %s	31337
7	135	Non posso sospendere un utente con accesso uguale o superiore al tuo.	31337
7	136	unit� di tempo sconosciuta	31337
7	137	Durata della sospensione non valida.	31337
7	138	La SOSPENSIONE per %s � stata annullata	31337
7	139	%s � gi� stato sospeso in %s	31337
7	140	La SOSPENSIONE per %s scadr� in %s	31337
7	141	ERRORE: Il topic non pu� superare i 145 caratteri	31337
7	142	Non hai accesso sufficiente per rimuovere il ban %s dal database di %s.	31337
7	143	Sono stati rimossi %i ban che corrispondono a %s	31337
7	144	Rimosso temporaneamente il tuo accesso di %i dal canale %s	31337
7	145	Non sembra che tu abbia un accesso forzato in %s, forse � scaduto?	31337
7	146	%s non � sospeso in %s	31337
7	147	%s � un operatore IRC	31337
7	148	%s NON � autenticato.	31337
7	149	%s � autenticato come %s%s	31337
7	150	%s � un Rappresentante Ufficiale di CService%s e autenticato come %s	31337
7	151	%s � un Ammistratore Ufficiale di CService%s e autenticato come %s	31337
7	152	%s � uno Sviluppatore Ufficiale di CService%s e autenticato come %s	31337
7	153	Ci sono pi� di %i corrispondenze di [%s]	31337
7	154	Per favore, restringi la tua maschera di ricerca	31337
7	155	Non ci sono corrispondenze per [%s]	31337
7	156	%s: Mode del canale annullati.	31337
7	158	Opzione non valida.	31337
7	159	%s � un Bot di Servizio Ufficiale di Undernet.	31337
7	160	%s is an Official Coder-Com Representative%s and logged in as %s	31337
7	161	%s is an Official Coder-Com Contributer%s and logged in as %s	31337
7	162	%s is an Official Coder-Com Developer%s and logged in as %s	31337
7	163	%s is an Official Coder-Com Senior%s and logged in as %s	31337
7	165	Aggiunto il ban %s a %s con livello %i	31337
7	166	%s: lista dei ban vuota.		31337
7	167	Sono gi� in quel canale!	31337
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
8	29	** SUSPENDAT ** - Expira in %s (Level %i)	31337
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
8	70	DE LA: %s	31337
8	71	EXP: %s	31337
8	72	\002*** SFARSIT ***\002	31337
8	73	Nu stiu cine e %s.	31337
8	74	Nu mai esti autorizat.	31337
8	75	%s nu pare sa aiba acces pe %s.	31337
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

-- Catalan language definition.
-- 09/03/2001 - Algol <algol@undernet.org>.

COPY "translations" FROM stdin;
9	1	Ho sento, ja est�s autentificat/da com a %s	31337
9	2	AUTENTICACI� REEIXIDA com a %s	31337
9	3	Ho sento, el teu acc�s �s insuficient per efectuar aquesta comanda.	31337
9	4	Ho sento, el canal %s �s buit.	31337
9	5	No veig cap %s enlloc.	31337
9	6	No trobo cap %s al canal %s	31337
9	7	El canal %s no consta com a enregistrat.	31337
9	8	Has rebut op de %s (%s).	31337
9	9	Has rebut veu de %s (%s).	31337
9	10	%s: No ets a aquest canal.	31337
9	11	%s ja t� op a %s	31337
9	12	%s ja t� veu a %s	31337
9	13	L'op t'ha estat llevat per %s (%s).	31337
9	14	La veu t'ha estat llevada per %s (%s).	31337
9	15	%s no t� op a %s	31337
9	16	%s no t� veu a %s	31337
9	17	AUTENTICACI� FALLIDA com a %s (Contrasenya no v�lida).	31337
9	18	No s�c pas a aquest canal!	31337
9	19	Nivell de bandeig no v�lid. Els nivells v�lids s�n entre 1-%i.	31337
9	20	Durada de bandeig no v�lida . La durada m�xima del bandeig �s de 336 hores.	31337
9	21	La ra� del ban no pot excedir de 128 car�cters.	31337
9	22	El bandeig especificat ja �s a la meva llista!	31337
9	23	El bandeig %s ja queda cobert per %s	31337
9	24	Nivell m�nim no v�lid.	31337
9	25	Nivell m�xim no v�lid.	31337
9	26	USUARI/A: %s ACC�S: %s %s	31337
9	27	CANAL: %s -- AUTOMODE: %s	31337
9	28	DARRERA MODIFICACI�: %s (fa %s)	31337
9	29	** SUSP�S/A ** - Expira d'aqu� a %s (Level %i)	31337
9	30	VIST/A PER DARRERA VEGADA: fa %s.	31337
9	31	Hi ha m�s de 15 entrades coincidents.	31337
9	32	Si et plau, restringeix la teva cerca.	31337
9	33	Fi de la llista d'accessos.	31337
9	34	Cap Coincid�ncia!	31337
9	35	No pots afegir cap usuari/a amb un nivell d'acc�s igual o superior al teu.	31337
9	36	Nivell d'acc�s no v�lid.	31337
9	37	%s ja t� acc�s a %s amb nivell %i.	31337
9	38	Afegit l'usuari/a %s a %s amb nivell d'acc�s %i.	31337
9	39	Quelcom no ha anat b�: %s	31337
9	40	%s: Fi de la llista de bandeigs.	31337
9	41	Els detalls de l'usuari/a no es poden veure pas (Invisible).	31337
9	42	Informaci� sobre: %s (%i)	31337
9	43	Connexi� actual autenticada: %s	31337
9	44	URL: %s	31337
9	45	Idioma: %i	31337
9	46	Canals: %s	31337
9	47	Punts de Flood d'Entrada: %i	31337
9	48	Flood de Sortida (Bytes): %i	31337
9	49	%s est� enregistrat per:	31337
9	50	%s - vist/a per darrer cop: fa %s	31337
9	51	Desc: %s	31337
9	52	Em vols floodejar, oi? Doncs no t'escoltar� m�s.	31337
9	53	Em penso que ja m'has fet enviar-te massa dades, ara t'ignorar� una estona.	31337
9	54	Comanda incompleta.	31337
9	55	Per fer %s, has de fer /msg %s@%s	31337
9	56	Ho sento, t'has d'haver autenticat per utilitzar aquesta comanda.	31337
9	57	El canal %s ha estat susp�s per una/a administrador/a de CService.	31337
9	58	El teu acc�s a %s ha estat susp�s.	31337
9	59	L'opci� NOOP est� activa a %s	31337
9	60	L'opci� STRICTOP est� activa a %s	31337
9	61	Acabes de llevar l'op a m�s de %i usuari/es.	31337
9	62	SINTAXI: %s	31337
9	63	El teu acc�s al canal %s ha estat incrementat temporalment fins %i.	31337
9	64	%s est� enregistrat.	31337
9	65	%s no est� enregistrat.	31337
9	66	No crec que a %s li fes gr�cia.	31337
9	67	\002*** Llista de Bandeigs al canal %s ***\002	31337
9	68	%s %s Nivell: %i	31337
9	69	AFEGIT PER: %s (%s)	31337
9	70	DES DE: %s	31337
9	71	EXP: %s	31337
9	72	\002*** FI ***\002	31337
9	73	Ho sento, no s� qui �s %s.	31337
9	74	Ho sento, ja no est�s autenticat/da.	31337
9	75	%s no figura amb acc�s a %s	31337
9	76	No pots modificar cap usuari/a amb nivell d'acc�s igual o superior al teu.	31337
9	77	No pots donar a un/a usuari/a un acc�s de nivell igual o superior al teu.	31337
9	78	El nivell d'acc�s de %s al canal %s s'ha canviat a %i.	31337
9	79	AUTOMODE canviat a OP per a %s al canal %s	31337
9	80	AUTOMODE canviat a VOICE pera %s al canal %s	31337
9	81	AUTOMODE canviat a NONE per a %s al canal %s	31337	
9	82	La teva contrasenya no pot pas ser el teu nom d'usuari/a ni el teu nick actual - la sintaxi �s: NEWPASS <nova contrasenya>	31337
9	83	Contrasenya canviada correctament.	31337
9	84	L'opci� NOOP est� activada a %s	31337
9	85	L'opci� STRICTOP est� activada a %s (i %s no s'ha autenticat).	31337
9	86	L'opci� STRICTOP est� activada a %s (i %s t� un acc�s insuficient).	31337
9	87	El canal %s ha estat purgat.	31337
9	88	%s ja est� enregistrat.	31337
9	89	Nom de canal no v�lid.	31337
9	90	El canal %s ha estat enregistrat.	31337
9	91	%s ha estat esborrat de la meva llista de silencis.	31337
9	92	No he trobat %s a la meva llista de silencis.	31337
9	93	No pots suprimir cap usuari amb un acc�s igual o superior al teu.	31337
9	94	No pots suprimir el teu propi acc�s a un canal que et pertany.	31337
9	95	S'ha suprimit l'usuari/a %s a %s	31337
9	96	La teva opci� INVISIBLE �s ara activa (ON).	31337
9	97	La teva opci� INVISIBLE �s ara inactiva (OFF).	31337
9	98	%s a %s �s %s	31337
9	99	el valor de %s ha d'�sser 'ON' o 'OFF'.	31337
9	100	Valor d'USERFLAGS no v�lid. Els valors correctes s�n 0, 1, 2.	31337
9	101	USERFLAGS per %s �s %i.	31337
9	102	el valor de MASSDEOPPRO ha d'estar entre 0-7.	31337
9	103	MASSDEOPPRO per %s s'ha fixat a %d.	31337
9	104	el valor de FLOODPRO ha d'estar entre 0-7.	31337
9	105	FLOODPRO per %s s'ha fixat a %d.	31337
9	106	DESCRIPTION pot tenir fins a 80 car�cters com a m�xim!	31337
9	107	La DESCRIPTION de %s ha estat esborrada.	31337
9	108	DESCRIPTION de %s �s: %s	31337
9	109	URL pot tenir fins a 75 car�cters com a m�xim!	31337
9	110	La URL de %s ha estat esborrada.	31337
9	111	La URL de %s �s: %s	31337
9	112	KEYWORDS pot tenir fins a 80 car�cters com a m�xim!	31337
9	113	Les KEYWORDS de %s s�n: %s	31337
9	114	L'idioma s'ha fixat a %s.	31337
9	115	ERROR: Selecci� d'idioma no v�lida.	31337
9	116	No puc trobar el canal %s a la xarxa!	31337
9	117	ERROR: Configuraci� de canal no v�lida.	31337
9	118	Llista d'ignoraments:	31337
9	119	La llista d'ignoraments �s buida.	31337
9	120	-- Fi de la Llista d'Ignoraments.	31337
9	121	Estat intern dels Serveis de Canals CMaster:	31337
9	122	[         Estad�stiques de Canals] \002Entrades en Cache:\002 %i    \002DB Peticions:\002 %i    \002Encerts de Cache:\002 %i    \002Efici�ncia:\002 %.2f%%	31337
9	123	[       Estad�stiques d'Usuari/es] \002Entrades en Cache:\002 %i    \002DB Peticions:\002 %i    \002Encerts de Cache:\002 %i    \002Efici�ncia:\002 %.2f%%	31337
9	124	[Estad�stiques de Nivells d'Acc�s] \002Entrades en Cache:\002 %i    \002DB Peticions:\002 %i    \002Encerts de Cache:\002 %i    \002Efici�ncia:\002 %.2f%%	31337
9	125	[       Estad�stiques de Bandeigs] \002Entrades en Cache:\002 %i    \002DB Peticions:\002 %i    \002Encerts de Cache:\002 %i    \002Efici�ncia:\002 %.2f%%	31337
9	126	Darrera recepci� de NOTIFY d'Usuari/a: %i	31337
9	127	Darrera recepci� de NOTIFY de Canal: %i	31337
9	128	Darrera recepci� de NOTIFY de Nivell: %i	31337
9	129	Darrera recepci� de NOTIFY de Bandeig: %i	31337
9	130	Contenidors de dades pr�pies assignats: %i	31337
9	131	\002En funcionament (Uptime):\002 %s	31337
9	132	El canal %s t� %d usuari/es (%i operador/es).	31337
9	133	Mode(s): %s	31337
9	134	Opcions actives: %s	31337
9	135	No pots suspendre cap usuari/a amb nivell d'acc�s igual o superior al teu.	31337
9	136	unitats de temps no v�lides.	31337
9	137	Durada de suspensi� no v�lida.	31337
9	138	La SUSPENSI� de %s ha estat cancel�lada.	31337
9	139	%s ja est� susp�s/a a %s	31337
9	140	La SUSPENSI� de %s espirar� en %s	31337
9	141	ERROR: TOPIC no pot superar els 145 car�cters.	31337
9	142	El teu acc�s �s insuficient per suprimir el bandeig %s de la base de dades de %s	31337
9	143	Suprimits %i bandeigs coincidents amb %s	31337
9	144	Suprimit el teu acc�s temporal de nivell %i al canal %s	31337
9	145	No consta que tinguis cap acc�s for�at a %s, potser ha expirat?	31337
9	146	%s no est� susp�s/a a %s	31337
9	147	%s �s un/a Operador/a d'IRC.	31337
9	148	%s NO est� autenticat/da.	31337
9	149	%s est� autenticat/da com a %s%s	31337
9	150	%s �s un/a Representant Oficial de CService%s i est� autenticat/da com a %s	31337
9	151	%s �s un/a Administrador/a Oficial de CService%s i est� autenticat/da com a %s	31337
9	152	%s �s un/a Programador/a Oficial de CService%s i est� autenticat/da com a %s	31337
9	153	Hi ha m�s de %i entrades coincidents amb [%s]	31337
9	154	Restringeix la m�scara de cerca, si et plau.	31337
9	155	Cap entrada coincident amb [%s]	31337
9	156	%s: Els modes del canal han estat esborrats.	31337
9	158	Opci� no v�lida.	31337
9	159	%s �s un Bot de Servei Oficial d'Undernet.	31337
9	160	%s �s un/a Representant Oficial de Coder-Com%s i est� autenticat/da com a %s	31337
9	161	%s �s un/a Contrbu�dor/a Oficial de Coder-Com%s i est� autenticat/da com a %s	31337
9	162	%s �s un/a Programador/a Oficial de Coder-Com%s i est� autenticat/da com a %s	31337
9	163	%s �s un/a Senior Oficial de Coder-Com%s i est� autenticat/da com a %s	31337
9	164	 i un/a Operador/a d'IRC.	31337
9	165	S'ha afegit el bandeig %s a %s amb nivell %i	31337
9	166	%s: la llista de bandeigs �s buida.		31337
9	167	Ja s�c a aquest canal!	31337
9	168	Aquesta comanda �s reservada per a Operador/es d'IRC.	31337
9	169	No tinc pas op a %s	31337
9	170	%s durant %i minuts.	31337
\.

-- Spanish language definition.
-- 09/03/2001 - Algol <algol@undernet.org>.

COPY "translations" FROM stdin;
10	1	Lo siento, ya est�s autentificado/a como %s	31337
10	2	AUTENTIFICACI�N COMPLETADA como %s	31337
10	3	Lo siento, tu acceso es insuficiente para ejecutar esta orden.	31337
10	4	Lo siento, el canal %s est� vac�o.	31337
10	5	No veo a %s en ning�n sitio.	31337
10	6	No encuentro a %s en el canal %s	31337
10	7	El canal %s no consta como registrado.	31337
10	8	Has recibido op de %s (%s)	31337
10	9	Has recibido voz de %s (%s)	31337
10	10	%s: No est�s en ese canal.	31337
10	11	%s ya tiene op en %s	31337
10	12	%s ya tiene voz en %s	31337
10	13	Te ha quitado el op: %s (%s)	31337
10	14	Te ha quitado la voz: %s (%s)	31337
10	15	%s no tiene op en %s	31337
10	16	%s no tiene voz en %s	31337
10	17	AUTENTIFICACI�N FALLIDA como %s (Contrase�a no v�lida).	31337
10	18	No estoy en ese canal!	31337
10	19	Nivel de ban no v�lido. El intervalo v�lido es 1-%i.	31337
10	20	Duraci�n de ban no v�lida. La duraci�n de ban m�xima es de 336 horas.	31337
10	21	El motivo del ban no puede superar los 128 caracteres.	31337
10	22	El ban especificado ya est� en mi lista de bans!	31337
10	23	El ban %s ya est� cubierto por %s	31337
10	24	Nivel m�nimo no v�lido.	31337
10	25	Nivel m�ximo no v�lido.	31337
10	26	USUARIO/A: %s ACCESO: %s %s	31337
10	27	CANAL: %s -- AUTOMODO: %s	31337
10	28	�LTIMA MODIFICACI�N: %s (hace %s)	31337
10	29	** SUSPENDIDO/A ** - Expira dentro de %s (Level %i)	31337
10	30	VISTO/A POR �LTIMA VEZ: hace %s.	31337
10	31	Hay m�s de 15 entradas coincidentes.	31337
10	32	Restringe tu b�squeda, por favor.	31337
10	33	Fin de la lista de accesos.	31337
10	34	No hay coincidencias!	31337
10	35	No puedes a�adir un usuario/a con un nivel igual o superior al tuyo.	31337
10	36	Nivel de acceso no v�lido.	31337
10	37	%s ya est� a�adido a %s con nivel de acceso %i.	31337
10	38	Se ha a�adido el/la usuario/a %s a %s con nivel de acceso %i	31337
10	39	Algo ha fallado: %s	31337
10	40	%s: Fin de la lista de bans.	31337
10	41	Imposible ver los detalles del/la usuario/a (Invisible).	31337
10	42	Informaci�n sobre: %s (%i)	31337
10	43	Conexi�n actual autentificada: %s	31337
10	44	URL: %s	31337
10	45	Idioma: %i	31337
10	46	Canales: %s	31337
10	47	Puntos de Flood de Entrada: %i	31337
10	48	Flood de Salida (Bytes): %i	31337
10	49	%s est� registrado por:	31337
10	50	%s - visto/a por �ltima vez: %s ago	31337
10	51	Desc: %s	31337
10	52	Pretendes floodearme? Pues no voy a hacerte m�s caso.	31337
10	53	Creo que ya te he enviado datos de sobra, ahora te ignorar� un rato.	31337
10	54	Orden incompleta	31337
10	55	Para usar %s, debes /msg %s@%s	31337
10	56	Lo siento, has de estar autentificado/a para utilizar esta orden.	31337
10	57	El canal %s ha sido suspendido por un/a administrador/a de CService.	31337
10	58	Tu acceso en %s ha sido suspendido.	31337
10	59	La opci�n NOOP est� activa en %s	31337
10	60	La opci�n STRICTOP est� activa en %s	31337
10	61	Acabas de quitar el op a m�s de %i usuarios/as.	31337
10	62	SINTAXIS: %s	31337
10	63	Tu acceso en el canal %s se ha incrementado temporalmente a %i.	31337
10	64	%s est� registrado.	31337
10	65	%s no est� registrado.	31337
10	66	Dudo que a %s le parezca bien.	31337
10	67	\002*** Lista de bans para el canal %s ***\002	31337
10	68	%s %s Nivel: %i	31337
10	69	A�ADIDO POR: %s (%s)	31337
10	70	DESDE: %s	31337
10	71	EXP: %s	31337
10	72	\002*** FIN ***\002	31337
10	73	Lo siento, no s� qui�n es %s.	31337
10	74	Lo siento, ya no est�s autentificado/a.	31337
10	75	%s no consta con acceso en %s	31337
10	76	No puedes modificar los usuarios/as con nivel de acceso igual o superior al tuyo.	31337
10	77	No puedes dar a un/a usuario/a un acceso de nivel igual o superior al tuyo.	31337
10	78	Se ha cambiado el nivel de acceso de %s en el canal %s a %i.	31337
10	79	AUTOMODE cambiado a OP para %s en el canal %s	31337
10	80	AUTOMODE cambiado a VOICE para %s en el canal %s	31337
10	81	AUTOMODE cambiado a NONE para %s en el canal %s	31337	
10	82	Tu contrase�a no puede ser tu nombre de usuario/a ni tu nick actual - la sintaxis es: NEWPASS <nueva contrase�a>	31337
10	83	Contrase�a cambiada con �xito.	31337
10	84	La opci�n NOOP est� acitavada en %s	31337
10	85	La opci�n STRICTOP est� activada en %s (y %s no est� autentificado/a).	31337
10	86	La opci�n STRICTOP est� activada en %s (y %s tiene un acceso insuficiente).	31337
10	87	El canal %s ha sido purgado.	31337
10	88	%s ya est� registrado.	31337
10	89	Nombre de canal no v�lido.	31337
10	90	El canal %s ha sido registrado.	31337
10	91	%s ha sido eliminado de mi lista de silencios.	31337
10	92	No he encontrado %s en mi lista de silencios.	31337
10	93	No puedes suprimir usuarios/as con nivel de acceso igual o superior al tuyo.	31337
10	94	No puedes suprimir tu propio acceso en un canal que es tuyo.	31337
10	95	Se ha suprimido el/la usuario/a %s de %s	31337
10	96	Tu opci�n INVISIBLE est� ahora activa (ON).	31337
10	97	Your INVISIBLE setting est� ahora inactiva (OFF).	31337
10	98	%s en %s est� %s	31337
10	99	el valor de %s s�lo puede ser 'ON' u 'OFF'.	31337
10	100	Valor de USERFLAGS no v�lido. Los valores v�lidos son 0, 1, 2.	31337
10	101	USERFLAGS en %s es %i.	31337
10	102	el valor de MASSDEOPPRO tiene que estar entre 0-7.	31337
10	103	MASSDEOPPRO en %s se ha establecido en %d.	31337
10	104	el valor de FLOODPRO tiene que estar entre 0-7.	31337
10	105	FLOODPRO en %s se ha establecido en %d.	31337
10	106	DESCRIPTION puede contener un m�ximo de 80 caracteres!	31337
10	107	La DESCRIPTION de %s ha sido borrada.	31337
10	108	La DESCRIPTION de %s es: %s	31337
10	109	URL puede contener un m�ximo de 75 caracteres!	31337
10	110	la URL de %s ha sido borrada.	31337
10	111	La URL de %s es: %s	31337
10	112	la cadena de KEYWORDS puede contener un m�ximo de 80 caracteres!	31337
10	113	KEYWORDS de %s son: %s	31337
10	114	Se ha seleccionado el idioma %s.	31337
10	115	ERROR: Selecci�n de idioma no v�lida.	31337
10	116	No puedo encontrar el canal %s en la red!	31337
10	117	ERROR: Configuraci�n de canal incorrecta.	31337
10	118	Lista de ignores:	31337
10	119	La lista de ignores est� vac�a	31337
10	120	-- Fin de la Lista de Ignores	31337
10	121	Estado interno de los Servicios de Canales CMaster:	31337
10	122	[          Estad�sticas de Registros de Canales] \002Entradas en Cache:\002 %i    \002DB Peticiones:\002 %i    \002Aciertos en Cache:\002 %i    \002Efficiency:\002 %.2f%%	31337
10	123	[         Estad�sticas de Registros de Usuarios] \002Entradas en Cache:\002 %i    \002DB Peticiones:\002 %i    \002Aciertos en Cache:\002 %i    \002Efficiency:\002 %.2f%%	31337
10	124	[Estad�sticas de Registros de Niveles de Acceso] \002Entradas en Cache:\002 %i    \002DB Peticiones:\002 %i    \002Aciertos en Cache:\002 %i    \002Efficiency:\002 %.2f%%	31337
10	125	[             Estad�sticas de Registros de Bans] \002Entradas en Cache:\002 %i    \002DB Peticiones:\002 %i    \002Aciertos en Cache:\002 %i    \002Efficiency:\002 %.2f%%	31337
10	126	�ltima recepci�n de NOTIFY de Usuario/a: %i	31337
10	127	�ltima recepci�n de NOTIFY de Canal: %i	31337
10	128	�ltima recepci�n de NOTIFY de Nivel: %i	31337
10	129	�ltima recepci�n de NOTIFY de Ban: %i	31337
10	130	Contenedores de datos propios asignados: %i	31337
10	131	\002Funcionando (Uptime):\002 %s	31337
10	132	El canal %s tiene %d usuarios/as (%i operadores/as)	31337
10	133	Modo(s): %s	31337
10	134	Opciones: %s	31337
10	135	No puedes suspender usuarios/as con un nivel de acceso igual o superior al tuyo.	31337
10	136	unidades de tiempo err�neas.	31337
10	137	Duraci�n de suspensi�n no v�lida.	31337
10	138	La SUSPENSI�N de %s ha sido cancelada	31337
10	139	%s ya est� suspendido/a en %s	31337
10	140	La SUSPENSI�N de %s expirar� dentro de %s.	31337
10	141	ERROR: TOPIC no puede contener m�s de 145 caracteres.	31337
10	142	Tu acceso es insuficiente para suprimir el ban %s de la base de datos de %s	31337
10	143	Suprimidos %i bans coincidentes con %s	31337
10	144	Se ha suprimido tu acceso temporal de nivel %i en el canal %s	31337
10	145	No consta que tengas un acceso forzado en %s, quiz�s ha expirado?	31337
10	146	%s no est� suspendido/a en %s	31337
10	147	%s es un/a Operador/a de IRC	31337
10	148	%s NO est� autentificado/a.	31337
10	149	%s est� autentificado/a como %s%s	31337
10	150	%s es un/a Representante Oficial de CService%s y est� autentificado/a como %s	31337
10	151	%s es un/a Administrador/a Oficial de %s y est� autentificado/a como %s	31337
10	152	%s es un/a Programador/a Oficial de CService%s y est� autentificado/a como %s	31337
10	153	Hay m�s de %i entradas coincidentes con [%s]	31337
10	154	Restringe la m�scara de b�squeda, por favor.	31337
10	155	No hay entradas coincidentes con [%s]	31337
10	156	%s: Los modos del canal han sido borrados.	31337
10	158	Opci�n inv�lida.	31337
10	159	%s es un Bot de Servicio Oficial de Undernet.	31337
10	160	%s es un/a Representante Oficial de Coder-Com%s y est� autentificado/a como %s	31337
10	161	%s es un/a Contribuidor Oficial de Coder-Com%s y est� autentificado/a como %s	31337
10	162	%s es un/a Programador/a Oficial de Coder-Com%s y est� autentificado/a como %s	31337
10	163	%s es un/a Senior Oficial de Coder-Com%s y est� autentificado/a como %s	31337
10	164	 y un/a Operador/a de IRC.	31337
10	165	Se ha a�adido el ban %s a %s con nivel %i.	31337
10	166	%s: la lista de bans est� vac�a.		31337
10	167	Ya estoy en ese canal!	31337
10	168	Esta orden est� reservada para Operadores/as de IRC.	31337
10	169	No tengo op en %s	31337
10	170	%s durante %i minutos.	31337
\.

-- Hungarian language definition.
-- 13/07/2001 - Laccc <blaszlo@kabelkon.ro>

COPY "translations" FROM stdin; 
11	1	Mar be vagy jelentkezve %s usernev alatt	31337
11	2	AZONOTITAS SIKERULT a(z) %s usernev alatt	31337
11	3	Nincs eleg jogod ahhoz, hogy hasznald ezt a parancsot	31337
11	4	A(z) %s csatorna ures	31337
11	5	Nem latok %s nevet sehol	31337
11	6	Nem kapok %s nevet a(z) %s csatin	31337
11	7	A(z) %s csatorna nincs meg regisztralva	31337
11	8	%s Operatori jogot adott neked (%s)	31337
11	9	%s beszelesi jogot adott neked (%s)	31337
11	10	%s: Nem tartozkodsz ezen a csatornan	31337
11	11	%s mar jenelneg is Operator a(z) %s csatornan	31337
11	12	%s mar jelenleg is voice-olva van a(z) %s csatornan	31337
11	13	%s elvette az Operatori jogodat (%s)	31337
11	14	%s elvette a beszelesi jogodat (%s)	31337
11	15	%s -nak/nek nincs Operatori jog adva a(z) %s csatornan	31337
11	16	%s -nak/nek nincs beszelesi jog adva a(z) %s csatornan	31337
11	17	AZONOSITAS SIKERTELEN a(z) %s usernev alatt (Hibas Jelszo)	31337
11	18	Nem vagyok azon a csatornan!	31337
11	19	Ervenytelen ban szint, 1-%i kozott valaszthatsz.	31337
11	20	Ervenytelen ban idotartalom. Maximum 336 orat hasznalhatsz.	31337
11	21	A Ban indokban maximum csak 128 betut addhatsz meg.	31337
11	22	A megjelolt ban mar benne van a banlistaban!	31337
11	23	A(z) %s bant mar a letezo %s ban magaba vonja!	31337
11	24	Ervenytelen minimum szint.	31337
11	25	Ervenytelen maximum szint.	31337
11	26	FELHASZNALO: %s JOG(ACCESS): %s %s	31337
11	27	CSATRONA: %s -- AUTOMODE: %s	31337
11	28	UTOLJARA MODOSITVA: %s (%s -el ezelott)	31337
11	29	** FELFUGGESZTETT ** - Veget er %s utan (Level %i)	31337
11	30	UTOLJARA VOLT: %s -el ezelott..	31337
11	31	15-nel tobb hasonlo adat van.	31337
11	32	Legyszives korlatold a keresed.	31337
11	33	Vege az access listanak	31337
11	34	Nem kaptam semmit!	31337
11	35	Nem rakhatsz fel naladnal nagyobb vagy a te jogoddal egyenlo erteku felhasznalokat.	31337
11	36	Ervenztelen access szint.	31337
11	37	%s nevu felhasznalo mar jelen van a(z) %s csatorna felhasznaloi listajaban %i joggal.	31337
11	38	%s nevu felhasznalot sikeresen hozzaadtam a(z) %s csatronahoz %i joggal	31337
11	39	Valamit nem sikerult befejeznem: %s	31337
11	40	%s: Vege a banlistanak	31337
11	41	Nem lehet latni a felhasznalo adatait (Lathatatlan)	31337
11	42	%s -rol az Informacio: (%i)	31337
11	43	Jelenleg be van jelentkezve: %s	31337
11	44	URL: %s	31337
11	45	Nyelv: %i	31337
11	46	Csatornak: %s	31337
11	47	Bejovo Flood pontok: %i	31337
11	48	Kimeno Flood (Bytes): %i	31337
11	49	A(z) %s csatorna regisztralva van:	31337
11	50	%s altal - Utoljara %s -el ezelott volt	31337
11	51	Koruliras: %s	31337
11	52	Akarsz floodolni, nemde? Tobbet nem halgatok rad.	31337
11	53	Azt hiszem kicsit sok adatot kuldtem neked, ignoralni foglak egy darabig.	31337
11	54	Befejezetlen parancs	31337
11	55	Hogy hasznalhasd %s -t, ahhoz /msg %s@%s -t kell hasznalj	31337
11	56	Eloszor azonositsd magad, mielott hasznalnad ezt a parancsot.	31337
11	57	A(z) %s csatornat felfuggesztette egy cservice administrator.	31337
11	58	A(z) %s csatornan levo jogod fel van fuggesztve.	31337
11	59	A NOOP opcio erteke: %s	31337
11	60	A STRICTOP opcio erteke %s	31337
11	61	Tobb mint %i embernek vetted el az Operatori jogat	31337
11	62	SZINTAXIS: %s	31337
11	63	Ideiglenesen a(z) %s csatornan %i -re nott a jogod.	31337
11	64	%s regisztralva van .	31337
11	65	%s nincs regisztralva.	31337
11	66	Nem hinnem, hogy %s ertekelne ezt.	31337
11	67	\002*** A %s csatorna Ban Listaja ***\002	31337
11	68	%s %s Szint: %i	31337
11	69	FELTETTE: %s (%s)	31337
11	70	OTA: %s	31337
11	71	LEJAR: %s	31337
11	72	\002*** VEGE ***\002	31337
11	73	Nem tudom ki az a(z) %s.	31337
11	74	Mar nem vagy azonositva.	31337
11	75	%s -nak/nek nincs joga a(z) %s csatornahoz.	31337
11	76	Nem tudod modositani egyenlo- vagy magasabb erteku joggal rendelkezo szemely adatait.	31337
11	77	Nem tudsz a te jogoddal egyenlo- vagy magasabb jogot adni.	31337
11	78	%s joga %s lett a(z) %i csatornan.	31337
11	79	%s AUTOMODE erteke most OP a(z) %s csatornan	31337
11	80	%s AUTOMODE erteke most VOICE a(z) %s csatornan	31337
11	81	%s AUTOMODE erteke most NONE a(z) %s csatornan	31337	
11	82	A jelszo nem egyezhet meg a userneveddel sem a nickneveddel - szintaxis : NEWPASS <uj jelszo>	31337
11	83	Sikeresen megvaltoztattam a jelszavad.	31337
11	84	A NOOP opcio erteke: %s	31337
11	85	A STRICTOP opcio erteke %s (es %s nincs azonositva)	31337
11	86	A STRICTOP opico erteke %s (es %s -nak/nek nincs eleg joga)	31337
11	87	A(z) %s torolt csatorna.	31337
11	88	%s mar jelenleg is regisztralva van.	31337
11	89	Ervenytelen csatorna nev.	31337
11	90	A(z) %s regisztralt csatorna.	31337
11	91	Letoroltem %s -t a silence listamrol	31337
11	92	Nem kapom %s -t a silence listamban	31337
11	93	Nem tudsz letorolni veled egyerteku- vagy magasabb joggal rendelkezo felhasznalot. 	31337
11	94	Nem tudod letorolni magad olyan csatornarol, ami a tied.	31337
11	95	Leszedtem %s -t a(z) %s csatornarol	31337
11	96	Az informacioid most lathatatnakok (INVISIBLE ON).	31337
11	97	Az informacioid most lathatoak (INVISIBLE OFF).	31337
11	98	A(z) %s %s -nak most %s	31337
11	99	A(z) %s erteke ON vagy OFF kell legyen	31337
11	100	Ervenytelen USERFLAGS beallitas. Ervenyes ertekek: 0, 1, 2.	31337
11	101	USERFLAGS erteke %s -nak most %i	31337
11	102	A MASSDEOPPRO erteke 0-7 kozott kell legyen	31337
11	103	A %s csatorna MASSDEOPPRO opciojanak az erteke %d	31337
11	104	A FLOODPRO erteke 0-7 kozott kell legyen	31337
11	105	A %s csatorna FLOODPRO opciojanak erteke %d	31337
11	106	A KORULIRAS maximum 80 betus lehet!	31337
11	107	A(z) %s csatorna KORULIRASA letorolve.	31337
11	108	A(z) %s csatorna KORULIRASA: %s	31337
11	109	Az URL maximum 75 betus lehet!	31337
11	110	A(z) %s csatorna URL-je letorolve.	31337
11	111	A(z) %s csatorna URL erteke: %s	31337
11	112	A csatorna kulcsszavainak hossza nem lehet tobb mint 80 betu!	31337
11	113	A(z) %s csatorna kulcsszavai: %s	31337
11	114	A nyelv mostantol Magyar (%s).	31337
11	115	HIBA: Ervenytelen nyelv valasztas.	31337
11	116	Nem kapom a(z) %s csatornat a halozaton!	31337
11	117	HIBA: Ervenytelen csatorna beallitas.	31337
11	118	Ignore lista:	31337
11	119	Az ignore lista ures	31337
11	120	-- Vege az ignore listanak	31337
11	121	CMaster Channel Services belso helyzete:	31337
11	122	[     Csatorna Record Helyzet] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337
11	123	[  Felhasznalo Record Helyzet] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337
11	124	[ Access szint Record Helyzet] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337
11	125	[          Ban Record Helyyet] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337
11	126	Utoljara kapott felhasznalo NOTIFY: %i	31337
11	127	Utoljara kapott csatorna NOTIFY: %i	31337
11	128	Utoljara kapott szint NOTIFY: %i	31337
11	129	Utoljara kapott Ban NOTIFY: %i	31337
11	130	A szokasos adat tartalmazok kiutalva: %i	31337
11	131	\002Uptime:\002 %s	31337
11	132	A(z) %s csatornanak %d felhasznaloja van (%i Operator)	31337
11	133	A csatorna mod: %s	31337
11	134	Opicok: %s	31337
11	135	Nem tudsz felfuggeszteni veled egyerteku- vagy magasabb joggal rendelkezo felhasznalokat.	31337
11	136	Hibas ido egysegek	31337
11	137	Hibas felfuggesztesi idotartalom.	31337
11	138	%s FELFUGGESZTESE visszavonva	31337
11	139	%s mar jelenleg is fel van fuggesztve a(z) %s csatornan	31337
11	140	%s FELFUGGESZTESE %s idon belul jar le	31337
11	141	HIBA: A topicban nem lehet tobb mint 145 betu	31337
11	142	Nincs eleg jogod ahhoz, hogy leszedjed a(z) %s bant a(z) %s csatornarol	31337
11	143	Leszedtem %i olyan bant, ami talalt %s -el.	31337
11	144	Leszedtem a(z) %i ideiglenes jogodat a(z) %s csatirol	31337
11	145	Nem ugy nez ki mintha nem lenne eroltetett jogod a(z) %s csatornan, lehet hogy lejart volna?	31337
11	146	%s nincs felfuggesztve a(z) %s csatornan	31337
11	147	%s egy IRC operator	31337
11	148	%s NINCS azonositva.	31337
11	149	%s azonositva van, userneve: %s%s	31337
11	150	%s egy Official CService Representative%s es %s a userneve	31337
11	151	%s egy Official CService Administrator%s es %s a userneve	31337
11	152	%s egy Official CService Developer%s es %s a userneve	31337
11	153	Tobb mint %i informacio talal. [%s]	31337
11	154	Legyszives korlatold a keresesi maszkot.	31337
11	155	Nem kaptam semmit [%s] -ra	31337
11	156	%s: Letoroltem a csatorna modokat.	31337
11	158	Ervenytelen opcio.	31337
11	159	%s egy Official Undernet Service Bot.	31337
11	160	%s egy Official Coder-Com Representative%s es %s a userneve	31337
11	161	%s egy Official Coder-Com Contributer%s es %s a userneve	31337
11	162	%s egy Official Coder-Com Developer%s es %s a userneve	31337
11	163	%s egy Official Coder-Com Senior%s es %s a userneve	31337
11	164	 es egy IRC operator	31337
11	165	Feltettem a(z) %s bant a(z) %s csatornara %i szintre.	31337
11	166	%s: Ures a ban lista.		31337
11	167	Mar jelenleg is bennt vagyok azon a csatornan!	31337
11	168	Ezt a parancsot csak IRC Operatorok hasznalhatjak.	31337
11	169	Nem vagyok op a(z) %s csatornan	31337
11	170	%s %i percre	31337
\.

-- Turkish language definition.
-- 21/08/01 - Mehmet Ak�in <cavalry@ircdestek.org>.
-- 21/08/01 - Ozan Ferah <claymore@ircdestek.org>.

COPY "translations" FROM stdin;
12	1	�zg�n�m, %s olarak zaten tan�ml�s�n�z.	31337
12	2	%s olarak TANIMLAMA BA�ARILI	31337
12	3	�zg�n�m, o komutu uygulamak i�in yeterli eri�iminiz yok	31337
12	4	�zg�n�m, %s kanal� bo�	31337
12	5	%s hi�bir yerde g�r�nm�yor	31337
12	6	%s rumuzunu %s kanal�nda g�remiyorum	31337
12	7	%s kanal� kay�tl� olarak g�z�km�yor	31337
12	8	%s size operat�rl�k yetkisi verdi (%s)	31337
12	9	%s size konu�ma yetkisi verdi (%s)	31337
12	10	%s: Siz kanalda de�ilsiniz	31337
12	11	%s zaten %s kanal�nda operat�r durumunda	31337
12	12	%s zaten %s kanal�nda konu�ma durumunda	31337
12	13	%s taraf�ndan operat�rl�k yetkiniz al�nd� (%s)	31337
12	14	%s taraf�ndan konu�ma yetkiniz al�nd� (%s)	31337
12	15	%s i�in %s kanal�nda operat�rl�k yetkisi yok	31337
12	16	%s i�in %s kanal�nda konu�ma yetkisi yok	31337
12	17	%s kanal�nda TANIMLAMA BA�ARISIZ (Ge�ersiz �ifre)	31337
12	18	Ben o kanalda de�ilim!	31337
12	19	Ge�ersiz yasaklama-seviye dizisi. Ge�erli dizi 1-%i olmal�.	31337
12	20	Ge�ersiz yasaklama s�resi. Sizin yasaklama s�reniz en fazla 336 saat olabilir.	31337
12	21	Yasaklama sebebi 128 karakteri a�mamal�d�r	31337
12	22	Belirtilen yasaklama zaten benim yasakl�lar listemde!	31337
12	23	%s yasaklamas� zaten %s ile kapsanm�� durumda	31337
12	24	Ge�ersiz en az seviye.	31337
12	25	Ge�ersiz en fazla seviye.	31337
12	26	KULLANICI: %s ER���M: %s %s	31337
12	27	KANAL: %s -- OTOMOD: %s	31337
12	28	SON DE����M: %s (%s �nce)	31337
12	29	** ASKIYA ALINMI� ** - %s i�inde s�resi dolacak (Seviye %i)	31337
12	30	SON G�R��: %s �nce.	31337
12	31	15'den fazla e�le�en giri� var.	31337
12	32	L�tfen soruyu s�n�rland�r�n.	31337
12	33	Eri�im listesi sonu	31337
12	34	E�le�me Yok!	31337
12	35	Sizikine e�it ya da y�ksek seviye ile kullan�c� ekleyemezsiniz.	31337
12	36	Ge�ersiz eri�im seviyesi.	31337
12	37	%s zaten %s kanal�nda %i eri�im seviyesi ile eklenmi�.	31337
12	38	%s kullan�c�s� %s kanal�nda %i eri�im seviyesi ile eklendi	31337
12	39	Bir�eyde aksama oldu: %s	31337
12	40	%s: Yasakl�lar listesi sonu	31337
12	41	Kullan�c� detaylar� listelenemedi (G�r�nmez)	31337
12	42	Hakk�nda bilgiler: %s (%i)	31337
12	43	�u anda giri� yapm�� olan: %s	31337
12	44	Site: %s	31337
12	45	Lisan: %i	31337
12	46	Kanallar: %s	31337
12	47	Giren Y�klenme Puan�: %i	31337
12	48	��kan Y�klenme (Bit): %i	31337
12	49	%s kanal�n� kay�t ettirmi� olan:	31337
12	50	%s - son g�r��: %s �nce	31337
12	51	Tan�m: %s	31337
12	52	Bana y�kleme yapmaya devam edecek misin? Bundan sonra seni dinlemeyece�im	31337
12	53	D���n�yorum da sana biraz fazla bilgi yollad�m, seni bir s�re �nemsemeyece�im.	31337
12	54	Noksan komut	31337
12	55	%s komutunu kullanmak i�in, yapman�z gereken /msg %s@%s	31337
12	56	�zg�n�m, bu komutu kullanmak i�in tan�mlanmal�s�n�z.	31337
12	57	%s kanal� bir cservice y�neticisi taraf�ndan ge�ici olarak kapat�lm��.	31337
12	58	%s kanal�ndaki eri�iminiz ask�ya al�nm��.	31337
12	59	%s kanal�nda NOOP �zelli�i aktif hale getirilmi�tir	31337
12	60	%s kanal�nda STRICTOP �zelli�i aktif hale getirilmi�tir	31337
12	61	Biraz �nce %i ki�iden fazlas�ndan operat�rl�k yetkisi ald�n�z	31337
12	62	S�ZD�Z�M�: %s	31337
12	63	%s kanal�nda eri�iminiz ge�ici olarak %i olarak y�kseltildi	31337
12	64	%s kay�tl�.	31337
12	65	%s kay�ts�z.	31337
12	66	%s kanal�n�n bunu takdir edece�ini sanm�yorum.	31337
12	67	\002*** %s i�in Yasakl�lar Listesi ***\002	31337
12	68	%s %s Seviye: %i	31337
12	69	EKLEYEN: %s (%s)	31337
12	70	BA�LANGI�: %s	31337
12	71	SONA ER��: %s	31337
12	72	\002*** SON ***\002	31337
12	73	%s kimdir bilmiyorum.	31337
12	74	Art�k bende tan�ml� de�ilsiniz.	31337
12	75	%s %s kanal�nda eri�imli g�z�km�yor.	31337
12	76	Sizinkine e�it ya da fazla eri�imli kullan�c�da de�i�iklik yapamazs�n�z.	31337
12	77	Sizinkine e�it ya da fazla eri�im veremezsiniz.	31337
12	78	%s i�in eri�im seviyesi %s kanal�nda %i olarak de�i�tirildi	31337
12	79	%s kullan�c� ad� i�in %s kanal�nda otomatik operat�rl�k verildi	31337
12	80	%s kullan�c� ad� i�in %s kanal�nda otomatik konu�ma verildi	31337
12	81	%s kullan�c� ad� i�in %s kanal�nda otomatik se�ene�i kald�r�ld�	31337	
12	82	�ifre ibareniz kullan�c� ad�n�z ya da �uanki rumuzunuz olamaz - s�zdizimi: NEWPASS <yeni ibare>	31337
12	83	�ifre de�i�imi ba�ar�l�.	31337
12	84	%s kanal�nda NOOP �zelli�i aktif hale getirilmi�tir	31337
12	85	%s kanal�nda STRICTOP �zelli�i aktif hale getirilmi�tir (ve %s tan�mlanmam��)	31337
12	86	%s kanal�nda STRICTOP �zelli�i aktif hale getirilmi�tir (ve %s yetersiz eri�imli)	31337
12	87	%s kanal� kapat�ld�	31337
12	88	%s zaten bende kay�tl�.	31337
12	89	Ge�ersiz kanal ismi.	31337
12	90	Kay�tl� kanal %s	31337
12	91	%s sessizlik listemden ��kar�ld�	31337
12	92	%s sessizlik listemde bulunamad�	31337
12	93	Sizinkine e�it ya da fazla eri�imli kullan�c�y� silemezsiniz	31337
12	94	Kendinizi, sahibi oldu�unuz kanaldan silemezsiniz	31337
12	95	%s kullan�c�s� %s kanal�nda silindi	31337
12	96	G�R�NMEZL�K ayar�n�z a��ld�.	31337
12	97	G�R�NMEZL�K ayar�n�z kapat�ld�.	31337
12	98	%s %s i�in %s	31337
12	99	%s i�in de�er ON ya da OFF olmal�	31337
12	100	Ge�ersiz USERFLAGS ayar�. Do�ru de�erler 0, 1, 2.	31337
12	101	%s i�in USERFLAGS ayar� %i	31337
12	102	MASSDEOPPRO i�in de�er 0-7 olmal�	31337
12	103	MASSDEOPPRO %s kanal� i�in %d yap�ld�	31337
12	104	FLOODPRO i�in de�er 0-7 olmal�	31337
12	105	FLOODPRO %s kanal� i�in %d yap�ld�	31337
12	106	KANAL TANIMI en fazla 80 karakter olabilir	31337
12	107	%s i�in KANAL TANIMI silindi.	31337
12	108	%s i�in KANAL TANIMI: %s	31337
12	109	Site ad� en fazla 75 karakter olabilir!	31337
12	110	%s i�in site ad� silindi.	31337
12	111	%s i�in site ad�: %s	31337
12	112	Anahtar kelimeler i�in dizgi 80 karakteri a�amaz!	31337
12	113	%s i�in ANAHTAR KEL�MELER: %s	31337
12	114	Lisan %s olarak belirlendi.	31337
12	115	HATA: Ge�ersiz lisan se�imi.	31337
12	116	A�da %s kanal�n�n yeri saptanam�yor!	31337
12	117	HATA: Ge�ersiz kanal ayar�.	31337
12	118	�nemsenmeyenler listesi:	31337
12	119	�nemsenmeyenler listesi bo�	31337
12	120	-- �nemsenmeyenler listesi sonu	31337
12	121	CMaster Kanal Servisleri dahili stat�s�:	31337
12	122	[     Kanal Kay�t �statistikleri] \002Kaydedilmi� giri�ler:\002 %i    \002VT �stekleri:\002 %i    \002Kay�t hitleri:\002 %i    \002Verim:\002 %.2f%%	31337
12	123	[        Kullan�c� Kay�t �statistikleri] \002Kaydedilmi� giri�ler:\002 %i    \002VT �stekleri:\002 %i    \002Kay�t hitleri:\002 %i    \002Verim:\002 %.2f%%	31337
12	124	[Eri�im Kay�t �statistikleri] \002Kaydedilmi� giri�ler:\002 %i    \002VT �stekleri:\002 %i    \002Kay�t hitleri:\002 %i    \002Verim:\002 %.2f%%	31337
12	125	[         Yasaklama Kay�t �statistikleri] \002Kaydedilmi� giri�ler:\002 %i    \002VT �stekleri:\002 %i    \002Kay�t hitleri:\002 %i    \002Verim:\002 %.2f%%	31337
12	126	Al�nan son Kullan�c� B�LD�R�M�: %i	31337
12	127	Al�nan son Kanal B�LD�R�M�: %i	31337
12	128	Al�nan son Seviye B�LD�R�M�: %i	31337
12	129	Al�nan son Yasaklama B�LD�R�M�: %i	31337
12	130	Belirli bilgi kaplar� tahsis edildi: %i	31337
12	131	\002A��k kald��� s�re:\002 %s	31337
12	132	%s kanal�nda %d tane kullan�c� var (%i operat�r)	31337
12	133	Mod: %s	31337
12	134	Bayraklar: %s	31337
12	135	Sizinkine e�it ya da fazla eri�imi olan ki�iyi ask�ya alamazs�n�z.	31337
12	136	sahte zaman �niteleri	31337
12	137	Ge�ersiz ak�ya alma s�resi.	31337
12	138	%s i�in ASKIYA ALMA iptal edildi	31337
12	139	%s zaten %s kanal�nda ask�ya al�nm�� durumda	31337
12	140	%s i�in ASKIYA ALMA %s s�re sonra dolacak	31337
12	141	HATA: Ba�l�k 145 karakteri a�amaz	31337
12	142	%s yasaklamas�n� %s kanal�n�n veri taban�ndan kald�rmak i�in yetersiz eri�iminiz var	31337
12	143	%i tane %s ile e�le�en yasaklama silindi	31337
12	144	%i olan ge�ici eri�iminiz %s kanal�ndan silindi	31337
12	145	%s kanal�nda g��lendirilmi� eri�imin yok gibi g�r�n�yor, belki s�resi dolmu�tur?	31337
12	146	%s %s kanal�nda ask�ya al�nmam��	31337
12	147	%s bir IRC operat�r�	31337
12	148	%s tan�mlanmam�� durumda.	31337
12	149	%s %s%s olarak tan�ml�	31337
12	150	%s Resmi bir CService Temsilcisi%s ve %s olarak tan�ml�	31337
12	151	%s Resmi bir CService Y�neticisi%s ve %s olarak tan�ml�	31337
12	152	%s Resmi bir CService Geli�tiricisi%s ve %s olarak tan�ml�	31337
12	153	%i taneden daha fazla e�le�me var [%s]	31337
12	154	L�tfen arama maskenizi s�n�rland�r�n	31337
12	155	[%s] i�in ge�erli e�le�me yok	31337
12	156	%s: Kanal modlar� temizlendi.	31337
12	158	Ge�ersiz se�enek.	31337
12	159	%s Resmi bir Undernet Servis Robotu.	31337
12	160	%s Resmi bir Coder-Com Temsilcisi%s ve %s olarak tan�ml�	31337
12	161	%s Resmi bir Coder-Com Katk�da Bulunucusu%s ve %s olarak tan�ml�	31337
12	162	%s Resmi bir Coder-Com Geli�tiricisi%s ve %s olarak tan�ml�	31337
12	163	%s Resmi bir Coder-Com Uzman�%s ve %s olarak tan�ml�	31337
12	164	 ve bir IRC operat�r�	31337
12	165	%s yasaklamas� %s kanal�nda %i seviye ile eklendi	31337
12	166	%s: yasakl�lar listesi bo�.		31337
12	167	Ben zaten o kanalday�m!	31337
12	168	Bu komut IRC operat�rlere ait	31337
12	169	%s kanal�nda operat�rl�k yetkim yok	31337
12	170	%s i�in %i dakika	31337
\.

-- Norwegian Language Definition.
-- 21/08/01
-- Bj�rn Osdal (Nick: Snatcher) <mrosdal@online.no>
-- Ronny Kvislavangen (Nick: KingGenie) <genie@undernet.org>
-- Past Contributions: Mr_|r0n, CISC.

COPY "translations" FROM stdin;
13	1	Beklager, men du er allerede logget inn som %s.	31337
13	2	INNLOGGING GODKJENT som %s.	31337
13	3	Beklager, men du har ikke nok aksess for � f� utf�rt den kommandoen.	31337
13	4	Beklager, men kanalen %s er tom.	31337
13	5	Jeg kan ikke finne %s noen steder	31337
13	6	Jeg kan ikke finne %s p� %s 	31337
13	7	Kanalen %s ser ikke ut til � v�re registrert.	31337
13	8	Du har f�tt operat�r status av %s (%s)	31337
13	9	Du har f�tt voice status av %s (%s)	31337
13	10	%s: Du er ikke i den kanalen	31337
13	11	%s er allerede operat�r i %s	31337
13	12	%s har allerede voice i %s	31337
13	13	%s (%s) tok ifra deg operat�r status	31337
13	14	%s (%s) tok ifra deg voice status	31337
13	15	%s er ikke operat�r i %s	31337
13	16	%s har ikke voice i %s	31337
13	17	INNLOGGING FEILET som %s (Feil passord)	31337
13	18	Jeg er ikke i den kanalen!	31337
13	19	Ugyldig banlevel. M� v�re 1-%i.	31337
13	20	Ugyldig ban varighet. Den kan maksimum v�re 336 timer.	31337
13	21	Ban grunn kan ikke overstige 128 tegn	31337
13	22	Den spesifiserte bannen er allerede i min banliste!	31337
13	23	Bannen %s er allerede dekket av %s	31337
13	24	Ikke korrekt minimums level	31337
13	25	Ikke korrekt maksimums level	31337
13	26	BRUKER: %s AKSESS: %s %s	31337
13	27	KANAL: %s -- AUTOMODE: %s	31337
13	28	SIST MODIFISERT: %s (%s siden)	31337
13	29	** SUSPENDERT ** - Utl�per om %s (Level %i)	31337
13	30	Sist sett for: %s timer siden	31337
13	31	Det er mere enn 15 treff som passer dine kriterier.	31337
13	32	Vennligst begrens ditt s�k.	31337
13	33	Slutt p� aksesslisten	31337
13	34	Ingen treff!	31337
13	35	Kan ikke legge til bruker med lik eller h�yere aksess enn din egen.	31337
13	36	Ikke gyldig aksess level.	31337
13	37	%s er allerede lagt til i %s med aksess level %i.	31337
13	38	Lagt til bruker %s i %s med aksess level %i	31337
13	39	Noe gikk galt: %s	31337
13	40	%s: Slutt p� banlisten	31337
13	41	Ikke mulig � se brukers detaljer (Usynlig)	31337
13	42	Informasjon om: %s (%i)	31337
13	43	Er akuratt n� logget p� via: %s	31337
13	44	URL: %s	31337
13	45	Spr�k: %i	31337
13	46	Kanaler: %s	31337
13	47	Input Flood Points: %i	31337
13	48	Ouput Flood (Bytes): %i	31337
13	49	%s er registrert av:	31337
13	50	%s - sist sett: %s siden	31337
13	51	Beskrivelse: %s	31337
13	52	Bare flood meg du. Jeg kommer ikke til � h�re p� deg igjen	31337
13	53	Jeg tror jeg sendte deg litt for mye data. Jeg kommer til � ignorere deg for en liten stund.	31337
13	54	Ufullstendig kommando	31337
13	55	For � bruke %s, m� du bruke /msg %s@%s	31337
13	56	Beklager, men du m� v�re logget inn for � bruke denne kommandoen.	31337
13	57	Kanalen %s har blitt suspendert av en Cservice Administrator.	31337
13	58	Din aksess p� %s er suspendert.	31337
13	59	NOOP flagget er satt p� %s	31337
13	60	STRICTOP flagget er satt p� %s	31337
13	61	Du deopet flere enn %i personer	31337
13	62	SYNTAKS: %s	31337
13	63	Midlertidig �ket din aksess p� kanal %s til %i	31337
13	64	%s er registrert.	31337
13	65	%s er ikke registrert.	31337
13	66	Jeg tror ikke at %s vil like det der.	31337
13	67	\002*** Banliste for %s ***\002	31337
13	68	%s %s Level: %i	31337
13	69	LAGT TIL AV: %s (%s)	31337
13	70	SIDEN: %s	31337
13	71	UTG�R: %s	31337
13	72	\002*** SLUTT ***\002	31337
13	73	Jeg vet ikke hvem %s er.	31337
13	74	Du er ikke logget inn hos meg lenger.	31337
13	75	%s ser ikke ut til � ha aksess i %s.	31337
13	76	Kan ikke modifisere en bruker med aksess som er lik eller h�yere enn din egen.	31337
13	77	Kan ikke gi en bruker aksess som er lik eller h�yere enn din egen.	31337
13	78	Modifisert %s sin aksess p� kanal %s til %i	31337
13	79	Satt AUTOMODE til OP for %s p� kanal %s	31337
13	80	Satt AUTOMODE til VOICE for %s p� kanal %s	31337
13	81	Satt AUTOMODE til IKKENOE for %s p�p kanal %s	31337	
13	82	Ditt passord kan ikke v�re ditt brukernavn eller ditt n�v�rende nick - syntaks er: NEWPASS <nytt passord>	31337
13	83	Passordet er byttet.	31337
13	84	NOOP flagget er satt p� %s	31337
13	85	STRICTOP flagget er satt p� %s (og %s er ikke logget inn)	31337
13	86	STRICTOP flagget er satt p� %s (og %s har ikke nok aksess)	31337
13	87	Slettet kanal %s	31337
13	88	%s er allerede registrert hos meg.	31337
13	89	Ugyldig kanalnavn.	31337
13	90	Registrert kanal %s	31337
13	91	Slettet %s fra min ignoreliste	31337
13	92	Kunne ikke slette %s fra min ignoreliste	31337
13	93	Kan ikke slette en bruker med lik eller h�yere aksess enn din egen	31337
13	94	Du kan ikke fjerne deg selv ifra en kanal du eier.	31337
13	95	SLettet bruker %s fra %s	31337
13	96	INVISIBLE(usynlig) er n� ON(p�).	31337
13	97	INVISIBLE(usynlig) er n� OFF(av).	31337
13	98	%s for %s er %s	31337
13	99	%s M� v�re ON eller OFF	31337
13	100	Ugyldig USERFLAGS instilling. Korrekte verdier er: 0, 1, 2.	31337
13	101	USERFLAGS for %s er %i	31337
13	102	Verdien for MASSDEOPPRO m� v�re 0-7	31337
13	103	MASSDEOPPRO for %s er satt til %d	31337
13	104	Verdien til FLOODPRO m� v�re 0-7	31337
13	105	FLOODPRO for %s er satt til %d	31337
13	106	BESKRIVELSE kan maksimalt v�re 80 tegn!	31337
13	107	BESKRIVELSE for %s er slettet.	31337
13	108	BESKRIVELSE for %s er: %s	31337
13	109	URL kan maksimum v�re 75 tegn!	31337
13	110	URL for %s er slettet.	31337
13	111	URL for %s er: %s	31337
13	112	KEYWORDS kan ikke overstige 80 tegn!	31337
13	113	KEYWORDS for %s er: %s	31337
13	114	Spr�k er satt til %s.	31337
13	115	ERROR: Ugyldig spr�k valg.	31337
13	116	Kan ikke finne kanal %s p� nettverket!	31337
13	117	ERROR: Ugyldig kanal innstilling.	31337
13	118	Ignoreliste:	31337
13	119	Ignorelista er tom	31337
13	120	-- Slutt p� ignorelista	31337
13	121	CMaster Channel Services intern status:	31337
13	122	[     Channel Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337
13	123	[        User Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337
13	124	[Access Level Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337
13	125	[         Ban Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337
13	126	Sist mottatt bruker NOTIFY: %i	31337
13	127	Sist mottatt kanal NOTIFY: %i	31337
13	128	Sist mottatt level NOTIFY: %i	31337
13	129	Sist mottatt ban NOTIFY: %i	31337
13	130	Custom data containers allocated: %i	31337
13	131	\002Oppetid:\002 %s	31337
13	132	Kanalen %s har %d brukere (%i operat�rer)	31337
13	133	Mode er: %s	31337
13	134	Flagg satt: %s	31337
13	135	Kan ikke suspendere en bruker med lik eller h�yere aksess enn din egen.	31337
13	136	bogus tids enheter	31337
13	137	Ugyldig suspend varighet.	31337
13	138	SUSPENSION for %s er slettet	31337
13	139	%s er allerede suspendert p� %s	31337
13	140	SUSPENSION for %s g�r ut om %s	31337
13	141	ERROR: Topic kan ikke overskride 145 tegn	31337
13	142	Du har ikke tilstrekkelig aksess for � slette bannen %s ifra %s's database	31337
13	143	Slettet %i bans som stemmer med %s	31337
13	144	Midlertidig slettet din aksess p� %i ifra kanal %s	31337
13	145	Det ser ikke ut som at du har en tvunget aksess i %s, kanskje den har utl�pt?	31337
13	146	%s er ikke suspendert p� %s	31337
13	147	%s er en IRC operat�r	31337
13	148	%s er IKKE logget inn.	31337
13	149	%s er logget inn som %s%s	31337
13	150	%s er en offisiell CService Representant%s og er logget inn som  %s	31337
13	151	%s er en offisiell CService Administrator%s og er logget inn som %s	31337
13	152	%s er en offisiell CService Utvikler%s og er logget inn som %s	31337
13	153	Det er flere enn %i muligheter som passer [%s]	31337
13	154	Vennligst begrens dine s�kekriterie	31337
13	155	Ingen postering funner for [%s]	31337
13	156	%s: Slettet kanal modes.	31337
13	158	Ugyldig valg.	31337
13	159	%s er en offisiell Undernet Service Bot.	31337
13	160	%s er en offisiell Coder-Com Representant%s og er logget inn som %s	31337
13	161	%s er en offisiell Coder-Com Bidragsyter%s og er logget inn som %s	31337
13	162	%s er en offisiell Coder-Com Utvikler%s og er logget inn som %s	31337
13	163	%s er en offisiell Coder-Com Senior%s og er logget inn som %s	31337
13	164	 og en IRC operat�r	31337
13	165	Lagt til ban %s til %s med level %i	31337
13	166	%s: banlisten er tom.		31337
13	167	Jeg er allerede p� den kanalen!	31337
13	168	Denne kommandoen er reservert for IRC operat�rer	31337
13	169	Jeg er ikke operat�r p� %s	31337
13	170	%s for %i minutter	31337
\.

-- Arabic Language Definition.
-- 02/09/01
-- B_O_S_S - boss@boss.org

COPY "translations" FROM stdin; 
14	1	Assif. anta mo3arafone sabi9ane %s	31337
14	2	ta3arof naji7li  %s	31337
14	3	Assif.	31337
14	4	Assif. Al9anatou %s farigha	31337
14	5	lam yatamakan li woujoud  %s	31337
14	6	Lam ajid %s fi 9anate %s	31337
14	7	al9anatou %s laysate mousajala	31337
14	8	la9ade asbahta op min tarafe %s (%s)	31337
14	9	la9ade asbahta voice min tarafe %s (%s)	31337
14	10	%s: anta lasta dakhila hadihi al9anate	31337
14	11	%s lahou sabi9ane op fi  %s	31337
14	12	%s lahou sabi9ane voice fi %s	31337
14	13	la9ade okhida laka al op min tarafe %s (%s)	31337
14	14	la9ade okhida laka voice min tarafe %s (%s)	31337
14	15	%s laissa op fi %s	31337
14	16	%s laissa voice fi %s	31337
14	17	TA3AROUFE GHAYR NAJI7 %s (KALIMATO ASIRE KHATI2A)	31337
14	18	Ana lastou dakhila hadidi al9anate!!	31337
14	19	Moustawa ban GHAYROU sahihe. Moustawa sahihe: bayna 1-%i.	31337
14	20	Moudatou ban ghayrou sahiha. Almoudatou alkouswa Hia 336 heures.	31337
14	21	Sababou al ban layomkino an yata3adda 128 7arfe .	31337
14	22	Hada al ban Youjadou sabi9ane fe la2i7a!	31337
14	23	Al ban %s youjado sabi9ane %s	31337
14	24	Almousstawa al adna ghayrou sahi7e.	31337
14	25	Almousstawa al a9ssa ghayrou sahi7e.	31337
14	26	MOUSSTA3MILE%: %s MOUSSTAWA: %s %s	31337
14	27	AL9ANATE: %s -- AMRE: %s	31337
14	28	AKHIRE TAGHYIRE: %s (%s kabla)	31337
14	29	** MOUTAWA9IFE ** - Yantahi fi %s (moustawa %i)	31337
14	30	AKHIRE MARA CHOUHIDA : %s 9able.	31337
14	31	Youjado aktare min 15 talabe .	31337
14	32	Min Fadlik 7addid talabake	31337
14	33	Nihayato lai7ati al mousstawayate	31337
14	34	lam youjade ayou chayeine youwafi9o talabakome 	31337
14	35	La youmkino zeyadata chakhs lahou moustawake awe aktare. 	31337
14	36	Mousstawa atadakhole ghayro sahi7e	31337
14	37	%s oudifa moussaba9ane %s fi mousstawa %i.	31337
14	38	ziyadatou moussta3mile %s ala %s fi mousstawa %i	31337
14	39	Youjado ghalate: %s	31337
14	40	%s: nihayatou lai7ati al ban		31337
14	41	la youmkinou idharou tafassile al moussta3mile.	31337
14	42	Ma3loumate 3ala: %s (%i)	31337
14	43	alane moutassile 3abra: %s	31337
14	44	URL: %s	31337
14	45	Lougha: %i	31337
14	46	9ANAWATE: %s	31337
14	47	Madkhale Flood: %i	31337
14	48	Makhraje Flood (Bytes): %i	31337
14	49	%s moussajaloune min tarafe:	31337
14	50	%s - chouhida lile mara akhira : %s 9able	31337
14	51	wassf: %s	31337
14	52	Touridouna flood ? lane arouda alaykoume ba3da hada.	31337
14	53	Adono anani arsaltou lakoume ma3loumatine katira. saatajahaloukome ba3da al wa9te.	31337
14	54	Amroune na9ise	31337
14	55	li issti3mali %s, alaykoume bi /msg %s@%s	31337
14	56	Assif. 3Alayka an takouna mo3arafe li sti3mali hada al amre.	31337
14	57	9anatou %s oughli9ate min tarafe moudire cservice.	31337
14	58	Tadakhouloukoume fi %s tawa9afa.	31337
14	59	Tari9ate NOOP mousta3mala biha fi %s	31337
14	60	Tari9ate STRICTOP mousta3mala fi %s	31337
14	61	La9ad fa3alta deop li aktare min %i chakhsse	31337
14	62	SYNTAXE: %s	31337
14	63	mou3arafe moua9atane fi %s bi mousstawa %i	31337
14	64	%s mousajala.	31337
14	65	%s ghayre moussajala.	31337
14	66	La adono ana %s you7iba hada.	31337
14	67	\002*** lai7ato al Ban li 9anate %s ***\002	31337
14	68	%s %s mousstawa: %i	31337
14	69	OUDIFA MIN Tarafe: %s (%s)	31337
14	70	MOUNDO: %s	31337
14	71	YANTAHI: %s	31337
14	72	\002*** NIHAYA ***\002	31337
14	73	Assif, La a3rifou man houa %s.	31337
14	74	Assif, Lasta mou3arafe.	31337
14	75	%s La youjado ayo tadakhoule mouwafi9e fi %s.	31337
14	76	La youmkinouka taghyire ayi moussata3mile lahou moustawakoume awe aktare. 	31337
14	77	La youmkino ane tane9asa tadakhoule lahou moustawake awe aktare .	31337
14	78	Taghyire mousstawa %s fi al9anate %s ila %i	31337
14	79	Tachghilou tari9ate (OP) atela9ai li %s fi 9anate %s	31337
14	80	Tachghilou tarikate (VOICE) atela9ai li %s fi 9anate %s	31337
14	81	Izalatou tari9ate atela9ai li %s fil 9anate %s	31337
14	82	ra9mouka asiri la younassibou isma lmousta3mile wa nick - al Syntaxe hiya: NEWPASS <nouveau mot-de-passe>	31337
14	83	ra9mouka asiri ghouyira bi naja7e.	31337
14	84	Tari9atou NOOP Mousta3mala fi %s	31337
14	85	Tari9atou STRICTOP mousta3mala fi %s (wa %s Layssa mou3arafe)	31337
14	86	Tari9atou STRICTOP mousta3mala fi %s (wa tadakhoulo %s ghayrou kafi)	31337
14	87	%s na9iye	31337
14	88	%s mousajale sabi9ane houna.	31337
14	89	Ismou al 9anate ghayrou sa7i7e.	31337
14	90	%s Soujilate	31337
14	91	Izalatou %s min lai7ate a soukoute/tajahoule	31337
14	92	Ghayrou 9adire 3ala ijadi %s fi lai7ati soukoute/tajahoule	31337
14	93	La youmkino izalate tadakhoule lahou moustawake awe aktare.	31337
14	94	La youmkino izalate tadakhoulaka fi 9anate tasta3miloha.	31337
14	95	Tadakhoulou al moussta3mile %s ounzila ila %s	31337
14	96	Tari9atou INVISIBLE li khiyaratikoume al khassa al ane ON.	31337
14	97	Tari9atou INVISIBLE li khiyaratikoume al khassa al ane off.	31337
14	98	%s li %s hia %s	31337
14	99	Kimatou %s yajibou ane takouna ON aw OFF	31337
14	100	Tahakoume USERFLAGS ghayrou sa7i7e! Al 9iyamou a sa7i7a hia 0,1,2.	31337
14	101	Al USERFLAGS li %s houa %i	31337
14	102	9imatou  MASSDEOPPRO yajibou ana takouna bayne 0-7	31337
14	103	MASSDEOPPRO li %s ta7akama fihi ila %d	31337
14	104	9imatou  FLOODPRO yajibou ane takouna bayne 0-7	31337
14	105	FLOODPRO %s ta7akama fihi ila %d	31337
14	106	Al wasfou la yajibou ane yata3ada 80 7arfane.	31337
14	107	Wasaf %s ouzila.	31337
14	108	Wasf %s houa: %s	31337
14	109	Al URL la yajibou ana yata3ada 75 7arfane.	31337
14	110	Al URL li %s ouzila.	31337
14	111	Al URL li %s houa: %s	31337
14	112	Al KEYWORDS (Alkalimatou almiftahe) la yajibou ane tata3ada 80 7arfane.	31337
14	113	KEYWORDS li %s houme: %s	31337
14	114	loughato mousta3emala: %s.	31337
14	115	kHATA2: ikhtiyarou loughatine ghayre sa7iha.	31337
14	116	Al 9anate %s ghayrou mawjouda 3ala chabakati arabte!	31337
14	117	KHATA2: Ta7akoumou al 9anate ghayrou sa7i7e.	31337
14	118	Lai7atou al motajahaline:	31337
14	119	Lai7atou almoutajahaline farigha	31337
14	120	-- Nihayatou lai7ati al moutajahaline 	31337
14	121	Status interne du Service de Canaux CMaster:	31337
14	130	almoutayatou moutafa9a: %i	31337
14	131	\002file khidma moundou (uptime):\002 %s	31337
14	132	Al 9anate %s li %d moussta3mile (%i Amiloune)	31337
14	133	Touroukou al 9anate: %s	31337
14	134	al flags almoussta3male : %s	31337
14	135	La youmkino tawa9ofe mousta3mile lahou moustawake awe aktare.	31337
14	136	Wa7adate alwa9te ghayre sa7i7a.	31337
14	137	Moudatou al i9afe ghayre sa7i7a.	31337
14	138	i9afe %s oulghiya.	31337
14	139	%s mouwa9afe sabi9ane 3ala %s	31337
14	140	al tawa9ofe li %s sa ya3male fi %s	31337
14	141	KHATA2: al topic la yajibou ane yata3ada 145 7arfane.	31337
14	142	tadakhoulouke ghayre kafi li izalati al ban %s min la2i7ate al mou3tayate li %s	31337
14	143	7adfou %i ban(s) alati ta7tawi 3alayhe %s	31337
14	144	Izalatou tadakhoulouke al moua9ate fi al mousstawa %i fi 9anate %s	31337
14	145	La tadhare 9ade da3amata tadakhoulake ila %s, hale satantahi?	31337
14	146	%s lame youwa9afe 3ala %s	31337
14	147	%s huwa Op�rateur IRC	31337
14	148	%s ghayre mou3arafe.	31337
14	149	%s mou3arafe 3abra userid %s%s	31337
14	150	%s moumatile rassmi li CService%s wa mou3arafe 3abra userid %s	31337
14	151	%s huwa moudire rassmi li CService%s wa mou3arafe 3abra userid %s	31337
14	152	%s huwa moubarmije rassmi li CService%s wa mou3arafe 3abra userid %s	31337
14	153	LA youjadou ayo %i doukhoule 7adire  [%s]	31337
14	154	Min fadlike 7adide talabake.	31337
14	155	La youjado doukhoule 7adire li talabouke. [%s]	31337
14	156	%s: Touroukou al 9anate 9ade ouzila.	31337
14	158	khiyare ghayrou sa7ihe.	31337
14	159	%s Undernet Service Bot Rassmi .	31337
14	160	%s is an Official Coder-Com Representative%s wa mou3arafe bi %s	31337
14	161	%s is an Official Coder-Com Contributer%s wa mou3arafe bi %s	31337
14	162	%s is an Official Coder-Com Developer%s wa mou3arafe bi %s	31337
14	163	%s is an Official Coder-Com Senior%s wa mou3arafe bi %s	31337
14	164	 wa IRC operator	31337
14	165	Tamate idafato ban %s li %s fi darajati %i	31337
14	166	%s: Lai7ato l ban farigha .		31337
14	167	Ana sabi9ane fi hadihi al 9anate!	31337
14	168	hada al amre moukhassasse li IRC Operators	31337
14	169	Ana lastou op fi %s	31337
14	170	%s ila %i da9i9a	31337
\.
