/**
 * USERSCORESCommand.cc
 *
 * 03/06/2006 - Wouter Coekaerts <wouter@coekaerts.be>
 * Initial Version
 *
 * Shows all the channels that <user> has a score in
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
 * $Id: USERSCORESCommand.cc,v 1.3 2010/03/04 04:24:11 hidden1 Exp $
 */

#include <sstream>
#include <vector>

#include "gnuworld_config.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlChanOp.h"
#include "sqlcfUser.h"

RCSTAG("$Id: USERSCORESCommand.cc,v 1.3 2010/03/04 04:24:11 hidden1 Exp $");

namespace gnuworld
{
namespace cf
{

void USERSCORESCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

sqlChanOp* curOp;
std::string firstop;
std::string lastop;
bool foundOne = false;

for (chanfix::sqlChanOpsType::iterator ptr = bot->sqlChanOps.begin();
     ptr != bot->sqlChanOps.end(); ptr++) {
  chanfix::sqlChanOpsType::mapped_type::iterator chanOp = ptr->second.find(st[1]);

  if (chanOp != ptr->second.end()) {
    if (!foundOne) {
      // print header
      foundOne = true;
      bot->SendTo(theClient,
		  bot->getResponse(theUser,
				   language::userscores_header,
				   std::string("Channel Score -- Time first opped / Time last opped")).c_str());
    }

    curOp = chanOp->second;
    firstop = bot->tsToDateTime(curOp->getTimeFirstOpped(), false);
    lastop = bot->tsToDateTime(curOp->getTimeLastOpped(), true);
    bot->SendTo(theClient, "%s %d -- %s / %s", ptr->first.c_str(),
		(curOp->getPoints() + curOp->getBonus()), firstop.c_str(), lastop.c_str());
  }
}

if (!foundOne) {
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			       language::userscores_noscore,
			       std::string("Account %s doesn't have any scores in the database.")).c_str(),
					st[1].c_str());
}

bot->logAdminMessage("%s (%s) USERSCORES %s",
		     theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		     theClient->getRealNickUserHost().c_str(),
		     st[1].c_str());

bot->logLastComMessage(theClient, Message);

return;
}

} // namespace cf
} // namespace gnuworld
