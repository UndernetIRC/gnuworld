---
mode: sequential
complexity: complex
type: refactor
playwright: false
frontend-design: false
spec-version: 1
created: 2026-03-22T12:00:00
branch: feat/openchanfix-incremental-sql
---

# Plan: mod.openchanfix Incremental SQL Updates

## Task Description

Refactor the mod.openchanfix module's SQL persistence layer to replace the
destructive full-dump strategy (TRUNCATE + bulk COPY every 24 hours) with
incremental batched UPSERTs every 5 minutes. Fix the critical `updateInProgress`
deadlock bug where any SQL error permanently prevents all future database writes.

## Objective

When complete, the openchanfix module will:
1. Track per-op dirty state in memory and only write changed ops to SQL
2. Sync dirty ops to PostgreSQL every 5 minutes in a single transaction
3. Always reset `updateInProgress` via RAII guard, even on SQL errors
4. Track in-memory deletions and propagate them to SQL on next sync
5. Never truncate or backup the chanOps table during normal operation
6. Handle legacy `chanOpsBackup` tables left by the old code on first startup

## Problem Statement

The openchanfix module accumulates all channel op scoring data in memory and only
flushes to PostgreSQL every 24 hours via a destructive TRUNCATE + bulk COPY. Two
critical bugs exist:

1. **`updateInProgress` permanently stuck on any SQL error** — `updateDB()` at
   `chanfix.cc:2978-3116` has 6 early-return error paths and none reset the flag
   to `false`. Only the success path at line 3113 resets it. Once any transient
   SQL error occurs, no further SQL writes happen for the process lifetime.

2. **Full-dump is fragile and wasteful** — every sync rewrites the entire table
   regardless of what changed. The destructive TRUNCATE leaves a window where the
   table is empty. The backup table strategy can leave orphaned backup tables on
   crash, further blocking future updates.

## Solution Approach

Replace the bulk-dump with per-op dirty tracking and batched UPSERTs:
- Add a `dirty` boolean to `sqlChanOp`. Mutating methods set it to `true`.
- Every 5 minutes, open a single SQL transaction, UPSERT all dirty ops, DELETE
  any ops removed by `rotateDB()`, then COMMIT. Clear dirty flags on success,
  ROLLBACK on error (dirty flags remain set for retry next cycle).
- Use an RAII `UpdateGuard` struct to guarantee `updateInProgress` is always
  reset when `syncToDB()` exits, regardless of success or failure.
- Add a UNIQUE constraint on `chanOps(channel, account)` to enable PostgreSQL's
  `INSERT ... ON CONFLICT DO UPDATE` syntax.

## Relevant Files

- `mod.openchanfix/sqlChanOp.h` — sqlChanOp class with inline mutators that need dirty flag
- `mod.openchanfix/sqlChanOp.cc` — constructor and setAllMembers (loads from DB)
- `mod.openchanfix/chanfix.h` — chanfix class: data structures, timer IDs, method declarations
- `mod.openchanfix/chanfix.cc` — main implementation: `prepareUpdate()`, `updateDB()`, `precacheChanOps()`, `rotateDB()`, `OnTimer()`, `startTimers()`, `OnDetach()`, `ClassUpdateDB` thread wrapper
- `mod.openchanfix/chanfix_config.h` — `SQL_UPDATE_TIME` (86400), `POINTS_UPDATE_TIME` (300), `DAYSAMPLES` (14)
- `mod.openchanfix/SHUTDOWNCommand.cc` — blocks on `isUpdateRunning()`
- `mod.openchanfix/RELOADCommand.cc` — blocks on `isUpdateRunning()`
- `mod.openchanfix/doc/chanfix.sql` — reference schema (chanOps table, no UNIQUE constraint)
- `libgnuworld/misc.h` — `escapeSQLChars()` function for SQL string escaping
- `db/pgsqlDB.h` / `db/pgsqlDB.cc` — `dbHandle` with `Exec()`, `PutLine()`, `StopCopyIn()` methods

### New Files

- `mod.openchanfix/doc/update_chanops_unique.sql` — migration script to deduplicate existing rows and add UNIQUE constraint

## Implementation Phases

### Phase 1: Foundation
Schema migration and dirty flag infrastructure. These changes are additive and
don't alter runtime behavior yet.

