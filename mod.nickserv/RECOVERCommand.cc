/**
 * RECOVERCommand.cc
 *
 * Allows a user to kill off another user that is
 * using their registered nickname.
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

#include "gnuworld_config.h"
#include "Network.h"

#include "nickserv.h"

namespace gnuworld
{

namespace ns
{

using std::string;

bool RECOVERCommand::Exec(iClient* theClient, const string& )
{
bot->theStats->incStat("NS.CMD.RECOVER");

string authedNick = theClient->getAccount();

iClient* targetClient = Network->findNick(authedNick);

if(!targetClient) {
  bot->Notice(theClient, "Unable to find the nick %s.",
    authedNick.c_str());
  return true;
}

if(theClient == targetClient) {
  bot->Notice(theClient, "Recovering yourself is a bad idea.");
  return true;
}

sqlUser* theUser = bot->isRegistered(authedNick);
if(theUser && !theUser->hasFlag(sqlUser::F_RECOVER)) {
  bot->Notice(theClient, "This user has disabled use of the RECOVER command.");
  return true;
}

bot->Kill(targetClient, "Nick recovered by " + theClient->getNickName());

bot->Notice(theClient, "Nick successfully recovered.");

return true;
}

} // namespace ns

} // namespace gnuworld
