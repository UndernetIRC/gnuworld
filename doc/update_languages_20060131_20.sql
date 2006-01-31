--
-- $Id: update_languages_20060131_20.sql,v 1.1 2006/01/31 00:25:13 kewlio Exp $
--
-- Update item 20 (ban duration) to move from static to dynamic durations
--

--
-- NOTE: it is safe to ignore messages in the format of the following:
--
-- DETAIL:  Key (language_id)=(x) is not present in table "languages".
--
-- This is caused if you don't have all the language files loaded and is safe
-- to ignore.

-- Delete all old records for this response_id
DELETE FROM translations WHERE response_id=20;

-- Insert new records in all known languages
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(1,20,'Invalid ban duration. Your ban duration can be a maximum of %d hours.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(2,20,'Durée du ban invalide. La durée maximale est de %d heures.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(3,20,'Ugyldigt ban varighed. Din ban varighed kan max bestå af %d timer.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(4,20,'Ongeldige banlengte. Je banlengte kan maximum %d uur bedragen.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(5,20,'Ungültige Banzeit. Deine Bandauer kann maximal %d Stunden sein.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(6,20,'Duração de ban inválida. Sua duração de ban aplicada pode ser de no máximo %d horas.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(7,20,'Durata del ban non valida. La durata massima di un ban può essere di %d ore.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(8,20,'Durata banului e incorecta. Poti da ban pe o durata maxima de %d de ore.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(9,20,'Durada de bandeig no vàlida . La durada màxima del bandeig és de %d hores.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(10,20,'Duración de ban no válida. La duración de ban máxima es de %d horas.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(11,20,'Ervenytelen ban idotartalom. Maximum %d orat hasznalhatsz.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(12,20,'Geçersiz yasaklama süresi. Sizin yasaklama süreniz en fazla %d saat olabilir.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(13,20,'Ugyldig ban varighet. Den kan maksimum være %d timer.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(14,20,'Moudatou ban ghayrou sahiha. Almoudatou alkouswa Hia %d heures.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(15,20,'îùê æîï äáàï ìà çå÷é, äæîï äîéøáé äåà %d ùòåú.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(16,20,'Pogreshno vremetraenje na banot. Vashiot ban moze da bide najmnogu %d chasa.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(18,20,'Invalid pitchfork duration. Your pitchfork duration can be a maximum of %d hours.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(19,20,'Avasimi diarkeia gia ban. H diarkeia tou ban den mporei na einai pano apo to anwtato orio twn %d hours.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(20,20,'Ogiltig ban varaktighet. Din ban varaktighet kan max vara %d timmar.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(21,20,'Invalid curse duration. Your curse duration can be a maximum of %d hours.',now()::abstime::int4,0);