### Phase 2: Core Implementation
Replace `prepareUpdate()`/`updateDB()` with `syncToDB()`, update timers, add
pending deletes to `rotateDB()`, handle legacy backup tables in
`precacheChanOps()`.

### Phase 3: Integration & Polish
Remove dead code (boost thread wrapper, snapshot structs), fix SHUTDOWN/RELOAD
commands, code review and validation.

## Step by Step Tasks

### 1. Schema Migration Script
- **Task ID**: schema-migration
- **Depends On**: none
- **Description**:
  - Create migration script `mod.openchanfix/doc/update_chanops_unique.sql` that:
    - Deduplicates existing rows: `DELETE FROM chanOps a USING chanOps b WHERE a.channel = b.channel AND a.account = b.account AND a.ctid < b.ctid;`
    - Adds constraint: `ALTER TABLE chanOps ADD CONSTRAINT chanops_channel_account_unique UNIQUE (channel, account);`
  - Modify the reference schema in `mod.openchanfix/doc/chanfix.sql` to add `UNIQUE (channel, account)` to the CREATE TABLE statement for `chanOps`. Add it as a table constraint on a new line after the `day13` column definition. The exact line to add after line 53 (closing the day13 row with a comma instead of no comma):
    ```sql
    day13 INT2 NOT NULL DEFAULT 0,
    UNIQUE (channel, account)
    ```
- **Files**:
  - creates: `mod.openchanfix/doc/update_chanops_unique.sql`
  - modifies: `mod.openchanfix/doc/chanfix.sql`
- **Tests**: N/A (SQL schema files, not compiled code)

### 2. Add Dirty Flag to sqlChanOp
- **Task ID**: dirty-flag
- **Depends On**: schema-migration
- **Description**:
  - In `mod.openchanfix/sqlChanOp.h`:
    - Add `bool dirty;` to the private members block (after `bool OldestOp;` at line 128)
    - Add inline getter and setter in the public section (near the other getters around line 74):
      ```cpp
      inline bool isDirty() const
          { return dirty; }
      inline void setDirty(bool _dirty)
          { dirty = _dirty; }
      ```
    - Add `dirty = true;` to each of these existing inline mutators:
      - `setPoints()` (line 92-93): change to `{ day[currentDay] = _points; calcTotalPoints(); dirty = true; }`
      - `addPoint()` (line 95-96): change to `{ day[currentDay]++; calcTotalPoints(); dirty = true; }`
      - `setLastSeenAs()` (line 98-99): change to `{ nickUserHost = _nickUserHost; dirty = true; }`
      - `setTimeFirstOpped()` (line 101-102): change to `{ ts_firstopped = _ts_firstopped; dirty = true; }`
      - `setTimeLastOpped()` (line 104-105): change to `{ ts_lastopped = _ts_lastopped; dirty = true; }`
      - `setDay()` (line 107-108): change to `{ day[_dayval] = _pointsval; dirty = true; }`
  - In `mod.openchanfix/sqlChanOp.cc`:
    - In the constructor (around line 41-52), add `dirty = false;` to the initialization list alongside the other member initializations
    - In `setAllMembers()` (around line 54-76), add `dirty = false;` at the end of the method (data loaded from DB is clean)
- **Files**:
  - modifies: `mod.openchanfix/sqlChanOp.h`
  - modifies: `mod.openchanfix/sqlChanOp.cc`
- **Tests**: N/A (inline flag additions to existing class, verified by build)

