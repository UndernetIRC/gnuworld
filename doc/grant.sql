-- Grant access to the username which will access the database.
-- Use this to grant full access to all tables for the username
-- 'gnuworld'.

grant all on languages to gnuworld;
grant all on translations to gnuworld; 
grant all on help to gnuworld;
grant all on channels to gnuworld;
grant all on bans to gnuworld;
grant all on users to gnuworld;
grant all on users_lastseen to gnuworld;
grant all on levels to gnuworld;
grant all on channellog to gnuworld;
grant all on log_users to gnuworld;
grant all on userlog to gnuworld;
grant all on supporters to gnuworld;
grant all on pending to gnuworld;
grant all on domain to gnuworld;
grant all on webcookies to gnuworld;
grant all on lastrequests to gnuworld;
grant all on pendingusers to gnuworld;
grant all on transition to gnuworld;
grant all on pending_traffic to gnuworld;
grant all on noreg to gnuworld;
grant all on regteam to gnuworld;
grant all on webaccessteam to gnuworld;
grant all on abuseteam to gnuworld;
grant all on mailq to gnuworld;
grant all on notes to gnuworld;

-- Sequences.
grant all on bans_id_seq to gnuworld;
grant all on channels_id_seq to gnuworld;
grant all on checkpoints_id_seq to gnuworld;
grant all on domain_id_seq to gnuworld;
grant all on languages_id_seq to gnuworld;
grant all on log_users_id_seq to gnuworld;
grant all on noreg_id_seq to gnuworld;
grant all on users_id_seq to gnuworld;
grant all on notes_message_id_seq to gnuworld;
