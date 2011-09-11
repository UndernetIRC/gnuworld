/**
 * ADDUSERCommand.cc
 *
 * 08/08/2005 - Jimmy Lipham <music0m@alltel.net>
 * Initial Version
 *
 * Adds a new user, without flags, and optionally with this hostmask
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
 * $Id: ADDUSERCommand.cc,v 1.5 2008/01/16 02:03:37 buzlip01 Exp $
 */

#include "gnuworld_config.h"
#include "Network.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlcfUser.h"

RCSTAG("$Id: ADDUSERCommand.cc,v 1.5 2008/01/16 02:03:37 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{
void ADDUSERCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

sqlcfUser* targetUser = bot->isAuthed(st[1]);
if (targetUser) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::user_already_exists,
                              std::string("User %s already exists.")).c_str(), st[1].c_str());
  return;
}

sqlcfUser *newUser = new sqlcfUser(bot->theManager);
assert(newUser != 0);
newUser->setUserName(st[1]);
newUser->setCreated(bot->currentTime());
newUser->setLastSeen(0);
newUser->setLastUpdated(bot->currentTime());
newUser->setLanguageId(1);
newUser->setLastUpdatedBy( std::string( "("
	+ theUser->getUserName()
	+ ") "
	+ theClient->getRealNickUserHost() ) );

/* A user added by a serveradmin automatically has the same group. */
if (theUser->getFlag(sqlcfUser::F_SERVERADMIN) &&
    !theUser->getFlag(sqlcfUser::F_USERMANAGER))
  newUser->setGroup(theUser->getGroup());
else
  newUser->setGroup("undernet.org");

if (!newUser->Insert(bot->getLocalDBHandle())) {
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::error_creating_user,
			std::string("Error creating user %s. (Insertion failed)")).c_str(),
			st[1].c_str());
  return;
}

bot->usersMap[newUser->getUserName()] = newUser;

if (st.size() > 2) {
  if (newUser->addHost(bot->getLocalDBHandle(),st[2].c_str())) {
    bot->SendTo(theClient,
		bot->getResponse(theUser,
				 language::created_user_w_host,
				 std::string("Created user %s (%s).")).c_str(),
				 st[1].c_str(), st[2].c_str());
    bot->logAdminMessage("%s (%s) ADDUSER %s %s",
			 theUser ? theUser->getUserName().c_str() : theClient->getNickName().c_str(),
			 theClient->getRealNickUserHost().c_str(),
			 st[1].c_str(), st[2].c_str());
  } else {
    bot->SendTo(theClient,
		bot->getResponse(theUser,
				language::failed_adding_hostmask,
				std::string("Failed adding hostmask %s to user %s.")).c_str(),
				st[2].c_str(), newUser->getUserName().c_str());
    return;
  }
} else {
  bot->SendTo(theClient,
		bot->getResponse(theUser,
				language::created_user_wo_host,
				std::string("Created user %s.")).c_str(), st[1].c_str());
  bot->logAdminMessage("%s (%s) ADDUSER %s",
		       theUser ? theUser->getUserName().c_str() : theClient->getNickName().c_str(),
		       theClient->getRealNickUserHost().c_str(),
		       st[1].c_str());
}

bot->logLastComMessage(theClient, Message);

return;
} //ADDUSERCommand::Exec
} //namespace cf
} //namespace gnuworld
