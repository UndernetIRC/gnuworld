--
-- $Id: update_complaints.sql,v 1.2 2003/10/20 02:26:50 nighty Exp $
--
-- This goes to remote DB if you have an older database schema... eg:
--	/usr/local/pgsql/bin/psql cservice < update_complaints.sql
--

CREATE TABLE complaints (
	id SERIAL,
	from_id int4 NOT NULL,
	from_email varchar (255) NOT NULL,
	inrec_email varchar (255) NOT NULL,
	complaint_type int4 NOT NULL,
	complaint_text text NOT NULL,
	complaint_logs text NOT NULL,
	complaint_channel1_id int4 NOT NULL,
	complaint_channel1_name text NOT NULL,
	complaint_channel2_id int4 NOT NULL,
	complaint_channel2_name text NOT NULL,
	complaint_users_id int4 NOT NULL,
	status int4 NOT NULL,
	nicelevel int4 NOT NULL,
	reviewed_by_id int4 NOT NULL,
	reviewed_ts int4 NOT NULL,
	created_ts int4 NOT NULL,
	created_ip varchar (15) DEFAULT '0.0.0.0' NOT NULL,
	created_crc varchar (128) NOT NULL,
	crc_expiration int4 NOT NULL,
	ticket_number varchar(32) NOT NULL,
	PRIMARY KEY (id)
);

CREATE TABLE complaints_threads (
	id SERIAL,
	complaint_ref int4 NOT NULL CONSTRAINT complaints_threads_ref REFERENCES complaints (id),
	reply_by int4 NOT NULL,
	reply_ts int4 NOT NULL,
	reply_text text NOT NULL,
	actions_text text NOT NULL,
	in_reply_to int4 NOT NULL,
	PRIMARY KEY (id)
);


CREATE TABLE complaint_types (
	-- not used for now...
	id SERIAL,
	complaint_label varchar(255) NOT NULL,
	PRIMARY KEY (id)
);

DELETE FROM complaint_types;
COPY "complaint_types" FROM stdin;
1	My username is suspended
2	Members of a registered channel are spamming my channel
3	I object to this channel application but I want to do so anonymously
4	My channel was purged and I want you to reconsider
5	My channel was purged and I want to know why
99	Other complaint
\.


