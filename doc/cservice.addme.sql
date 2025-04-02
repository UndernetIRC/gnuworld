-- "$Id: cservice.addme.sql,v 1.7 2002/08/30 10:18:44 nighty Exp $"
-- 2001-02-23 : |MrBean|
-- 2001-10-14 : nighty
-- 2025-04-02 : Empus: Added IPR advice
--
-- This script will reigster the admin channel * 
-- And will create an admin by the name of Admin  
-- with pass : temPass2020@ And access level 1000
-- Note this script assumes that you didnt add any users/channels to the db
--
--
-- YOU MUST RUN THIS SCRIPT ONLY ONCE AND MODIFYING ONLY 'Admin' BY
-- YOUR PREFERRED NICKNAME.
--
-- This action will also register #coder-com to you, its still needed for 'VERIFY'
-- to work on authenticated users.

INSERT into channels (name,flags,channel_ts,registered_ts,last_updated) 
VALUES ('*',1,date_part('epoch', CURRENT_TIMESTAMP)::int,31337,date_part('epoch', CURRENT_TIMESTAMP)::int);

INSERT into channels (name,flags,channel_ts,registered_ts,last_updated)
VALUES ('#coder-com',1,date_part('epoch', CURRENT_TIMESTAMP)::int,31337,date_part('epoch', CURRENT_TIMESTAMP)::int);

-- Change 'Admin' to your username if you prefer
INSERT into users (user_name,password,language_id,last_updated) 
VALUES ('Admin','xEDi1V791f7bddc526de7e3b0602d0b2993ce21d',1,date_part('epoch', CURRENT_TIMESTAMP)::int);

--INSERT into users_lastseen (user_id,last_seen,last_updated) 
--VALUES (1,date_part('epoch', CURRENT_TIMESTAMP)::int,date_part('epoch', CURRENT_TIMESTAMP)::int);

INSERT into levels (channel_id,user_id,access,last_updated)
VALUES (1,1,1000,date_part('epoch', CURRENT_TIMESTAMP)::int);

INSERT into levels (channel_id,user_id,access,last_updated)
VALUES (2,1,500,date_part('epoch', CURRENT_TIMESTAMP)::int);

-- IP Restrictions
-- If you have IPR (IP Restrictions) enabled and are unable to login with *
--access, you will need to insert your IP entries into ip_restrict

-- Example entry for user_id 1 and IP address 10.0.0.1:

-- INSERT INTO ip_restrict (id, user_id, added_by, added, type, expiry, value)
-- VALUES (1, 1, 1, EXTRACT(EPOCH FROM NOW())::int, 1, 0, '10.0.0.1');
