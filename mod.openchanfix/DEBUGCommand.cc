/**
 * DEBUGCommand.cc
 *
 * 01/16/2006 - Reed Loden <reed@reedloden.com>
 * Initial Version
 *
 * Overview command for old ROTATE and UPDATE commands
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
 * $Id: DEBUGCommand.cc,v 1.4 2006/12/09 00:29:18 buzlip01 Exp $
 */

#include	<string>

#include	"gnuworld_config.h"
#include	"StringTokenizer.h"

#include	"chanfix.h"
#include	"responses.h"

RCSTAG("$Id: DEBUGCommand.cc,v 1.4 2006/12/09 00:29:18 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void DEBUGCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
#ifndef CHANFIX_DEBUG
return;
#endif

StringTokenizer st(Message);

std::string option = string_upper(st[1]);

if (option == "ROTATE") {
  bot->logDebugMessage("%s (%s) ordered a manual DB rotation.",
		       theUser ? theUser->getUserName().c_str() : theClient->getNickName().c_str(),
		       theClient->getRealNickUserHost().c_str());
  bot->rotateDB();
  return;
}

if (option == "UPDATE") {
  if (bot->isUpdateRunning()) {
    bot->SendTo(theClient,
		bot->getResponse(theUser,
				language::update_in_progress,
                        	std::string("This command cannot proceed while an update is in progress. Please try again later.")).c_str());
    return;
  }

  if ((st.size() > 2) && string_upper(st[2]) == "THREADED") {
    bot->logDebugMessage("%s (%s) ordered a manual DB update (threaded).",
			 theUser ? theUser->getUserName().c_str() : theClient->getNickName().c_str(),
			 theClient->getRealNickUserHost().c_str());
    bot->prepareUpdate(true);
    return;
  } else {
    bot->logDebugMessage("%s (%s) ordered a manual DB update.",
			 theUser ? theUser->getUserName().c_str() : theClient->getNickName().c_str(),
			 theClient->getRealNickUserHost().c_str());
    bot->prepareUpdate(false);
    return;
  }
}

bot->logLastComMessage(theClient, Message);

}

} // namespace cf
} // namespace gnuworld
