/**
 * WHOFLAGCommand.cc
 *
 * 12/21/2006 - Jimmy Lipham <music0m@alltel.net>
 * Initial Version
 *
 * Shows the number of opers/users with <flag>
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
 * $Id: WHOFLAGCommand.cc,v 1.1 2006/12/22 03:11:44 buzlip01 Exp $
 */
 
#include "gnuworld_config.h"

#include "chanfix.h"
#include "Network.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlcfUser.h"

RCSTAG("");

namespace gnuworld
{
namespace cf
{

void WHOFLAGCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);
	
char flag = st[1][0];
if (st[1].size() > 1) {
  if (flag == '+')
    flag = st[1][1];
  else {
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::one_flag_per_whoflag,
                                std::string("You may only WHOFLAG one flag.")).c_str());   
    return;
  }
}

if (!bot->getFlagType(flag)) {
  Usage(theClient);
  return;
}

chanfix::usersIterator ptr = bot->usersMap_begin();
while (ptr != bot->usersMap_end()) {
  sqlcfUser* tmpUser = ptr->second;
  if (tmpUser->getFlag(bot->getFlagType(flag)))
    bot->SendTo(theClient,
		std::string("USER: %s   FLAGS: %s   GROUP: %s").c_str(),
			tmpUser->getUserName().c_str(),
			(tmpUser->getFlags()) ? std::string("+" + bot->getFlagsString(tmpUser->getFlags())).c_str() : "None",
			tmpUser->getGroup().c_str());
  ptr++;
}

bot->logAdminMessage("%s (%s) WHOFLAG %s",
		     theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		     theClient->getRealNickUserHost().c_str(), 
		     st[1].c_str());

bot->logLastComMessage(theClient, Message);

return;
}

} //namespace cf
} //namespace gnuworld
