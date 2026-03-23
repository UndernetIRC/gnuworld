/**
 * MigrationChecker.cc
 * Database schema migration tracking utility for GNUWorld
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <regex>

#include "MigrationChecker.h"
#include "dbHandle.h"
#include "logger.h"
#include "ELog.h"

namespace gnuworld {

MigrationChecker::MigrationChecker(const std::string& moduleName, dbHandle* db, Logger* logger,
                                   const std::string& migrationsDir)
    : moduleName(moduleName), db(db), logger(logger), migrationsDir(migrationsDir) {}

bool MigrationChecker::check() {
    if (!db) {
        std::string msg = "ERROR [MigrationChecker]: Database handle is null";
        if (logger) logger->write(ERROR) << msg << std::endl;
        return false;
    }

    // Ensure the gnuworld_migrations table exists
    if (!ensureMigrationsTableExists()) {
        if (logger) logger->write(ERROR) << "Failed to create/access gnuworld_migrations table" << std::endl;
        return false;
    }

    // Scan for migration files
    auto onDisk = scanMigrationFiles();
    if (onDisk.empty()) {
        std::string msg = "*** [MigrationChecker]: Module '" + moduleName
                          + "' has no migrations to check";
        if (logger) logger->write(INFO) << msg << std::endl;
        return true;
    }

    // Get list of already-applied migrations
    auto applied = getAppliedMigrations();

    // Find unapplied migrations
    std::vector<std::string> unapplied;
    for (const auto& file : onDisk) {
        if (std::find(applied.begin(), applied.end(), file) == applied.end()) {
            unapplied.push_back(file);
        }
    }

    if (!unapplied.empty()) {
        std::string msg = "*** [MigrationChecker]: Module '" + moduleName
                          + "' has " + std::to_string(unapplied.size()) + " unapplied migration(s). Applying now...";
        if (logger) logger->write(INFO) << msg << std::endl;

        // Apply the unapplied migrations
        if (!applyMigrations(unapplied)) {
            return false;  // Error already logged by applyMigrations
        }
    }

    std::string successMsg = "*** [MigrationChecker]: Module '" + moduleName
                             + "' migrations verified - all applied";
    if (logger) logger->write(INFO) << successMsg << std::endl;
    return true;
}

bool MigrationChecker::ensureMigrationsTableExists() {
    // Check if gnuworld_migrations table already exists
    const char* checkTableSQL = R"(
        SELECT EXISTS (
            SELECT 1 FROM information_schema.tables
            WHERE table_name = 'gnuworld_migrations'
        );
    )";

    if (!db->Exec(checkTableSQL, true)) {
        std::string msg = "ERROR [MigrationChecker]: Failed to check for gnuworld_migrations table";
        if (logger) logger->write(ERROR) << msg << std::endl;
        return false;
    }

    // Check if table exists (GetValue returns "t" for true in PostgreSQL)
    std::string exists = db->GetValue(0, 0);
    if (exists == "t" || exists == "true") {
        return true;  // Table already exists
    }

    // Table doesn't exist, create it
    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS gnuworld_migrations (
            id SERIAL PRIMARY KEY,
            module VARCHAR(50) NOT NULL,
            file VARCHAR(255) NOT NULL,
            applied_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
            UNIQUE(module, file)
        );
    )";

    if (!db->Exec(createTableSQL)) {
        std::string msg = "ERROR [MigrationChecker]: Failed to create gnuworld_migrations table: " +
                          db->ErrorMessage();
        if (logger) logger->write(ERROR) << msg << std::endl;
        return false;
    }

    if (logger) logger->write(INFO) << "Created gnuworld_migrations table for module '"
                                    << moduleName << "'" << std::endl;
    return true;
}

std::vector<std::string> MigrationChecker::scanMigrationFiles() {
    std::vector<std::string> files;

    try {
        if (!std::filesystem::exists(migrationsDir)) {
            // Directory doesn't exist yet, which is fine
            return files;
        }

        // Pattern: NNN_*.sql where NNN are digits
        std::regex migrationPattern(R"(^(\d{3})_.*\.sql$)");

        for (const auto& entry : std::filesystem::directory_iterator(migrationsDir)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (std::regex_match(filename, migrationPattern)) {
                    files.push_back(filename);
                }
            }
        }

        // Sort numerically by the NNN prefix
        std::sort(files.begin(), files.end(),
                  [](const std::string& a, const std::string& b) {
                      int aNum = std::stoi(a.substr(0, 3));
                      int bNum = std::stoi(b.substr(0, 3));
                      return aNum < bNum;
                  });

    } catch (const std::exception& e) {
        elog << "ERROR [MigrationChecker]: Failed to scan migrations directory '" << migrationsDir
             << "': " << e.what() << std::endl;
    }

    return files;
}

std::vector<std::string> MigrationChecker::getAppliedMigrations() {
    std::vector<std::string> applied;

    std::stringstream query;
    query << "SELECT file FROM gnuworld_migrations WHERE module = '"
          << moduleName << "' ORDER BY id;";

    if (!db->Exec(query.str(), true)) {
        if (logger) logger->write(WARN) << "Failed to query applied migrations for '"
                                        << moduleName << "': " << db->ErrorMessage() << std::endl;
        return applied;
    }

    // Parse results
    unsigned int numRows = db->countTuples();
    for (unsigned int i = 0; i < numRows; ++i) {
        std::string file = db->GetValue(i, 0);
        if (!file.empty()) {
            applied.push_back(file);
        }
    }

    return applied;
}

bool MigrationChecker::applyMigrations(const std::vector<std::string>& unappliedFiles) {
    for (const auto& filename : unappliedFiles) {
        // Build the full path to the migration file
        std::filesystem::path migrationPath = std::filesystem::path(migrationsDir) / filename;

        // Read the SQL file
        std::ifstream sqlFile(migrationPath);
        if (!sqlFile.is_open()) {
            std::string msg = "ERROR [MigrationChecker]: Failed to open migration file '" + filename + "' at " + migrationPath.string();
            if (logger) logger->write(ERROR) << msg << std::endl;
            return false;
        }

        // Read the entire file content
        std::stringstream sqlContent;
        sqlContent << sqlFile.rdbuf();
        sqlFile.close();

        // Execute the migration SQL
        if (!db->Exec(sqlContent.str())) {
            std::string msg = "ERROR [MigrationChecker]: Failed to apply migration '" + filename + "' for module '" + moduleName + "': " + db->ErrorMessage();
            if (logger) logger->write(ERROR) << msg << std::endl;
            return false;
        }

        // Record the migration as applied
        if (!recordMigration(filename)) {
            return false;  // Error already logged by recordMigration
        }

        std::string msg = "*** [MigrationChecker]: Successfully applied migration '" + filename + "' to module '" + moduleName + "'";
        if (logger) logger->write(INFO) << msg << std::endl;
    }

    return true;
}

bool MigrationChecker::recordMigration(const std::string& filename) {
    std::stringstream insertQuery;
    insertQuery << "INSERT INTO gnuworld_migrations (module, file) VALUES ('"
                << moduleName << "', '" << filename << "');";

    if (!db->Exec(insertQuery.str())) {
        std::string msg = "ERROR [MigrationChecker]: Failed to record migration '" + filename + "' in database: " + db->ErrorMessage();
        if (logger) logger->write(ERROR) << msg << std::endl;
        return false;
    }

    return true;
}

} // namespace gnuworld
