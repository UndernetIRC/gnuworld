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
20	1	Tyv�rr, du �r redan inloggad som %s	31337	0
20	2	Inloggningen som %s Lyckades!	31337	0
20	3	Tyv�rr, du har inte tillr�cklig beh�righet f�r att anv�nda detta kommando	31337	0
20	4	Tyv�rr, kanalen %s �r tom	31337	0
20	5	Jag kan inte hitta %s n�gonstans	31337	0
20	6	Jag kan inte hitta %s p� kanalen %s	31337	0
20	7	Kanalen %s verkar inte vara registrerad	31337	0
20	8	Du fick op av %s (%s) on %s	31337	0
20	9	Du fick voice av %s (%s) on %s	31337	0
20	10	%s: Du �r inte i den kanalen	31337	0
20	11	%s: Du har redan voice i %s	31337	0
20	12	%s har redan voice i %s	31337	0
20	13	Du blev deopad av %s (%s) on %s	31337	0
20	14	Du blev devoicead av %s (%s) on %s	31337	0
20	15	%s har inte op i %s	31337	0
20	16	%s har inte voice i %s	31337	0
20	17	INLOGGNINGEN MISSLYCKADES som %s (Felaktigt l�senord)	31337	0
20	18	Jag �r inte i den kanalen!	31337	0
20	19	Ogiltig ban niv�. Giltig niv� �r 1-%i.	31337	0
20	20	Ogiltig ban varaktighet. Din ban varaktighet kan max vara %d timmar.	31337	0
20	21	Ban anledning kan max vara 128 bokst�ver	31337	0
20	22	Den specifika banen finns redan i min banlista!	31337	0
20	23	Banen %s �r redan t�ckt av %s	31337	0
20	24	Ogiltig min level.	31337	0
20	25	Ogiltig max level.	31337	0
20	26	ANV�NDARE: %s BEH�RIGHET: %s %s	31337	0
20	27	KANAL: %s -- AUTOMODE: %s	31337	0
20	28	SENAST MODIFIERAD: %s (%s sen)	31337	0
20	29	** SUSPENDERAD ** - Utg�r %s (Niv� %i)	31337	0
20	30	S�GS SENAST: %s sen.	31337	0
20	31	Det �r mer �n %d tr�ffar.	31337	0
20	32	Var v�nlig specificera din fr�ga.	31337	0
20	33	Slut p� beh�righetslistan	31337	0
20	34	Ingen Tr�ff!	31337	0
20	35	Kan inte l�gga till en anv�ndare med lika h�g eller h�gre beh�righet �n din egen.	31337	0
20	36	Ogiltig beh�righets niv�.	31337	0
20	37	%s finns redan i %s med beh�righetsniv� %i.	31337	0
20	38	Lade till anv�ndare %s till %s med beh�righetsniv� %i	31337	0
20	39	N�gonting gick fel:: %s	31337	0
20	40	%s: Slut p� banlista	31337	0
20	41	Kan inte visa anv�ndardetaljer (Osynlig)	31337	0
20	42	Information om: %s (%i)	31337	0
20	43	F�r tillf�llet inloggad via: %s	31337	0
20	44	URL: %s	31337	0
20	45	Spr�k: %i	31337	0
20	46	Kanaler: %s	31337	0
20	47	Input Flood Points: %i	31337	0
20	48	Ouput Flood (Bytes): %i	31337	0
20	49	%s �r registrerad av:	31337	0
20	50	%s - S�gs senast: %s sen	31337	0
20	51	Beskrivning: %s	31337	0
20	52	Floodar du mig? Ja kommer inte att lyssna p� dig n�got mer!	31337	0
20	53	Jag tror att jag har sett lite f�r mycket data, jag ignorerar dig ett tag.	31337	0
20	54	Icke komplett kommando	31337	0
20	55	F�r att anv�nda %s, m�ste du /msg %s@%s	31337	0
20	56	Tyv�rr, du m�ste vara inloggad f�r att kunna anv�nda detta kommando.	31337	0
20	57	Kanalen %s har blivit suspenderad av en cservice administrat�r.	31337	0
20	58	Din beh�righet p� %s har blivit suspenderad.	31337	0
20	59	NOOP �r satt "ON" %s	31337	0
20	60	STRICTOP �r satt "ON" %s	31337	0
20	61	Du deopade just mer �n %i personer	31337	0
20	62	SYNTAX: %s	31337	0
20	63	�kade tillf�lligt din access p� kanalen %s till %i	31337	0
20	64	%s �r registrerad.	31337	0
20	65	%s �r inte registrerad.	31337	0
20	66	Jag tror inte %s skulle uppskatta det.	31337	0
20	67	\002*** Banlista f�r kanalen %s ***\002	31337	0
20	68	%s %s Niv�: %i	31337	0
20	69	LADES AV: %s (%s)	31337	0
20	70	SEDAN: %s	31337	0
20	71	UTG�R: %s	31337	0
20	72	\002*** SLUT ***\002	31337	0
20	73	Jag vet inte vem %s �r.	31337	0
20	74	Du �r inte inloggad med mig l�ngre.	31337	0
20	75	%s verkar inte ha beh�righet i %s.	31337	0
20	76	Kan inte modifiera en anv�ndare med lika h�g eller h�gre beh�righet �n din egen.	31337	0
20	77	Kan inte ge en anv�ndare lika h�g eller h�gre beh�righet �n din egen.	31337	0
20	78	Modifierade %s's beh�righet p� kanalen %s till %i	31337	0
20	79	AUTOMODE till OP f�r %s p� kanalen %s	31337	0
20	80	AUTOMODE till VOICE f�r %s p� kanalen %s	31337	0
20	81	AUTOMODE till INGET f�r %s p� kanalen %s	31337	0
20	82	Ditt l�senordsfras kan inte vara ditt tillf�lliga nick - syntax �r: NEWPASS <ny l�senordsfras>	31337	0
20	83	L�senordet �ndrades!	31337	0
20	84	NOOP �r satt "ON" p� %s	31337	0
20	85	STRICTOP �r satt "ON" p� %s (och %s �r inte inloggad)	31337	0
20	86	STRICTOP �r satt "ON" p� %s (och %s har inte tillr�cklig beh�righet)	31337	0
20	87	Rensade kanalen %s	31337	0
20	88	%s �r redan registrerad med mig.	31337	0
20	89	Ogiltigt kanalnamn.	31337	0
20	90	Registrerade kanalen %s	31337	0
20	91	Tog bort %s fr�n min tystnads lista	31337	0
20	92	Kunde inte hitta %s i min tystnads lista	31337	0
20	93	Kan inte ta bort en anv�ndare med lika h�g eller h�gre beh�righet �n din egen.	31337	0
20	94	Du kan inte ta bort dig sj�lv fr�n en kanal som du �ger.	31337	0
20	95	Tog bort anv�ndaren %s fr�n %s	31337	0
20	96	Din OSYNLIGHETS inst�llning �r nu "ON".	31337	0
20	97	Din OSYNLIGHETS inst�llning �r nu "OFF".	31337	0
20	98	%s f�r %s �r %s	31337	0
20	99	V�rdet f�r %s m�ste vara "ON" eller "OFF"	31337	0
20	100	Ogiltig anv�ndar inst�llning. Korrekta v�rden �r NONE, OP, VOICE.	31337	0
20	101	ANV�NDAR INST�LLNINGAR f�r %s �r %s	31337	0
20	102	V�rdet f�r MASSDEOPPRO m�ste vara 0-7	31337	0
20	103	MASSDEOPPRO f�r %s �r satt till %d	31337	0
20	104	V�rdet f�r FLOODPRO m�ste vara 0-7	31337	0
20	105	FLOODPRO f�r %s �r satt till %d	31337	0
20	106	BESKRIVNINGEN kan MAX vara 80 tecken!	31337	0
20	107	BESKRIVNINGEN f�r %s �r rensad.	31337	0
20	108	BESKRIVNINGEN f�r %s �r: %s	31337	0
20	109	URL:en kan max vara 75 tecken!	31337	0
20	110	URL:en f�r %s �r rensad.	31337	0
20	111	URL f�r %s �r: %s	31337	0
20	112	Nyckelorden kan max vara 80 tecken!	31337	0
20	113	NYCKELORDEN f�r %s �r: %s	31337	0
20	114	Spr�ket �r satt till %s.	31337	0
20	115	ERROR: Ogiltig spr�k inst�llning	31337	0
20	116	Kan inte hitta kanalen %s p� n�tverket!	31337	0
20	117	ERROR: Ogiltig kanal inst�llning.	31337	0
20	118	Ignorerings lista:	31337	0
20	119	Ignorerings listan �r tom	31337	0
20	120	-- Slut p� Ignorerng listan	31337	0
20	121	CMaster Kanal Servicens interna status:	31337	0
20	122	[           Kanal Record Status] \002Cached Entries:\002 %i    \002DB Efterfr�gan:\002 %i    \002Cache Tr�ffar:\002 %i    \002Effektivitet:\002 %.2f%%	31337	0
20	123	[        Anv�ndar Record Status] \002Cached Entries:\002 %i    \002DB Efterfr�gan:\002 %i    \002Cache Tr�ffar:\002 %i    \002Effektivitet:\002 %.2f%%	31337	0
20	124	[Beh�righets niv� Record Status] \002Cached Entries:\002 %i    \002DB Efterfr�gan:\002 %i    \002Cache Tr�ffar:\002 %i    \002Effektivitet:\002 %.2f%%	31337	0
20	125	[         Ban Record Status] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
20	126	Senast mottagna Anv�ndar NOTIFIERING: %i	31337	0
20	127	Senast mottagna Kanal NOTIFIERING: %i	31337	0
20	128	Senast mottagna Niv� NOTIFIERING: %i	31337	0
20	129	Senast mottagna Ban NOTIFIERING: %i	31337	0
20	130	Blandat datainneh�ll lokaliserad: %i	31337	0
20	131	\002Upp tid:\002 %s	31337	0
20	132	Kanalen %s har %d anv�ndare (%i operat�rer)	31337	0
20	133	Mode: %s	31337	0
20	134	Flaggor satta: %s	31337	0
20	135	Kan  inte suspendera en anv�ndare med lika h�g eller h�gre beh�righetet �n din egen.	31337	0
20	136	Ogiltiga tids enheter	31337	0
20	137	Ogiltig suspenderings varaktighet.	31337	0
20	138	SUSPENDERING f�r %s �r avbruten	31337	0
20	139	%s �r redan suspenderad p� %s	31337	0
20	140	SUSPENDERING f�r %s utg�r %s	31337	0
20	141	ERROR: Topicen kan max inneh�lla 145 tecken	31337	0
20	142	Du har otillr�cklig access f�r att ta bort banen %s fr�n %s's databas	31337	0
20	143	Tog bort %i banar som matchade %s	31337	0
20	144	Tog bort din tillf�lliga access (%i) fr�n kanalen %s	31337	0
20	145	Du verkar inte ha tvingad access i %s, kanske har den utg�tt?	31337	0
20	146	%s �r inte suspenderad p� %s	31337	0
20	147	%s �r en IRC Operat�r	31337	0
20	148	%s �r INTE inloggad.	31337	0
20	149	%s inloggad via %s%s	31337	0
20	150	%s �r en Officiell CService Representat�r%s och inloggad som %s	31337	0
20	151	%s �r en Officiell CService Administrat�r%s och inloggad som %s	31337	0
20	152	%s �r en Officiell CService Utvecklare%s och inloggad som %s	31337	0
20	153	Det �r mer �n %i tr�far matchande [%s]	31337	0
20	154	Var v�nlig specificera din s�k mask	31337	0
20	155	Inga tr�ffar f�r [%s]	31337	0
20	156	%s: Rensade kanal inst�llningarna.	31337	0
20	158	Ogiltig valm�jlighet.	31337	0
20	159	%s �r en Officiell Undernet Service Bot.	31337	0
20	160	%s �r en Officiell Coder-Com Representat�r%s och inloggad som %s	31337	0
20	161	%s �r en Officiell Coder-Com Bidragare%s och inloggad som %s	31337	0
20	162	%s �r en Officiell Coder-Com Utvecklare%s och inloggad som %s	31337	0
20	163	%s �r en Officiell Coder-Com Senior%s och inloggad som %s	31337	0
20	164	 och en IRC Operat�r	31337	0
20	165	Lade till ban %s till %s vid niv� %i	31337	0
20	166	%s: ban listan �r tom.	31337	0
20	167	Jag �r redan i den kanalen!	31337	0
20	168	Det kommandot �r reserverat f�r IRC Operat�rer	31337	0
20	169	Jag har inte Op i %s	31337	0
20	170	%s i %i minuter	31337	0
20	9998	Kom ih�g: Ingen fr�n CService kommer n�gonsin att fr�ga efter ditt l�senord, ge INTE ut det till n�gon som s�ger sig vara fr�n CService	31337	0
\.

