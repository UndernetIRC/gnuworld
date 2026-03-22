/**
 * DUMPSQLCommand.cc
 *
 * Dumps all in-memory chanOp data as SQL INSERT statements to a file on disk.
 * Intended as a last-resort data recovery mechanism when the database is
 * unreachable and sync has failed repeatedly.
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 */
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

#include "gnuworld_config.h"
#include "StringTokenizer.h"
#include "misc.h"
#include "chanfix.h"
#include "responses.h"
#include "sqlChanOp.h"

namespace gnuworld {
namespace cf {

void DUMPSQLCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message) {
    /* Build timestamped filename */
    time_t now = ::time(0);
    struct tm* tm = ::gmtime(&now);
    char timebuf[64];
    ::strftime(timebuf, sizeof(timebuf), "%Y%m%d-%H%M%S", tm);

    std::string filename = std::string("chanops-dump-") + timebuf + ".sql";

    std::ofstream outFile(filename.c_str(), std::ios::out | std::ios::trunc);
    if (!outFile.is_open()) {
        bot->SendTo(theClient, "ERROR: Could not open file '%s' for writing.",
                    filename.c_str());
        return;
    }

    outFile << "-- chanOps memory dump" << std::endl;
    outFile << "-- Generated: " << timebuf << " UTC" << std::endl;
    outFile << "-- Use: psql -f " << filename << " <dbname>" << std::endl;
    outFile << "-- These are UPSERT statements — safe to run against an existing table." << std::endl;
    outFile << std::endl;
    outFile << "BEGIN;" << std::endl;
    outFile << std::endl;

    int count = 0;
    for (chanfix::sqlChanOpsType::iterator ptr = bot->sqlChanOps.begin();
         ptr != bot->sqlChanOps.end(); ++ptr) {
        for (chanfix::sqlChanOpsType::mapped_type::iterator chanOp = ptr->second.begin();
             chanOp != ptr->second.end(); ++chanOp) {
            sqlChanOp* curOp = chanOp->second;

            outFile << "INSERT INTO chanOps (channel, account, last_seen_as, "
                    << "ts_firstopped, ts_lastopped) VALUES ('"
                    << escapeSQLChars(curOp->getChannel()) << "', '"
                    << escapeSQLChars(curOp->getAccount()) << "', '"
                    << escapeSQLChars(curOp->getLastSeenAs()) << "', "
                    << curOp->getTimeFirstOpped() << ", "
                    << curOp->getTimeLastOpped()
                    << ") ON CONFLICT (channel, account) DO UPDATE SET "
                    << "last_seen_as = EXCLUDED.last_seen_as, "
                    << "ts_firstopped = EXCLUDED.ts_firstopped, "
                    << "ts_lastopped = EXCLUDED.ts_lastopped;" << std::endl;
            for (size_t i = 0; i < curOp->getDaySize(); i++) {
                if (curOp->getDay(i) == 0) continue;
                outFile << "INSERT INTO chanops_daily (channel, account, day, points) VALUES ('"
                        << escapeSQLChars(curOp->getChannel()) << "', '"
                        << escapeSQLChars(curOp->getAccount()) << "', "
                        << i << ", " << curOp->getDay(i)
                        << ") ON CONFLICT (channel, account, day) DO UPDATE SET "
                        << "points = EXCLUDED.points;" << std::endl;
            }

            count++;
        }
    }

    outFile << std::endl;
    outFile << "COMMIT;" << std::endl;
    outFile.close();

    bot->SendTo(theClient, "Dumped %d chanops to '%s'.", count, filename.c_str());

    bot->logAdminMessage("%s (%s) DUMPSQL: %d chanops written to %s",
                         theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
                         theClient->getRealNickUserHost().c_str(),
                         count, filename.c_str());

    bot->logLastComMessage(theClient, Message);
}

} // namespace cf
} // namespace gnuworld
