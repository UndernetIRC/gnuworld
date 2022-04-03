-- "$Id: cservice.config.sql,v 1.11 2008/01/01 13:49:19 kewlio Exp $"
-- Default configuration entries for CMaster bot.

INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('REQUIRED_SUPPORTERS', '5', date_part('epoch', CURRENT_TIMESTAMP)::int, 'Required supporters for an application');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('MAX_BAN_REASON_LENGTH', '300', date_part('epoch', CURRENT_TIMESTAMP)::int, 'Maximum length for ban reason.');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('MAX_BANS', '300', date_part('epoch', CURRENT_TIMESTAMP)::int, 'Maximum bans per channel');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('BAN_CHECK_ON_BURST', '1', date_part('epoch', CURRENT_TIMESTAMP)::int, 'Check bans during burst');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('FLOOD_MESSAGES', '1', date_part('epoch', CURRENT_TIMESTAMP)::int, 'Show flood messages');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('FAILED_LOGINS', '50', date_part('epoch', CURRENT_TIMESTAMP)::int, 'Number of concurrent failed logins before reporting');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('ALERT_FAILED_LOGINS', '1', date_part('epoch', CURRENT_TIMESTAMP)::int, 'Tell users the number of failed logins?');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('FAILED_LOGINS_RATE', '900', date_part('epoch', CURRENT_TIMESTAMP)::int, 'Time between alerts about failed logins per user');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('MAX_FAILED_LOGINS', '20', date_part('epoch', CURRENT_TIMESTAMP)::int, 'Maximum failed logins per client');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('MAX_BAN_DURATION', '31536000', date_part('epoch', CURRENT_TIMESTAMP)::int, 'Maximum ban duration (in seconds)');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('LOGINS_FROM_SAME_IP', '0', date_part('epoch', CURRENT_TIMESTAMP)::int, 'Maximum logins from same IP when maxlogins=1');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('LOGINS_FROM_SAME_IP_AND_IDENT', '0', date_part('epoch', CURRENT_TIMESTAMP)::int, 'Force check of ident as well as IP when maxlogins=1');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('USE_LOGIN_DELAY', '1', date_part('epoch', CURRENT_TIMESTAMP)::int, 'Use login delay when cservice links if set to 1');

