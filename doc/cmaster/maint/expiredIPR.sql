-- hourly crontab
-- 0	*	*	*	*	/usr/local/pgsql/bin/psql -U cmaster cmaster < /home/gnuworld/live-x/scripts/maint/expiredIPR.sql

-- delete when expired >30 days ago
DELETE FROM ip_restrict WHERE expiry<now()::abstime::int4-(86400*30) AND expiry <> 0;

