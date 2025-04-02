--
-- $Id: update_purge_20071227.sql,v 1.1 2007/12/28 01:43:14 kewlio Exp $
--
-- Update help for the PURGE command (shows new flag)
--

DELETE FROM help WHERE topic='PURGE' AND language_id=1;

COPY "help" FROM stdin;
PURGE	1	/msg X purge <#channel> [-noop] <reason>\nPurges a channel. This command is for CService Admins only.\n-noop flag causes no reops during purge
\.

