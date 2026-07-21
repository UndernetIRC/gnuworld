-- 011_spam_gateway_ips.sql
--
-- Seed spam_exclusions with exclusion_type = 'GATEWAYIP' for the IPs/CIDR
-- blocks that were already listed (but never wired up) under the stale
-- ipsToGlineWithIdent config key in ddd.conf. These are known shared IRC
-- gateways (irccloud, mibbit, etc.) where a GLINE must use "user@ip"
-- instead of "*@ip" to avoid collateral-damaging unrelated users. Only the
-- entries that were active (not '#'-commented-out) in ddd.conf are seeded
-- here; commented-out entries are intentionally omitted.
--
-- ON CONFLICT is safe to re-run: spam_exclusions has
-- UNIQUE (exclusion_type, value).

INSERT INTO spam_exclusions (exclusion_type, value, created_ts, modified_ts) VALUES
	('GATEWAYIP', '192.184.8.103',       extract(epoch from now())::int4, extract(epoch from now())::int4),
	('GATEWAYIP', '192.184.8.73',        extract(epoch from now())::int4, extract(epoch from now())::int4),
	('GATEWAYIP', '192.184.9.108',       extract(epoch from now())::int4, extract(epoch from now())::int4),
	('GATEWAYIP', '192.184.9.110',       extract(epoch from now())::int4, extract(epoch from now())::int4),
	('GATEWAYIP', '192.184.9.112',       extract(epoch from now())::int4, extract(epoch from now())::int4),
	('GATEWAYIP', '192.184.10.118',      extract(epoch from now())::int4, extract(epoch from now())::int4),
	('GATEWAYIP', '192.184.10.9',        extract(epoch from now())::int4, extract(epoch from now())::int4),
	('GATEWAYIP', '5.254.36.56/29',      extract(epoch from now())::int4, extract(epoch from now())::int4),
	('GATEWAYIP', '2a03:5180:f:4::/63',  extract(epoch from now())::int4, extract(epoch from now())::int4),
	('GATEWAYIP', '2a03:5180:f::/62',    extract(epoch from now())::int4, extract(epoch from now())::int4),
	('GATEWAYIP', '2001:67c:2f08::/48',  extract(epoch from now())::int4, extract(epoch from now())::int4),
	('GATEWAYIP', '51.77.77.10',         extract(epoch from now())::int4, extract(epoch from now())::int4)
ON CONFLICT (exclusion_type, value) DO NOTHING;
