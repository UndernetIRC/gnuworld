--
-- $Id: update_config_20080101.sql,v 1.1 2008/01/01 13:49:47 kewlio Exp $
--
-- Set the variable for using the login grace period when cservice links.
--

INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('USE_LOGIN_DELAY', '1', now()::abstime::int4, 'Use login delay when cservice links if set to 1');

