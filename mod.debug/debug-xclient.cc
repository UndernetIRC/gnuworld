/**
 * debug-xclient.cc
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

#include "StringTokenizer.h"
#include "misc.h"

#include "debug.h"
#include "debug-commands.h"

namespace gnuworld {

void debug::OnPrivateMessage(iClient* theClient, const std::string& Message, bool) {
    if (!hasAccess(theClient->getAccount())) {
        return;
    }

    StringTokenizer st(Message);
    if (st.empty()) {
        return;
    }

    const auto command = string_upper(st[0]);
    const auto commandHandler = commandMap.find(command);
    if (commandHandler == commandMap.end()) {
        Notice(theClient, "Invalid command: %s", command.c_str());
        return;
    }

    commandHandler->second->Exec(theClient, Message);
}

} // namespace gnuworld
