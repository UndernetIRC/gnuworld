-- This script removes old pending_traffic records to conserve storage space.
-- Select those applications where status is one of the following, and remove
-- the pending_traffic records - they are no longer required for runtime checking.
-- 3 - 'Completed'
-- 4 - 'Cancelled'
-- 9 - 'Rejected'
SELECT channel_id INTO to_die_pending FROM pending WHERE status = 3 OR status = 4 OR status = 9;
BEGIN;
DELETE FROM pending_traffic where channel_id = to_die_pending.channel_id;
DROP TABLE to_die_pending;
END;
