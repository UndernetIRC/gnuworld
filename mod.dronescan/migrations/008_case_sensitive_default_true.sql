-- =============================================================================
-- 008_case_sensitive_default_true.sql
-- spam_events.case_sensitive now only controls case-folding of the
-- TEXT_REPEAT dedup key; it no longer affects TEXT event regex compilation
-- (use an inline "(?i)" prefix in the pattern for case-insensitive TEXT
-- matching, which PCRE2 honors natively). Flips the default for new rows to
-- case-sensitive, matching PCRE2's native default. Existing rows are left
-- untouched.
--
-- Apply with:
--   psql -d <dbname> -f 008_case_sensitive_default_true.sql
-- =============================================================================

ALTER TABLE spam_events ALTER COLUMN case_sensitive SET DEFAULT true;
