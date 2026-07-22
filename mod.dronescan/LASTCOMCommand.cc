/**
 * LASTCOMCommand.cc
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 * Lists the last N commands from the comlog audit table (see
 * dronescan::logCommandMessage). Based on the mod.openchanfix LASTCOM
 * command.
 */

#include <sstream>
#include <cstdlib>

#include "dbHandle.h"
#include "levels.h"
#include "dronescan.h"
#include "dronescanCommands.h"
#include "StringTokenizer.h"
#include "sqlUser.h"

namespace gnuworld {

namespace ds {

void LASTCOMCommand::Exec(const iClient* theClient, const string& Message, const sqlUser* theUser) {
    if (theUser->getAccess() < level::lastcom)
        return;

    StringTokenizer st(Message);

    /* Usage:
     *  LASTCOM (<count>) (<days>)
     */

    unsigned int count = 20;
    unsigned int days = 0;

    if (st.size() >= 2)
        count = atoi(st[1].c_str());

    if (st.size() >= 3) {
        days = atoi(st[2].c_str());
        if (days > 365) {
            bot->Reply(theClient, "You cannot see logs more than one year ago.");
            return;
        }
    }

    dbHandle* cacheCon = bot->getSqlDb();

    std::stringstream theQuery;
    theQuery << "SELECT ts, username, command FROM comlog ";
    if (days > 0)
        theQuery << "WHERE ts > " << (::time(0) - days * 24 * 3600) << ' ';
    theQuery << "ORDER BY ts DESC LIMIT " << count;

    if (!cacheCon->Exec(theQuery.str(), true)) {
        bot->log(ERR, "LASTCOM query failed: %s", cacheCon->ErrorMessage().c_str());
        return;
    }

    const std::string daysSuffix =
        days > 0 ? (" from the last " + std::to_string(days) + " day(s)") : std::string();
    bot->Reply(theClient, "Listing last %u commands%s.", count, daysSuffix.c_str());

    for (unsigned int i = 0; i < cacheCon->Tuples(); ++i) {
        bot->Reply(theClient, "[ %s ago - %s ] %s",
                   bot->Ago(atol(cacheCon->GetValue(i, 0).c_str())), cacheCon->GetValue(i, 1).c_str(),
                   cacheCon->GetValue(i, 2).c_str());
    }

    bot->Reply(theClient, "End of LASTCOM report.");

    return;
} // LASTCOMCommand::Exec(const iClient*, const string&, const sqlUser*)

} // namespace ds
} // namespace gnuworld
