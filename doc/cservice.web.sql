CREATE TABLE webcookies (
	user_id INT4 CONSTRAINT users_supporters_ref REFERENCES users( id ),
	cookie VARCHAR(32) UNIQUE,
	expire INT4
);

CREATE TABLE pendingusers (
	user_name VARCHAR(63),
	cookie VARCHAR(32),
	expire INT4
);
