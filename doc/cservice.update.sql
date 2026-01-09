-- Timestamped updates for mod.cservice
-- Apply appropriate updates when upgrading to a new version of cservice

-- 2025-04-01: Empus
--             Added ident column to user_sec_history table
--             Added deleted column to user_sec_history table
--             Added materialized views functions to track linked users by IP and ident
--             Added indexes on user_sec_history table to improve performance
--             Added get_linked_users function to get the linked users for a given username
ALTER TABLE user_sec_history ADD COLUMN ident TEXT NOT NULL;
ALTER TABLE user_sec_history ADD COLUMN deleted TEXT NOT NULLDEFAULT 'N';

CREATE INDEX idx_user_sec_history_user_id ON user_sec_history(user_id);
CREATE INDEX idx_user_sec_history_hostmask ON user_sec_history(hostmask);
CREATE INDEX idx_user_sec_history_deleted ON user_sec_history(deleted);
CREATE INDEX idx_user_sec_history_ip_hostmask ON user_sec_history(ip, hostmask);
CREATE INDEX idx_ip_ident_username ON user_sec_history(ip, ident, user_name);

-- Populate the ident column with the ident from the hostmask
UPDATE user_sec_history SET ident = substring(hostmask FROM '!(.+?)@') WHERE ident IS NULL;

-- Update the deleted column to Y for all users that are not in the users table
UPDATE user_sec_history SET deleted = 'Y' WHERE user_id NOT IN (SELECT id FROM users);

-- Create a materialized view to track the number of unique users per IP and ident
CREATE MATERIALIZED VIEW multiusers_ip_ident AS
SELECT 
  ip,
  ident,
  COUNT(*) AS user_count,
  array_agg(user_name ORDER BY user_name) AS user_names
FROM (
  SELECT DISTINCT ip, ident, user_name
  FROM user_sec_history
  WHERE deleted = 'N'
) AS unique_rows
GROUP BY ip, ident
HAVING COUNT(*) > 3;

CREATE INDEX idx_multiusers_ip_ident_usercount ON multiusers_ip_ident(user_count DESC);

-- Create a materialized view to correlate linked users by IP and ident, with a linked_count and list of users
CREATE MATERIALIZED VIEW multiusers_linked AS
WITH user_fingerprints AS (
  SELECT DISTINCT ip, ident, user_name
  FROM user_sec_history
  WHERE ident IS NOT NULL AND deleted = 'N'
),
linked_pairs AS (
  SELECT DISTINCT a.user_name AS user_name, b.user_name AS linked_user
  FROM user_fingerprints a
  JOIN user_fingerprints b
    ON a.ip = b.ip AND a.ident = b.ident
   AND a.user_name <> b.user_name
)
SELECT
  user_name,
  COUNT(*) AS user_count,
  array_agg(linked_user ORDER BY linked_user) AS linked_usernames
FROM linked_pairs
GROUP BY user_name
HAVING COUNT(*) > 0
ORDER BY user_count DESC;

CREATE INDEX idx_multiusers_linked_username ON multiusers_linked(user_name);
CREATE INDEX idx_multiusers_linked_usercount ON multiusers_linked(user_count DESC);

-- Create a function to get the linked users for a given username
CREATE OR REPLACE FUNCTION get_linked_users(user_id INTEGER)
RETURNS TABLE (
  total_usernames INTEGER,
  all_usernames TEXT[]
)
AS $$
  SELECT
    COUNT(DISTINCT uname),
    array_agg(DISTINCT uname ORDER BY uname)
  FROM (
    SELECT unnest(linked_usernames) AS uname
    FROM multiusers_linked
    WHERE user_name = (
      SELECT user_name
      FROM user_sec_history
      WHERE user_id = get_linked_users.user_id
      AND deleted = 'N'
      LIMIT 1
    )
  ) AS related
  WHERE uname IS DISTINCT FROM (
    SELECT user_name
    FROM user_sec_history
    WHERE user_id = get_linked_users.user_id
    AND deleted = 'N'
    LIMIT 1
  );
$$ LANGUAGE sql STABLE;


