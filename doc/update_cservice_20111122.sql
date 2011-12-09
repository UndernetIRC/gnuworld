-- Update cservice database, adding glines and whitelist tables

CREATE TABLE glines (
        Id SERIAL,
        Host VARCHAR(128) UNIQUE NOT NULL,
        AddedBy VARCHAR(128) NOT NULL,
        AddedOn INT4 NOT NULL,
        ExpiresAt INT4 NOT NULL,
        LastUpdated INT4 NOT NULL DEFAULT now()::abstime::int4,
        Reason VARCHAR(255)
        );


CREATE TABLE whitelist (
	Id SERIAL,
	IP VARCHAR(15) UNIQUE NOT NULL,
	AddedBy VARCHAR(128) NOT NULL,
	AddedOn INT4 NOT NULL,
	ExpiresAt INT4 NOT NULL,
	Reason VARCHAR(255)
	);