### 3. Add Pending Deletes and UpdateGuard to chanfix.h
- **Task ID**: chanfix-header-updates
- **Depends On**: dirty-flag
- **Description**:
  - In `mod.openchanfix/chanfix.h`:
    - Add `#include <list>` and `#include <utility>` if not already present (check existing includes at top of file)
    - Add the pending deletes type and member in the public section near the other typedefs (around line 362):
      ```cpp
      typedef std::list<std::pair<std::string, std::string>> pendingDeletesType;
      pendingDeletesType pendingDeletes;
      ```
    - Add a sync failure counter and max constant in the protected section (near `updateInProgress` at line 497):
      ```cpp
      unsigned int	syncFailures;
      ```
    - Add a constant for max consecutive failures. Define in the public section or as a static const:
      ```cpp
      static const unsigned int MAX_SYNC_FAILURES = 3;
      ```
    - Add the RAII guard struct inside the `chanfix` class, in the public section:
      ```cpp
      struct UpdateGuard {
          bool& flag;
          UpdateGuard(bool& f) : flag(f) { flag = true; }
          ~UpdateGuard() { flag = false; }
      };
      ```
    - Add new method declarations in the public section (near `prepareUpdate` and `updateDB` declarations, around line 283):
      ```cpp
      void syncToDB(bool forceAll = false);
      ```
    - Initialize `syncFailures = 0;` in the chanfix constructor in `chanfix.cc` (near `updateInProgress = false;` at line 106)
    - Remove declarations that will be deleted:
      - Remove `void prepareUpdate(bool);` (line 283)
      - Remove `void updateDB();` (line 284)
    - Remove the snapshot data structures (lines 376-385):
      - Remove the `snapShotStruct` typedef
      - Remove the `DBMapType` typedef
      - Remove the `snapShot` member
- **Files**:
  - modifies: `mod.openchanfix/chanfix.h`
- **Tests**: N/A (header declarations, verified by build)

### 4. Update chanfix_config.h Timer Constant
- **Task ID**: config-timer
- **Depends On**: chanfix-header-updates
- **Description**:
  - In `mod.openchanfix/chanfix_config.h`:
    - Replace the `SQL_UPDATE_TIME` block (lines 122-126):
      ```
      /**
       * Interval between two consecutive SQL updates.
       * Default: every 12 hours
       */
      #define SQL_UPDATE_TIME		86400
      ```
      With:
      ```
      /**
       * Interval between two consecutive SQL sync operations.
       * Dirty ops are flushed to the database at this interval.
       * Default: every 5 minutes (matches POINTS_UPDATE_TIME)
       */
      #define SQL_SYNC_TIME		300
      ```
- **Files**:
  - modifies: `mod.openchanfix/chanfix_config.h`
- **Tests**: N/A (config constant, verified by build)

