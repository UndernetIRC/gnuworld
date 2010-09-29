-- $Id: update.statistics.sql,v 1.1 2003/03/14 04:41:25 nighty Exp $
--
-- in order to update your schema to support the new 'statistics' table, you need to run this *ONCE*
--
--	your-system:/home/gnuworld/gnuworld/doc$ /usr/local/pgsql/bin/psql cservice < update.statistics.sql
--
--		enjoy!

CREATE TABLE statistics (
	users_id	INT4 NOT NULL,
	stats_type	INT4 NOT NULL,
--	1	Total reviewed applications count
--	...	more capabilities
	stats_value_int	INT4 DEFAULT 0 NOT NULL,
	stats_value_chr	VARCHAR(255) DEFAULT '' NOT NULL,
	last_updated	INT4 NOT NULL
);

CREATE INDEX statistics_users_id_idx ON statistics(users_id);

