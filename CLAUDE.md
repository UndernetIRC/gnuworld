## General instructions to the AI agent
* You speak English at all times. Your code is also in English.
* When you want to modify the database structure, you never modify the database directly.
  * You create a file that contains the modifications and you ask me to apply the changes myself, you don't do it.
  * You modify the file that contains the original database construction to keep it up to date. It will be defined in instructions below. If it's not, you ask.
* If you're not sure you're doing what I want you to do, don't assume. Ask me.
* When you're done with your changes (it compiles, etc.), commit them with a appropriate message(s).
  * Do not list yourself as an author (added 2026-07-21).
* Never oh never commit all uncommitted changes. Only the files you modified, or the specific changes I asked you to commit.
* Never push changes.

## Instructions to AI for this specific project
* The module you will be writing your code in is `mod.dronescan` (there's a directory for that).
* The current database for that module is defined in `doc/dronescan.sql`. Keep that file up to date.
* We will implement things one by one. i.e. I don't want to implement the code for event types all at once. I'll start with regex based on privmsg, and then expant to others.
* If you make changes to Makefile.am, you'll have to do, in the project's root directory, `./autogen.sh` followed by `./configure --enable-modules=dronescan,ccontrol,openchanfix --prefix=/home/hidden/dev/git/gnuworld/gw --with-log4cplus --with-log4cplus-lib=/usr/lib/x86_64-linux-gnu --with-log4cplus-include=/usr/include` before you attempt a recompile (`make && make install`).
* Encoding is ISO-8859-1 for this project. Be careful to avoid using some chars that are not supported.
* If you modify modify anything related to the SPAM feature, make sure you modify `mod.dronescan/SPAM.md`.
* Apply `clang-format -i file` on every files of type `.cc` and `.h` modified before committing.
* Database migration files (`mod.dronescan/migrations/`, `mod.openchanfix/migrations/`, and any other module's `migrations/` directory) must be idempotent: safe to run both against an older database AND against a database freshly created from the module's current `doc/*.sql` (which is kept at the current, post-migration schema shape, not the original install shape). Use `CREATE TABLE/INDEX IF NOT EXISTS`, `ADD COLUMN IF NOT EXISTS`, `DROP COLUMN IF EXISTS`, and wrap `RENAME COLUMN`/`ADD CONSTRAINT` (no `IF NOT EXISTS` form in Postgres) in a `DO $$ ... $$` block that checks `information_schema.columns`/`pg_constraint` first.
  * Before considering migration work done, test it yourself end-to-end against a throwaway local database (never the project's real database): `createdb` a scratch DB, load the module's `doc/*.sql`, apply every file in its `migrations/` directory in order with `psql -U postgres -v ON_ERROR_STOP=1 -d <scratch_db> -f <file>`, confirm no errors, re-apply the changed migration file(s) a second time to confirm they no-op cleanly, then `dropdb` the scratch DB.

## Commands execution
* You have my permission to execute commands for this project.