### 5. Implement syncToDB() in chanfix.cc
- **Task ID**: sync-to-db
- **Depends On**: config-timer
- **Description**:
  This is the core task. Replace `prepareUpdate()` and `updateDB()` with `syncToDB()`.
  - **Remove** the following from `mod.openchanfix/chanfix.cc`:
    - The `ClassUpdateDB` class (lines 527-538, inside `#ifdef CHANFIX_HAVE_BOOST_THREAD`)
    - The `#include <boost/thread/thread.hpp>` conditional include (lines 61-63)
    - The entire `prepareUpdate()` function (lines 2896-2971)
    - The entire `updateDB()` function (lines 2978-3116)
    - The `printResourceStats()` function (lines 3118-3124) — keep if desired for debugging, but it's no longer called from the update path
  - **Add** the new `syncToDB(bool forceAll)` method. Implementation:
    ```cpp
    void chanfix::syncToDB(bool forceAll)
    {
      if (updateInProgress) {
        elog << "*** [chanfix::syncToDB] Sync already in progress; skipping."
             << std::endl;
        return;
      }

      UpdateGuard guard(updateInProgress);

      dbHandle* cacheCon = localDBHandle;

      // Begin transaction
      if (!cacheCon->Exec("BEGIN")) {
        elog << "*** [chanfix::syncToDB] Error starting transaction: "
             << cacheCon->ErrorMessage() << std::endl;
        syncFailures++;
        if (syncFailures >= MAX_SYNC_FAILURES) {
          logAdminMessage("CRITICAL: SQL sync has failed %d consecutive times. "
                          "Data is accumulating in memory only!",
                          syncFailures);
        }
        return;
      }

      // 1. Process pending deletes
      for (pendingDeletesType::iterator it = pendingDeletes.begin();
           it != pendingDeletes.end(); ++it) {
        std::stringstream delQuery;
        delQuery << "DELETE FROM chanOps WHERE channel = '"
                 << escapeSQLChars(it->first) << "' AND account = '"
                 << escapeSQLChars(it->second) << "'";
        if (!cacheCon->Exec(delQuery.str())) {
          elog << "*** [chanfix::syncToDB] Error deleting chanOp: "
               << cacheCon->ErrorMessage() << std::endl;
          cacheCon->Exec("ROLLBACK");
          syncFailures++;
          if (syncFailures >= MAX_SYNC_FAILURES) {
            logAdminMessage("CRITICAL: SQL sync has failed %d consecutive times. "
                            "Data is accumulating in memory only!",
                            syncFailures);
          }
          return;
        }
      }
      int deletesProcessed = pendingDeletes.size();
      pendingDeletes.clear();

      // 2. UPSERT dirty ops
      int upsertsProcessed = 0;
      for (sqlChanOpsType::iterator ptr = sqlChanOps.begin();
           ptr != sqlChanOps.end(); ++ptr) {
        for (sqlChanOpsType::mapped_type::iterator chanOp = ptr->second.begin();
             chanOp != ptr->second.end(); ++chanOp) {
          sqlChanOp* curOp = chanOp->second;
          if (!forceAll && !curOp->isDirty())
            continue;

          std::stringstream upsertQuery;
          upsertQuery << "INSERT INTO chanOps (channel, account, last_seen_as, "
                      << "ts_firstopped, ts_lastopped";
          for (int i = 0; i < DAYSAMPLES; i++)
            upsertQuery << ", day" << i;
          upsertQuery << ") VALUES ('"
                      << escapeSQLChars(curOp->getChannel()) << "', '"
                      << escapeSQLChars(curOp->getAccount()) << "', '"
                      << escapeSQLChars(curOp->getLastSeenAs()) << "', "
                      << curOp->getTimeFirstOpped() << ", "
                      << curOp->getTimeLastOpped();
          for (int i = 0; i < DAYSAMPLES; i++)
            upsertQuery << ", " << curOp->getDay(i);
          upsertQuery << ") ON CONFLICT (channel, account) DO UPDATE SET "
                      << "last_seen_as = EXCLUDED.last_seen_as, "
                      << "ts_firstopped = EXCLUDED.ts_firstopped, "
                      << "ts_lastopped = EXCLUDED.ts_lastopped";
          for (int i = 0; i < DAYSAMPLES; i++)
            upsertQuery << ", day" << i << " = EXCLUDED.day" << i;

          if (!cacheCon->Exec(upsertQuery.str())) {
            elog << "*** [chanfix::syncToDB] Error upserting chanOp ("
                 << curOp->getChannel() << ", " << curOp->getAccount()
                 << "): " << cacheCon->ErrorMessage() << std::endl;
            cacheCon->Exec("ROLLBACK");
            syncFailures++;
            if (syncFailures >= MAX_SYNC_FAILURES) {
              logAdminMessage("CRITICAL: SQL sync has failed %d consecutive times. "
                              "Data is accumulating in memory only!",
                              syncFailures);
            }
            return;  // dirty flags remain set; next cycle will retry
          }
          curOp->setDirty(false);
          upsertsProcessed++;
        }
      }

      // Commit transaction
      if (!cacheCon->Exec("COMMIT")) {
        elog << "*** [chanfix::syncToDB] Error committing transaction: "
             << cacheCon->ErrorMessage() << std::endl;
        // Mark all ops dirty again since commit failed
        for (sqlChanOpsType::iterator ptr = sqlChanOps.begin();
             ptr != sqlChanOps.end(); ++ptr) {
          for (sqlChanOpsType::mapped_type::iterator chanOp = ptr->second.begin();
               chanOp != ptr->second.end(); ++chanOp) {
            chanOp->second->setDirty(true);
          }
        }
        syncFailures++;
        if (syncFailures >= MAX_SYNC_FAILURES) {
          logAdminMessage("CRITICAL: SQL sync has failed %d consecutive times. "
                          "Data is accumulating in memory only!",
                          syncFailures);
        }
        return;
      }

      // Success — reset failure counter
      if (syncFailures > 0) {
        logAdminMessage("SQL sync recovered after %d consecutive failure(s).",
                        syncFailures);
      }
      syncFailures = 0;

      if (upsertsProcessed > 0 || deletesProcessed > 0) {
        logDebugMessage("SQL sync complete: %d upserts, %d deletes.",
                        upsertsProcessed, deletesProcessed);
      }
    }
    ```
  - Use `escapeSQLChars()` from `libgnuworld/misc.h` (already included via `chanfix_misc.h` or `misc.h`) for all string values in SQL queries.
  - Note: `setDirty(false)` inside the loop won't trigger the dirty flag in a problematic way because `setDirty` is a direct setter, not a mutating method that sets `dirty = true`.
