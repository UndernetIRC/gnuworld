-- $Id: greeting.sql,v 1.10 2003/04/09 10:13:41 nighty Exp $
-- spanish + catalan by Algol.
-- norwegian by MrIron.
-- french by nighty.
-- romanian by Tyranus.
-- Dutch by Spike.
-- Hungarian by Laccc.

DELETE FROM translations where response_id = 9998;
COPY "translations" FROM stdin;
1	9998	Remember: Nobody from CService will ever ask you for your password, do NOT give out your password to anyone claiming to be CService.	31337	0
2	9998	Rappelez-vous: Jamais personne de CService ne vous demandera votre mot de passe, ne donnez votre mot de passe à personne meme pas à quelqu'un qui dit être de CService	31337	0
4	9998	Denk eraan: Geen enkele CService medewerker zal ooit je paswoord vragen.  Geef je paswoord dan ook in GEEN geval aan iemand die beweert een CService medewerker te zijn.	31337	0
8	9998	Nu Uita: Nici un reprezentant CService nu iti va cere vreodata parola, NU iti da parola de la username cuiva care spune ca este reprezentant Cservice.	31337	0
9	9998	Recordeu: Cap membre de CService us demanarà mai la vostra contrasenya, NO doneu la vostra contrasenya a ningú encara que afirmi que és membre de CService	31337	0
10	9998	Recuerda: Ningún miembro de CService te ha de pedir nunca la contraseña, NUNCA des tu contraseña a nadie aunque afirme que es miembro de CService	31337	0
11	9998	Figyelmeztetes: Soha senkinek NE add oda a jelszavad, meg akkor sem, ha CService kepviselonek mondja magat.	31337	0
13	9998	Husk: Ingen fra CService vil noen gang spørre deg om ditt passord, så derfor må du ALDRI gi det til noen som utgir seg for å være med i CService	31337	0
\.
