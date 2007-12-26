-- $Id: language_swedish.sql,v 1.7 2007/12/26 19:13:00 kewlio Exp $
-- Swedish language definition.
-- 06/24/2002 - Ponte <tpin@telia.com>.
-- 11/06/2002 - Comet <Comet@irc-solution.net>
-- 11/06/2002 - typo/grammar spellchecking by LexTbomb.

-- Run this once.

DELETE FROM languages WHERE id=20;

INSERT INTO languages VALUES(20,'SV','Svenska',31337);

DELETE FROM translations where language_id = 20;
COPY "translations" FROM stdin;
20	1	Tyvärr, du är redan inloggad som %s	31337	0
20	2	Inloggningen som %s Lyckades!	31337	0
20	3	Tyvärr, du har inte tillräcklig behörighet för att använda detta kommando	31337	0
20	4	Tyvärr, kanalen %s är tom	31337	0
20	5	Jag kan inte hitta %s någonstans	31337	0
20	6	Jag kan inte hitta %s på kanalen %s	31337	0
20	7	Kanalen %s verkar inte vara registrerad	31337	0
20	8	Du fick op av %s (%s) on %s	31337	0
20	9	Du fick voice av %s (%s) on %s	31337	0
20	10	%s: Du är inte i den kanalen	31337	0
20	11	%s: Du har redan voice i %s	31337	0
20	12	%s har redan voice i %s	31337	0
20	13	Du blev deopad av %s (%s) on %s	31337	0
20	14	Du blev devoicead av %s (%s) on %s	31337	0
20	15	%s har inte op i %s	31337	0
20	16	%s har inte voice i %s	31337	0
20	17	INLOGGNINGEN MISSLYCKADES som %s (Felaktigt lösenord)	31337	0
20	18	Jag är inte i den kanalen!	31337	0
20	19	Ogiltig ban nivå. Giltig nivå är 1-%i.	31337	0
20	20	Ogiltig ban varaktighet. Din ban varaktighet kan max vara %d timmar.	31337	0
20	21	Ban anledning kan max vara 128 bokstäver	31337	0
20	22	Den specifika banen finns redan i min banlista!	31337	0
20	23	Banen %s är redan täckt av %s	31337	0
20	24	Ogiltig min level.	31337	0
20	25	Ogiltig max level.	31337	0
20	26	ANVÄNDARE: %s BEHÖRIGHET: %s %s	31337	0
20	27	KANAL: %s -- AUTOMODE: %s	31337	0
20	28	SENAST MODIFIERAD: %s (%s sen)	31337	0
20	29	** SUSPENDERAD ** - Utgår %s (Nivå %i)	31337	0
20	30	SÅGS SENAST: %s sen.	31337	0
20	31	Det är mer än %d träffar.	31337	0
20	32	Var vänlig specificera din fråga.	31337	0
20	33	Slut på behörighetslistan	31337	0
20	34	Ingen Träff!	31337	0
20	35	Kan inte lägga till en användare med lika hög eller högre behörighet än din egen.	31337	0
20	36	Ogiltig behörighets nivå.	31337	0
20	37	%s finns redan i %s med behörighetsnivå %i.	31337	0
20	38	Lade till användare %s till %s med behörighetsnivå %i	31337	0
20	39	Någonting gick fel:: %s	31337	0
20	40	%s: Slut på banlista	31337	0
20	41	Kan inte visa användardetaljer (Osynlig)	31337	0
20	42	Information om: %s (%i)	31337	0
20	43	För tillfället inloggad via: %s	31337	0
20	44	URL: %s	31337	0
20	45	Språk: %i	31337	0
20	46	Kanaler: %s	31337	0
20	47	Input Flood Points: %i	31337	0
20	48	Ouput Flood (Bytes): %i	31337	0
20	49	%s är registrerad av:	31337	0
20	50	%s - Sågs senast: %s sen	31337	0
20	51	Beskrivning: %s	31337	0
20	52	Floodar du mig? Ja kommer inte att lyssna på dig något mer!	31337	0
20	53	Jag tror att jag har sett lite för mycket data, jag ignorerar dig ett tag.	31337	0
20	54	Icke komplett kommando	31337	0
20	55	För att använda %s, måste du /msg %s@%s	31337	0
20	56	Tyvärr, du måste vara inloggad för att kunna använda detta kommando.	31337	0
20	57	Kanalen %s har blivit suspenderad av en cservice administratör.	31337	0
20	58	Din behörighet på %s har blivit suspenderad.	31337	0
20	59	NOOP är satt "ON" %s	31337	0
20	60	STRICTOP är satt "ON" %s	31337	0
20	61	Du deopade just mer än %i personer	31337	0
20	62	SYNTAX: %s	31337	0
20	63	Ökade tillfälligt din access på kanalen %s till %i	31337	0
20	64	%s är registrerad.	31337	0
20	65	%s är inte registrerad.	31337	0
20	66	Jag tror inte %s skulle uppskatta det.	31337	0
20	67	\002*** Banlista för kanalen %s ***\002	31337	0
20	68	%s %s Nivå: %i	31337	0
20	69	LADES AV: %s (%s)	31337	0
20	70	SEDAN: %s	31337	0
20	71	UTGÅR: %s	31337	0
20	72	\002*** SLUT ***\002	31337	0
20	73	Jag vet inte vem %s är.	31337	0
20	74	Du är inte inloggad med mig längre.	31337	0
20	75	%s verkar inte ha behörighet i %s.	31337	0
20	76	Kan inte modifiera en användare med lika hög eller högre behörighet än din egen.	31337	0
20	77	Kan inte ge en användare lika hög eller högre behörighet än din egen.	31337	0
20	78	Modifierade %s's behörighet på kanalen %s till %i	31337	0
20	79	AUTOMODE till OP för %s på kanalen %s	31337	0
20	80	AUTOMODE till VOICE för %s på kanalen %s	31337	0
20	81	AUTOMODE till INGET för %s på kanalen %s	31337	0
20	82	Ditt lösenordsfras kan inte vara ditt tillfälliga nick - syntax är: NEWPASS <ny lösenordsfras>	31337	0
20	83	Lösenordet ändrades!	31337	0
20	84	NOOP är satt "ON" på %s	31337	0
20	85	STRICTOP är satt "ON" på %s (och %s är inte inloggad)	31337	0
20	86	STRICTOP är satt "ON" på %s (och %s har inte tillräcklig behörighet)	31337	0
20	87	Rensade kanalen %s	31337	0
20	88	%s är redan registrerad med mig.	31337	0
20	89	Ogiltigt kanalnamn.	31337	0
20	90	Registrerade kanalen %s	31337	0
20	91	Tog bort %s från min tystnads lista	31337	0
20	92	Kunde inte hitta %s i min tystnads lista	31337	0
20	93	Kan inte ta bort en användare med lika hög eller högre behörighet än din egen.	31337	0
20	94	Du kan inte ta bort dig själv från en kanal som du äger.	31337	0
20	95	Tog bort användaren %s från %s	31337	0
20	96	Din OSYNLIGHETS inställning är nu "ON".	31337	0
20	97	Din OSYNLIGHETS inställning är nu "OFF".	31337	0
20	98	%s för %s är %s	31337	0
20	99	Värdet för %s måste vara "ON" eller "OFF"	31337	0
20	100	Ogiltig användar inställning. Korrekta värden är NONE, OP, VOICE.	31337	0
20	101	ANVÄNDAR INSTÄLLNINGAR för %s är %s	31337	0
20	102	Värdet för MASSDEOPPRO måste vara 0-7	31337	0
20	103	MASSDEOPPRO för %s är satt till %d	31337	0
20	104	Värdet för FLOODPRO måste vara 0-7	31337	0
20	105	FLOODPRO för %s är satt till %d	31337	0
20	106	BESKRIVNINGEN kan MAX vara 80 tecken!	31337	0
20	107	BESKRIVNINGEN för %s är rensad.	31337	0
20	108	BESKRIVNINGEN för %s är: %s	31337	0
20	109	URL:en kan max vara 75 tecken!	31337	0
20	110	URL:en för %s är rensad.	31337	0
20	111	URL för %s är: %s	31337	0
20	112	Nyckelorden kan max vara 80 tecken!	31337	0
20	113	NYCKELORDEN för %s är: %s	31337	0
20	114	Språket är satt till %s.	31337	0
20	115	ERROR: Ogiltig språk inställning	31337	0
20	116	Kan inte hitta kanalen %s på nätverket!	31337	0
20	117	ERROR: Ogiltig kanal inställning.	31337	0
20	118	Ignorerings lista:	31337	0
20	119	Ignorerings listan är tom	31337	0
20	120	-- Slut på Ignorerng listan	31337	0
20	121	CMaster Kanal Servicens interna status:	31337	0
20	122	[           Kanal Record Status] \002Cached Entries:\002 %i    \002DB Efterfrågan:\002 %i    \002Cache Träffar:\002 %i    \002Effektivitet:\002 %.2f%%	31337	0
20	123	[        Användar Record Status] \002Cached Entries:\002 %i    \002DB Efterfrågan:\002 %i    \002Cache Träffar:\002 %i    \002Effektivitet:\002 %.2f%%	31337	0
20	124	[Behörighets nivå Record Status] \002Cached Entries:\002 %i    \002DB Efterfrågan:\002 %i    \002Cache Träffar:\002 %i    \002Effektivitet:\002 %.2f%%	31337	0
20	125	[         Ban Record Status] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
20	126	Senast mottagna Användar NOTIFIERING: %i	31337	0
20	127	Senast mottagna Kanal NOTIFIERING: %i	31337	0
20	128	Senast mottagna Nivå NOTIFIERING: %i	31337	0
20	129	Senast mottagna Ban NOTIFIERING: %i	31337	0
20	130	Blandat datainnehåll lokaliserad: %i	31337	0
20	131	\002Upp tid:\002 %s	31337	0
20	132	Kanalen %s har %d användare (%i operatörer)	31337	0
20	133	Mode: %s	31337	0
20	134	Flaggor satta: %s	31337	0
20	135	Kan  inte suspendera en användare med lika hög eller högre behörighetet än din egen.	31337	0
20	136	Ogiltiga tids enheter	31337	0
20	137	Ogiltig suspenderings varaktighet.	31337	0
20	138	SUSPENDERING för %s är avbruten	31337	0
20	139	%s är redan suspenderad på %s	31337	0
20	140	SUSPENDERING för %s utgår %s	31337	0
20	141	ERROR: Topicen kan max innehålla 145 tecken	31337	0
20	142	Du har otillräcklig access för att ta bort banen %s från %s's databas	31337	0
20	143	Tog bort %i banar som matchade %s	31337	0
20	144	Tog bort din tillfälliga access (%i) från kanalen %s	31337	0
20	145	Du verkar inte ha tvingad access i %s, kanske har den utgått?	31337	0
20	146	%s är inte suspenderad på %s	31337	0
20	147	%s är en IRC Operatör	31337	0
20	148	%s är INTE inloggad.	31337	0
20	149	%s inloggad via %s%s	31337	0
20	150	%s är en Officiell CService Representatör%s och inloggad som %s	31337	0
20	151	%s är en Officiell CService Administratör%s och inloggad som %s	31337	0
20	152	%s är en Officiell CService Utvecklare%s och inloggad som %s	31337	0
20	153	Det är mer än %i träfar matchande [%s]	31337	0
20	154	Var vänlig specificera din sök mask	31337	0
20	155	Inga träffar för [%s]	31337	0
20	156	%s: Rensade kanal inställningarna.	31337	0
20	158	Ogiltig valmöjlighet.	31337	0
20	159	%s är en Officiell Undernet Service Bot.	31337	0
20	160	%s är en Officiell Coder-Com Representatör%s och inloggad som %s	31337	0
20	161	%s är en Officiell Coder-Com Bidragare%s och inloggad som %s	31337	0
20	162	%s är en Officiell Coder-Com Utvecklare%s och inloggad som %s	31337	0
20	163	%s är en Officiell Coder-Com Senior%s och inloggad som %s	31337	0
20	164	 och en IRC Operatör	31337	0
20	165	Lade till ban %s till %s vid nivå %i	31337	0
20	166	%s: ban listan är tom.	31337	0
20	167	Jag är redan i den kanalen!	31337	0
20	168	Det kommandot är reserverat för IRC Operatörer	31337	0
20	169	Jag har inte Op i %s	31337	0
20	170	%s i %i minuter	31337	0
20	9998	Kom ihåg: Ingen från CService kommer någonsin att fråga efter ditt lösenord, ge INTE ut det till någon som säger sig vara från CService	31337	0
\.

