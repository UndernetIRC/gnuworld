/**
 * WHOSERVERCommand.cc
 *
 * 08/27/2005 - Reed Loden <reed@reedloden.com>
 * Initial Version
 *
 * Shows all users in group <group>
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
 * $Id: WHOGROUPCommand.cc,v 1.2 2006/03/21 23:12:37 buzlip01 Exp $
 */

#include "gnuworld_config.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlUser.h"

RCSTAG("$Id: WHOGROUPCommand.cc,v 1.2 2006/03/21 23:12:37 buzlip01 Exp $");

namespace gnuworld
{

void WHOGROUPCommand::Exec(iClient* theClient, sqlUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

if (st.size() == 1) {
  /* No parameter supplied, so list all groups */
  PgDatabase* cacheCon = bot->theManager->getConnection();

  std::stringstream theQuery;
  theQuery << "SELECT DISTINCT faction FROM users ORDER BY faction ASC";

  if (!cacheCon->ExecTuplesOk(theQuery.str().c_str())) {
    elog	<< "chanfix::WHOGROUPCommand> SQL Error: "
		<< cacheCon->ErrorMessage()
		<< std::endl;
    return;
  }

  // SQL query returned no errors
  unsigned int numGroups = 0;
  bot->SendTo(theClient,
	bot->getResponse(theUser,
		language::whogroup_list_groups,
		std::string("List of all groups:")).c_str());

  for (int i = 0 ; i < cacheCon->Tuples(); i++) {
    bot->SendTo(theClient, cacheCon->GetValue(i, 0));
    numGroups++;
  }

  /* Dispose of our connection instance */
  bot->theManager->removeConnection(cacheCon);

  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::number_of_groups,
			std::string("Number of groups: %d.")).c_str(),
			numGroups);
  
  return;
}
	
/* A serveradmin can only WHOGROUP on his/her own group. */
if (theUser->getFlag(sqlUser::F_SERVERADMIN) &&
    !theUser->getFlag(sqlUser::F_USERMANAGER)) {
  if (string_lower(st[1]) != theUser->getGroup()) {
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::whogroup_your_group,
                                std::string("You can only WHOGROUP on your group.")).c_str());
    return;
  }
}

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::users_with_group,
                            std::string("Users with group %s [username (flags)]:")).c_str(),
                                        st[1].c_str());

std::string groupUsers;
unsigned int numUsersInGroup = 0;
chanfix::usersIterator ptr = bot->usersMap_begin();
while (ptr != bot->usersMap_end()) {
  sqlUser* tmpUser = ptr->second;
  if (tmpUser->getGroup() == string_lower(st[1])) {
    if (numUsersInGroup++ && !groupUsers.empty())
      groupUsers += " ";
    groupUsers += tmpUser->getUserName();
    groupUsers += " (+";
    groupUsers += (tmpUser->getFlags()) ? bot->getFlagsString(tmpUser->getFlags()) : "";
    groupUsers += ")";
    if (groupUsers.size() > 460) {
      bot->SendTo(theClient, "%s", groupUsers.c_str());
      groupUsers.erase();
    }
  }
  ptr++;
}

if (!groupUsers.empty())
  bot->SendTo(theClient, "%s", groupUsers.c_str());

bot->SendTo(theClient,
	    bot->getResponse(theUser,
			language::number_of_users,
			std::string("Number of users: %d.")).c_str(),
			numUsersInGroup);

return;
} //WHOGROUPCommand::Exec
} //namespace gnuworld
