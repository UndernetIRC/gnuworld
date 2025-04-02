--
-- MODE, KICK and (UN)BAN specific help topics
-- last updated by MrIron @ 2024.07.01
--

UPDATE help SET contents = E'/msg X kick <#channel> <nicks | *!*@*.host> [reason]\nMakes X kick one or more persons from your channel.\nYou can make X kick several nicknames separates by a comma. If your access level is 200 or higher you can also kick by pattern.' WHERE topic = 'KICK' and language_id = 1;
UPDATE help SET contents = E'/msg X ban <#channel> <nicks | *!*user@*.host> [duration] [level] [reason]\nAdds a specific *!*user@*.host to the X banlist of your channel.\nYou may place a ban on one or several nicks separated by comma if the persons are online or ban their *!*user@*.host if the persons are not online.\nBans that cover a broader range than previously set bans will replace previous bans. \nAny user in the channel that matches the added ban will be kicked out if the ban level is 75+. \nThe duration can be specified in the following format: 400s, 300m, 200h, 100d (secs,mins,hours,days)\nSpecifying a duration of 0 will set a permanent ban (no expiry) \nThe ban level can range from 1 to your own level. The ban reason can be a maximum of 128 characters long. \nIf no duration or level is specified, the default duration will be 3 hours, and the level will be 75.\nBan Levels: 1-74 = Prevents the user from having ops (+o) on the channel. \nBan Levels: 75-500 = Prevents the user from being in the channel at all.'  WHERE topic = 'BAN' and language_id = 1;
UPDATE help SET contents = E'/msg X unban <#channel> <nicks | *!*user@*.host>\nRemoves the ban on one or more nicks or *!*user@*.hosts separated by comma from the X banlist of your channel and any matching bans from the channel banlist also. \nYou can only remove bans from X''s banlist that are equal to or lower than your own access. \nTo search X''s banlist, refer to the LBANLIST command (level 0).\nIf more than one ban matches the mask you are trying to remove, and one is an exact match, will only remove the exact match. \nResend the command to remove the rest.'  WHERE topic = 'UNBAN' and language_id = 1;

UPDATE translations SET text = 'The ban %s is already in my banlist!' WHERE language_id=1 AND response_id=22;
UPDATE translations SET text = 'Le ban %s est déjà dans ma liste!' WHERE language_id=2 AND response_id=22;
UPDATE translations SET text = 'Ban %s er allerede i min banliste!' WHERE language_id=3 AND response_id=22;
UPDATE translations SET text = '%s staat reeds in mijn banlijst!' WHERE language_id=4 AND response_id=22;
UPDATE translations SET text = 'Ban %s ist bereits in der Banliste!' WHERE language_id=5 AND response_id=22;
UPDATE translations SET text = 'O ban %s já está na banlist!' WHERE language_id=6 AND response_id=22;
UPDATE translations SET text = 'Il ban %s è già presente nella mia lista dei ban!' WHERE language_id=7 AND response_id=22;
UPDATE translations SET text = '%s e deja pe ban lista!' WHERE language_id=8 AND response_id=22;
UPDATE translations SET text = 'El bandeig %s ja és a la meva llista!' WHERE language_id=9 AND response_id=22;
UPDATE translations SET text = 'El ban %s ya está en mi lista de bans!' WHERE language_id=10 AND response_id=22;
UPDATE translations SET text = '%s mar benne van a banlistaban!' WHERE language_id=11 AND response_id=22;
UPDATE translations SET text = '%s zaten benim yasaklýlar listemde!' WHERE language_id=12 AND response_id=22;
UPDATE translations SET text = '%s er allerede i min banliste!' WHERE language_id=13 AND response_id=22;
UPDATE translations SET text = '%s muharram min qabl fi qaimat al-hazr!' WHERE language_id=14 AND response_id=22;
UPDATE translations SET text = 'אין כניסה זו %s כבר ברשימה שלי!' WHERE language_id=15 AND response_id=22;
UPDATE translations SET text = '%s e veke na mojata ban lista!' WHERE language_id=16 AND response_id=22;
UPDATE translations SET text = 'The pitchfork %s is already in my pitchfork-list!' WHERE language_id=18 AND response_id=22;
UPDATE translations SET text = 'Ban %s einai eidi stin lista twn bans mou!' WHERE language_id=19 AND response_id=22;
UPDATE translations SET text = 'Banen %s finns redan i min banlista!' WHERE language_id=20 AND response_id=22;
UPDATE translations SET text = 'The curse %s is already in my curse-list!' WHERE language_id=21 AND response_id=22;
UPDATE translations SET text = 'The destructor %s is already in my war path' WHERE language_id=22 AND response_id=22;
UPDATE translations SET text = 'The villager with host %s is already lynched!' WHERE language_id=23 AND response_id=22;
UPDATE translations SET text = 'The citizen with host %s is already imprisoned!' WHERE language_id=24 AND response_id=22;

