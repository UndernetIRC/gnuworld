--
-- $Id: autokill.sql,v 1.16 2003/09/14 23:57:24 nighty Exp $
--

--
-- Start an undoable process...
\qecho [*] Starting autokill.sql..
BEGIN WORK;
--
-- Clean up the temporary table from last time.
DROP TABLE to_die;
-- Select all user_id's idle > 60 days into a temp table.
--
\qecho [*] Fetching list of idle user accounts:
SELECT users_lastseen.user_id,users_lastseen.last_seen,users.user_name INTO TABLE to_die FROM users_lastseen,users WHERE (users_lastseen.last_seen <= now()::abstime::int4 - (86400 * 60)) AND users.id=users_lastseen.user_id;
-- Remove any who currently have a pending app (Shouldn't happen <g>)
\qecho [*] Moving those with old channel applications to user "AutoPurged".
UPDATE pending set manager_id = (select id from users where lower(user_name) = 'autopurged') where pending.manager_id = to_die.user_id;
\qecho [*] Not deleting those users who have the NoPurge flag.
DELETE FROM to_die WHERE (users.id = to_die.user_id) AND ((users.flags::int4 & 32) = 32 );
-- Output who are are going to kill, log stdout.
\qecho [*] Listing Final Accounts to be removed:
SELECT users.user_name,to_die.last_seen from users,to_die where users.id = to_die.user_id;
--
-- Clean up userlogs.
\qecho [*] Removing userlogs..
DELETE FROM userlog WHERE user_id = to_die.user_id;
--
-- Clean up level records.
\qecho [*] Removing access levels..
DELETE FROM levels where user_id = to_die.user_id;
--
-- Clean up FRAUD USERNAMES.
\qecho [*] Removing FRAUD USERNAME records..
DELETE FROM noreg WHERE type=4 AND user_name= to_die.user_name;
--
-- Clean up supporter records.
\qecho [*] Removing supporter records..
DELETE FROM supporters where user_id = to_die.user_id;
--
-- Clean up objection records.
\qecho [*] Removing Objection records..
DELETE FROM objections where user_id = to_die.user_id;
-- Clean up mailq records.
\qecho [*] Removing mailq records..
DELETE FROM mailq where user_id = to_die.user_id;
--
-- Clean up notes records.
\qecho [*] Removing notes received and/or sent..
DELETE FROM notes where user_id= to_die.user_id OR from_user_id = to_die.user_id;
--
-- Clean up webaccessteam records..
--\qecho [*] Removing webaccessteam records..
--DELETE FROM webaccessteam where admin_id = to_die.user_id;
--
-- Clean up fraud_lists records..
\qecho [*] Removing fraud_list_data..
DELETE FROM fraud_list_data WHERE user_id = to_die.user_id;
--
-- Clean up last_seens.
\qecho [*] Removing last_seens..
DELETE FROM users_lastseen where user_id = to_die.user_id;
--
-- Clean up users (Finally!).
\qecho [*] Removing user accounts..
DELETE FROM users where id = to_die.user_id;
--
-- Commit all the work (if this line is not executed, you can "undo" the done stuff using 'ROLLBACK WORK;'
\qecho [*] Comitting changes to database..
COMMIT WORK;

