/**
 * USETCommand.cc
 *
 * 08/14/2005 - Reed Loden <reed@reedloden.com>
 * Initial Version
 *
 * Sets user <option> to value <value>
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
 * $Id: USETCommand.cc,v 1.6 2008/01/16 02:03:39 buzlip01 Exp $
 */

#include	<string>

#include	"gnuworld_config.h"
#include	"StringTokenizer.h"

#include	"chanfix.h"
#include	"responses.h"
#include	"sqlcfUser.h"

RCSTAG("$Id: USETCommand.cc,v 1.6 2008/01/16 02:03:39 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void USETCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

std::string option;
std::string value;

if (st.size() == 4) {
  sqlcfUser* targetUser = bot->isAuthed(st[1]);
  option = string_upper(st[2]);
  value = string_upper(st[3]);
  sqlcfUser::flagType requiredFlags;

  if (option == "NEEDOPER") {
    requiredFlags = sqlcfUser::F_USERMANAGER;
    if (!theUser->getFlag(requiredFlags)) {
      bot->SendTo(theClient,
		  bot->getResponse(theUser,
			language::requires_auth_and_flag,
			std::string("This command requires authentication and flag '%c'.")).c_str(),
				    bot->getFlagChar(requiredFlags));
      return;
    }
    if (value == "ON" || value == "YES" || value == "1") {
      targetUser->setNeedOper(true);
      bot->SendTo(theClient, "%s is now required to be an IRC Operator", 
		  targetUser->getUserName().c_str());
      targetUser->commit(bot->getLocalDBHandle());
      return;
    } else if (value == "OFF" || value == "NO" || value == "0") {
      targetUser->setNeedOper(false);
      bot->SendTo(theClient, "%s is now not required to be an IRC Operator", 
		  targetUser->getUserName().c_str());
      targetUser->commit(bot->getLocalDBHandle());
      return;
    } else {
      bot->SendTo(theClient, "Please use USET <username> NEEDOPER <on/off>.");
      return;
    }
  }
}

option = string_upper(st[1]);
value = string_upper(st[2]);

bot->logAdminMessage("%s (%s) USET %s %s",
		     theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		     theClient->getRealNickUserHost().c_str(),
		     option.c_str(), value.c_str());

if (option == "NOTICE") {
  if (value == "ON" || value == "YES" || value == "1") {
    theUser->setNotice(true);
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::send_notices,
                                std::string("I will now send you notices.")).c_str());
    theUser->commit(bot->getLocalDBHandle());
    return;
  } else if (value == "OFF" || value == "NO" || value == "0") {
    theUser->setNotice(false);
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::send_privmsgs,
                                std::string("I will now send you privmsgs.")).c_str());
    theUser->commit(bot->getLocalDBHandle());
    return;
  } else {
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::uset_notice_on_off,
                                std::string("Please use USET NOTICE <on/off>.")).c_str());
    return;
  }
}

if (option == "LANG")
{
  chanfix::languageTableType::iterator ptr = bot->languageTable.find(value);
  if (ptr != bot->languageTable.end()) {
    std::string lang = ptr->second.second;
    theUser->setLanguageId(ptr->second.first);
    theUser->commit(bot->getLocalDBHandle());
    bot->SendTo(theClient,
		bot->getResponse(theUser,
			language::lang_set_to,
			std::string("Language is set to %s.")).c_str(), lang.c_str());
    return;
  }

  bot->SendTo(theClient, "ERROR: Invalid language selection.");
  return;
}

bot->SendTo(theClient,
	    bot->getResponse(theUser,
			     language::setting_doesnt_exist,
			     std::string("This setting does not exist.")).c_str());

bot->logLastComMessage(theClient, Message);

return;
}

} // namespace cf
} // namespace gnuworld
