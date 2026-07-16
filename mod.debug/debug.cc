/**
 * debug.cc
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

#include <algorithm>
#include <memory>
#include <string>

#include "EConfig.h"
#include "misc.h"

#include "debug.h"
#include "debug-commands.h"

namespace gnuworld {

extern "C" {
xClient* _gnuwinit(const std::string& args) { return new debug(args); }
}

debug::debug(const std::string& configFileName) : xClient(configFileName) {
    EConfig conf(configFileName);

    for (auto ptr = conf.Find("permit_user"); ptr != conf.end() && ptr->first == "permit_user";
         ++ptr) {
        allowAccess.push_back(ptr->second);
    }

    RegisterCommand(std::make_unique<USERINFOCommand>(this, "USERINFO", "[-num] <nickname|numeric>"));
    RegisterCommand(std::make_unique<CHANINFOCommand>(this, "CHANINFO", "<#channel>"));
    RegisterCommand(std::make_unique<SERVERSCommand>(this, "SERVERS", ""));
    RegisterCommand(
        std::make_unique<SERVERINFOCommand>(this, "SERVERINFO", "[-num] <servername|mask|numeric>"));
    RegisterCommand(std::make_unique<SHUTDOWNCommand>(this, "SHUTDOWN", "[reason]"));
}

debug::~debug() = default;

bool debug::hasAccess(const std::string& accountName) const {
    return std::ranges::any_of(allowAccess, [&](const std::string& account) {
        return strcasecmp(accountName, account) == 0;
    });
}

} // namespace gnuworld
