-- "$Id: chanfix.addme.sql,v 1.3 2006/04/05 02:37:35 buzlip01 Exp $"
-- 2005-08-13 : reed
--
-- This script will add an account for user "Admin" with owner (+o) access.
-- Please modify the default user ("Admin") to your account name.
--
-- This script may be used to create (not modify) multiple owner
-- accounts provided that the account name is changed each time.
--

INSERT INTO users (user_name,created,last_updated,last_updated_by,language_id,flags)
VALUES ('Admin',now()::abstime::int4,now()::abstime::int4,'(added via addme script)',1,16);

INSERT INTO hosts (user_id,host)
VALUES (1,'*!*@*');

-- END HERE
