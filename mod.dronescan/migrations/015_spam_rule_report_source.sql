-- 015_spam_rule_report_source.sql
--
-- Add spam_rules.report_source: who the combined [S] console report line
-- appears to be sent by when the rule fires. BOT (default) = always the
-- bot itself (E), matching existing behavior. SPYCLIENT = sent as the spy
-- client currently covering the channel the triggering event happened in,
-- falling back to BOT when no spy client covers that channel. Defaults to
-- 'BOT' so every existing rule keeps reporting exactly as it does today.

ALTER TABLE spam_rules ADD COLUMN IF NOT EXISTS report_source varchar(16) NOT NULL DEFAULT 'BOT';
