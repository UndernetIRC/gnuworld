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
1	35	Cannot add a user with equal or higher access than your own. 	31337
1	36	Invalid access level.	31337
1	37	%s is already added to %s with access level %i.	31337
1	38	Added user %s to %s with access level %i	31337
1	39	Something went wrong: %s	31337


1	73	Sorry, I don't know who %s is.	31337

1	89	Invalid channel name.	31337

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
4	11	%s is reeds geopt op %s	31337
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
