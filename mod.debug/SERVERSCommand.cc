/**
 * SERVERSCommand.cc
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

#include "Network.h"
#include "iServer.h"

#include "debug.h"
#include "debug-commands.h"

namespace gnuworld {

void SERVERSCommand::Exec(const iClient* theClient, const std::string&) {
    unsigned int totalServers = 0;

    bot->Notice(theClient, "Network servers:");
    for (const auto& [id, theServer] : Network->servers()) {
        (void)id;
        if (theServer == nullptr) {
            continue;
        }

        std::string flags;
        if (theServer->isHub())
            flags += 'H';
        if (theServer->isService())
            flags += 'S';
        if (theServer->isIPv6())
            flags += '6';
        if (theServer->isJupe())
            flags += 'J';
        if (theServer->isBursting())
            flags += 'B';
        if (flags.empty())
            flags = "-";

        const iServer* uplink = Network->findServer(theServer->getUplinkIntYY());

        bot->Notice(theClient, "  %s numeric=%s clients=%zu flags=%s uplink=%s",
                    theServer->getName().c_str(), theServer->getCharYY().c_str(),
                    Network->countClients(theServer), flags.c_str(),
                    uplink ? uplink->getName().c_str() : "(none)");
        ++totalServers;
    }

    bot->Notice(theClient, "Total servers: %u", totalServers);
}

} // namespace gnuworld
