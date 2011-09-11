/**
 * LISTBLOCKEDCommand.cc
 *
 * 03/25/2006 - Jimmy Lipham <music0m@alltel.net>
 * Initial Version
 *
 * Lists all blocked chans that chanfix has stored
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
 * $Id: LISTBLOCKEDCommand.cc,v 1.4 2008/01/16 02:03:37 buzlip01 Exp $
 */

#include "gnuworld_config.h"
#include "Network.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlChannel.h"
#include "sqlcfUser.h"

RCSTAG("$Id: LISTBLOCKEDCommand.cc,v 1.4 2008/01/16 02:03:37 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{
void LISTBLOCKEDCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{

/* Check if channel blocking has been disabled in the config. */
if (!bot->doChanBlocking()) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::channel_blocking_disabled,
                              std::string("Channel blocking is disabled.")).c_str());
  return;
}

/* List blocks */
dbHandle* cacheCon = bot->getLocalDBHandle();

std::stringstream theQuery;
theQuery << "SELECT channel FROM channels WHERE (flags & "
	 << sqlChannel::F_BLOCKED
	 << ") = "
	 << sqlChannel::F_BLOCKED
	 << " ORDER BY channel ASC";

if (!cacheCon->Exec(theQuery.str(),true)) {
  elog	<< "chanfix::LISTBLOCKEDCommand> SQL Error: "
		<< cacheCon->ErrorMessage()
		<< std::endl;
  return;
}

// SQL query returned no errors
unsigned int numBlocks = 0;
std::string strBlocks;
bot->SendTo(theClient,
	bot->getResponse(theUser,
		language::listblocks_blocked_chans,
		std::string("List of all blocked channels:")).c_str());

for (unsigned int i = 0 ; i < cacheCon->Tuples(); i++) {
  strBlocks += cacheCon->GetValue(i, 0);
  strBlocks += " ";
  if (strBlocks.size() >= 410) {
    bot->SendTo(theClient, strBlocks.c_str());
    strBlocks = "";
  }
  numBlocks++;
}

/* Dispose of our connection instance */
//bot->theManager->removeConnection(cacheCon);

if (strBlocks.size())
  bot->SendTo(theClient, strBlocks.c_str());

bot->SendTo(theClient,
	bot->getResponse(theUser,
		language::listblocked_total_blocked,
		std::string("%d channels blocked.")).c_str(),
		numBlocks);

bot->logAdminMessage("%s (%s) LISTBLOCKED",
		     theUser ? theUser->getUserName().c_str() : theClient->getNickName().c_str(),
		     theClient->getRealNickUserHost().c_str());


bot->logLastComMessage(theClient, Message);

return;
}
} // namespace cf
} // namespace gnuworld
