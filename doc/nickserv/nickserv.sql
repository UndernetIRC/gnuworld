-- Creation of tables needed for nickserv

CREATE TABLE "users" (
	"id" integer DEFAULT nextval('"users_id_seq"'::text) NOT NULL,
	"name" character varying(40) NOT NULL,
	"lastseen_ts" integer DEFAULT 0 NOT NULL,
	"registered_ts" integer DEFAULT 0 NOT NULL,
	"flags" integer DEFAULT 0 NOT NULL,
	"level" smallint DEFAULT 0 NOT NULL,
  "logmask" smallint DEFAULT 0 NOT NULL,
	Constraint "users_pkey" Primary Key ("id")
);

CREATE INDEX id_users_key ON users USING btree (id);

