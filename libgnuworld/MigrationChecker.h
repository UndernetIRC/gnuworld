/**
 * MigrationChecker.h
 * Database schema migration tracking utility for GNUWorld
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __MIGRATIONCHECKER_H
#define __MIGRATIONCHECKER_H

#include <string>
#include <vector>

namespace gnuworld {

// Forward declarations
class dbHandle;
class Logger;

/**
 * MigrationChecker handles database schema migration tracking.
 * It maintains a gnuworld_migrations table to track which SQL migration
 * files have been applied to the database, and checks for unapplied
 * migrations on startup. All timestamps are stored as ISO 8601 format
 * with timezone offset for consistent handling across systems and time zones.
 */
class MigrationChecker {
  public:
    /**
     * Constructor for MigrationChecker.
     *
     * @param moduleName The name of the module (e.g., "cservice", "ccontrol")
     * @param db Pointer to the database connection handle
     * @param logger Pointer to the Logger instance for logging
     * @param migrationsDir Path to the migrations directory (e.g., "./mod.cservice/migrations")
     */
    MigrationChecker(const std::string& moduleName, dbHandle* db, Logger* logger,
                     const std::string& migrationsDir);

    /**
     * Checks for unapplied migrations and applies them automatically.
     * Automatically creates the gnuworld_migrations table if it doesn't exist.
     * For each unapplied migration:
     *   1. Reads the SQL file from disk
     *   2. Executes it against the database
     *   3. Records the successful application in gnuworld_migrations
     * If any migration fails to apply, exits with an error message.
     *
     * @return true if all migrations are applied successfully, false on error
     *         On failure, detailed error messages are logged to logger and stderr
     */
    bool check();

  private:
    std::string moduleName;
    dbHandle* db;
    Logger* logger;
    std::string migrationsDir;

    /**
     * Ensures the gnuworld_migrations table exists in the database.
     * Creates it with baseline columns if it doesn't exist.
     *
     * @return true on success, false on error
     */
    bool ensureMigrationsTableExists();

    /**
     * Scans the migrations directory for migration files.
     * Returns files matching pattern NNN_*.sql, sorted numerically.
     *
     * @return vector of migration filenames (e.g., {"001_initial.sql", "002_add_col.sql"})
     */
    std::vector<std::string> scanMigrationFiles();

    /**
     * Queries the gnuworld_migrations table for this module.
     * Returns the list of migration files that have already been applied.
     *
     * @return vector of applied migration filenames
     */
    std::vector<std::string> getAppliedMigrations();

    /**
     * Applies a list of unapplied migrations to the database.
     * For each migration:
     *   1. Reads the SQL file from disk
     *   2. Executes it against the database
     *   3. Records the successful application via recordMigration()
     * Stops and returns false on the first error.
     *
     * @param unappliedFiles vector of unapplied migration filenames
     * @return true if all migrations applied successfully, false on error
     */
    bool applyMigrations(const std::vector<std::string>& unappliedFiles);

    /**
     * Records a successfully applied migration in the gnuworld_migrations table.
     * Inserts a new row with the module name and migration filename.
     *
     * @param filename the migration filename to record
     * @return true on success, false on error
     */
    bool recordMigration(const std::string& filename);
};

} // namespace gnuworld

#endif // __MIGRATIONCHECKER_H
