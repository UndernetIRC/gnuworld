/**
 * RELOADCommand.cc
 *
 * 18/12/2003 - Reed Loden <reed@reedloden.com>
 * Initial Version
 *
 * Unloads the bot's module and reloads it
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 *
 * $Id: RELOADCommand.cc,v 1.5 2006/12/09 00:29:19 buzlip01 Exp $
 */

#include	<string>

#include	"gnuworld_config.h"
#include	"StringTokenizer.h"

#include	"chanfix.h"
#include	"responses.h"

RCSTAG("$Id: RELOADCommand.cc,v 1.5 2006/12/09 00:29:19 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void RELOADCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);
	
if (bot->isUpdateRunning()) {
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::update_in_progress,
			std::string("This command cannot proceed while an update is in progress. Please try again later.")).c_str());
  return;
}

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::reloading_client,
                            std::string("Reloading client...see you on the flip side")).c_str());

bot->logDebugMessage("%s (%s) is reloading the chanfix module.",
		     theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		     theClient->getRealNickUserHost().c_str());

bot->logAdminMessage("%s (%s) RELOAD %s",
		     theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		     theClient->getRealNickUserHost().c_str(),
		     (st.size() < 2) ? "" : st.assemble(1).c_str());


if (st.size() < 2)
  server->UnloadClient(bot, "Reloading...");
else
  server->UnloadClient(bot, st.assemble(1));

server->LoadClient("libchanfix", bot->getConfigFileName());

bot->logLastComMessage(theClient, Message);

return;
}

} // namespace cf

} // namespace gnuworld
