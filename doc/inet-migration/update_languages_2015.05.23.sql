-- Update item 20, replace max ban duration of 'hours' with 'days'	--Seven (gergo_f@yahoo.com)
-- Note: Couldn't translate to last 3 languages: Arabic, Hebrew, Macedonian.

DELETE FROM translations WHERE response_id=20;

-- Insert new records in all known languages
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(1,20,'Invalid ban duration. Your ban duration can be a maximum of %d days.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(2,20,'Durée du ban invalide. La durée maximale est de %d journé.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(3,20,'Ugyldigt ban varighed. Din ban varighed kan max bestå af %d dage.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(4,20,'Ongeldige banlengte. Je banlengte kan maximum %d uur dagen.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(5,20,'Ungültige Banzeit. Deine Bandauer kann maximal %d Tage sein.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(6,20,'Duração de ban inválida. Sua duração de ban aplicada pode ser de no máximo %d dias.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(7,20,'Durata del ban non valida. La durata massima di un ban può essere di %d giorni.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(8,20,'Durata banului e incorecta. Poti da ban pe o durata maxima de %d de zile.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(9,20,'Durada de bandeig no vàlida. La durada màxima del bandeig és de %d dia.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(10,20,'Duración de ban no válida. La duración de ban máxima es de %d día.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(11,20,'Ervenytelen ban idotartalom. Maximum %d napot hasznalhatsz.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(12,20,'Geçersiz yasaklama süresi. Sizin yasaklama süreniz en fazla %d günler olabilir.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(13,20,'Ugyldig ban varighet. Den kan maksimum være %d dager.',31337,0);
-- TODO: Arabic
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(14,20,'Moudatou ban ghayrou sahiha. Almoudatou alkouswa Hia %d heures.',31337,0);
-- TODO: Hebrew
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(15,20,'îùê æîï äáàï ìà çå÷é, äæîï äîéøáé äåà %d ùòåú.',31337,0);
-- TODO: Macedonian
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(16,20,'Pogreshno vremetraenje na banot. Vashiot ban moze da bide najmnogu %d chasa.',31337,0);
-- ------------------------------------------------------------------------------------------------------
-- Language update for new features

DELETE FROM translations WHERE response_id > 183 AND response_id < 195;

INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(1,184,'The NOVOICE flag is set on %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(1,185,'Your access on %s has been suspended.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(1,186,'Your suspension on %s has been cancelled.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(1,187,'You have been added to channel %s with access level %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(1,188,'Your access on %s has been modified from %i to %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(1,189,'Your access from %s has been removed.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(1,190,'Target user %s on channel %s has insufficient access for an automode VOICE',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(1,191,'Target user %s on channel %s has insufficient access for an automode OP',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(1,192,'Your reason must be %i - %i characters long.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(1,193,'Reason: %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(1,194,'UNSUSPENDED - %s',31337,0);
-- TODO: French language
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(2,184,'The NOVOICE flag is set on %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(2,185,'Your access on %s has been suspended.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(2,186,'Your suspension on %s has been cancelled.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(2,187,'You have been added to channel %s with access level %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(2,188,'Your access on %s has been modified from %i to %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(2,189,'Your access from %s has been removed.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(2,190,'Target user %s on channel %s has insufficient access for an automode VOICE',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(2,191,'Target user %s on channel %s has insufficient access for an automode OP',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(2,192,'Your reason must be %i - %i characters long.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(2,193,'Reason: %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(2,194,'UNSUSPENDED - %s',31337,0);
-- TODO: Danish language
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(3,184,'The NOVOICE flag is set on %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(3,185,'Your access on %s has been suspended.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(3,186,'Your suspension on %s has been cancelled.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(3,187,'You have been added to channel %s with access level %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(3,188,'Your access on %s has been modified from %i to %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(3,189,'Your access from %s has been removed.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(3,190,'Target user %s on channel %s has insufficient access for an automode VOICE',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(3,191,'Target user %s on channel %s has insufficient access for an automode OP',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(3,192,'Your reason must be %i - %i characters long.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(3,193,'Reason: %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(3,194,'UNSUSPENDED - %s',31337,0);
-- TODO: Dutch language
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(4,184,'The NOVOICE flag is set on %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(4,185,'Your access on %s has been suspended.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(4,186,'Your suspension on %s has been cancelled.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(4,187,'You have been added to channel %s with access level %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(4,188,'Your access on %s has been modified from %i to %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(4,189,'Your access from %s has been removed.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(4,190,'Target user %s on channel %s has insufficient access for an automode VOICE',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(4,191,'Target user %s on channel %s has insufficient access for an automode OP',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(4,192,'Your reason must be %i - %i characters long.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(4,193,'Reason: %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(4,194,'UNSUSPENDED - %s',31337,0);
-- TODO: German language
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(5,184,'The NOVOICE flag is set on %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(5,185,'Your access on %s has been suspended.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(5,186,'Your suspension on %s has been cancelled.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(5,187,'You have been added to channel %s with access level %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(5,188,'Your access on %s has been modified from %i to %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(5,189,'Your access from %s has been removed.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(5,190,'Target user %s on channel %s has insufficient access for an automode VOICE',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(5,191,'Target user %s on channel %s has insufficient access for an automode OP',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(5,192,'Your reason must be %i - %i characters long.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(5,193,'Reason: %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(5,194,'UNSUSPENDED - %s',31337,0);
-- TODO: Portuguese language
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(6,184,'The NOVOICE flag is set on %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(6,185,'Your access on %s has been suspended.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(6,186,'Your suspension on %s has been cancelled.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(6,187,'You have been added to channel %s with access level %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(6,188,'Your access on %s has been modified from %i to %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(6,189,'Your access from %s has been removed.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(6,190,'Target user %s on channel %s has insufficient access for an automode VOICE',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(6,191,'Target user %s on channel %s has insufficient access for an automode OP',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(6,192,'Your reason must be %i - %i characters long.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(6,193,'Reason: %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(6,194,'UNSUSPENDED - %s',31337,0);
-- TODO: Italian language
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(7,184,'The NOVOICE flag is set on %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(7,185,'Your access on %s has been suspended.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(7,186,'Your suspension on %s has been cancelled.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(7,187,'You have been added to channel %s with access level %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(7,188,'Your access on %s has been modified from %i to %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(7,189,'Your access from %s has been removed.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(7,190,'Target user %s on channel %s has insufficient access for an automode VOICE',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(7,191,'Target user %s on channel %s has insufficient access for an automode OP',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(7,192,'Your reason must be %i - %i characters long.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(7,193,'Reason: %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(7,194,'UNSUSPENDED - %s',31337,0);
-- Romanian language
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(8,184,'Flag-ul NOVOICE e activ pe %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(8,185,'Accesul tau pe %s este suspendat.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(8,186,'Suspendarea ta pe %s a fost anulat.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(8,187,'Ai fost adaugat pe canalul %s cu nivelul de acces %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(8,188,'Nivelul tau de acces pe %s a fost modificat de la %i la %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(8,189,'Accesul tau de pe %s a fost inlaturat.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(8,190,'Userul target %s pe canalul %s nu are acces suficient pentru un automode VOICE',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(8,191,'Userul target %s pe canalul %s nu are acces suficient pentru un automode OP',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(8,192,'Motivul poate sa aiba o lungime de caractere intre %i si %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(8,193,'Motivul: %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(8,194,'UNSUSPENDED - %s',31337,0);
-- TODO: Catalan language
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(9,184,'The NOVOICE flag is set on %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(9,185,'Your access on %s has been suspended.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(9,186,'Your suspension on %s has been cancelled.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(9,187,'You have been added to channel %s with access level %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(9,188,'Your access on %s has been modified from %i to %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(9,189,'Your access from %s has been removed.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(9,190,'Target user %s on channel %s has insufficient access for an automode VOICE',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(9,191,'Target user %s on channel %s has insufficient access for an automode OP',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(9,192,'Your reason must be %i - %i characters long.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(9,193,'Reason: %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(9,194,'UNSUSPENDED - %s',31337,0);
-- TODO: Spanish language
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(10,184,'The NOVOICE flag is set on %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(10,185,'Your access on %s has been suspended.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(10,186,'Your suspension on %s has been cancelled.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(10,187,'You have been added to channel %s with access level %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(10,188,'Your access on %s has been modified from %i to %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(10,189,'Your access from %s has been removed.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(10,190,'Target user %s on channel %s has insufficient access for an automode VOICE',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(10,191,'Target user %s on channel %s has insufficient access for an automode OP',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(10,192,'Your reason must be %i - %i characters long.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(10,193,'Reason: %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(10,194,'UNSUSPENDED - %s',31337,0);
-- Hungarian language
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(11,184,'A NOVOICE flag aktiv a(z) %s csatornan',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(11,185,'A jogod a %s csatornan fel lett fuggesztve.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(11,186,'A felfuggesztesed a %s csatornan vissza lett vonva.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(11,187,'Hozza lettel adva a(z) %s csatornahoz %i joggal.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(11,188,'A jogod a %s csatornan %i -rol %i -ra(re) lett modositva.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(11,189,'A jogod a %s csatornarol el lett tavolitva.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(11,190,'A celuser %s -nak(nek) a(z) %s csatornan nincs eleg joga egy automod VOICE -hoz.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(11,191,'A celuser %s -nak(nek) a(z) %s csatornan nincs eleg joga egy automod OP -hoz.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(11,192,'Az indok %i es %i karakter hosszusagu lehet.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(11,193,'Indok: %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(11,194,'FELFUGGESZTVE VOLT - %s',31337,0);
-- TODO: Turkish language
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(12,184,'The NOVOICE flag is set on %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(12,185,'Your access on %s has been suspended.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(12,186,'Your suspension on %s has been cancelled.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(12,187,'You have been added to channel %s with access level %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(12,188,'Your access on %s has been modified from %i to %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(12,189,'Your access from %s has been removed.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(12,190,'Target user %s on channel %s has insufficient access for an automode VOICE',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(12,191,'Target user %s on channel %s has insufficient access for an automode OP',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(12,192,'Your reason must be %i - %i characters long.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(12,193,'Reason: %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(12,194,'UNSUSPENDED - %s',31337,0);
-- TODO: Norwegian language
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(13,184,'The NOVOICE flag is set on %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(13,185,'Your access on %s has been suspended.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(13,186,'Your suspension on %s has been cancelled.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(13,187,'You have been added to channel %s with access level %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(13,188,'Your access on %s has been modified from %i to %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(13,189,'Your access from %s has been removed.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(13,190,'Target user %s on channel %s has insufficient access for an automode VOICE',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(13,191,'Target user %s on channel %s has insufficient access for an automode OP',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(13,192,'Your reason must be %i - %i characters long.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(13,193,'Reason: %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(13,194,'UNSUSPENDED - %s',31337,0);
-- TODO: Arabic language
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(14,184,'The NOVOICE flag is set on %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(14,185,'Your access on %s has been suspended.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(14,186,'Your suspension on %s has been cancelled.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(14,187,'You have been added to channel %s with access level %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(14,188,'Your access on %s has been modified from %i to %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(14,189,'Your access from %s has been removed.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(14,190,'Target user %s on channel %s has insufficient access for an automode VOICE',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(14,191,'Target user %s on channel %s has insufficient access for an automode OP',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(14,192,'Your reason must be %i - %i characters long.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(14,193,'Reason: %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(14,194,'UNSUSPENDED - %s',31337,0);
-- TODO: Hebrew language
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(15,184,'The NOVOICE flag is set on %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(15,185,'Your access on %s has been suspended.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(15,186,'Your suspension on %s has been cancelled.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(15,187,'You have been added to channel %s with access level %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(15,188,'Your access on %s has been modified from %i to %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(15,189,'Your access from %s has been removed.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(15,190,'Target user %s on channel %s has insufficient access for an automode VOICE',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(15,191,'Target user %s on channel %s has insufficient access for an automode OP',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(15,192,'Your reason must be %i - %i characters long.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(15,193,'Reason: %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(15,194,'UNSUSPENDED - %s',31337,0);
-- TODO: Macedonian language
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(16,184,'The NOVOICE flag is set on %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(16,185,'Your access on %s has been suspended.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(16,186,'Your suspension on %s has been cancelled.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(16,187,'You have been added to channel %s with access level %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(16,188,'Your access on %s has been modified from %i to %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(16,189,'Your access from %s has been removed.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(16,190,'Target user %s on channel %s has insufficient access for an automode VOICE',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(16,191,'Target user %s on channel %s has insufficient access for an automode OP',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(16,192,'Your reason must be %i - %i characters long.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(16,193,'Reason: %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(16,194,'UNSUSPENDED - %s',31337,0);
-- TODO: Star Wars language
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(22,184,'The NOVOICE flag is set on %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(22,185,'Your access on %s has been suspended.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(22,186,'Your suspension on %s has been cancelled.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(22,187,'You have been added to channel %s with access level %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(22,188,'Your access on %s has been modified from %i to %i',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(22,189,'Your access from %s has been removed.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(22,190,'Target user %s on channel %s has insufficient access for an automode VOICE',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(22,191,'Target user %s on channel %s has insufficient access for an automode OP',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(22,192,'Your reason must be %i - %i characters long.',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(22,193,'Reason: %s',31337,0);
INSERT INTO translations (language_id,response_id,text,last_updated,deleted)
VALUES(22,194,'UNSUSPENDED - %s',31337,0);

-- Update channel description and URL length to a new value

UPDATE translations SET text = REPLACE(text, '80', '%i') WHERE response_id=106;
UPDATE translations SET text = REPLACE(text, '75', '%i') WHERE response_id=109;

-- Update ban reason length

UPDATE translations SET text = REPLACE(text, '128', '%i') WHERE response_id=21;

