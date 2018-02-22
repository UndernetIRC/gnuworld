------------------------------------------------------------------------------------
-- Channel service DB SQL file for PostgreSQL.
--
-- .. if you wonder why some tables have moved and you have them here when
-- you set up your dbs, then you may need to have a look in the script 'movetables'
-- after you modified the parameters in it to fit your configuration, run it to
-- properly move your tables from the 'local' db to the 'remote' db.
-- If you use a single db for both cservice*.sql and local_db.sql then just dont run anything.
--
-- nighty <nighty@undernet.org>

CREATE TABLE custom_cookies (
    user_id integer,
    session_time integer
);

CREATE TABLE exclusions (
    excluded character varying(40) DEFAULT '0.0.0.0'::character varying NOT NULL
);

CREATE TABLE gfxcodes (
    code character varying(25) NOT NULL,
    crc character varying(128) NOT NULL,
    expire integer NOT NULL
);

CREATE TABLE ip_restrict (
    id integer NOT NULL,
    user_id integer NOT NULL,
    allowmask character varying(255) NOT NULL,
    allowrange1 integer NOT NULL,
    allowrange2 integer NOT NULL,
    added integer NOT NULL,
    added_by integer NOT NULL,
    type integer NOT NULL
);

CREATE SEQUENCE ip_restrict_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


CREATE TABLE ips (
    ipnum character varying(255) DEFAULT '0.0.0.0'::character varying NOT NULL,
    user_name character varying(20) NOT NULL,
    expiration integer NOT NULL,
    hit_counts integer,
    set_on integer NOT NULL
);

CREATE TABLE logmsg (
    ts integer,
    name character varying(128),
    event integer,
    message text
);

CREATE TABLE newu_ipcheck (
    ts integer NOT NULL,
    ip inet NOT NULL,
    expiration integer NOT NULL
);

CREATE SEQUENCE nickserv_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

CREATE TABLE themes (
    id integer DEFAULT nextval(('"themes_id_seq"'::text)::regclass) NOT NULL,
    name character varying(50) NOT NULL,
    tstart character varying(5) DEFAULT '01/01'::character varying NOT NULL,
    tend character varying(5) DEFAULT '12/31'::character varying NOT NULL,
    created_ts integer NOT NULL,
    created_by integer DEFAULT 0 NOT NULL,
    sub_dir character varying(128) NOT NULL,
    left_bgcolor character varying(6) NOT NULL,
    left_bgimage character varying(255) DEFAULT ''::character varying NOT NULL,
    left_textcolor character varying(6) NOT NULL,
    left_linkcolor character varying(6) NOT NULL,
    left_linkover character varying(6) NOT NULL,
    left_loadavg0 character varying(6) NOT NULL,
    left_loadavg1 character varying(6) NOT NULL,
    left_loadavg2 character varying(6) NOT NULL,
    top_bgcolor character varying(6) NOT NULL,
    top_bgimage character varying(255) DEFAULT ''::character varying NOT NULL,
    top_logo character varying(255) NOT NULL,
    bottom_bgcolor character varying(6) NOT NULL,
    bottom_bgimage character varying(255) DEFAULT ''::character varying NOT NULL,
    bottom_textcolor character varying(6) NOT NULL,
    bottom_linkcolor character varying(6) NOT NULL,
    bottom_linkover character varying(6) NOT NULL,
    main_bgcolor character varying(6) NOT NULL,
    main_bgimage character varying(255) DEFAULT ''::character varying NOT NULL,
    main_textcolor character varying(6) NOT NULL,
    main_textlight character varying(6) NOT NULL,
    main_linkcolor character varying(6) NOT NULL,
    main_linkover character varying(6) NOT NULL,
    main_warnmsg character varying(6) NOT NULL,
    main_no character varying(6) NOT NULL,
    main_yes character varying(6) NOT NULL,
    main_appst0 character varying(6) NOT NULL,
    main_appst1 character varying(6) NOT NULL,
    main_appst2 character varying(6) NOT NULL,
    main_appst3 character varying(6) NOT NULL,
    main_appst4 character varying(6) NOT NULL,
    main_appst8 character varying(6) NOT NULL,
    main_appst9 character varying(6) NOT NULL,
    main_vlinkcolor character varying(6) NOT NULL,
    main_support character varying(6) NOT NULL,
    main_nonsupport character varying(6) NOT NULL,
    main_notyet character varying(6) NOT NULL,
    main_frauduser character varying(6) NOT NULL,
    main_xat_revert character varying(6) NOT NULL,
    main_xat_goperm character varying(6) NOT NULL,
    main_xat_deny character varying(6) NOT NULL,
    main_xat_accept character varying(6) NOT NULL,
    main_acl_create character varying(6) NOT NULL,
    main_acl_edit character varying(6) NOT NULL,
    table_bgcolor character varying(6) NOT NULL,
    table_bgimage character varying(255) DEFAULT ''::character varying NOT NULL,
    table_headcolor character varying(6) NOT NULL,
    table_headtextcolor character varying(6) NOT NULL,
    table_sepcolor character varying(6) NOT NULL,
    table_septextcolor character varying(6) NOT NULL,
    table_tr_enlighten character varying(6) NOT NULL,
    table_tr_enlighten2 character varying(6) NOT NULL,
    table_tr_enlighten3 character varying(6) NOT NULL,
    table_headimage character varying(255) DEFAULT ''::character varying NOT NULL
);


