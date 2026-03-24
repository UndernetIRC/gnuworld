# mod.openchanfix SQL Persistence Bug Report

## Summary

The openchanfix module accumulates all channel op scoring data in memory and only
periodically flushes it to PostgreSQL. Multiple bugs in the SQL update path cause
the `updateInProgress` flag to become permanently stuck, silently preventing all
future SQL writes for the lifetime of the process. This results in months of
scoring data being held only in RAM and lost on restart.

---

## How the Module Stores Data

### Normal Operation

1. **Scoring (every 5 minutes):** `giveAllOpsPoints()` iterates all network
   channels and calls `addPoint()` on in-memory `sqlChanOp` objects stored in
   the `sqlChanOps` map. No SQL is involved.
   - Timer: `tidGivePoints`, interval: `POINTS_UPDATE_TIME` (300s)

2. **SQL sync (every 24 hours):** `prepareUpdate(true)` snapshots the in-memory
   map, then `updateDB()` does a bulk `COPY` into PostgreSQL.
   - Timer: `tidUpdateDB`, interval: `SQL_UPDATE_TIME` (86400s)
   - Source: `chanfix_config.h:126`

3. **DB rotation (daily at midnight GMT):** `rotateDB()` zeros out the oldest
   day slot and deletes zero-score ops from the in-memory map.
   - Timer: `tidRotateDB`, interval: `getSecsTilMidnight()`

4. **Shutdown/detach save:** `OnDetach()` calls `prepareUpdate(false)` for a
   final unthreaded sync before the module unloads.

### Key Observation

The comment at `chanfix_config.h:122-126` says "Default: every 12 hours" but the
actual value is **86400 seconds (24 hours)**. Even under perfectly normal
operation, a crash loses up to 24 hours of scoring data.

---

## Root Cause: `updateInProgress` Never Reset on Error

### The Guard Flag

```
chanfix.h:497    bool updateInProgress;
```

`prepareUpdate()` checks this flag at entry (`chanfix.cc:2902-2905`):

```cpp
if (updateInProgress) {
    elog << "*** [chanfix::prepareUpdate] Update already in progress; not starting."
         << std::endl;
    return;
}
```

If the flag is `true`, every subsequent call to `prepareUpdate()` — whether from
the 24-hour timer, from `OnDetach()`, or from `OnShutdown()` — silently returns
without writing anything to SQL.

### The Bug

`updateDB()` (`chanfix.cc:2978-3116`) contains **six early-return error paths**,
and **none of them reset `updateInProgress` to `false`**:

| Line  | Error Condition                              | Resets flag? |
|-------|----------------------------------------------|--------------|
| 2994  | Error checking for backup table presence     | NO           |
| 3003  | Error dropping existing backup table         | NO           |
| 3012  | Error creating backup table (`CREATE TABLE`) | NO           |
| 3020  | Error truncating chanOps table               | NO           |
| 3029  | Error starting `COPY FROM stdin`             | NO           |
| 3071  | Error on `StopCopyIn()` sync                 | NO           |
| 3113  | **Successful completion (only path)**        | **YES**      |

**Any transient SQL error — a brief network hiccup to PostgreSQL, a disk-full
condition, a lock timeout — permanently locks out all future SQL updates.** The
timer fires every 24 hours and logs "Update already in progress; not starting."
but the operator would only see this in the debug log and might not notice for
months.

### Secondary Consequence: Cannot Shut Down Cleanly

Both the `SHUTDOWN` command (`SHUTDOWNCommand.cc:41-47`) and the `RELOAD` command
(`RELOADCommand.cc:41-47`) check `isUpdateRunning()` and refuse to proceed:

```cpp
if (bot->isUpdateRunning()) {
    bot->SendTo(theClient, "This command cannot proceed while an update is in progress.");
    return;
}
```

Once `updateInProgress` is stuck, these commands are also blocked. The only way to
restart the service is to kill the process externally, which guarantees loss of
all in-memory data that was never flushed to SQL.

Additionally, `OnDetach()` (`chanfix.cc:604-605`) calls `prepareUpdate(false)`,
which also checks `updateInProgress` and silently returns — so even a clean
module unload loses all data.

---

## Contributing Factor: Leftover Backup Table

The `updateDB()` function uses a destructive update strategy:

1. Check if `chanOpsBackup` exists (from a previous failed run)
2. If it exists, `DROP TABLE chanOpsBackup`
3. `CREATE TABLE chanOpsBackup AS SELECT * FROM chanOps`
4. `TRUNCATE TABLE chanOps`
5. `COPY chanOps FROM stdin` (bulk insert from snapshot)
6. Verify row count
7. `DROP TABLE chanOpsBackup`

If the process crashes between steps 3 and 7, the backup table is left behind.
On the next run, step 2 attempts to drop it. If that `DROP` fails for any reason
(e.g., the table is locked), `updateDB()` returns at line 3003 without resetting
the flag — triggering the permanent lockout described above.

Worse: if the process crashes between steps 4 and 5, the live `chanOps` table
has been **truncated** but the new data was never written. The backup table
exists but is never restored. On the next startup, `precacheChanOps()` loads
from the now-empty `chanOps` table, so all historical data is lost even though
the backup table still contains it.

---

## Minor Issue: Misleading Indentation in Threaded Path

