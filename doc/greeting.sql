-- $Id: greeting.sql,v 1.2 2002/03/05 17:40:08 nighty Exp $
-- spanish + catalan by Algol.

DELETE FROM translations where response_id = 9998;
COPY "translations" FROM stdin; 
1	9998	Remember: Nobody from CService will ever ask you for your password, do NOT give out your password to anyone claiming to be CService.	31337
9	9998	Recordeu: Cap membre de CService us demanarà mai la vostra contrasenya, NO doneu la vostra contrasenya a ningú encara que afirmi que és membre de CService	31337
10	9998	Recuerda: Ningún miembro de CService te ha de pedir nunca la contraseña, NUNCA des tu contraseña a nadie aunque afirme que es miembro de CService	31337
\.
