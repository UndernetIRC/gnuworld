--
-- $Id: notes.sql,v 1.1 2002/08/09 01:10:46 reedloden Exp $
--
-- Referenced from config.inc,
-- creates the proper "notes" table when needed (e.g. when it's not present on you database already ;P)
-- run this like 'psql cservice < notes.sql'. *cheers*
--

CREATE TABLE notes (
        message_id SERIAL,
        user_id INT4 CONSTRAINT users_notes_ref REFERENCES users( id ),
        from_user_id INT4 CONSTRAINT users_notes_ref REFERENCES users( id ),
        message VARCHAR( 300 ),
        last_updated INT4 NOT NULL,

        PRIMARY KEY(message_id, user_id)
);