In `chanfix.cc:2961-2969`:

```cpp
#ifdef CHANFIX_HAVE_BOOST_THREAD
  if (threaded) {
    ClassUpdateDB updateDB(*this);
    boost::thread pthrd(updateDB);
    pthrd.join();
  } else
#endif /* CHANFIX_HAVE_BOOST_THREAD */
    updateDB();
    printResourceStats();   // <-- always runs, not part of else
```

`printResourceStats()` is not inside a block and always executes regardless of
the `if/else` branch. This is a cosmetic issue (no functional impact) but
indicates the code was not carefully reviewed.

Also note that `pthrd.join()` blocks the calling thread, so the "threaded" path
provides zero concurrency benefit — it has all the overhead of thread creation
with none of the advantages.

---

## Recommended Fixes

### Fix 1: Reset `updateInProgress` on All Error Paths (Critical)

Every early `return` in `updateDB()` must reset `updateInProgress = false` before
returning. The cleanest approach is a scope guard or restructuring to use a single
exit point:

```cpp
void chanfix::updateDB()
{
  // ... existing code ...

  // At every early return, add:
  updateInProgress = false;
  return;

  // Or better: use RAII / scope guard pattern
}
```

### Fix 2: Reduce `SQL_UPDATE_TIME` (Important)

Change `SQL_UPDATE_TIME` from 86400 (24h) to something more reasonable like 3600
(1h) or even 1800 (30 min) to reduce the data loss window:

```cpp
// chanfix_config.h:126
#define SQL_UPDATE_TIME    3600
```

### Fix 3: Restore from Backup Table on Startup (Important)

In `precacheChanOps()` or at startup, check if `chanOpsBackup` exists and
`chanOps` is empty. If so, restore from the backup before proceeding:

```sql
-- If chanOps is empty and chanOpsBackup exists:
INSERT INTO chanOps SELECT * FROM chanOpsBackup;
DROP TABLE chanOpsBackup;
```

### Fix 4: Use Transactions Instead of Backup Tables (Ideal)

Replace the destructive truncate-and-copy strategy with a transactional approach:

```sql
BEGIN;
TRUNCATE TABLE chanOps;
COPY chanOps FROM stdin;
-- ... data ...
COMMIT;  -- atomic: either all data is written or none
```

This eliminates the need for the backup table entirely and prevents the
inconsistent state where `chanOps` is truncated but not repopulated.

### Fix 5: Allow SHUTDOWN/RELOAD When Update Is Stuck (Defense in Depth)

Consider allowing `SHUTDOWN` to proceed even when `updateInProgress` is true,
or add a timeout/force flag. Currently, a stuck flag makes the service
impossible to manage via IRC commands.

---

## Files Involved

| File | Role |
|------|------|
| `mod.openchanfix/chanfix.cc:2897-2971` | `prepareUpdate()` — snapshots memory, calls `updateDB()` |
| `mod.openchanfix/chanfix.cc:2978-3116` | `updateDB()` — writes snapshot to SQL, bug location |
| `mod.openchanfix/chanfix.cc:542-599` | `OnTimer()` — dispatches periodic SQL update |
| `mod.openchanfix/chanfix.cc:601-630` | `OnDetach()` — final save attempt on shutdown |
| `mod.openchanfix/chanfix_config.h:126` | `SQL_UPDATE_TIME` — 86400s (24h) update interval |
| `mod.openchanfix/chanfix.h:497` | `updateInProgress` flag declaration |
| `mod.openchanfix/SHUTDOWNCommand.cc:41` | Blocked by stuck `updateInProgress` |
| `mod.openchanfix/RELOADCommand.cc:41` | Blocked by stuck `updateInProgress` |

---

## Resolution

All issues described in this report have been fixed. The changes are:

1. **Replaced full-dump with incremental sync** (`syncToDB()`) — only dirty ops are
   written to SQL via batched UPSERTs inside a single transaction every 5 minutes.
2. **Fixed `updateInProgress` deadlock** — RAII `UpdateGuard` struct guarantees the
   flag is always reset when `syncToDB()` exits, regardless of success or failure.
3. **Added failure alerting** — after 3 consecutive sync failures, an admin alert
   is sent to the console channel via `logAdminMessage()`. Recovery is also logged.
4. **Added pending deletes** — ops removed by `rotateDB()` are tracked and deleted
   from SQL on the next sync cycle.
5. **SHUTDOWN/RELOAD no longer blocked** — these commands now warn instead of
   refusing to proceed when a sync is in progress.
6. **Backup table eliminated** — the new transactional approach does not need a
   backup table. Legacy backup tables from the old code are handled on first startup.
7. **Schema migration** — `doc/update_chanops_unique.sql` adds the UNIQUE
   constraint on `(channel, account)` required for the UPSERT syntax.
8. **Background thread sync** — `syncToDB()` now snapshots dirty ops into a
   `std::vector` on the main thread (<50ms) and dispatches a detached `std::thread`
   to write UPSERTs. The main event loop is never blocked by SQL operations.
   The background thread opens its own DB connection via `getConnection()`.
   `std::atomic<bool> syncThreadRunning` replaces the old `updateInProgress` flag.
   On shutdown (`forceAll=true`), the sync runs synchronously after waiting for
   any running background thread to complete.
