-- Creation of tables needed for nickserv

CREATE TABLE "users" (
   "id" int4 DEFAULT nextval('"users_id_seq"'::text) NOT NULL,
   "name" varchar(40) NOT NULL,
   "lastseen_ts" int4 NOT NULL,
   "registered_ts" int4 NOT NULL,
   "flags" int4 NOT NULL,
   "level" int2 NOT NULL,
   CONSTRAINT "users_pkey" PRIMARY KEY ("id")
);
CREATE  INDEX "id_users_key" ON "users" ("id");

