-- "$Id: cservice.addme.sql,v 1.3 2001/02/23 20:19:43 mrbean_ Exp $"
-- 2001-02-23 : |MrBean|
-- This script will reigster the admin channel * 
-- And will create an admin by the name of Admin  
-- with pass : temPass And access level 1000
-- Note this script assumes that you didnt add any users/channels to the db

INSERT into channels (name,flags,channel_ts,registered_ts,last_updated) 
VALUES ('*',1,now()::abstime::int4,31337,now()::abstime::int4);

INSERT into users (user_name,password,language_id,last_updated) 
VALUES ('Admin','07d976a29bdb3ff42e19c102852151b28097bbad',1,now()::abstime::int4);

INSERT into users_lastseen (user_id,last_seen,last_updated) 
VALUES (1,now()::abstime::int4,now()::abstime::int4);

INSERT into levels (channel_id,user_id,access,last_updated)
VALUES (1,1,1000,now()::abstime::int4);

-- END HERE

