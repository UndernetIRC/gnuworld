-- Logging table

CREATE TABLE "logging" (
	"user_id" integer NOT NULL,
	"logmask" integer DEFAULT 0 NOT NULL,
	Constraint "logging_pkey" Primary Key ("user_id")
);

CREATE INDEX logging_user_id_key ON logging USING btree (user_id);

CREATE CONSTRAINT TRIGGER "<unnamed>" AFTER INSERT OR UPDATE ON "logging"  FROM "users" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_check_ins" ('<unnamed>', 'logging', 'users', 'UNSPECIFIED', 'user_id', 'id');