- **Files**:
  - modifies: `mod.openchanfix/chanfix.cc`
- **Tests**: N/A (no test harness exists for this module; verified by build + manual testing)

### 6. Update OnTimer and startTimers
- **Task ID**: update-timers
- **Depends On**: sync-to-db
- **Description**:
  - In `mod.openchanfix/chanfix.cc`, modify the `OnTimer()` method:
    - Replace the `tidUpdateDB` handler block (lines 583-589):
      ```cpp
      else if (theTimer == tidUpdateDB) {
        /* Prepare to synchronize the database in a thread */
        prepareUpdate(true);

        /* Refresh Timer */
        theTime = time(NULL) + SQL_UPDATE_TIME;
        tidUpdateDB = MyUplink->RegisterTimer(theTime, this, NULL);
      }
      ```
      With:
      ```cpp
      else if (theTimer == tidUpdateDB) {
        /* Sync dirty ops to the database */
        syncToDB();

        /* Refresh Timer */
        theTime = time(NULL) + SQL_SYNC_TIME;
        tidUpdateDB = MyUplink->RegisterTimer(theTime, this, NULL);
      }
      ```
  - In `startTimers()`, replace the `tidUpdateDB` registration (line 2885-2886):
    ```cpp
    theTime = time(NULL) + SQL_UPDATE_TIME;
    ```
    With:
    ```cpp
    theTime = time(NULL) + SQL_SYNC_TIME;
    ```
  - In `OnDetach()`, replace line 605:
    ```cpp
    prepareUpdate(false);
    ```
    With:
    ```cpp
    syncToDB(true);
    ```
- **Files**:
  - modifies: `mod.openchanfix/chanfix.cc`
- **Tests**: N/A (timer wiring, verified by build)

### 7. Add Pending Deletes to rotateDB()
- **Task ID**: rotate-db-deletes
- **Depends On**: update-timers
- **Description**:
  - In `mod.openchanfix/chanfix.cc`, in the `rotateDB()` method (lines 3154-3246):
    - When an op is deleted from the in-memory map (at lines 3201-3202):
      ```cpp
      ptr->second.erase(chanOp++);
      delete curOp; curOp = 0;
      ```
      Add a line before the erase to record the deletion:
      ```cpp
      pendingDeletes.push_back(std::make_pair(ptr->first, curOp->getAccount()));
      ptr->second.erase(chanOp++);
      delete curOp; curOp = 0;
      ```
- **Files**:
  - modifies: `mod.openchanfix/chanfix.cc`
- **Tests**: N/A (single line addition, verified by build)

### 8. Handle Legacy Backup Table in precacheChanOps()
- **Task ID**: legacy-backup
- **Depends On**: rotate-db-deletes
- **Description**:
  - In `mod.openchanfix/chanfix.cc`, in `precacheChanOps()` (around lines 1314-1383):
    - The existing code at lines 1324-1336 checks for `chanOpsBackup` and **exits if found**. Replace this logic with recovery:
      ```cpp
      // Check for legacy backup table from old full-dump code
      if (cacheCon->Exec("SELECT count(*) FROM pg_tables WHERE tablename = 'chanopsbackup'", true)
          && cacheCon->Tuples() && atoi(cacheCon->GetValue(0, 0))) {
        // Check if the main chanOps table is empty
        if (cacheCon->Exec("SELECT count(*) FROM chanOps", true)
            && cacheCon->Tuples() && atoi(cacheCon->GetValue(0, 0)) == 0) {
          // Main table is empty — restore from backup
          elog << "*** [chanfix::precacheChanOps] Restoring chanOps from legacy backup table."
               << std::endl;
          cacheCon->Exec("INSERT INTO chanOps SELECT * FROM chanOpsBackup");
        }
        // Drop the backup table either way
        elog << "*** [chanfix::precacheChanOps] Dropping legacy chanOpsBackup table."
             << std::endl;
        cacheCon->Exec("DROP TABLE chanOpsBackup");
      }
      ```
    - After the existing loop that loads ops into memory (which calls `setAllMembers` which already sets `dirty = false`), no additional change is needed — ops loaded from DB are already clean.
- **Files**:
  - modifies: `mod.openchanfix/chanfix.cc`
- **Tests**: N/A (startup recovery path, verified by manual testing)