-- 2025-04-17: Empus
--             Added deleted column to languages table
ALTER TABLE languages ADD COLUMN deleted INT2 DEFAULT 0;

-- 2026-01-09: MrIron
--             Added new users_fingerprints table
--             Added new translations and help entries for certauth
--			       Added column for scram records.
--             Changed flags column to INT4 to allow for more flags.
--             Added new translations for AUTOHIDE setting.
CREATE TABLE users_fingerprints (
	user_id INT4 CONSTRAINT fingerprints_users_id_ref REFERENCES users ( id ),
	fingerprint VARCHAR( 128 ),
	added_ts BIGINT NOT NULL,
	added_by VARCHAR( 128 ),
  note TEXT,
	PRIMARY KEY (user_id)
);

ALTER TABLE users
  ADD COLUMN scram_record TEXT;

ALTER TABLE users
  ALTER COLUMN flags TYPE INT4;

INSERT INTO translations (language_id, response_id, text, last_updated) VALUES(1, 197, 'Your AUTOHIDE setting is now ON.', 31337);
INSERT INTO translations (language_id, response_id, text, last_updated) VALUES(1, 198, 'Your AUTOHIDE setting is now OFF.', 31337);
INSERT INTO translations (language_id, response_id, text, last_updated) VALUES(1, 195, 'Your CERTONLY setting is now ON.', 31337);
INSERT INTO translations (language_id, response_id, text, last_updated) VALUES(1, 196, 'Your CERTONLY setting is now OFF.', 31337);
INSERT INTO translations (language_id, response_id, text, last_updated) VALUES(1, 197, 'Your AUTOHIDE setting is now ON.', 31337);
INSERT INTO translations (language_id, response_id, text, last_updated) VALUES(1, 198, 'Your AUTOHIDE setting is now OFF.', 31337);
INSERT INTO translations (language_id, response_id, text, last_updated) VALUES(1, 225, 'You currently don''t have any fingerprints added to your account. For more information, use ''/msg X help cert''', 31337);
INSERT INTO translations (language_id, response_id, text, last_updated) VALUES(1, 226, 'No fingerprints found.', 31337);
INSERT INTO translations (language_id, response_id, text, last_updated) VALUES(1, 227, 'Your current fingerprint is: %s', 31337);
INSERT INTO translations (language_id, response_id, text, last_updated) VALUES(1, 228, 'You already have %d fingerprints added to your username.', 31337);
INSERT INTO translations (language_id, response_id, text, last_updated) VALUES(1, 229, 'Invalid fingerprint.', 31337);
INSERT INTO translations (language_id, response_id, text, last_updated) VALUES(1, 230, 'That fingerprint is already added to a username.', 31337);
INSERT INTO translations (language_id, response_id, text, last_updated) VALUES(1, 231, 'Successfully added ''%s'' to your username.', 31337);
INSERT INTO translations (language_id, response_id, text, last_updated) VALUES(1, 232, 'Successfully removed ''%s'' from your username.', 31337);
INSERT INTO translations (language_id, response_id, text, last_updated) VALUES(1, 233, 'You cannot remove that fingerprint unless you disable CERTONLY or add another fingerprint.', 31337);
INSERT INTO translations (language_id, response_id, text, last_updated) VALUES(1, 234, 'That fingerprint was not found on your username.', 31337);

INSERT INTO help VALUES ('CERT', '1', E'/msg X cert <add|rem|list> [fingerprint] [note]\nThis command allows you to add, remove and list TLS fingerprints added to your username.\nBy connecting to Undernet using TLS and a certificate, you may login to X without using a password if the certificate''s fingerprint has been added to your username.\nTo login to X with a fingerprint, use ''/msg X@channels.undernet.org login <username> [TOTP token]''.\nUsing CERT ADD or REM without specifying a fingerprint will add or remove the fingerprint you are currently connected with (if any).\nA specified fingerprint must be in a SHA-256 format (i.e. AB:CD:12:ED:34 ...).\nWhen having a fingerprint added to your account, you may deactivate password login on IRC using ''SET CERTONLY ON/OFF''. You will still be able to login to the website with your password.');
