--
-- You must create a database called 'ipcheck' on your LOCAL pgsql database
-- NOTE: if you dont have a local pgsql with that database filled in it,
--       it will just DONT do the IP_CHECK on your website.
--
--
-- IP_CHECK lock IPs after 3 attempts (failed) in wheter "login" or "forgotten password" sections.
-- to reduce the load on the db host.
--
--
-- you must import this file using : /path/to/pgsql/bin/psql ipcheck < ipcheck.sql
-- after you created the "ipcheck" database.
--
--
-- nighty <nighty@undernet.org> - 09/13/2001

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
   CONSTRAINT ips_pkey PRIMARY KEY (expiration, ipnum, user_name)
);

CREATE  INDEX hit_counts_ips_key ON ips (hit_counts);
CREATE  INDEX ips_expiration_key ON ips (expiration);
CREATE  INDEX ips_ipnum_key ON ips (ipnum);
CREATE  INDEX ips_user_name_key ON ips (user_name);


