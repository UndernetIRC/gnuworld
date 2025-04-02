--
-- $Id: update_suspend_2071228.sql,v 1.1 2007/12/28 02:12:35 kewlio Exp $
--
-- Update help for the SUSPEND command (modified duration)
--

DELETE FROM help WHERE topic='SUSPEND' AND language_id=1;

COPY "help" FROM stdin;
SUSPEND	1	/msg X suspend <#channel> <username> <duration<M|H|D>> [level]\nSuspends a user's access to X on your channel's userlist for the specified period of time, at the specified level if given.\nIf a level is not specified, the default suspension level used will be the access level of the issuer. \nYou can only SUSPEND someone with access lower than your own. The maximum duration is 372 days (8928 hours, 535680 minutes).
\.

