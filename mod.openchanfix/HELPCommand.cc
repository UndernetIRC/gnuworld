/**
 * HELPCommand.cc
 *
 * 08/08/2005 - Jimmy Lipham <music0m@alltel.net>
 * Initial Version
 *
 * Shows help about <command> or gives a general list of commands
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
 * $Id: HELPCommand.cc,v 1.4 2006/12/09 00:29:18 buzlip01 Exp $
 */

#include "gnuworld_config.h"

#include "chanfix.h"
#include "chanfixCommands.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlcfUser.h"

RCSTAG("$Id: HELPCommand.cc,v 1.4 2006/12/09 00:29:18 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void HELPCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

if (st.size() < 2) {
  if (bot->isAllowingTopFix())
    bot->SendTo(theClient, bot->getHelpMessage(theUser, "<INDEXNORMAL>"));

  if (theClient->isOper())
    bot->SendTo(theClient, bot->getHelpMessage(theUser, "<INDEXOPER>"));

  if (!theUser)
    return;

  bot->SendTo(theClient, bot->getHelpMessage(theUser, "<INDEXLOGGEDIN>"));

  if (theUser->getFlag(sqlcfUser::F_SERVERADMIN))
    bot->SendTo(theClient, bot->getHelpMessage(theUser, "<INDEXSERVERADMIN>"));

  if (theUser->getFlag(sqlcfUser::F_BLOCK))
    bot->SendTo(theClient, bot->getHelpMessage(theUser, "<INDEXBLOCK>"));

  if (theUser->getFlag(sqlcfUser::F_COMMENT))
    bot->SendTo(theClient, bot->getHelpMessage(theUser, "<INDEXCOMMENT>"));

  if (theUser->getFlag(sqlcfUser::F_CHANFIX))
    bot->SendTo(theClient, bot->getHelpMessage(theUser, "<INDEXCHANFIX>"));

  if (theUser->getFlag(sqlcfUser::F_OWNER))
    bot->SendTo(theClient, bot->getHelpMessage(theUser, "<INDEXOWNER>"));

  if (theUser->getFlag(sqlcfUser::F_USERMANAGER))
    bot->SendTo(theClient, bot->getHelpMessage(theUser, "<INDEXUSERADMIN>"));

} else {
  std::string msg = bot->getHelpMessage(theUser, string_upper(st.assemble(1)));

  if (!msg.empty()) {
    bool real = true;
    const std::string Command = string_upper(st[1]);

    chanfix::commandMapType::iterator commHandler = bot->commandMap.find(Command);
    if (commHandler == bot->commandMap.end())
      real = false;

    bot->SendTo(theClient, "\002%s\002", real ? commHandler->second->getInfo().c_str() : Command.c_str());

    bot->SendFmtTo(theClient, msg);

    if (real) {
      /* If you change this code, remember to change it in chanfix.cc */
      sqlcfUser::flagType requiredFlags = commHandler->second->getRequiredFlags();
      if (requiredFlags) {
	if (requiredFlags == sqlcfUser::F_LOGGEDIN)
	  bot->SendTo(theClient, "This command requires authentication.");
	else {
	  if (bot->getFlagChar(requiredFlags) != ' ')
	    bot->SendTo(theClient,
			bot->getResponse(theUser,
				language::requires_auth_and_flag,
				std::string("This command requires authentication and flag '%c'.")).c_str(),
					bot->getFlagChar(requiredFlags));
	  else
	    bot->SendTo(theClient,
			bot->getResponse(theUser,
				language::requires_auth_and_flags,
				std::string("This command requires authentication and one of these flags: \"%s\".")).c_str(),
					bot->getFlagsString(requiredFlags).c_str());
	}
      }
    }
  } else
    bot->SendTo(theClient, "There is no help available for that topic.");

}

} //HELPCommand::Exec

} //namespace cf
} //namespace gnuworld