CREATE SEQUENCE themes_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

CREATE TABLE totp_ips (
    ipnum character varying(255),
    user_name character varying(20),
    expiration integer,
    hit_counts integer,
    set_on integer
);

CREATE TABLE types (
    id integer DEFAULT nextval(('types_id_seq'::text)::regclass) NOT NULL,
    label character varying(128) NOT NULL
);

CREATE SEQUENCE types_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    MAXVALUE 2147483647
    CACHE 1;

CREATE TABLE userlogmsg (
    ts integer,
    name character varying(128),
    event integer,
    message text,
    last_updated integer
);

CREATE TABLE users (
    id integer DEFAULT nextval(('users_id_seq'::text)::regclass) NOT NULL,
    username character varying(20) NOT NULL,
    real_name character varying(128) DEFAULT ''::character varying NOT NULL,
    url character varying(255) DEFAULT ''::character varying NOT NULL,
    type integer DEFAULT 7 NOT NULL,
    picture character varying(50) DEFAULT 'undernet.jpg'::character varying NOT NULL,
    uniqid character varying(50) NOT NULL,
    location character varying(128),
    is_alumni integer
);

CREATE SEQUENCE users_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    MAXVALUE 2147483647
    CACHE 1;

CREATE TABLE webcookies (
    user_id integer,
    cookie character varying(32),
    expire integer,
    tz_setting character varying(255) DEFAULT ''::character varying,
    is_admin smallint DEFAULT 0,
    totp_cookie character varying(40)
);


ALTER TABLE ONLY ip_restrict ALTER COLUMN id SET DEFAULT nextval('ip_restrict_id_seq'::regclass);
ALTER TABLE ONLY exclusions
    ADD CONSTRAINT exclusions_pkey PRIMARY KEY (excluded);
ALTER TABLE ONLY ips
    ADD CONSTRAINT ips_pkey PRIMARY KEY (expiration, ipnum, user_name);
ALTER TABLE ONLY newu_ipcheck
    ADD CONSTRAINT newu_ipcheck_pkeys PRIMARY KEY (ip);
ALTER TABLE ONLY custom_cookies
    ADD CONSTRAINT one_user UNIQUE (user_id);
ALTER TABLE ONLY themes
    ADD CONSTRAINT themes_pkey PRIMARY KEY (name);
ALTER TABLE ONLY types
    ADD CONSTRAINT types_pkey PRIMARY KEY (id);
