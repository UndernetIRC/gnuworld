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

    bot->Notice(theClient, "Server: {}", theServer->getName());
    bot->Notice(theClient, "Description: {}", theServer->getDescription());
    bot->Notice(theClient, "Numeric: {} (intYY={})", theServer->getCharYY(), theServer->getIntYY());
    bot->Notice(theClient, "Uplink: {} (intYY={})", uplink ? uplink->getName() : "(none)",
                theServer->getUplinkIntYY());
    bot->Notice(theClient, "Clients: {}", Network->countClients(theServer));
    bot->Notice(theClient, "Connected: {} ({} ago)", theServer->getConnectTime(),
                prettyDuration(theServer->getConnectTime()));
    bot->Notice(theClient, "Start time: {} ({} ago)", theServer->getStartTime(),
                prettyDuration(theServer->getStartTime()));
    bot->Notice(theClient, "Lag: {} seconds (last update: {})", theServer->getLag(),
                theServer->getLastLagTS());
    bot->Notice(theClient, "Flags: {:#x}  hub={} service={} ipv6={} jupe={} bursting={}",
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
            bot->Notice(theClient, "Unable to find server numeric: {}", target);
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
        bot->Notice(theClient, "Unable to find server: {}", target);
        return;
    }

    if (matches.size() > 1) {
        bot->Notice(theClient, "Found {} servers matching {}:", matches.size(), target);
    }

    for (std::size_t i = 0; i < matches.size(); ++i) {
        if (matches.size() > 1) {
            bot->Notice(theClient, "--- Match {}/{} ---", i + 1, matches.size());
        }
        dumpServerInfo(bot, theClient, matches[i]);
    }
}

} // namespace gnuworld
