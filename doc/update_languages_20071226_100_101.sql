--
-- $Id: update_languages_20071226_100_101.sql,v 1.2 2007/12/26 20:33:31 kewlio Exp $
--
-- Update items 100 and 101 (userflags info) to support textual representations.
--

--
-- NOTE: it is safe to ignore messages in the format of the following:
--
-- DETAIL:  Key (language_id)=(x) is not present in table "languages".
-- ERROR:   invalid byte sequence for encoding "UTF8"
--
-- This is caused if you don't have all the language files loaded and is safe
-- to ignore.  The error is caused by your server not supporting the language
-- again, this is safe to ignore.

-- Delete all old records for this response_id
DELETE FROM translations WHERE response_id=100 OR response_id=101;

-- Insert new records in all known languages
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(1,100,'Invalid USERFLAGS setting. Correct values are NONE, OP or VOICE.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(1,101,'USERFLAGS for %s is %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(2,100,'Configuration USERFLAGS invalide! Les valeurs correctes sont NONE, OP, VOICE.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(2,101,'Le USERFLAGS de %s est %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(3,100,'Ugyldig USERFLAGS indstilling. Korrekte v�rdiger er NONE, OP, VOICE.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(3,101,'USERFLAGS for %s er %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(4,100,'Ongeldige USERFLAGS instelling. Juiste waarden zijn: NONE, OP, VOICE.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(4,101,'USERFLAGS voor %s is %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(5,100,'Ung�ltige USERFLAG Einstellung. Korrekte Werte sind NONE, OP, VOICE.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(5,101,'USERFLAGS f�r %s ist %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(6,100,'Ajuste de USERFLAGS inv�lido. Os valores corretos s�o NONE, OP, VOICE.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(6,101,'USERFLAGS para %s � %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(7,100,'Impostazione USERFLAGS non valida. I valori corretti sono NONE, OP o VOICE.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(7,101,'USERFLAGS per %s � %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(8,100,'Valoarea pentru USERFLAGS e incorecta. Valorile admise sunt NONE, OP, VOICE.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(8,101,'USERFLAGS pentru %s este %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(9,100,'Valor d''USERFLAGS no v�lid. Els valors correctes s�n NONE, OP, VOICE.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(9,101,'USERFLAGS per %s �s %s.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(10,100,'Valor de USERFLAGS no v�lido. Los valores v�lidos son NONE, OP, VOICE.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(10,101,'USERFLAGS en %s es %s.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(11,100,'Ervenytelen USERFLAGS beallitas. Ervenyes ertekek: NONE, OP, VOICE.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(11,101,'A(z) %s csatornanak az USERFLAGS erteke most %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(12,100,'Ge�ersiz USERFLAGS ayar�. Do�ru de�erler NONE, OP, VOICE.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(12,101,'%s i�in USERFLAGS ayar� %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(13,100,'Ugyldig USERFLAGS instilling. Korrekte verdier er: NONE, OP, VOICE.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(13,101,'USERFLAGS for %s er %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(14,100,'Tahakoume USERFLAGS ghayrou sa7i7e! Al 9iyamou a sa7i7a hia NONE,OP,VOICE.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(14,101,'Al USERFLAGS li %s houa %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(15,100,'������ �������� ������, ������ ������ ��  NONE, OP, VOICE.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(15,101,'%s � %s �������� ���',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(16,100,'Pogreshni KORISNICKI PODESUVANJA. Tocnite podesuvanja se NONE, OP, VOICE.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(16,101,'KORISNICKITE PODESUVANJA za %s se %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(18,100,'Invalid PUMPKINFLAGS setting. Correct values are NONE, OP, VOICE.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(18,101,'PUMPKINFLAGS for %s is %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(19,100,'Avasimi rithmisi gia USERFLAGS. Oi swstes axies einai NONE, OP, VOICE.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(19,101,'USERFLAGS gia to %s einai %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(20,100,'Ogiltig anv�ndar inst�llning. Korrekta v�rden �r NONE, OP, VOICE.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(20,101,'ANV�NDAR INST�LLNINGAR f�r %s �r %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(21,100,'Invalid ELFFLAGS setting. Correct values are NONE, OP, VOICE.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(21,101,'ELFFLAGS for %s is %s',31337,0);

