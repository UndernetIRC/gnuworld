-- "Unregister" previously "deleted" channels (Purged).

UPDATE channels SET registered_ts = 0 WHERE deleted = 1;
