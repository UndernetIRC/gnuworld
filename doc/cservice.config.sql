-- "$Id: cservice.config.sql,v 1.3 2005/10/03 19:58:05 kewlio Exp $"
-- Default configuration entries for CMaster bot.

INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('MAX_BANS', '300', now()::abstime::int4, 'Hint');
INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('BAN_CHECK_ON_BURST', '1', now()::abstime::int4, 'Hint');

