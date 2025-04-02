--
-- $Id: update_languages_20070807_31.sql,v 1.1 2007/08/07 21:22:28 kewlio Exp $
--
-- Update item 31 (too many matching bans) to move from static to dynamic bans
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
DELETE FROM translations WHERE response_id=31;

-- Insert new records in all known languages
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(1,31,'There are more than %d matching entries.',date_part('epoch', CURRENT_TIMESTAMP)::int,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(2,31,'Il y a plus de %d requêtes trouvées.',date_part('epoch', CURRENT_TIMESTAMP)::int,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(3,31,'Der er mere end %d matchende indtastninger.',date_part('epoch', CURRENT_TIMESTAMP)::int,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(4,31,'Er zijn meer dan %d bijhorende records.',date_part('epoch', CURRENT_TIMESTAMP)::int,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(5,31,'Es gibt mehr als %d passende Einträge.',date_part('epoch', CURRENT_TIMESTAMP)::int,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(6,31,'Há mais de %d entradas.',date_part('epoch', CURRENT_TIMESTAMP)::int,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(7,31,'Sono presenti più di %d corrispondenze.',date_part('epoch', CURRENT_TIMESTAMP)::int,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(8,31,'Sunt mai mult de %d rezultate.',date_part('epoch', CURRENT_TIMESTAMP)::int,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(9,31,'Hi ha més de %d entrades coincidents.',date_part('epoch', CURRENT_TIMESTAMP)::int,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(10,31,'Hay más de %d entradas coincidentes.',date_part('epoch', CURRENT_TIMESTAMP)::int,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(11,31,'%d-nel tobb hasonlo adat van.',date_part('epoch', CURRENT_TIMESTAMP)::int,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(12,31,'%d den fazla eþleþen giriþ var.',date_part('epoch', CURRENT_TIMESTAMP)::int,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(13,31,'Det er mere enn %d treff som passer dine kriterier.',date_part('epoch', CURRENT_TIMESTAMP)::int,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(14,31,'Youjado aktare min %d talabe',date_part('epoch', CURRENT_TIMESTAMP)::int,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(15,31,'There are more than %d matching entries.',date_part('epoch', CURRENT_TIMESTAMP)::int,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(16,31,'Ima poveke od %d isti.',date_part('epoch', CURRENT_TIMESTAMP)::int,0);
INSERT INTO translations VALUES (16,31,'There are more than %d matching entries.',31337,0);
VALUES(19,31,'Iparxoun pio polles apo %d teriastes Eisodous i Simetoxes.',date_part('epoch', CURRENT_TIMESTAMP)::int,0);
INSERT INTO translations VALUES (16,31,'There are more than %d matching entries.',31337,0);
VALUES(20,31,'Det är mer än %d träffar.',date_part('epoch', CURRENT_TIMESTAMP)::int,0);