### 9. Fix SHUTDOWN and RELOAD Commands
- **Task ID**: fix-shutdown-reload
- **Depends On**: legacy-backup
- **Description**:
  - In `mod.openchanfix/SHUTDOWNCommand.cc`, replace the blocking check at lines 41-47:
    ```cpp
    if (bot->isUpdateRunning()) {
      bot->SendTo(theClient,
            bot->getResponse(theUser,
                    language::update_in_progress,
                    std::string("This command cannot proceed while an update is in progress. Please try again later.")).c_str());
      return;
    }
    ```
    With a warning that does not block:
    ```cpp
    if (bot->isUpdateRunning()) {
      bot->logDebugMessage("WARNING: SHUTDOWN issued while SQL sync in progress.");
    }
    ```
  - In `mod.openchanfix/RELOADCommand.cc`, apply the same change at lines 41-47:
    ```cpp
    if (bot->isUpdateRunning()) {
      bot->logDebugMessage("WARNING: RELOAD issued while SQL sync in progress.");
    }
    ```
- **Files**:
  - modifies: `mod.openchanfix/SHUTDOWNCommand.cc`
  - modifies: `mod.openchanfix/RELOADCommand.cc`
- **Tests**: N/A (command handler change, verified by build)

### 10. Remove Dead Code
- **Task ID**: remove-dead-code
- **Depends On**: fix-shutdown-reload
- **Description**:
  Clean up all remnants of the old full-dump approach:
  - In `mod.openchanfix/chanfix.h`:
    - Remove the `tidCheckDB` timer ID if `checkDBConnection()` is commented out (verify — the OnTimer handler for `tidCheckDB` at lines 560-566 calls the commented-out `/*checkDBConnection();*/`). If keeping the timer for future use, leave it; otherwise remove both the timer ID and its OnTimer block.
    - Verify `snapShotStruct`, `DBMapType`, and `snapShot` were already removed in task 3. If any remnants remain, remove them.
    - Remove `void prepareUpdate(bool);` and `void updateDB();` declarations if not already removed in task 3.
  - In `mod.openchanfix/chanfix.cc`:
    - Verify the `ClassUpdateDB` class, `prepareUpdate()`, and `updateDB()` were removed in task 5
    - Verify the `#include <boost/thread/thread.hpp>` conditional was removed in task 5
    - Remove `printResourceStats()` if it is no longer called anywhere (search for all call sites first)
  - In `mod.openchanfix/chanfix.h`:
    - Remove the `printResourceStats()` declaration if the function was removed
- **Files**:
  - modifies: `mod.openchanfix/chanfix.h`
  - modifies: `mod.openchanfix/chanfix.cc`
- **Tests**: N/A (dead code removal, verified by build)

### 11. Code Review
- **Task ID**: review-all
- **Depends On**: schema-migration, dirty-flag, chanfix-header-updates, config-timer, sync-to-db, update-timers, rotate-db-deletes, legacy-backup, fix-shutdown-reload, remove-dead-code
- **Description**: Review all code changes for correctness, style, edge cases, and security. Report issues by severity (Critical, Important, Minor).
  Review your own work: re-read every file you changed, check for bugs, missing edge cases, security issues, and style problems. Fix any issues found before proceeding to validation.
  Specifically check:
  - All SQL strings use `escapeSQLChars()` for string values
  - `UpdateGuard` correctly resets `updateInProgress` on all exit paths including exceptions
  - `setDirty(false)` in `syncToDB()` only clears flags after successful `Exec()` but before `COMMIT` — verify this is safe (if COMMIT fails, the re-dirtying loop handles it)
  - `pendingDeletes` is cleared inside the transaction but before COMMIT — verify that on ROLLBACK, we don't lose track of deletes (they will be re-added by rotateDB on next rotation, and the ops are already deleted from memory, so the SQL rows become orphaned until next rotation — document this as acceptable)
  - No dangling references to `prepareUpdate`, `updateDB`, `SQL_UPDATE_TIME`, `snapShot`, `snapShotStruct`, or `ClassUpdateDB` remain in the codebase
  - The `DAYSAMPLES` loop in `syncToDB()` generates correct column names `day0` through `day13`

### 12. Final Validation
- **Task ID**: validate-all
- **Depends On**: review-all
- **Description**: Run all validation commands, verify every acceptance criterion is met.

