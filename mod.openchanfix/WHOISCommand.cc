/**
 * WHOISCommand.cc
 *
 * 08/07/2005 - Jimmy Lipham <music0m@alltel.net>
 * Initial Version
 *
 * Shows information about this user
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
 * $Id: WHOISCommand.cc,v 1.4 2006/12/09 00:29:19 buzlip01 Exp $
 */

#include "gnuworld_config.h"
#include "Network.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlcfUser.h"

RCSTAG("$Id: WHOISCommand.cc,v 1.4 2006/12/09 00:29:19 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void WHOISCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

bot->logAdminMessage("%s (%s) WHOIS %s",
		     theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		     theClient->getRealNickUserHost().c_str(),
		     st[1].c_str());

if (st[1] == "*") {
  unsigned int numUsers = 0;
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::list_of_all_users,
                              std::string("List of all users:")).c_str());
  chanfix::usersIterator ptr = bot->usersMap_begin();
  while (ptr != bot->usersMap_end()) {
    sqlcfUser* tmpUser = ptr->second;
    bot->SendTo(theClient,
		bot->getResponse(theUser,
			language::user_flags_group,
			std::string("User: %s, Flags: %s, Group: %s")).c_str(),
				tmpUser->getUserName().c_str(),
				(tmpUser->getFlags()) ? std::string("+" + bot->getFlagsString(tmpUser->getFlags())).c_str() : "None",
				tmpUser->getGroup().c_str());
    ptr++;
    numUsers++;
  }
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::number_of_users,
			std::string("Number of users: %d.")).c_str(),
			numUsers);
  return;
}

sqlcfUser* theUser2 = bot->isAuthed(st[1]);

const char* username = st[1].c_str();
if (username[0] == '=') {
  /* Skip the '='. */
  ++username;
  iClient* theClient2 = Network->findNick(username);
  if (theClient2)
    theUser2 = bot->isAuthed(theClient2->getAccount());
}

if (!theUser2) 
{ 
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::no_such_user,
                              std::string("No such user %s.")).c_str(),
			      username);
  return;
}

bot->SendTo(theClient, "*** \002%s\002 ***", theUser2->getUserName().c_str()); 

if (theUser2->getIsSuspended())
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			       language::whois_suspended,
			       std::string("--SUSPENDED--")).c_str());

if (!theUser2->getFlags())
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::whois_flags_none,
                              std::string("Flags: none.")).c_str());
else
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::whois_flags,
                              std::string("Flags: +%s")).c_str(),
                                          bot->getFlagsString(theUser2->getFlags()).c_str());

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::whois_group,
                            std::string("Group: %s")).c_str(),
			    theUser2->getGroup().c_str());

std::string langName = "Unknown";
std::string langCode = "UN";
for( chanfix::languageTableType::iterator ptr = bot->languageTable.begin() ;
  ptr != bot->languageTable.end() ; ++ptr )
  {
	if ((unsigned int)ptr->second.first == theUser2->getLanguageId())
	{
	  langName = ptr->second.second;
	  langCode = ptr->first;
	  break;
	}
}

bot->SendTo(theClient,
	    bot->getResponse(theUser,
			    language::whois_lang,
			    std::string("Language: %s (%s)")).c_str(),
			    langName.c_str(), langCode.c_str());

if (theUser2->getNeedOper())
  bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::whois_needoper_yes,
                            std::string("NeedOper: Yes")).c_str());
else
  bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::whois_needoper_no,
                            std::string("NeedOper: No")).c_str());

if (theUser != theUser2) {
  if (theUser2->getLastSeen() > 0)
    bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::whois_lastused,
			std::string("Last used the service: %s ago")).c_str(),
			bot->prettyDuration(theUser2->getLastSeen()).c_str());
  else
    bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::whois_lastused_never,
			std::string("Last used the service: Never")).c_str());
}

if (st.size() > 2 && string_upper(st[2]) == "-MODIF")
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::whois_modif,
			std::string("Last modified: %s (%s ago)")).c_str(),
			theUser2->getLastUpdatedBy().c_str(),
			bot->prettyDuration(theUser2->getLastUpdated()).c_str());

bot->logLastComMessage(theClient, Message);

return;
} //WHOISCommand::Exec
} //namespace cf
} //namespace gnuworld
