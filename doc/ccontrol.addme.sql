-- "$Id: ccontrol.addme.sql,v 1.1 2001/03/29 21:33:46 mrbean_ Exp $"
-- 2001-03-29 :  |MrBean|
-- This script will add the first user to ccontrol
-- it will create a user with a coder access by the handle of Admin
-- with pass : temPass
-- the host that will be added is *!*@* , this is a security risk, and should be changed along with the password
-- Note: this script should be run only once , and when the database got no other opers

INSERT INTO OPERS (user_name,password,access,last_updated,flags) VALUES ('Admin','07d976a29bdb3ff42e19c102852151b28097bbad',8191,now()::abstime::int4,8);
INSERT INTO HOSTS (user_id,host) VALUES (1,'*!*@*');
