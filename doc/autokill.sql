--
-- Clean up the temporary table from last time.
DROP TABLE to_die;
-- Select all user_id's idle > 90 days into a temp table.
--
\qecho [*] Fetching list of idle user accounts:
SELECT user_id,last_seen INTO TABLE to_die FROM users_lastseen WHERE (last_seen <= now()::abstime::int4 - (86400 * 90));
-- Remove any who currently have a pending app (Shouldn't happen <g>)
\qecho [*] Moving those with old channel applications to user "AutoPurged".
UPDATE pending set manager_id = 279907 where pending.manager_id = to_die.user_id;
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
-- Clean up supporter records.
\qecho [*] Removing supporter records..
DELETE FROM supporters where user_id = to_die.user_id;
--
-- Clean up objection records.
\qecho [*] Removing Objection records..
DELETE FROM objections where user_id = to_die.user_id;
-- Clean up mailq records.
--\qecho [*] Removing mailq records..
--DELETE FROM mailq where user_id = to_die.user_id;
--
-- Clean up webaccessteam records..
\qecho [*] Removing webaccessteam records..
DELETE FROM webaccessteam where admin_id = to_die.user_id;
--
-- Clean up last_seens.
\qecho [*] Removing last_seens..
DELETE FROM users_lastseen where user_id = to_die.user_id;
--
-- Clean up users (Finally!).
\qecho [*] Removing user accounts..
DELETE FROM users where id = to_die.user_id;
