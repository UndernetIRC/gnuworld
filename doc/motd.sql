-- Used to store MOTD text. Use \n for multiple lines.
-- MOTD is just a language response with id 9999.

DELETE FROM translations where response_id = 9999;
COPY "translations" FROM stdin; 
1	9999	Happy Easter from the Undernet Channel Services!\nWibble!\nWhat you say?	31337
\.
