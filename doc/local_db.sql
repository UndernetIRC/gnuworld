------------------------------------------------------------------------------------
-- "$Id: local_db.sql,v 1.12 2001/11/24 04:37:43 nighty Exp $"
-- Channel service DB SQL file for PostgreSQL.
--
-- .. if you wonder why some tables have moved and you have them here when
-- you set up your dbs, then you may need to have a look in the script 'movetables'
-- after you modified the parameters in it to fit your configuration, run it to
-- properly move your tables from the 'local' db to the 'remote' db.
-- If you use a single db for both cservice*.sql and local_db.sql then just dont run anything.
--
-- nighty <nighty@undernet.org>

CREATE TABLE webcookies (
        user_id INT4,
        cookie VARCHAR(32) UNIQUE,
        expire INT4
);

CREATE INDEX webcookies_user_id_idx ON webcookies(user_id);


--
-- IP_CHECK lock IPs after 3 attempts (failed) in wheter "login" or "forgotten password" sections.
-- to reduce the load on the db host.
--

CREATE TABLE exclusions (
   excluded varchar(15) DEFAULT '0.0.0.0' NOT NULL,
   CONSTRAINT exclusions_pkey PRIMARY KEY (excluded)
);

CREATE  INDEX exclusions_excluded_key ON exclusions (excluded);


CREATE TABLE ips (
   ipnum varchar(15) DEFAULT '0.0.0.0' NOT NULL,
   user_name varchar(20) NOT NULL,
   expiration int4 NOT NULL,
   hit_counts int4,
   set_on int4 NOT NULL,
   CONSTRAINT ips_pkey PRIMARY KEY (expiration, ipnum, user_name)
);

CREATE  INDEX hit_counts_ips_key ON ips (hit_counts);
CREATE  INDEX ips_expiration_key ON ips (expiration);
CREATE  INDEX ips_set_on_key ON ips (set_on);
CREATE  INDEX ips_ipnum_key ON ips (ipnum);
CREATE  INDEX ips_user_name_key ON ips (user_name);

