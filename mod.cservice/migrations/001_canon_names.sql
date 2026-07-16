-- 2026-03-16: MrIron
--             Added canonicalized channel name column
ALTER TABLE channels ADD COLUMN canon_name TEXT UNIQUE;
UPDATE channels SET canon_name = lower(
  replace(replace(replace(replace(name, '[', '{'), ']', '}'), '\\', '|'), '~', '^')
);
ALTER TABLE channels ALTER COLUMN canon_name SET NOT NULL;
CREATE UNIQUE INDEX channels_canon_name_idx ON channels(canon_name);
