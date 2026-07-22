-- 012_comlog.sql
--
-- Command audit trail: every dispatched mod.dronescan command (matched in
-- commandMap or one of the legacy hardcoded keywords) is logged here by
-- dronescan::logCommandMessage(), and read back by the LASTCOM command.
--
-- command is TEXT (not a bounded varchar) so long commands are never
-- truncated.

CREATE TABLE comlog (
	id       serial      PRIMARY KEY,
	ts       int4        NOT NULL,
	username varchar(128) NOT NULL,
	command  text        NOT NULL
);
CREATE INDEX comlog_ts_idx ON comlog (ts);
