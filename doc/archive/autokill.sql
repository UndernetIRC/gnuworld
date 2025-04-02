--
-- $Id: autokill.sql,v 1.17 2008/04/18 11:33:26 danielaustin Exp $
--

--
-- Starting the process...
\qecho [*] Starting autokill.sql..
BEGIN WORK;
--
-- Clean up the temporary table from last time.
DROP TABLE IF EXISTS to_die;
-- Select all user_id's idle > 60 days into a temp table.
--
\qecho [*] Fetching list of idle user accounts:
SELECT users_lastseen.user_id,users_lastseen.last_seen,users.user_name INTO TABLE to_die FROM users_lastseen,users WHERE (users_lastseen.last_seen <= extract(epoch from now() - interval '60 days')::integer) AND users.id=users_lastseen.user_id;
-- Remove any who currently have a pending application (Shouldn't happen <g>) 
\qecho [*] Moving those with old channel applications to user "AutoPurged".
UPDATE pending SET manager_id = (SELECT id FROM users WHERE lower(user_name) = 'AutoPurged') WHERE pending.manager_id IN (select user_id from to_die);
\qecho [*] Not deleting those users who have the NoPurge flag.
DELETE FROM to_die WHERE user_id IN (SELECT id FROM users WHERE (flags::int4 & 32) = 32);
-- Output who are are going to kill, log stdout.
\qecho [*] Listing Final Accounts to be removed:
SELECT users.user_name,to_die.last_seen FROM users,to_die WHERE users.id IN (SELECT user_id);
--
-- Clean up userlogs.
\qecho [*] Removing userlogs..
DELETE FROM userlog WHERE user_id IN (SELECT user_id FROM to_die);
--
-- Clean up level records.
\qecho [*] Removing access levels..
DELETE FROM levels WHERE user_id IN (SELECT user_id FROM to_die);
--
-- Clean up FRAUD USERNAMES.
\qecho [*] Removing FRAUD USERNAME records..
DELETE FROM noreg WHERE type=4 AND user_name IN (SELECT user_name FROM to_die);
--
-- Clean up supporter records.
\qecho [*] Removing supporter records..
DELETE FROM supporters WHERE user_id IN (SELECT user_id FROM to_die);
--
-- Clean up objection records.
\qecho [*] Removing Objection records..
DELETE FROM objections WHERE user_id IN (SELECT user_id FROM to_die);
-- Clean up mailq records.
--\qecho [*] Removing mailq records..
--DELETE FROM mailq WHERE user_id IN (SELECT user_id FROM to_die);
--
-- Clean up notes records.
\qecho [*] Removing notes received and/or sent..
DELETE FROM notes WHERE user_id IN (SELECT user_id FROM to_die) OR from_user_id IN (SELECt user_id FROM to_die);
--
-- Clean up webaccessteam records..
--\qecho [*] Removing webaccessteam records..
--DELETE FROM webaccessteam where admin_id IN (SELECT admin_id FROM to_die);
--
-- Clean up fraud_lists records..
\qecho [*] Removing fraud_list_data..
DELETE FROM fraud_list_data WHERE user_id IN (SELECT user_id FROM to_die);
--
-- Clean up last_seens.
\qecho [*] Removing last_seens..
DELETE FROM users_lastseen WHERE user_id IN (SELECT user_id FROM to_die);
--
-- Clean up users (Finally!).
\qecho [*] Removing user accounts..
DELETE FROM users WHERE user_name IN (SELECT user_name FROM to_die);
--
-- Commit all the work (if this line is not executed, you can "undo" the done stuff using 'ROLLBACK WORK;'
\qecho [*] Comitting changes to database..
COMMIT WORK;

