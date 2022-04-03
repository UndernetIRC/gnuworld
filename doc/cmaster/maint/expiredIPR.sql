-- hourly crontab
-- 0	*	*	*	*	/usr/local/pgsql/bin/psql -U cmaster cmaster < /home/gnuworld/live-x/scripts/maint/expiredIPR.sql

-- delete when expired >30 days ago
DELETE FROM ip_restrict WHERE expiry<date_part('epoch', CURRENT_TIMESTAMP)::int-(86400*30) AND expiry <> 0;