UPDATE translations SET text = 'Removed %i channel ban(s) and %i internal ban(s) that matched %s' WHERE language_id=1 AND response_id=143;
UPDATE translations SET text = '%i canal ban(s) et %i interne ban(s) correpondant à %s ont été supprimés.' WHERE language_id=2 AND response_id=143;
UPDATE translations SET text = 'Fjernet %i kanalbans og %i interne bans der matcher %s' WHERE language_id=3 AND response_id=143;
UPDATE translations SET text = '%i kanaal ban(s) en %i interne ban(s) verwijderd die overeenkwam(en) met %s' WHERE language_id=4 AND response_id=143;
UPDATE translations SET text = 'Habe %i Kanal Bans und %i Intern Bans entfernt, die auf %s zutrafen' WHERE language_id=5 AND response_id=143;
UPDATE translations SET text = 'Removido %i canal ban(s) e %i interno ban(s) combinado %s' WHERE language_id=6 AND response_id=143;
UPDATE translations SET text = 'Sono stati rimossi %i canal ban e %i interno ban che corrispondono a %s' WHERE language_id=7 AND response_id=143;
UPDATE translations SET text = 'Am scos %i canal banuri si %i intern banuri care s-au potrivit cu %s' WHERE language_id=8 AND response_id=143;
UPDATE translations SET text = 'Suprimits %i canal bandeigs i %i intern bandeigs coincidents amb %s' WHERE language_id=9 AND response_id=143;
UPDATE translations SET text = 'Suprimidos %i canal bans y %i interno bans coincidentes con %s' WHERE language_id=10 AND response_id=143;
UPDATE translations SET text = 'Leszedtem %i olyan csatorna bant és %i olyan belső bant, ami talalt %s -el.' WHERE language_id=11 AND response_id=143;
UPDATE translations SET text = '%i kanal yasağı ve %i dahili yasağı %s ile eşleştiği için kaldırıldı.' WHERE language_id=12 AND response_id=143;
UPDATE translations SET text = 'Slettet %i kanalbans og %i interne bans som stemmer med %s' WHERE language_id=13 AND response_id=143;
UPDATE translations SET text = 'Azalat %i hazr kanat wa %i hazr dakheli mutabiq li %s' WHERE language_id=14 AND response_id=143;
UPDATE translations SET text = 'הוסר ערוץ %i ללא כניסות ו-%i פנימי ללא כניסה (ים) שתואמים ל-%s' WHERE language_id=15 AND response_id=143;
UPDATE translations SET text = 'Trgnati %i banovi na kanalot i %i vnatresni banovi shto go pokrivaat %s' WHERE language_id=16 AND response_id=143;
UPDATE translations SET text = 'Removed %i channel pitchforks and %i internal pitchforks that matched %s' WHERE language_id=18 AND response_id=143;
UPDATE translations SET text = 'Aferethikan %i kanáli bans kai %i esoterikós bans pou teriazan sto %s' WHERE language_id=19 AND response_id=143;
UPDATE translations SET text = 'Tog bort %i kanal banar och %i interne banar som matchade %s' WHERE language_id=20 AND response_id=143;
UPDATE translations SET text = 'Removed %i channel curses and %i internal curses that matched %s' WHERE language_id=21 AND response_id=143;
UPDATE translations SET text = 'Removed %i channel destructors and %i internal destructors that matched %s' WHERE language_id=22 AND response_id=143;
UPDATE translations SET text = 'Removed %i channel lynches and %i internal lynches that matched %s' WHERE language_id=23 AND response_id=143;
UPDATE translations SET text = 'Released %i channel detainees and %i internal detainees that matched %s' WHERE language_id=24 AND response_id=143;

INSERT INTO help VALUES ('MODE', '1', E'/msg X mode <#channel> <modes> [parameters]\nMakes X set or remove a channel mode in your channel.\nThe mode must start with either + or - to specify whether a mode shall be set or removed.\nFor channel mode l(imit) or k(ey), the limit or key must be provided.\nUse the OP, DEOP, VOICE and DEVOICE commands to give or remove ops or voice.');

INSERT INTO translations (text, response_id, language_id, last_updated) VALUES ('Please use the OP, DEOP, VOICE and DEVOICE commands to give or remove ops or voice.', 220, 1, 31337);
INSERT INTO translations (text, response_id, language_id, last_updated) VALUES ('Mode for %s is now: %s', 221, 1, 31337);
INSERT INTO translations (text, response_id, language_id, last_updated) VALUES ('Ban-modes set for: %s', 222, 1, 31337);
INSERT INTO translations (text, response_id, language_id, last_updated) VALUES ('You need to provide the existing key in order to unset the key.', 223, 1, 31337);
INSERT INTO translations (text, response_id, language_id, last_updated) VALUES ('The channel key cannot exceed %i characters.', 224, 1, 31337);