ALTER TABLE ONLY users
    ADD CONSTRAINT users_pkey PRIMARY KEY (id);

CREATE INDEX exclusions_excluded_key ON exclusions USING btree (excluded);
CREATE INDEX gfxcodes_code_idx ON gfxcodes USING btree (code);
CREATE INDEX gfxcodes_crc_idx ON gfxcodes USING btree (crc);
CREATE INDEX gfxcodes_exp_idx ON gfxcodes USING btree (expire);
CREATE INDEX hit_counts_ips_key ON ips USING btree (hit_counts);
CREATE INDEX ip_restrict_idx ON ip_restrict USING btree (user_id, type);
CREATE INDEX ip_restrict_uidx ON ip_restrict USING btree (user_id);
CREATE INDEX ips_expiration_key ON ips USING btree (expiration);
CREATE INDEX ips_ipnum_key ON ips USING btree (ipnum);
CREATE INDEX ips_set_on_key ON ips USING btree (set_on);
CREATE INDEX ips_user_name_key ON ips USING btree (user_name);
CREATE INDEX logmsg_idx_name ON logmsg USING btree (name);
CREATE INDEX logmsg_idx_namevt ON logmsg USING btree (name, event);
CREATE INDEX newu_ipcheck_expiration ON newu_ipcheck USING btree (expiration);
CREATE INDEX newu_ipcheck_ip ON newu_ipcheck USING btree (ip);
CREATE INDEX newu_ipcheck_ts ON newu_ipcheck USING btree (ts);
CREATE UNIQUE INDEX themes_id_key ON themes USING btree (id);
CREATE INDEX types_id_key ON types USING btree (id);
CREATE INDEX ulogmsg_idx_name ON userlogmsg USING btree (name);
CREATE INDEX ulogmsg_idx_namevt ON userlogmsg USING btree (name, event);
CREATE INDEX users_id_key ON users USING btree (id);
CREATE INDEX users_type_key ON users USING btree (type);
CREATE INDEX webcook_ce_idx ON webcookies USING btree (cookie, expire);
CREATE INDEX webcook_cu_idx ON webcookies USING btree (cookie, user_id);
CREATE INDEX webcook_ia_idx ON webcookies USING btree (is_admin);
CREATE INDEX webcookies_cookie_idx ON webcookies USING btree (cookie);
CREATE UNIQUE INDEX webcookies_cookie_key ON webcookies USING btree (cookie);
CREATE INDEX webcookies_expire_idx ON webcookies USING btree (expire);
CREATE INDEX webcookies_user_id_idx ON webcookies USING btree (user_id);

ALTER TABLE ONLY users
    ADD CONSTRAINT users_type_fkey FOREIGN KEY (type) REFERENCES types(id);
ALTER TABLE ONLY users
    ADD CONSTRAINT users_type_fkey1 FOREIGN KEY (type) REFERENCES types(id);

INSERT INTO themes VALUES (
-- head
'1',
'default',
'01/01',
'12/31',
'31337',
'0',

'default',

-- left
'60659c',
'',
'000000',
'aaaaaa',
'ffffff',

'aaaaaa',
'000000',
'ffff00',

-- top
'60659c',
'',
'default_logo.jpg',

-- bottom (footer)
'60659c',
'',
'000000',
'aaaaaa',
'ffffff',

-- main
'aaafe4',
'',
'000000',
'505050',
'60659c',
'ff7700',
'ff0000',

'990000',
'009900',

-- main/regproc
'ffff00',
'0000ff',
'0000ff',
'00ff00',
'eeeeee',

'990099',
'ff0000',

'60659c',

'00ff00',
'ff0000',
'ffffff',
'ffeeff',

'00ffff',
'eeeeee',
'990000',
'007700',

'ddffdd',
'ffdddd',

-- tables
'ffffff',
'',
'60659c',
'ffffff',
'dddddd',
'4c4c4c',
'ffff00',
'777777',
'60659c',
''

);
