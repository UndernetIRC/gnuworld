DELETE FROM help WHERE command = 'REMSGLINE';
INSERT INTO help (command, subcommand, line, help) VALUES ('REMSGLINE','',1,'****** REMSGLINE COMMAND ******');
INSERT INTO help (command, subcommand, line, help) VALUES ('REMSGLINE','',2,'Removes an SGLINE from the network');
INSERT INTO help (command, subcommand, line, help) VALUES ('REMSGLINE','',3,'Syntax: /msg $BOT$ remsgline [-fr] <user@host>');
INSERT INTO help (command, subcommand, line, help) VALUES ('REMSGLINE','',4,'-fr - Removes all the matching and the specified gline, otherwise just the specified gline.');
INSERT INTO help (command, subcommand, line, help) VALUES ('REMSGLINE','',5,'<user@host> - the user@host to remove from the sgline list');
