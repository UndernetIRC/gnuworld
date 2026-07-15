/**
 * debug-commands.cc
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

#include <utility>

#include "debug.h"
#include "debug-commands.h"

namespace gnuworld {

bool debug::RegisterCommand(std::unique_ptr<Command> theCommand) {
    const std::string name = theCommand->getName();
    return commandMap.emplace(name, std::move(theCommand)).second;
}

void Command::Usage(const iClient* theClient) {
    bot->Notice(theClient, "SYNTAX: %s - %s", getName().c_str(), getHelp().c_str());
}

} // namespace gnuworld
