ALTER TABLE whitelist ALTER COLUMN ip TYPE inet USING ip::inet;
ALTER TABLE pending_traffic ALTER COLUMN ip_number TYPE inet USING ip_number::inet;
DROP TABLE ip_restrict;
CREATE TABLE ip_restrict (
	id		SERIAL,
	user_id		int4 NOT NULL,
	added		int4 NOT NULL,
	added_by	int4 NOT NULL,
	type		int4 NOT NULL DEFAULT 0,
	value		inet NOT NULL,
	last_updated	int4 NOT NULL DEFAULT date_part('epoch', CURRENT_TIMESTAMP)::int,
	last_used	int4 NOT NULL DEFAULT 0,
	expiry		int4 NOT NULL,
	description	VARCHAR(255)
);
