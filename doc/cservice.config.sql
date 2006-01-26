-- "$Id: cservice.config.sql,v 1.7 2006/01/26 22:43:07 kewlio Exp $"
-- Default configuration entries for CMaster bot.

INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('REQUIRED_SUPPORTERS', '10', now()::abstime::int4, 'Required supporters for an application');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('MAX_BANS', '300', now()::abstime::int4, 'Maximum bans per channel');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('BAN_CHECK_ON_BURST', '1', now()::abstime::int4, 'Check bans during burst');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('FLOOD_MESSAGES', '1', now()::abstime::int4, 'Show flood messages');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('FAILED_LOGINS', '50', now()::abstime::int4, 'Number of concurrent failed logins before reporting');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('ALERT_FAILED_LOGINS', '1', now()::abstime::int4, 'Tell users the number of failed logins?');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('FAILED_LOGINS_RATE', '900', now()::abstime::int4, 'Time between alerts about failed logins per user');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('MAX_FAILED_LOGINS', '20', now()::abstime::int4, 'Maximum failed logins per client');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('MAX_BAN_DURATION', '2400', now()::abstime::int4, 'Maximum ban duration (in hours)');

