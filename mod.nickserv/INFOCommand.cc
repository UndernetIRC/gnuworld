/**
 * INFOCommand.cc
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
 * Allows an administrator to see what attributes a registered user has.
 */

#include "gnuworld_config.h"
#include "StringTokenizer.h"

#include "levels.h"
#include "nickserv.h"
#include "responses.h"

namespace gnuworld {

namespace ns {

bool INFOCommand::Exec(iClient* theClient, const string& Message)
{
bot->theStats->incStat("NS.CMD.INFO");

sqlUser* theUser = bot->isAuthed(theClient);

if(!theUser || (theUser->getLevel() < level::admin::info)) {
  bot->Notice(theClient, responses::noAccess);
  return true;
}

StringTokenizer st(Message);

// INFO nick
if(st.size() != 2) {
  Usage(theClient);
  return true;
}

sqlUser* targetUser = bot->isRegistered(st[1]);

if(!targetUser) {
  bot->Notice(theClient, responses::noSuchUser);
  return true;
}

bot->Notice(theClient, "Nick: %s", targetUser->getName().c_str());
bot->Notice(theClient, "Level: %u; Flags: %u; LogMask: %u",
  targetUser->getLevel(), targetUser->getFlags(), targetUser->getLogMask());

return true;
}

} // namespace ns

} // namespace gnuworld
