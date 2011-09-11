/**
 * SUSPENDCommand.cc
 *
 * 08/29/2005 - Reed Loden <reed@reedloden.com>
 * Initial Version
 *
 * Suspends a user indefinitely
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
 * $Id: SUSPENDCommand.cc,v 1.5 2008/01/16 02:03:39 buzlip01 Exp $
 */

#include "gnuworld_config.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlcfUser.h"

RCSTAG("$Id: SUSPENDCommand.cc,v 1.5 2008/01/16 02:03:39 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void SUSPENDCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
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

if (theUser == targetUser) {
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::user_cant_suspend_self,
			std::string("Suspending yourself is not a very wise thing to do.")).c_str());
  return;
}

/* Can't suspend an owner unless you're an owner. */
if (targetUser->getFlag(sqlcfUser::F_OWNER) && !theUser->getFlag(sqlcfUser::F_OWNER)) {
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::cant_suspend_an_owner,
			std::string("You cannot suspend an owner unless you're an owner.")).c_str());
  return;
}

/* Can only suspend a user manager if you're an owner. */
if (targetUser->getFlag(sqlcfUser::F_USERMANAGER) && !theUser->getFlag(sqlcfUser::F_OWNER)) {
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::cant_suspend_manager,
			std::string("You cannot suspend a user manager unless you're an owner.")).c_str());
  return;
}


/* A serveradmin can only suspend users in his/her own group. */
if (theUser->getFlag(sqlcfUser::F_SERVERADMIN) &&
    !theUser->getFlag(sqlcfUser::F_USERMANAGER)) {
  if (targetUser->getGroup() != theUser->getGroup()) {
    bot->SendTo(theClient,
		bot->getResponse(theUser,
			language::cant_suspend_diff_group,
			std::string("You cannot suspend a user in a different group.")).c_str());
    return;
  }
}

if (targetUser->getIsSuspended()) {
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::user_already_suspended,
			std::string("User %s is already suspended.")).c_str(),
				targetUser->getUserName().c_str());
  return;
}

targetUser->setSuspended(true);
targetUser->setLastUpdated(bot->currentTime());
targetUser->setLastUpdatedBy( std::string( "("
	+ theUser->getUserName()
	+ ") "
	+ theClient->getRealNickUserHost() ) );
targetUser->commit(bot->getLocalDBHandle());

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::user_suspended,
                            std::string("Suspended user %s indefinitely.")).c_str(),
                                        targetUser->getUserName().c_str());

bot->logAdminMessage("%s (%s) SUSPEND %s",
		     theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		     theClient->getRealNickUserHost().c_str(),
		     targetUser->getUserName().c_str());

bot->logLastComMessage(theClient, Message);

return;
} //SUSPENDCommand::Exec
} //Namespace cf
} //Namespace gnuworld
