/**
 * SHUTDOWNCommand.cc
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
 * Allows an administrator to shut down NickServ cleanly.
 */

#include	<sstream>

#include "gnuworld_config.h"
#include "StringTokenizer.h"

#include "levels.h"
#include "logTarget.h"
#include "nickserv.h"
#include "sqlUser.h"

namespace gnuworld {

namespace ns {

bool SHUTDOWNCommand::Exec(iClient* theClient, const string& Message)
{

StringTokenizer st(Message);

if(st.size() < 2) {
  Usage(theClient);
  return true;
}

sqlUser* theUser = bot->isAuthed(theClient);

if(!theUser || (theUser->getLevel() < level::admin::shutdown)) {
  bot->Notice(theClient, "Sorry, you do not have access to this command.");
  return true;
}

bot->theLogger->log(logging::events::E_ERROR, "%s (%s) is asking me to shutdown: %s",
                    theClient->getNickName().c_str(), theUser->getName().c_str(),
                    st.assemble(1).c_str());

std::stringstream toSQuit;
toSQuit << server->getCharYY() << " SQ "
        << server->getName() << " 0 :("
        << theUser->getName() << ") "
        << st.assemble(1);

std::stringstream toQuit;
toQuit  << bot->getCharYYXXX() << " Q :"
        << st.assemble(1);

bot->Write(toQuit);
bot->Write(toSQuit);

return true;
} // SHUTDOWNCommand::Exec(iClient*, const string&)

} // namespace ns

} // namespace gnuworld
