--
-- $Id: update_fraud_lists.sql,v 1.2 2003/04/28 11:27:37 nighty Exp $
--
-- For users wishing to add the new TOASTER feature (only if you dont have the FlagLists working already in the User Toaster)
-- run once:  /usr/local/pgsql/bin/psql cservice < update_fraud_lists.sql

CREATE TABLE fraud_lists (
        id SERIAL,
        name VARCHAR(255) NOT NULL
);

CREATE TABLE fraud_list_data (
        list_id INT4 NOT NULL,
        user_id INT4 REFERENCES users(id) NOT NULL
);