## Documentation Requirements

- Update the debug report at `mod.openchanfix/DEBUG-REPORT-sql-persistence.md` with a "Resolution" section describing the changes made
- Add a comment block at the top of `syncToDB()` explaining the incremental sync strategy and error recovery behavior
- Add a note in `mod.openchanfix/doc/update_chanops_unique.sql` explaining when and why to run the migration

## Acceptance Criteria

- `./configure --enable-modules=openchanfix && make` compiles with zero errors and zero warnings related to changed files
- No references to `prepareUpdate`, `updateDB`, `SQL_UPDATE_TIME`, `snapShot`, `snapShotStruct`, or `ClassUpdateDB` remain in the codebase (except in git history or the debug report)
- `chanfix_config.h` defines `SQL_SYNC_TIME` as `300`
- `sqlChanOp` has a `dirty` flag that is set by all point/time mutators and cleared by `setAllMembers()` and `syncToDB()`
- `chanfix.h` declares `syncToDB(bool forceAll = false)`, `pendingDeletes`, and `UpdateGuard`
- `syncToDB()` uses `BEGIN`/`COMMIT`/`ROLLBACK` transaction pattern
- `syncToDB()` uses `INSERT ... ON CONFLICT (channel, account) DO UPDATE` for upserts
- `UpdateGuard` RAII pattern guarantees `updateInProgress` is reset on all exit paths
- `syncToDB()` tracks consecutive failures via `syncFailures` counter and sends `logAdminMessage()` (visible in console channel) after `MAX_SYNC_FAILURES` (3) consecutive failures
- `syncFailures` resets to 0 on success, and logs a recovery message if it was previously non-zero
- `rotateDB()` pushes deleted ops to `pendingDeletes` before erasing from memory
- `precacheChanOps()` handles legacy `chanOpsBackup` table (restore if chanOps empty, drop either way)
- `SHUTDOWNCommand` and `RELOADCommand` warn instead of blocking on `updateInProgress`
- `OnDetach()` calls `syncToDB(true)` to persist all ops on shutdown
- `mod.openchanfix/doc/chanfix.sql` includes `UNIQUE (channel, account)` in the CREATE TABLE
- `mod.openchanfix/doc/update_chanops_unique.sql` exists with dedup + ALTER TABLE

## Validation Commands

```bash
# Build the module
./configure --enable-modules=openchanfix && make

# Verify no references to old code remain
grep -rn 'prepareUpdate\|ClassUpdateDB\|SQL_UPDATE_TIME\|snapShotStruct' mod.openchanfix/ --include='*.cc' --include='*.h' | grep -v '\.git'

# Verify new code is present
grep -n 'syncToDB\|UpdateGuard\|SQL_SYNC_TIME\|pendingDeletes\|isDirty\|ON CONFLICT' mod.openchanfix/ -r --include='*.cc' --include='*.h'

# Verify schema migration file exists
test -f mod.openchanfix/doc/update_chanops_unique.sql && echo "Migration script exists" || echo "MISSING"

# Verify UNIQUE constraint in reference schema
grep -n 'UNIQUE.*channel.*account' mod.openchanfix/doc/chanfix.sql
```

## Cleanup

N/A

## Notes

- The `dbHandle` class does not support parameterized queries — all SQL is built via string concatenation with `escapeSQLChars()` for escaping. This is consistent with the rest of the codebase.
- The `ENABLE_NEWSCORES` ifdef adds a `bonus` field to `sqlChanOp` but this field is NOT stored in the `chanOps` SQL table — it's computed at runtime. No changes needed for it.
- The `chanOps` table has VARCHAR columns: `channel(200)`, `account(24)`, `last_seen_as(128)`. Ensure `escapeSQLChars()` is applied to all three in the UPSERT query.
- On COMMIT failure in `syncToDB()`, all ops are re-marked dirty. However, `pendingDeletes` has already been cleared. This means if COMMIT fails, pending deletes are lost until `rotateDB()` runs again and re-deletes the same ops from memory (they're already gone from memory, so the SQL rows become orphaned until the next rotation cycle detects them again). This is acceptable — orphaned rows are harmless (they'll have zero points and get cleaned up).
- The old `printResourceStats()` method uses `getrusage()` — it can be preserved as a standalone debug utility if desired, but it's not needed in the sync path.
