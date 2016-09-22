ALTER TABLE whitelist ALTER COLUMN ip TYPE inet USING ip::inet;
