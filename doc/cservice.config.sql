-- "$Id: cservice.config.sql,v 1.1 2003/01/01 04:43:51 gte Exp $"
-- Default configuration entries for CMaster bot.
-- Note: Not in use yet ;)

INSERT into variables (var_name,contents,last_updated,hint) 
VALUES ('CMASTER.RELAY_CHAN', '#coder-com_relay',now()::abstime::int4, 'Hint');

INSERT into variables (var_name,contents,last_updated,hint)
VALUES ('CMASTER.DEBUG_CHAN', '#coder-com_debug',now()::abstime::int4, 'Hint');

INSERT into variables (var_name,contents,last_updated,hint)
VALUES ('CMASTER.CODER_CHAN', '#coder-com',now()::abstime::int4, 'Hint');

INSERT into variables (var_name,contents,last_updated,hint)
VALUES ('CMASTER.PENDING_PAGE_URL', 'http://www.yoursite.com/live/view_app.php',now()::abstime::int4, 'Hint');

INSERT into variables (var_name,contents,last_updated,hint)
VALUES ('CMASTER.UPDATE_INTERVAL', '180',now()::abstime::int4, 'Hint');

INSERT into variables (var_name,contents,last_updated,hint)
VALUES ('CMASTER.FLOOD_DURATION', '30',now()::abstime::int4, 'Hint');

INSERT into variables (var_name,contents,last_updated,hint)
VALUES ('CMASTER.INPUT_FLOOD', '30',now()::abstime::int4, 'Hint');

INSERT into variables (var_name,contents,last_updated,hint)
VALUES ('CMASTER.OUTPUT_FLOOD', '20480',now()::abstime::int4, 'Hint');

INSERT into variables (var_name,contents,last_updated,hint)
VALUES ('CMASTER.TOPIC_DURATION', '1800',now()::abstime::int4, 'Hint');

INSERT into variables (var_name,contents,last_updated,hint)
VALUES ('CMASTER.EXPIRE_INTERVAL', '360',now()::abstime::int4, 'Hint');

INSERT into variables (var_name,contents,last_updated,hint)
VALUES ('CMASTER.CACHE_INTERVAL', '360',now()::abstime::int4, 'Hint');

INSERT into variables (var_name,contents,last_updated,hint)
VALUES ('CMASTER.PENDING_DURATION', '1800',now()::abstime::int4, 'Hint');

INSERT into variables (var_name,contents,last_updated,hint)
VALUES ('CMASTER.CONN_CHECK_FREQ', '10',now()::abstime::int4, 'Hint');

INSERT into variables (var_name,contents,last_updated,hint)
VALUES ('CMASTER.CONN_RETRY_TOTAL', '5',now()::abstime::int4, 'Hint');

INSERT into variables (var_name,contents,last_updated,hint)
VALUES ('CMASTER.LIMIT_CHECK', '30',now()::abstime::int4, 'Hint');

INSERT into variables (var_name,contents,last_updated,hint)
VALUES ('CMASTER.LOGIN_DELAY', '5',now()::abstime::int4, 'Hint');

INSERT into variables (var_name,contents,last_updated,hint)
VALUES ('CMASTER.NOTE_DURATION', '3600',now()::abstime::int4, 'Hint');

INSERT into variables (var_name,contents,last_updated,hint)
VALUES ('CMASTER.NOTE_LIMIT', '5',now()::abstime::int4, 'Hint');

INSERT into variables (var_name,contents,last_updated,hint)
VALUES ('CMASTER.PRELOAD_USER_DAYS', '1',now()::abstime::int4, 'Hint');
