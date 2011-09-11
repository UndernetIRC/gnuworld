/**
 * LISTHOSTSCommand.cc
 *
 * 12/28/2005 - Reed Loden <reed@reedloden.com>
 * Initial Version
 *
 * Shows all the host entries assigned to an oper
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
 * $Id: LISTHOSTSCommand.cc,v 1.4 2006/12/09 00:29:18 buzlip01 Exp $
 */

#include "gnuworld_config.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlcfUser.h"

RCSTAG("$Id: LISTHOSTSCommand.cc,v 1.4 2006/12/09 00:29:18 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void LISTHOSTSCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{	 
StringTokenizer st(Message);

sqlcfUser* targetUser;
if (st.size() == 2)
  targetUser = bot->isAuthed(st[1]);
else
  targetUser = theUser;

if (!targetUser) {
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::no_such_user,
			std::string("No such user %s.")).c_str(),
			st[1].c_str());
  return;
}

sqlcfUser::flagType requiredFlags = sqlcfUser::F_USERMANAGER | sqlcfUser::F_SERVERADMIN;
if ((targetUser != theUser) && !theUser->getFlag(requiredFlags)) {
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::requires_auth_and_flags,
			std::string("This command requires authentication and one of these flags: \"%s\".")).c_str(),
			bot->getFlagsString(requiredFlags).c_str());
  return;
}

/* A serveradmin can only view hosts of users in his/her own group. */
if (theUser->getFlag(sqlcfUser::F_SERVERADMIN) &&
    !theUser->getFlag(sqlcfUser::F_USERMANAGER)) {
  if (targetUser->getGroup() != theUser->getGroup()) {
    bot->SendTo(theClient,
		bot->getResponse(theUser,
			language::cant_view_hosts_diff_group,
			std::string("You cannot view hosts of a user in a different group.")).c_str());
    return;
  }
}

bot->SendTo(theClient,
	    bot->getResponse(theUser,
			language::host_list_header,
			std::string("Host list for %s:")).c_str(),
			targetUser->getUserName().c_str());

sqlcfUser::hostListType sqlHostList = targetUser->getHostList();
if (!sqlHostList.empty()) {
  for (sqlcfUser::hostListType::iterator itr = sqlHostList.begin();
       itr != sqlHostList.end(); ++itr)
    bot->SendTo(theClient, *itr);
} else {
  bot->SendTo(theClient, "None.");
}

bot->SendTo(theClient,
	    bot->getResponse(theUser,
			language::host_list_footer,
			std::string("End of host list.")).c_str());

bot->logAdminMessage("%s (%s) LISTHOSTS %s",
		     theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		     theClient->getRealNickUserHost().c_str(),
		     targetUser->getUserName().c_str());

bot->logLastComMessage(theClient, Message);

return;
} //LISTHOSTSCommand::Exec

} //namespace cf
} //namespace gnuworld
