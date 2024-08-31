--
-- This file reflects the latest changes in mod.cservice to introduce JOINLIM
-- so, you NEED to update your cservice database using that file if you
-- upgraded mode.cservice to a version after this date or you are using an older db schema.
--

ALTER TABLE channels ADD COLUMN limit_joinmax INT4 DEFAULT '3';
ALTER TABLE channels ADD COLUMN limit_joinsecs INT4 DEFAULT '1';
ALTER TABLE channels ADD COLUMN limit_joinperiod INT4 DEFAULT '180';
ALTER TABLE channels ADD COLUMN limit_joinmode VARCHAR(255) DEFAULT '+rb *!~*@*';

UPDATE channels SET limit_joinmax = 3;
UPDATE channels SET limit_joinsecs = 1;
UPDATE channels SET limit_joinperiod = 180;
UPDATE channels SET limit_joinmode = '+rb *!~*@*';

UPDATE help SET contents = E'/msg X set <#channel> <variable> <value>\n/msg X set <variable> <value>\nThis will set a channel or user account setting. \nUseraccount settings: INVISIBLE MAXLOGINS LANG\nChannel settings: AUTOJOIN AUTOTOPIC DESCRIPTION FLOATLIM FLOODPRO JOINLIM KEYWORDS MASSDEOPPRO MODE NOOP OPLOG STRICTOP URL USERFLAGS\nFor more help type /msg X help SET <variable>' WHERE topic = 'SET' and language_id = 1;

INSERT INTO help VALUES ('SET JOINLIM', 1, E'/msg X set <#channel> JOINLIM ON|OFF\nMakes X set a mode (see SET JOINMODE) on the channel if a defined number of unidented and unauthenticated clients join the channel during a defined period (see SET JOINMAX).\nThe JOINMODE will automatically be unset by X after a defined period (see SET JOINPERIOD). The JOINPERIOD timer will be reset if additional unidented and unauthenticated clients join the channel during this period.');
INSERT INTO help VALUES ('SET JOINMAX', 1, E'/msg X set <#channel> JOINMAX <joins:secs>\nDefines the threshold for when JOINLIM (if activated) shall be triggered.\nThe threshold is defined by <JOINS> unidented and unauthenticated clients joining the channel within <SECS> seconds (JOINS:SECS).');
INSERT INTO help VALUES ('SET JOINMODE', 1, E'/msg X set <#channel> JOINMODE <modes> [parameters]\nThe mode to be set by X in your channel if JOINLIM is triggered.\nThe allowed JOINMODE modes are: +DbcCikmrs\nFor channel mode l(imit), k(ey) or b(an), the limit, key or banmask must be provided.');
INSERT INTO help VALUES ('SET JOINPERIOD', 1, E'/msg X set <#channel> JOINPERIOD <1-600>\nThe period after JOINLIM has been triggered (and JOINMODE set) that X will remove the set JOINMODE.\nThe duration can be between 1-600 seconds. If uidented and unauthenticated clients join while the JOINMODE is set, the JOINPERIOD timer will be reset.');

INSERT INTO translations (text, response_id, language_id, last_updated) VALUES ('Value of joins has to be %i-%i',210,1,31337);
INSERT INTO translations (text, response_id, language_id, last_updated) VALUES ('Value of seconds has to be %i-%i',211,1,31337);
INSERT INTO translations (text, response_id, language_id, last_updated) VALUES ('JOINMAX for %s is now %i:%i (JOINS:SECS)',212,1,31337);
INSERT INTO translations (text, response_id, language_id, last_updated) VALUES ('JOINMODE for %s is now: %s',213,1,31337);
INSERT INTO translations (text, response_id, language_id, last_updated) VALUES ('The allowed JOINMODE modes are: %s',214,1,31337);
INSERT INTO translations (text, response_id, language_id, last_updated) VALUES ('Invalid number of JOINMODE parameters: %i modes and %i parameters provided',215,1,31337);
INSERT INTO translations (text, response_id, language_id, last_updated) VALUES ('The banmask ''%s'' is not valid',216,1,31337);
INSERT INTO translations (text, response_id, language_id, last_updated) VALUES ('Value of JOINPERIOD has to be 1-%i',217,1,31337);
INSERT INTO translations (text, response_id, language_id, last_updated) VALUES ('JOINPERIOD for %s is now: %i seconds',218,1,31337);
INSERT INTO translations (text, response_id, language_id, last_updated) VALUES ('Active JOINMODE modes (+%s) will expire in %i seconds',219,1,31337);
