
ALTER TABLE ip_restrict ADD COLUMN value inet, ADD COLUMN last_updated int4 NOT NULL DEFAULT now()::abstime::int4, ADD COLUMN last_used int4, ADD COLUMN expiry int4 NOT NULL DEFAULT 0, ADD COLUMN description VARCHAR(255);

ALTER TABLE ip_restrict ALTER COLUMN added DROP NOT NULL;
ALTER TABLE ip_restrict ALTER COLUMN added SET DEFAULT now()::abstime::int4;
ALTER TABLE ip_restrict ALTER COLUMN allowmask DROP NOT NULL;
ALTER TABLE ip_restrict ALTER COLUMN allowrange1 DROP NOT NULL;
ALTER TABLE ip_restrict ALTER COLUMN allowrange2 DROP NOT NULL;

UPDATE ip_restrict SET value='192.168.0.0/24' WHERE id=1;
UPDATE ip_restrict SET value='172.16.7.1' WHERE id=2;
UPDATE ip_restrict SET value='10.0.0.0/8' WHERE id=3;
-- !ERROR_HOSTMASK! cannot convert HOST mask *.nycap.res.rr.com to new format (id: 4 -- user: Hodari -- added_by: Hodari -- added_date: 2011-12-13 02:49:29+00:00)
UPDATE ip_restrict SET type=0,description='[error] old hostmask: *.nycap.res.rr.com' WHERE id=4;
UPDATE ip_restrict SET value='76.178.195.3' WHERE id=5;
-- !ERROR_DNS! unresolved host: undernet.org (id: 6 -- user: C0llector -- added_by: DinTn -- added_date: 2008-01-09 15:38:06+00:00)
UPDATE ip_restrict SET type=0,description='[error] dns lookup failed: undernet.org' WHERE id=6;


INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (417986, '109.166.129.0/24', now()::abstime::int4, 1058480, 1, now()::abstime::int4);
INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (417986, '109.166.130.0/23', now()::abstime::int4, 1058480, 1, now()::abstime::int4);
INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (417986, '109.166.132.0/22', now()::abstime::int4, 1058480, 1, now()::abstime::int4);
INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (417986, '109.166.136.0/22', now()::abstime::int4, 1058480, 1, now()::abstime::int4);

INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (341, '72.27.17.0/24', now()::abstime::int4, 1058480, 1, now()::abstime::int4);
INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (341, '72.27.18.0/23', now()::abstime::int4, 1058480, 1, now()::abstime::int4);
INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (341, '72.27.20.0/22', now()::abstime::int4, 1058480, 1, now()::abstime::int4);
INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (341, '72.27.24.0/21', now()::abstime::int4, 1058480, 1, now()::abstime::int4);
INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (341, '72.27.32.0/22', now()::abstime::int4, 1058480, 1, now()::abstime::int4);
INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (341, '72.27.36.0/23', now()::abstime::int4, 1058480, 1, now()::abstime::int4);
INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (341, '72.27.38.0/24', now()::abstime::int4, 1058480, 1, now()::abstime::int4);


INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (376545, '81.26.105.1/32', now()::abstime::int4, 323, 1, now()::abstime::int4);
INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (376545, '81.26.105.2/31', now()::abstime::int4, 323, 1, now()::abstime::int4);
INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (376545, '81.26.105.4/30', now()::abstime::int4, 323, 1, now()::abstime::int4);
INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (376545, '81.26.105.8/29', now()::abstime::int4, 323, 1, now()::abstime::int4);
INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (376545, '81.26.105.16/29', now()::abstime::int4, 323, 1, now()::abstime::int4);
INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (376545, '81.26.105.24/30', now()::abstime::int4, 323, 1, now()::abstime::int4);
INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (376545, '81.26.105.28/31', now()::abstime::int4, 323, 1, now()::abstime::int4);
INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (376545, '81.26.105.30/32', now()::abstime::int4, 323, 1, now()::abstime::int4);


INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (2189, '200.223.104.0/22', now()::abstime::int4, 270, 1, now()::abstime::int4);
INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated) VALUES (2189, '200.223.108.0/24', now()::abstime::int4, 270, 1, now()::abstime::int4);


-- ALTER TABLE ip_restrict DROP COLUMN allowmask, DROP COLUMN allowrange1, DROP COLUMN allowrange2;

-- DELETE FROM ip_restrict WHERE value IS null;
-- ALTER TABLE ip_restrict ALTER COLUMN value SET NOT NULL;

-- TOTAL: 7 -- IPmask (2) -- DNS (1) -- IPrange (2 -- expanded to 2)
-- ERRORS: 2 -- IPmask (0) -- hostmask (1) -- DNS (1) -- IPrange (0)


