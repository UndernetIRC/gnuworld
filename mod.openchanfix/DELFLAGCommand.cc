/**
 * DELFLAGCommand.cc
 *
 * 08/08/2005 - Jimmy Lipham <music0m@alltel.net>
 * Initial Version
 *
 * Removes this flag from the user
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
 * $Id: DELFLAGCommand.cc,v 1.5 2008/01/16 02:03:37 buzlip01 Exp $
 */

#include "gnuworld_config.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlcfUser.h"

RCSTAG("$Id: DELFLAGCommand.cc,v 1.5 2008/01/16 02:03:37 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void DELFLAGCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

char flag = st[2][0];
if (st[2].size() > 1) {
  if (flag == '-')
    flag = st[2][1];
  else {
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::one_flag_per_delflag,
                                std::string("You may only remove one flag per DELFLAG command.")).c_str());
    return;
  }
}

if (!bot->getFlagType(flag)) {
  Usage(theClient);
  return;
}

sqlcfUser* targetUser = bot->isAuthed(st[1]);
if (!targetUser) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::no_such_user,
                              std::string("No such user %s.")).c_str(), st[1].c_str());
  return;
}

if (flag == bot->getFlagChar(sqlcfUser::F_OWNER) &&
    !theUser->getFlag(sqlcfUser::F_OWNER)) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::only_owner_del_owner_flag,
                              std::string("Only an owner can delete the owner flag.")).c_str());
  return;
}

if (flag == bot->getFlagChar(sqlcfUser::F_USERMANAGER) &&
    !theUser->getFlag(sqlcfUser::F_OWNER)) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::only_owner_del_user_flag,
                              std::string("Only an owner can delete the user management flag.")).c_str());
  return;
}

/* A serveradmin can only delete flags from users on his/her own group. */
if (theUser->getFlag(sqlcfUser::F_SERVERADMIN) &&
    !theUser->getFlag(sqlcfUser::F_USERMANAGER)) {
  if (targetUser->getGroup() != theUser->getGroup()) {
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::cant_delete_flag_diff_group,
                                std::string("You cannot delete a flag from a user in a different group.")).c_str());
    return;
  }
  if (flag == bot->getFlagChar(sqlcfUser::F_BLOCK)) {
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::cant_remove_block_flag,
                                std::string("You cannot remove a block flag.")).c_str());
    return;
  }
  if (flag == bot->getFlagChar(sqlcfUser::F_SERVERADMIN)) {
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::cant_remove_server_flag,
                                std::string("You cannot remove a serveradmin flag.")).c_str());
    return;
  }
}

if (!targetUser->getFlag(bot->getFlagType(flag))) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::user_does_not_have_flag,
                              std::string("User %s does not have flag '%c'.")).c_str(),
                                          targetUser->getUserName().c_str(), flag);
  return;
}

targetUser->removeFlag(bot->getFlagType(flag));
targetUser->setLastUpdated(bot->currentTime());
targetUser->setLastUpdatedBy( std::string( "("
	+ theUser->getUserName()
	+ ") "
	+ theClient->getRealNickUserHost() ) );
targetUser->commit(bot->getLocalDBHandle());
bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::deleted_flag,
                            std::string("Deleted flag '%c' from user %s.")).c_str(), flag,
                                        targetUser->getUserName().c_str());

bot->logAdminMessage("%s (%s) DELFLAG %s %c",
		     theUser ? theUser->getUserName().c_str() : theClient->getNickName().c_str(),
		     theClient->getRealNickUserHost().c_str(),
		     targetUser->getUserName().c_str(), flag);

bot->logLastComMessage(theClient, Message);

} //DELFLAGCommand::Exec

} //Namespace cf
} //Namespace gnuworld
