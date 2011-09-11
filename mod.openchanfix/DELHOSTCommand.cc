/**
 * DELHOSTCommand.cc
 *
 * 08/26/2005 - Jimmy Lipham <music0m@alltel.net>
 * Initial Version
 *
 * Deletes this hostmask from the user's list of hostmasks
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
 * $Id: DELHOSTCommand.cc,v 1.5 2008/01/16 02:03:37 buzlip01 Exp $
 */

#include "gnuworld_config.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlcfUser.h"

RCSTAG("$Id: DELHOSTCommand.cc,v 1.5 2008/01/16 02:03:37 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void DELHOSTCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
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

/* Can't delete a host from an owner unless you're an owner. */
if (targetUser->getFlag(sqlcfUser::F_OWNER) && !theUser->getFlag(sqlcfUser::F_OWNER)) {
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::cant_del_host_an_owner,
			std::string("You cannot delete a host from an owner unless you're an owner.")).c_str());
  return;
}

/* Can only delete a host from a user manager if you're an owner. */
if (targetUser->getFlag(sqlcfUser::F_USERMANAGER) && !theUser->getFlag(sqlcfUser::F_OWNER)) {
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::cant_del_host_manager,
			std::string("You cannot delete a host from a user manager unless you're an owner.")).c_str());
  return;
}

/* A serveradmin can only add flags to users on his/her own group. */
if (theUser->getFlag(sqlcfUser::F_SERVERADMIN) &&
    !theUser->getFlag(sqlcfUser::F_USERMANAGER)) {
  if (targetUser->getGroup() != theUser->getGroup()) {
    bot->SendTo(theClient,
		bot->getResponse(theUser,
			language::cant_del_host_diff_group,
			std::string("You cannot delete a host from a user in a different group.")).c_str());
    return;
  }
}

if (!targetUser->hasHost(st[2].c_str())) {
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::user_doesnt_have_host,
			std::string("User %s doesn't have hostmask %s.")).c_str(),
				targetUser->getUserName().c_str(), st[2].c_str());
  return;
}

if (!targetUser->delHost(bot->getLocalDBHandle(),st[2].c_str())) {
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::failed_deleting_host,
			std::string("Failed deleting hostmask %s from user %s.")).c_str(),
			st[2].c_str(), targetUser->getUserName().c_str());
  return;
}

targetUser->setLastUpdated(bot->currentTime());
targetUser->setLastUpdatedBy( std::string( "("
	+ theUser->getUserName()
	+ ") "
	+ theClient->getRealNickUserHost() ) );
targetUser->commit(bot->getLocalDBHandle());

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::deleted_hostmask,
                            std::string("Deleted hostmask %s from user %s.")).c_str(),
                                        st[2].c_str(), targetUser->getUserName().c_str());

bot->logAdminMessage("%s (%s) DELHOST %s %s",
		     theUser ? theUser->getUserName().c_str() : theClient->getNickName().c_str(),
		     theClient->getRealNickUserHost().c_str(),
		     targetUser->getUserName().c_str(), st[2].c_str());

bot->logLastComMessage(theClient, Message);

return;
}

} //namespace cf
} //namespace gnuworld
