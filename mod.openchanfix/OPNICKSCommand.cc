/**
 * OPNICKSCommand.cc
 *
 * 07/18/2005 - Reed Loden <reed@reedloden.com>
 * Initial Version
 *
 * Shows the nicks of all ops in <channel>
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
 * $Id: OPNICKSCommand.cc,v 1.4 2006/12/09 00:29:19 buzlip01 Exp $
 */

#include "gnuworld_config.h"
#include "Network.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"

RCSTAG("$Id: OPNICKSCommand.cc,v 1.4 2006/12/09 00:29:19 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void OPNICKSCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

Channel* netChan = Network->findChannel(st[1]);
if (!netChan) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::no_such_channel,
                              std::string("No such channel %s.")).c_str(), st[1].c_str());
  return;
}

/* Send list of opped clients. */
bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::opped_clients_on,
                            std::string("Opped clients on channel %s:")).c_str(),
                                        netChan->getName().c_str());

ChannelUser* curUser;
std::string oppedUsers;
unsigned int numOppedUsers = 0;
for (Channel::userIterator ptr = netChan->userList_begin(); ptr != netChan->userList_end(); ptr++) {
  curUser = ptr->second;
  if (curUser->isModeO()) {
    if ((oppedUsers.size() + std::string(curUser->getNickName().c_str()).size() + 1) >= 450) {
      bot->SendTo(theClient, "%s", oppedUsers.c_str());
      oppedUsers.erase(oppedUsers.begin(), oppedUsers.end());
    }
    if (numOppedUsers++ && !oppedUsers.empty())
      oppedUsers += " ";
    oppedUsers += curUser->getNickName();
  }
}

if (numOppedUsers)
  bot->SendTo(theClient, "%s", oppedUsers.c_str());

if (numOppedUsers == 1)
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::one_opped_client,
                              std::string("I see 1 opped client in %s.")).c_str(),
                                          netChan->getName().c_str());
else
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::opped_clients,
                              std::string("I see %u opped clients in %s.")).c_str(),
                                          numOppedUsers, netChan->getName().c_str());

/* Log command */
/* ... */

bot->logAdminMessage("%s (%s) OPNICKS %s",
		     theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		     theClient->getRealNickUserHost().c_str(),
		     netChan->getName().c_str());

bot->logLastComMessage(theClient, Message);

return;
}

} // namespace cf
} // namespace gnuworld
