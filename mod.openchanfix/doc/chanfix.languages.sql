-- Use this file to populate the 'languages' table.
--
-- $Id: chanfix.languages.sql,v 1.3 2006/04/05 02:37:35 buzlip01 Exp $

DELETE FROM languages;
COPY "languages" FROM stdin;
1	EN	English	31337	0
\.
