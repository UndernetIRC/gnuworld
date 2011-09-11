/**
 * SETGROUPCommand.cc
 *
 * 08/27/2005 - Reed Loden <reed@reedloden.com>
 * Initial Version
 *
 * Sets the group of the user
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
 * $Id: SETGROUPCommand.cc,v 1.5 2008/01/16 02:03:39 buzlip01 Exp $
 */

#include "gnuworld_config.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlcfUser.h"

RCSTAG("$Id: SETGROUPCommand.cc,v 1.5 2008/01/16 02:03:39 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void SETGROUPCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

sqlcfUser* targetUser = bot->isAuthed(st[1]);
if (!targetUser) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::no_such_user,
                              std::string("No such user %s.")).c_str(), st[1].c_str());
  return;
}

std::string group = string_lower(st[2]);

if (targetUser->getGroup() == group) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::user_already_in_group,
                              std::string("User %s is already in group %s.")).c_str(),
                                          targetUser->getUserName().c_str(), group.c_str());
  return;
}

targetUser->setGroup(group);
targetUser->setLastUpdated(bot->currentTime());
targetUser->setLastUpdatedBy( std::string( "("
	+ theUser->getUserName()
	+ ") "
	+ theClient->getRealNickUserHost() ) );
targetUser->commit(bot->getLocalDBHandle());

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::set_group_for_user,
                            std::string("Set group %s for user %s.")).c_str(),
                                        group.c_str(), targetUser->getUserName().c_str());

bot->logAdminMessage("%s (%s) SETGROUP %s %s",
	    theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
	    theClient->getRealNickUserHost().c_str(),
	    targetUser->getUserName().c_str(),
	    group.c_str());

bot->logLastComMessage(theClient, Message);

return;
} //SETGROUPCommand::Exec
} //namespace cf
} //namespace gnuworld
