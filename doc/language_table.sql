-- Language Definition Table for CMaster Undernet channel services.
-- English language definition.
-- 27/12/00 - Greg Sikorski <gte@atomicrevs.demon.co.uk>.

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
\.

-- French language definition.
-- 29/12/00 - Daniel Simard <svr@undernet.org>.

COPY "translations" FROM stdin;
2	1	Désolé. Vous êtes déjà authentifié avec le nick %s	31337
2	2	AUTHENTICATION ACCEPTÉ pour %s	31337
2	3	Désolé. Votre accès est insuffisante pour utiliser cette commande	31337
2	4	Désolé. Le canal %s est vide	31337
2	5	Je ne vois pas %s nul part	31337
2	6	Je ne vois pas %s sur le canal %s	31337
2	7	Le canal %s ne semble pas être enregistré	31337
2	8	Vous avez été oppé par %s (%s)	31337
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
