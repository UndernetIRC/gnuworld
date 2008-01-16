/**
 * DELUSERCommand.cc
 *
 * 08/08/2005 - Jimmy Lipham <music0m@alltel.net>
 * Initial Version
 *
 * Deletes a user
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
 * $Id: DELUSERCommand.cc,v 1.5 2008/01/16 02:03:37 buzlip01 Exp $
 */

#include "gnuworld_config.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlcfUser.h"

RCSTAG("$Id: DELUSERCommand.cc,v 1.5 2008/01/16 02:03:37 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void DELUSERCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
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

/* Can't delete an owner unless you're an owner. */
if (targetUser->getFlag(sqlcfUser::F_OWNER) && !theUser->getFlag(sqlcfUser::F_OWNER)) {
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::cant_delete_an_owner,
			std::string("You cannot delete an owner unless you're an owner.")).c_str());
  return;
}

/* Can only delete a user manager if you're an owner. */
if (targetUser->getFlag(sqlcfUser::F_USERMANAGER) && !theUser->getFlag(sqlcfUser::F_OWNER)) {
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::cant_delete_manager,
			std::string("You cannot delete a user manager unless you're an owner.")).c_str());
  return;
}

/* A serveradmin can only delete users in his/her own group. */
if (theUser->getFlag(sqlcfUser::F_SERVERADMIN) &&
    !theUser->getFlag(sqlcfUser::F_USERMANAGER)) {
  if (targetUser->getGroup() != theUser->getGroup()) {
    bot->SendTo(theClient,
		bot->getResponse(theUser,
			language::cant_delete_from_diff_group,
			std::string("You cannot delete a user in a different group.")).c_str());
    return;
  }
}

if (targetUser->Delete(bot->getLocalDBHandle())) {
  bot->usersMap.erase(bot->usersMap.find(targetUser->getUserName()));
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::deleted_user,
			std::string("Deleted user %s.")).c_str(), targetUser->getUserName().c_str());
  bot->logAdminMessage("%s (%s) DELUSER %s",
		       theClient->getAccount().c_str(),
		       theClient->getRealNickUserHost().c_str(),
		       targetUser->getUserName().c_str());
  delete targetUser; targetUser = 0;
} else {
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::error_deleting_user,
			std::string("Error deleting user %s.")).c_str(), st[1].c_str());
}

bot->logLastComMessage(theClient, Message);

return;
} //DELUSERCommand::Exec

} //namespace cf
} //namespace gnuworld
