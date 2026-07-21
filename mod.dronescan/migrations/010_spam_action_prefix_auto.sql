-- 010_spam_action_prefix_auto.sql
--
-- Add spam_actions.prefix_auto: whether a GLINE issued by this action gets
-- "AUTO " prepended ahead of the existing "[N] " connected-client-count
-- prefix in the gline reason ("AUTO [N] reason" vs "[N] reason"). Defaults
-- to true so existing actions keep producing exactly the same gline
-- reason text they do today.

ALTER TABLE spam_actions ADD COLUMN IF NOT EXISTS prefix_auto bool NOT NULL DEFAULT true;
