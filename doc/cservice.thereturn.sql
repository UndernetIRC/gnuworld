--
-- Run this file if mod.cservice has integration with X/W (2002 edition)
--

CREATE TABLE channels_w (
    channel_id SERIAL PRIMARY KEY,
    status INT CHECK (status IN (0, 1)),
    timestamp BIGINT
);

INSERT INTO help VALUES ('W', 1, E'/msg X W <join | part | purge> <#channel>\nW is the Undernet Channel Service Bot decommissioned in 2002 when it was replaced by the current X.\nChannel registration is not available in W, but W is made available to channels registered with X. Channel managers can request W to join their channel with ''/msg X W JOIN <#channel>'', and request W to permanently leave the channel with ''/msg X W PURGE <#channel>''.\nFor channels already registered with W, the JOIN command will request W to join the channel, and the PART command will request W to part the channel.\n\nW does not recognise usernames, but hostmasks, and persons with access to channels will be required to login to all channels using /msg W@channels2.undernet.org login <#channel> <password>.\nFor security purposes, only *!*@ACCOUNT.users.undernet.org will be permitted hostmasks in W''s access list, irrespective of access level.\nW will recognise persons in its userlist, and enable them to login to the channel, when such persons are authenticated to X even if not having usermode +x (hidden host).');
INSERT INTO help VALUES ('W JOIN', 1, E'/msg X W join <#channel>\nFor channels not already registered with W, this command is available for managers of channels registered with X, to also register the channel with W.\nYou will be added with the hostmask *!*@YOURACCOUNT.users.undernet.org. W will allow you to login with W provided that you are authenticated with X, but irrespective of whether you have usermode +x.\n\nFor channels already registered with W, the JOIN command will request W to re-join the channel.\nThis command is available for users with access 450+ in X. The same can be achieved by using ''/msg W join <#channel>''');
INSERT INTO help VALUES ('W PURGE', 1, E'/msg X W purge <#channel>\nThis command purges (unregisters) a channel with W.\nThe channel can be re-registered, but the userlist will be cleared.');
INSERT INTO help VALUES ('W PART', 1, E'/msg X W part <#channel>\nThis command requests W to part a channel.\nW can be requested to re-join by using ''/msg X W join <#channel>'' for persons with access 450+ in X, or ''/msg W join <#channel>'' for persons with access 450+ in W.');

INSERT INTO variables (var_name,contents,last_updated,hint)
VALUES ('THERETURN_INTEGRATION', '0', date_part('epoch', CURRENT_TIMESTAMP)::int, 'Enable integration of the return of W');