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
2	1	D�sol�. Vous �tes d�j� authentifi� avec le nick %s	31337
2	2	AUTHENTICATION R�USSIE pour %s	31337
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
2	17	AUTHENTIFICATION REFUS�E pour %s (Mot-de-passe Invalide)	31337
2	18	Je ne suis pas dans ce canal!!	31337
2	19	Niveau de ban invalide. Niveau valide: entre 1-%i.	31337
2	20	Duration du ban invalide. La duration maximale est de 336 heures.	31337
2	21	La raison d'un ban ne peut d�passer 128 caract�res.	31337
2	22	Ce ban est d�j� dans ma liste!	31337
2	23	Le ban %s est dej� affect� par %s	31337
2	24	Niveau minimum invalide.	31337
2	25	Niveau maximum invalide.	31337
2	26	USAGER: %s NIVEAU: %s %s	31337
2	27	CANAL: %s -- AUTOMODE: %s	31337
2	28	DERNI�RE MODIFICATION: %s (%s avant)	31337
2	29	** SUSPENDU ** - Expire dans %s	31337
2	30	DERNI�RE FOIS VU: %s avant.	31337
2	31	Il y a plus de 15 requ�tes trouv�es.	31337
2	32	SVP restreindre votre requ�te	31337
2	33	Fin de la liste des acc�s	31337
2	34	Aucune correspondace trouv�e	31337
2	35	Vous ne pouvez additionner un usager au m�me niveau que vous ou plus �lev�. 	31337
2	36	Niveau d'acc�s invalide	31337
2	37	%s a d�j� �t� ajout� sur %s au niveau %i.	31337
2	38	Addition de l'usager %s sur %s au niveau %i	31337
2	39	Quelque chose semble incorrecte: %s	31337
2	40	%s: fin de la liste de ban		31337
2	41	Impossible de visualiser les d�tails de l'usager (Invisible).	31337
2	42	Information � propos de: %s (%i)	31337
2	43	Pr�sentement connect� via: %s	31337
2	44	URL: %s	31337
2	45	Langue: %i	31337
2	46	Canaux: %s	31337
2	47	Entr�e Flood: %i	31337
2	48	Sortie Flood (Bytes): %i	31337
2	49	%s a �t� enregistr� par:	31337
2	50	%s - vu pour la derni�re fois: %s avant	31337
2	51	Desc: %s	31337
2	52	Floodez moi voulez-vous? Je ne vais plus vous r�pondre dor�navant.	31337
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
2	63	Temporairement authentifi� sur %s au niveau %i	31337
2	64	%s est enregistr�.	31337
2	65	%s n'est pas enregistr�.	31337
2	66	Je ne crois pas que %s appr�cierait �a.	31337
2	67	\002*** Liste de Ban pour le canal %s ***\002	31337
2	68	%s %s Niveau: %i	31337
2	69	AJOUT� PAR: %s (%s)	31337
2	70	DEPUIS: %i	31337
2	71	EXP: %s	31337
2	72	\002*** FIN ***\002	31337
2	73	D�sol�, Je ne sais pas qui est %s.	31337
2	74	D�sol�, Vous n'�tes plus authentifi�.	31337
2	75	Aucune acc�s correspondant sur %s.	31337
2	76	Vous ne pouvez pas modifier un usager au m�me niveau ou sup�rieur � vous.	31337
2	77	Vous ne pouvez pas donner un acc�s � un usager au m�me niveau ou sup�rieur � vous.	31337
2	78	Modification du niveau de %s sur le canal %s � %i	31337
2	79	Activation du MODE Automatique (OP) pour %s sur le canal %s	31337
2	80	Activation du MODE Automatique (VOICE) pour %s sur le canal %s	31337
2	81	D�sactivation des MODES Automatiques pour %s sur le canal %s	31337	
2	82	Votre mot de passe ne peut �tre votre nom d'usager ni votre nick - La Syntaxe est: NEWPASS <nouveau mot-de-passe>	31337
2	83	Votre mot de passe a �t� chang� avec succ�s.	31337
2	84	Le mode NOOP est actif sur %s	31337
2	85	Le mode STRICTOP est actif sur %s (et %s n'est pas authentifi�)	31337
2	86	Le mode STRICTOP est actif sur %s (et l'acc�s de %s est insuffisant)	31337
2	87	%s a �t� purg�	31337
2	88	%s est d�j� enregistr� avec moi.	31337
2	89	Nom de canal invalide.	31337
2	90	%s a �t� enregistr�	31337
2	91	Enlev� %s de ma liste silence/ignore	31337
2	92	Incapable de trouver %s dans ma liste silence/ignore	31337
2	93	Vous ne pouvez pas effacer un acc�s donc le niveau est �gal ou plus �lev� que le votre.	31337
2	94	Vous ne pouvez pas effacer votre acc�s sur un canal que vous poss�dez.	31337
2	95	L'acc�s de l'usager %s a �t� effac� sur %s	31337
2	96	Le mode INVISIBLE pour vos options confidentiels est maintenant � ON.	31337
2	97	Le mode INVISIBLE pour vos options confidentiels est maintenant � OFF.	31337
2	98	%s pour %s est %s	31337
2	99	La valeur de %s doit �tre ON ou OFF	31337
2	100	Configuration USERFLAGS invalide! Les valeurs correctes sont 0, 1, 2.	31337
2	101	Le USERFLAGS de %s est %i	31337
2	102	La valeur de MASSDEOPPRO doit �tre entre 0-7	31337
2	103	MASSDEOPPRO de %s a �t� configur� � %d	31337
2	104	La valeur de FLOODPRO doit �tre entre 0-7	31337
2	105	FLOODPRO de %s a �t� configur� � %d	31337
2	106	La DESCRIPTION ne doit pas d�passer 80 caract�res.	31337
2	107	La DESCRIPTION de %s a �t� �ffac�.	31337
2	108	La DESCRIPTION de %s est: %s	31337
2	109	L'URL ne doit pas d�passer 75 caract�res.	31337
2	110	L'URL de %s a �t� effac�.	31337
2	111	L'URL de %s est: %s	31337
2	112	Les KEYWORDS (MotsCl�s) ne doivent pas d�passer 80 caract�res.	31337
2	113	KEYWORDS de %s sont: %s	31337
2	114	Langue activ�: %s.	31337
2	115	ERREUR: S�lection d'une langue invalide.	31337
2	116	Le canal %s est introuvable sur le r�seau!	31337
2	117	ERREUR: Configuration de canal invalide.	31337
2	118	Liste d'Ignor�(s):	31337
2	119	La liste d'ignore est vide	31337
2	120	-- Fin de la liste d'ignore 	31337
2	121	Status interne du Service de Canaux CMaster:	31337
2	122	[          Statistiques du Canal] \002Entr�es en cache:\002 %i    \002Requ�tes BaseD:\002 %i    \002# de fois Cache touch�:\002 %i    \002Efficacit�:\002 %.2f%%	31337
2	123	[           Statistiques Usagers] \002Entr�es en cache:\002 %i    \002Requ�tes BaseD:\002 %i    \002# de fois Cache touch�:\002 %i    \002Efficacit�:\002 %.2f%%	31337
2	124	[   Statistiques niveaux d'acc�s] \002Entr�es en cache:\002 %i    \002Requ�tes BaseD:\002 %i    \002# de fois Cache touch�:\002 %i    \002Efficacit�:\002 %.2f%%	31337
2	125	[          Statistiques des bans] \002Entr�es en cache:\002 %i    \002Requ�tes BaseD:\002 %i    \002# de fois Cache touch�:\002 %i    \002Efficacit�:\002 %.2f%%	31337
2	126	Derni�re r�ception NOTIFY d'usager: %i	31337
2	127	Derni�re r�ception NOTIFY de canal: %i	31337
2	128	Derni�re r�ception NOTIFY de niveau: %i	31337
2	129	Derni�re r�ception NOTIFY de ban: %i	31337
2	130	Donn�es assign�es: %i	31337
2	131	\002En fonction depuis (uptime):\002 %s	31337
2	132	Le canal %s a %d usager(s) (%i op�rateurs)	31337
2	133	Le(s) Mode(s) du canal: %s	31337
2	134	Flags activ�s: %s	31337
2	135	Vous ne pouvez pas suspendre un usager au m�me niveau que vous ni sup�rieur.	31337
2	136	Unit� de temps invalide.	31337
2	137	Dur�e de suspension invalide.	31337
2	138	La SUSPENSION pour %s a �t� annul�.	31337
2	139	%s est d�j� suspendu sur %s	31337
2	140	La SUSPENSION pour %s va expirer dans %s	31337
2	141	ERREUR: Le topic ne peut d�passer 145 caract�res.	31337
2	142	votre acc�s est insuffisant pour enlever le ban %s de la base de donn�es de %s	31337
2	143	Supprim� %i ban(s) qui figuraient %s	31337
2	144	Supprim� votre acc�s temporaire de niveau %i du canal %s	31337
2	145	Vous ne semblez pas d'avoir forcer l'acc�s � %s, Serait-elle expir�e?	31337
2	146	%s n'est pas suspendu sur %s	31337
2	147	%s est un Op�rateur IRC	31337
2	148	%s n'est pas authentifi�.	31337
2	149	%s est authentifi� via le userid %s%s	31337
2	150	%s est un repr�sentant Officiel de CService%s et authentific� via le userid %s	31337
2	151	%s est un Administrateur Officiel de CService%s et authentific� via le userid %s	31337
2	152	%s est un Programmeur Officiel de CService%s et authentific� via le userid %s	31337
2	153	Il y a plus de %i entr�es figurants [%s]	31337
2	154	Veuillez svp restreindre votre requ�te.	31337
2	155	Aucune entr�e figurante pour votre requ�te. [%s]	31337
2	156	%s: Les modes du canal on �t� effac�s.	31337
2	158	Option invalide.	31337
2	159	%s est un Service Officiel du r�seau Undernet.	31337
2	160	Addition du ban %s sur %s au niveau %i	31337
2	161	%s: la liste des bans est vide.		31337
2	162	Je suis d�j� sur ce canal!	31337
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
3	29	** SUSPENDERET ** - Udl�ber om %s	31337
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
3	70	SIDEN: %i	31337
3	71	UDL: %s	31337
3	72	\002*** SLUT ***\002	31337
3	73	Sorry, Jeg ved ikke hvem %s er.	31337
3	74	Sorry, du er ikke l�ngere autoriseret med mig.	31337
3	75	Ser ikke ud til at have adgang i %s.	31337
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
7	29	** SOSPESO ** - Scade il %s	31337
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
7	70	DAL: %i	31337
7	71	SCADENZA: %s	31337
7	72	\002*** FINE ***\002	31337
7	73	Spiacente, ma non conosco chi sia %s.	31337
7	74	Spiacente, non sei pi� autorizzato con me.	31337
7	75	non sembra avere accesso in %s.	31337
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
7	160	Aggiunto il ban %s a %s con livello %i	31337
7	161	%s: lista dei ban vuota.		31337
7	162	Sono gi� in quel canale!	31337
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
y8	98	%s pentru %s e %s	31337
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
9	29	** SUSP�S/A ** - Expira d'aqu� a %s	31337
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
9	70	DES DE: %i	31337
9	71	EXP: %s	31337
9	72	\002*** FI ***\002	31337
9	73	Ho sento, no s� qui �s %s.	31337
9	74	Ho sento, ja no est�s autenticat/da.	31337
9	75	no figura amb acc�s a %s	31337
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
10	29	** SUSPENDIDO/A ** - Expira dentro de %s	31337
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
10	70	DESDE: %i	31337
10	71	EXP: %s	31337
10	72	\002*** FIN ***\002	31337
10	73	Lo siento, no s� qui�n es %s.	31337
10	74	Lo siento, ya no est�s autentificado/a.	31337
10	75	no consta con acceso en %s	31337
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
