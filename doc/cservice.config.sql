-- "$Id: cservice.config.sql,v 1.2 2005/09/30 00:48:43 kewlio Exp $"
-- Default configuration entries for CMaster bot.

INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('MAX_BANS', '300', now()::abstime::int4, 'Hint');

