-- Use this file to populate the 'languages' table.
--
-- $Id: chanfix.languages.sql,v 1.1 2006/03/20 02:07:43 buzlip01 Exp $

DELETE FROM languages;
COPY "languages" FROM stdin;
1	EN	English	31337	0
\.
