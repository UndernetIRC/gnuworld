-- $Id: language_german.sql,v 1.4 2007/12/26 19:13:00 kewlio Exp $
-- Language Definition Table for German
-- 16/8/2002 volta <volta2@gmx.de>
-- beta readers: Tha_Funk

-- Delete everything german
DELETE FROM translations WHERE language_id = 5;

COPY translations FROM stdin;
5	1	Verzeihung, aber du bist schon eingeloggt als %s	31337	0
5	2	AUTHENTIFIKATION ERFOLGREICH als %s	31337	0
5	3	Verzeihung, aber du hast nicht genug Berechtigung um diesen Befehl auszuf�hren	31337	0
5	4	Verzeihung, aber der Channel %s ist leer	31337	0
5	5	Ich sehe %s nirgendswo	31337	0
5	6	Ich kann %s nicht im Channel %s finden	31337	0
5	7	Der Channel %s scheint nicht registriert zu sein	31337	0
5	8	Du wurdest von %s (%s) in %s geopt	31337	0
5	9	Du wurdest von %s (%s) in %s gevoict	31337	0
5	10	%s: Du bist nicht in diesen Channel	31337	0
5	11	%s ist bereits Op in %s	31337	0
5	12	%s ist bereits Voice in %s	31337	0
5	13	Du wurdest von %s (%s) in %s gedeopt	31337	0
5	14	Du wurdest von %s (%s) in %s gedevoict	31337	0
5	15	%s ist nicht Op in %s	31337	0
5	16	%s ist nicht Voice in %s	31337	0
5	17	AUTHENTIFIKATION FEHLGESCHLAGEN als %s (Falsches Passwort)	31337	0
5	18	Ich bin nicht in diesem Channel!	31337	0
5	19	Ung�ltiges Banlevel. G�ltig ist 1-%i.	31337	0
5	20	Ung�ltige Banzeit. Deine Bandauer kann maximal %d Stunden sein.	31337	0
5	21	Bangrund darf nicht 128 Zeichen �berschreiten	31337	0
5	22	Spezifizierter Ban ist bereits in der Banliste!	31337	0
5	23	Der Ban %s ist bereits eingedeckt von %s	31337	0
5	24	Ung�ltiges Minimallevel.	31337	0
5	25	Ung�ltiges Maximallevel.	31337	0
5	26	BENUTZER: %s LEVEL: %s %s	31337	0
5	27	CHANNEL: %s -- AUTOMODE: %s	31337	0
5	28	ZULETZT VER�NDERT: %s (vor %s)	31337	0
5	29	** SUSPENDIERT ** - Verf�llt in %s (Level %i)	31337	0
5	30	ZULETZT GESEHEN: vor %s.	31337	0
5	31	Es gibt mehr als %d passende Eintr�ge.	31337	0
5	32	Bitte begrenze deine Anfrage.	31337	0
5	33	Ende der Benutzerliste	31337	0
5	34	Kein passender Eintrag!	31337	0
5	35	Kann keinen Benutzer mit gleichen oder h�heren Level als dein eigenes hinzuf�gen.	31337	0
5	36	Ung�ltiges Berechtigungslevel.	31337	0
5	37	%s ist bereits in der Benutzerliste von %s mit Level %i.	31337	0
5	38	Habe Benutzer %s in %s mit Level %i hinzugef�gt	31337	0
5	39	Irgendetwas lief falsch: %s	31337	0
5	40	%s: Ende der Banliste	31337	0
5	41	Nicht in der Lage die Benutzerdetails zu zeigen (unsichtbar)	31337	0
5	42	Informationen �ber: %s (%i)	31337	0
5	43	Zur Zeit eingeloggt via: %s	31337	0
5	44	URL: %s	31337	0
5	45	Sprache: %i	31337	0
5	46	Channels: %s	31337	0
5	47	Eingabefloodpunkte: %i	31337	0
5	48	Ausgabeflood (Bytes): %i	31337	0
5	49	%s ist registriert von:	31337	0
5	50	%s - zuletzt gesehen: vor %s	31337	0
5	51	Beschreibung: %s	31337	0
5	52	Du willst mich flooden? Ich werde dir nicht weiter zuh�ren	31337	0
5	53	Ich glaube ich habe dir zu viele Daten geschickt. Ich werde dich eine Weile lang ignorieren.	31337	0
5	54	Unvollst�ndiger Befehl	31337	0
5	55	Um %s zu benutzen, musst du mir via /msg %s@%s schreiben	31337	0
5	56	Verzeihung, aber du musst eingeloggt sein um diesen Befehl zu benutzen.	31337	0
5	57	Der Channel %s wurde von einen CService Administrator suspendiert.	31337	0
5	58	Deine Berechtigung in %s wurde suspendiert.	31337	0
5	59	Die NOOP Einstellung wurde in %s gesetzt	31337	0
5	60	Die STRICTOP Einstellung ist in %s gesetzt	31337	0
5	61	Du hast gerade mehr als %i User gedeopt	31337	0
5	62	SYNTAX: %s	31337	0
5	63	Habe deine Berechtigung in %s vor�bergehend auf %i erh�ht	31337	0
5	64	%s ist registriert.	31337	0
5	65	%s ist nicht registriert.	31337	0
5	66	Ich denke nicht, da� %s dies sch�tzen w�rde.	31337	0
5	67	\002*** Banliste f�r den Channel %s ***\002	31337	0
5	68	%s %s Level: %i	31337	0
5	69	HINZUGEF�GT VON: %s (%s)	31337	0
5	70	SEIT: %s	31337	0
5	71	VERF�LLT: %s	31337	0
5	72	\002*** ENDE ***\002	31337	0
5	73	Ich wei� nicht wer %s ist.	31337	0
5	74	Du bist nicht mehr bei mir authentifiziert.	31337	0
5	75	%s scheint keine Berechtigung in %s zu haben.	31337	0
5	76	Kann keinen Benutzer mit gleichen oder h�heren Level als dein eigenes modifizieren.	31337	0
5	77	Kann keinen Benutzer ein gleiches oder h�heres Level als dein eigenes geben.	31337	0
5	78	Habe %s's Berechtigungslevel im Channel %s zu %i modifizert	31337	0
5	79	Habe AUTOMODE f�r %s auf OP im Channel %s gesetzt	31337	0
5	80	Habe AUTOMODE f�r %s auf VOICE im Channel %s gesetzt	31337	0
5	81	Habe AUTOMODE f�r %s auf NONE im Channel %s gesetzt	31337	0
5	82	Dein Passwort kann nicht dein Username oder aktueller Nickname sein - Syntax ist: NEWPASS <neues Passwort>	31337	0
5	83	Passwort wurde erfolgreich ge�ndert.	31337	0
5	84	Die NOOP Einstellung ist in %s gesetzt	31337	0
5	85	Die STRICTOP Einstellung ist in %s gesetzt (und %s ist nicht authentifiziert)	31337	0
5	86	Die STRICTOP Einstellung ist in %s gesetzt (und %s hat nicht gen�gend Berechtigung)	31337	0
5	87	Habe Channel %s bereinigt	31337	0
5	88	%s ist bereits mit mir registriert.	31337	0
5	89	Ung�ltiger Channelname.	31337	0
5	90	Habe Channel %s registriert	31337	0
5	91	Habe %s von meiner Silenceliste entfernt	31337	0
5	92	Kann %s nicht in meiner Silenceliste finden	31337	0
5	93	Kann keinen Benutzer mit gleichen oder h�heren Level als dein eigenes entfernen	31337	0
5	94	Du kannst dich nicht selbst von einen Channel entfernen, den du selber besitzt	31337	0
5	95	Habe Benutzer %s von %s entfernt	31337	0
5	96	Deine INVISIBLE Einstellung ist jetzt AN.	31337	0
5	97	Deine INVISIBLE Einstellung ist jetzt AUS.	31337	0
5	98	%s f�r %s ist %s	31337	0
5	99	Wert f�r %s mu� ON oder OFF sein.	31337	0
5	100	Ung�ltige USERFLAG Einstellung. Korrekte Werte sind NONE, OP, VOICE.	31337	0
5	101	USERFLAGS f�r %s ist %s	31337	0
5	102	Wert f�r MASSDEOPPRO mu� 0-7 sein	31337	0
5	103	MASSDEOPPRO f�r %s ist auf %d gesetzt	31337	0
5	104	Wert f�r FLOODPRO mu� 0-7 sein	31337	0
5	105	FLOODPRO f�r %s ist auf %d gesetzt	31337	0
5	106	DESCRIPTION kann maximal 80 Zeichen sein!	31337	0
5	107	DESCRIPTION f�r %s wurde gel�scht.	31337	0
5	108	DESCRIPTION f�r %s ist: %s	31337	0
5	109	URL kann maximal 75 Zeichen sein!	31337	0
5	110	URL f�r %s wurde gel�scht.	31337	0
5	111	URL f�r %s ist: %s	31337	0
5	112	Die Zeichenkette der Schl�sselw�rter darf nicht 80 Zeichen �berschreiten!	31337	0
5	113	KEYWORDS f�r %s sind: %s	31337	0
5	114	Sprache wurde auf %s gesetzt.	31337	0
5	115	FEHLER: Ung�ltige Sprachwahl.	31337	0
5	116	Kann Channel %s nicht auf dem Netzwerk lokalisieren!	31337	0
5	117	FEHLER: Ung�ltige Channeleinstellung.	31337	0
5	118	Ignoreliste:	31337	0
5	119	Ignoreliste ist leer	31337	0
5	120	-- Ende der Ignoreliste	31337	0
5	121	CMaster Channel Services interner Status:	31337	0
5	122	[           Channel Rekord Statistiken] \002Gecachte Eintr�ge:\002 %i    \002DB Anfragen:\002 %i    \002Cachetreffer:\002 %i    \002Effektivit�t:\002 %.2f%%	31337	0
5	123	[              User Rekord Statistiken] \002Gecachte Eintr�ge:\002 %i    \002DB Anfragen:\002 %i    \002Cachetreffer:\002 %i    \002Effektivit�t:\002 %.2f%%	31337	0
5	124	[Berechtigungslevel Rekord Statistiken] \002Gecachte Eintr�ge:\002 %i    \002DB Anfragen:\002 %i    \002Cachetreffer:\002 %i    \002Effektivit�t:\002 %.2f%%	31337	0
5	125	[               Ban Rekord Statistiken] \002Gecachte Eintr�ge:\002 %i    \002DB Anfragen:\002 %i    \002Cachetreffer:\002 %i    \002Effektivit�t:\002 %.2f%%	31337	0
5	126	Letzte empfangene Benutzerbenachrichtigung: %i	31337	0
5	127	Letzte empfangene Channelbenachrichtigung: %i	31337	0
5	128	Letzte empfangene Levelbenachrichtigung: %i	31337	0
5	129	Letzte empfangene Banbenachrichtigung: %i	31337	0
5	130	Vordefinierte Datencontainer zugewiesen: %i	31337	0
5	131	\002Uptime:\002 %s	31337	0
5	132	Channel %s hat %d User (%i Operatoren)	31337	0
5	133	Mode ist: %s	31337	0
5	134	Einstellungen: %s	31337	0
5	135	Kann keinen Benutzer mit gleichen oder h�heren Level als dein eigenes suspendieren	31337	0
5	136	Ung�ltige Zeiteinheiten	31337	0
5	137	Ung�ltige Suspendierungsl�nge.	31337	0
5	138	SUSPENDIERUNG f�r %s wurde aufgehoben	31337	0
5	139	%s ist bereits suspendiert in %s	31337	0
5	140	SUSPENDIERUNG f�r %s wird in %s verfallen	31337	0
5	141	FEHLER: Topic darf nicht 145 Zeichen �berschreiten	31337	0
5	142	Du hast nicht genug Berechtigung um den Ban %s von %s's Database zu entfernen	31337	0
5	143	Habe %i Bans entfernt, die auf %s zutrafen	31337	0
5	144	Habe deine vor�bergehende Berechtigung von %i vom Channel %s entfernt	31337	0
5	145	Es scheint so als h�ttest du keine erzwungene Berechtigung in %s, vielleicht ist sie verfallen?	31337	0
5	146	%s ist nicht suspendiert in %s	31337	0
5	147	%s ist ein IRC Operator	31337	0
5	148	%s ist NICHT eingeloggt.	31337	0
5	149	%s ist eingeloggt als %s%s	31337	0
5	150	%s ist ein offizieller CService Vertreter%s und eingeloggt als %s	31337	0
5	151	%s ist ein offizieller CService Administrator%s und eingeloggt als %s	31337	0
5	152	%s ist ein offizieller CService Entwickler%s und eingeloggt als %s	31337	0
5	153	Es gibt mehr als %i passende Eintr�ge [%s]	31337	0
5	154	Bitte begrenze deine Suchmaske	31337	0
5	155	Keine passenden Eintr�ge f�r [%s]	31337	0
5	156	%s: Habe Channelmodes gel�scht.	31337	0
5	158	Ung�ltige Option.	31337	0
5	159	%s ist ein offizieller Undernet Service Bot.	31337	0
5	160	%s ist ein offizieller Coder-Com Vertreter%s und eingeloggt als %s	31337	0
5	161	%s ist ein offizieller Coder-Com Mitarbeiter%s und eingeloggt als %s	31337	0
5	162	%s ist ein offizieller Coder-Com Entwickler%s und eingeloggt als %s	31337	0
5	163	%s ist ein offizieller Coder-Com Dienst�ltester%s und eingeloggt als %s	31337	0
5	164	 und ein IRC Operator	31337	0
5	165	Habe den ban %s zu %s mit Level %i hinzugef�gt	31337	0
5	166	%s: Banliste ist leer.	31337	0
5	167	Ich bin bereits in diesen Channel!	31337	0
5	168	Dieser Befehl ist f�r IRC Operatoren reserviert	31337	0
5	169	Ich bin nicht geopt in %s	31337	0
5	170	%s f�r %i Minuten	31337	0
\.

-- And also the greeting :)
INSERT INTO translations VALUES (5,9998,'Denke immer daran: Niemand vom CService wird dich jemals nach Deinem Passwort fragen. Gebe dein Passwort NIEMALS zu jemanden, der behauptet ein CService Mitglied zu sein.',31337,0);



