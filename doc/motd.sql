-- $Id: motd.sql,v 1.3 2002/01/13 07:44:44 nighty Exp $
-- Used to store MOTD text. Use \n for multiple lines.
-- MOTD is just a language response with id 9999.

DELETE FROM translations where response_id = 9999;
COPY "translations" FROM stdin; 
1	9999	\026Undernet Channel Services MOTD\026\n\nService Announcements:\nNone at present.	31337
\.
