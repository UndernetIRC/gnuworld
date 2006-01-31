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
VALUES(2,20,'Dur�e du ban invalide. La dur�e maximale est de %d heures.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(3,20,'Ugyldigt ban varighed. Din ban varighed kan max best� af %d timer.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(4,20,'Ongeldige banlengte. Je banlengte kan maximum %d uur bedragen.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(5,20,'Ung�ltige Banzeit. Deine Bandauer kann maximal %d Stunden sein.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(6,20,'Dura��o de ban inv�lida. Sua dura��o de ban aplicada pode ser de no m�ximo %d horas.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(7,20,'Durata del ban non valida. La durata massima di un ban pu� essere di %d ore.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(8,20,'Durata banului e incorecta. Poti da ban pe o durata maxima de %d de ore.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(9,20,'Durada de bandeig no v�lida . La durada m�xima del bandeig �s de %d hores.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(10,20,'Duraci�n de ban no v�lida. La duraci�n de ban m�xima es de %d horas.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(11,20,'Ervenytelen ban idotartalom. Maximum %d orat hasznalhatsz.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(12,20,'Ge�ersiz yasaklama s�resi. Sizin yasaklama s�reniz en fazla %d saat olabilir.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(13,20,'Ugyldig ban varighet. Den kan maksimum v�re %d timer.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(14,20,'Moudatou ban ghayrou sahiha. Almoudatou alkouswa Hia %d heures.',now()::abstime::int4,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(15,20,'��� ��� ���� �� ����, ���� ������ ��� %d ����.',now()::abstime::int4,0);
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

