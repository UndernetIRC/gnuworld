/**
 * REHASHCommand.cc
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
 * On-demand reload of every database-backed cache (fake clients, users,
 * exceptional channels, and the full spam detection graph), replacing the
 * periodic tidRefreshCaches timer.
 */

#include "levels.h"
#include "dronescan.h"
#include "dronescanCommands.h"
#include "sqlUser.h"

#include <string>

#include "iClient.h"

#include "ELog.h"

namespace gnuworld {

namespace ds {

void REHASHCommand::Exec(const iClient* theClient, const string& /*Message*/,
                         const sqlUser* theUser) {
    if (!theUser || theUser->getAccess() < level::reload) {
        bot->Reply(theClient, "Access denied.");
        return;
    }

    elog << "DS.REHASHCommand> Reloading all caches..." << std::endl;
    bot->Reply(theClient, "Reloading all caches from the database...");

    bot->preloadFakeClientCache();
    bot->preloadUserCache();
    bot->preloadExceptionalChannels();
    bot->refreshSpamCaches();

    bot->Reply(theClient, "Done.");
}

} // namespace ds

} // namespace gnuworld
