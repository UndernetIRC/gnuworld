-- 009_ctcp_priv_chan_split.sql
--
-- Split spam_target::CTCP (single bit, matched both private and channel CTCP)
-- into CTCP_PRIV and CTCP_CHAN.
--   - CTCP_PRIV=64  UNCHANGED (keeps the historical CTCP=64 value; this was
--     the original direct-CTCP-to-bot bit before channel CTCP support existed)
--   - CTCP_CHAN=128 NEW (CTCP sent to a channel, seen by the bot or a spy client)
--   - ALL=255       was 127
--
-- Bit VALUES for every other existing source are UNCHANGED -- no per-row
-- migration is required for chan_priv (1), privmsg (2), chan_not (4), part (8),
-- quit (16), notice (32).
--
-- Backfill assumption: any row that previously had bit 64 set was relying on
-- CTCP=64 to match CTCP in BOTH contexts (that was the only meaning available
-- before this split). The UPDATE below ORs in the new CTCP_CHAN=128 bit for
-- every such row, so existing configured events keep matching channel CTCP
-- exactly as they did before the split. Bit 64 keeps its private-CTCP meaning
-- on those same rows via CTCP_PRIV, so no separate action is needed to
-- preserve the private-CTCP match. Admins may later narrow a given event to
-- one context only by clearing the bit they don't want.
--
-- Apply with:
--   psql -d <dbname> -f 009_ctcp_priv_chan_split.sql

UPDATE spam_events
    SET target = target | 128
    WHERE (target & 64) = 64;
