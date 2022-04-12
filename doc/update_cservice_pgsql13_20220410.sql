-- Refactoring for Postgresql 13, which no longer supports now()::abstime::int4
alter table pending_chanfix_scores alter column last_updated set default date_part('epoch', CURRENT_TIMESTAMP)::int;
alter table ip_restrict alter column last_updated set default date_part('epoch', CURRENT_TIMESTAMP)::int;
alter table glines alter column LastUpdated set default date_part('epoch', CURRENT_TIMESTAMP)::int;

CREATE OR REPLACE FUNCTION new_user() RETURNS TRIGGER AS '
-- creates the users associated last_seen record
BEGIN
        INSERT INTO users_lastseen (user_id, last_seen, last_updated) VALUES(NEW.id, extract(epoch FROM now())::int, extract(epoch FROM now())::int);
        RETURN NEW;
END;
' LANGUAGE 'plpgsql';


CREATE OR REPLACE FUNCTION delete_user() RETURNS TRIGGER AS '
BEGIN
        INSERT INTO deletion_transactions (tableID, key1, key2, key3, last_updated)
        VALUES(1, OLD.id, 0, 0, extract(epoch FROM now())::int);
        RETURN OLD;
END;
' LANGUAGE 'plpgsql';


CREATE OR REPLACE FUNCTION delete_channel() RETURNS TRIGGER AS '
BEGIN
        INSERT INTO deletion_transactions (tableID, key1, key2, key3, last_updated)
        VALUES(2, OLD.id, 0, 0, extract(epoch FROM now())::int);
        RETURN OLD;
END;
' LANGUAGE 'plpgsql';


CREATE OR REPLACE FUNCTION delete_level() RETURNS TRIGGER AS '
BEGIN
        INSERT INTO deletion_transactions (tableID, key1, key2, key3, last_updated)
        VALUES(3, OLD.channel_id, OLD.user_id, 0, extract(epoch FROM now())::int);
        RETURN OLD;
END;
' LANGUAGE 'plpgsql';


CREATE OR REPLACE FUNCTION delete_ban() RETURNS TRIGGER AS '
BEGIN
        INSERT INTO deletion_transactions (tableID, key1, key2, key3, last_updated)
        VALUES(4, OLD.id, 0, 0, extract(epoch FROM now())::int);
        RETURN OLD;
END;
' LANGUAGE 'plpgsql';

