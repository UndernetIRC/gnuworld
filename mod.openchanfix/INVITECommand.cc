/**
 * INVITECommand.cc
 *
 * 18/12/2003 - Reed Loden <reed@reedloden.com>
 * Initial Version
 *
 * Invite the user to the console channel
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
 * $Id: INVITECommand.cc,v 1.4 2006/12/09 00:29:18 buzlip01 Exp $
 */

#include "gnuworld_config.h"
#include "Network.h"
#include "responses.h"
#include "StringTokenizer.h"

#include "chanfix.h"

RCSTAG("$Id: INVITECommand.cc,v 1.4 2006/12/09 00:29:18 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void INVITECommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{

StringTokenizer st(Message);

Channel* theChannel = 0;
if (st.size() == 1)
  theChannel = Network->findChannel(bot->consoleChan);
else
  theChannel = Network->findChannel(st[1]);

if (!theChannel) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::cant_find_channel,
                              std::string("Could not find channel %s on the network.")).c_str(),
                                          st[1].c_str());
  return;
}

ChannelUser* theBot = theChannel->findUser(bot->getInstance());
if (!theBot) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::chanfix_not_in_chan,
                              std::string("I am not in %s.")).c_str(),
                                          theChannel->getName().c_str());
  return;
}

ChannelUser* theChannelUser = theChannel->findUser(theClient);
if (theChannelUser) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::already_in_channel,
                              std::string("You are already in %s!")).c_str(),
                                          theChannel->getName().c_str());
  return;
}

bot->Invite(theClient, theChannel);

bot->logAdminMessage("%s (%s) INVITE %s [CODER-CHAN]",
		     theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		     theClient->getRealNickUserHost().c_str(),
		     theChannel->getName().c_str());

bot->logLastComMessage(theClient, Message);

return;
}

} // namespace cf
} // namespace gnuworld
