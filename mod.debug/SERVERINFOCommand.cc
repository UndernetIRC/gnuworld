/**
 * SERVERINFOCommand.cc
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
 */

#include <string>
#include <vector>

#include "Network.h"
#include "StringTokenizer.h"
#include "iServer.h"
#include "match.h"
#include "misc.h"

#include "debug.h"
#include "debug-commands.h"

namespace gnuworld {
namespace {

void dumpServerInfo(debug* bot, const iClient* theClient, iServer* theServer) {
    const iServer* uplink = Network->findServer(theServer->getUplinkIntYY());

    bot->Notice(theClient, "Server: %s", theServer->getName().c_str());
    bot->Notice(theClient, "Description: %s", theServer->getDescription().c_str());
    bot->Notice(theClient, "Numeric: %s (intYY=%u)", theServer->getCharYY().c_str(),
                theServer->getIntYY());
    bot->Notice(theClient, "Uplink: %s (intYY=%u)", uplink ? uplink->getName().c_str() : "(none)",
                theServer->getUplinkIntYY());
    bot->Notice(theClient, "Clients: %zu", Network->countClients(theServer));
    bot->Notice(theClient, "Connected: %ld (%s ago)", theServer->getConnectTime(),
                prettyDuration(theServer->getConnectTime()).c_str());
    bot->Notice(theClient, "Start time: %ld (%s ago)", theServer->getStartTime(),
                prettyDuration(theServer->getStartTime()).c_str());
    bot->Notice(theClient, "Lag: %ld seconds (last update: %ld)", theServer->getLag(),
                theServer->getLastLagTS());
    bot->Notice(theClient, "Flags: 0x%x  hub=%s service=%s ipv6=%s jupe=%s bursting=%s",
                theServer->getFlags(), theServer->isHub() ? "yes" : "no",
                theServer->isService() ? "yes" : "no", theServer->isIPv6() ? "yes" : "no",
                theServer->isJupe() ? "yes" : "no", theServer->isBursting() ? "yes" : "no");
}

} // namespace

void SERVERINFOCommand::Exec(const iClient* theClient, const std::string& Message) {
    StringTokenizer st(Message);
    if (st.size() < 2) {
        Usage(theClient);
        return;
    }

    bool byNumeric = false;
    std::size_t argPos = 1;
    if (!strcasecmp(st[1], "-num")) {
        byNumeric = true;
        argPos = 2;
    }

    if (st.size() <= argPos) {
        Usage(theClient);
        return;
    }

    const std::string& target = st[argPos];

    if (byNumeric) {
        iServer* theServer = Network->findServer(target);
        if (theServer == nullptr) {
            bot->Notice(theClient, "Unable to find server numeric: %s", target.c_str());
            return;
        }
        dumpServerInfo(bot, theClient, theServer);
        return;
    }

    std::vector<iServer*> matches;
    for (const auto& [id, theServer] : Network->servers()) {
        (void)id;
        if (theServer == nullptr) {
            continue;
        }
        // match() returns 0 on a successful match
        if (match(target, theServer->getName()) == 0) {
            matches.push_back(theServer);
        }
    }

    if (matches.empty()) {
        bot->Notice(theClient, "Unable to find server: %s", target.c_str());
        return;
    }

    if (matches.size() > 1) {
        bot->Notice(theClient, "Found %zu servers matching %s:", matches.size(), target.c_str());
    }

    for (std::size_t i = 0; i < matches.size(); ++i) {
        if (matches.size() > 1) {
            bot->Notice(theClient, "--- Match %zu/%zu ---", i + 1, matches.size());
        }
        dumpServerInfo(bot, theClient, matches[i]);
    }
}

} // namespace gnuworld
