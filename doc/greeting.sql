-- $Id: greeting.sql,v 1.1 2002/01/26 23:11:05 gte Exp $

DELETE FROM translations where response_id = 9998;
COPY "translations" FROM stdin; 
1	9998	Remember: Nobody from CService will ever ask you for your password, do NOT give out your password to anyone claiming to be CService.	31337
\.
