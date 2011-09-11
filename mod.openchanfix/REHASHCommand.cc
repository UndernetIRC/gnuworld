/**
 * REHASHCommand.cc
 *
 * 07/22/2005 - Reed Loden <reed@reedloden.com>
 * Initial Version
 *
 * Reload the bot's configuration file
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
 * $Id: REHASHCommand.cc,v 1.4 2006/12/09 00:29:19 buzlip01 Exp $
 */

#include	"gnuworld_config.h"
#include	"StringTokenizer.h"

#include	"chanfix.h"
#include	"responses.h"

RCSTAG("$Id: REHASHCommand.cc,v 1.4 2006/12/09 00:29:19 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void REHASHCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

std::string option;
if (st.size() > 1)
  option = string_upper(st[1]);

if (option == "CONFIG" || option.empty()) {
  bot->readConfigFile(bot->getConfigFileName());

  bot->SendTo(theClient,
	bot->getResponse(theUser,
		language::reloaded_conf,
		std::string("Successfully rehashed configuration file.")).c_str());
  bot->logDebugMessage("%s (%s) rehashed the chanfix module.",
		       theUser->getUserName().c_str(),
		       theClient->getRealNickUserHost().c_str());
  return;
}

if (option == "HELP") {
  bot->helpTable.clear();
  bot->loadHelpTable();

  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::reloaded_help,
                              std::string("Successfully reloaded help tables. %i entries in table.", bot->helpTable.size())).c_str());
  bot->logDebugMessage("%s (%s) rehashed the chanfix help tables.",
		       theUser->getUserName().c_str(),
		       theClient->getRealNickUserHost().c_str());
  return;
}

if (option == "TRANSLATIONS") {
  bot->languageTable.clear();
  bot->translationTable.clear();
  bot->loadTranslationTable();

  bot->SendTo(theClient, "Successfully reloaded translation tables. %i entries in table.",
              bot->translationTable.size());
  bot->logDebugMessage("%s (%s) rehashed the chanfix translation tables.",
		       theUser->getUserName().c_str(),
		       theClient->getRealNickUserHost().c_str());
  return;
}

bot->logAdminMessage("%s (%s) REHASH %s",
		     theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		     theClient->getRealNickUserHost().c_str(),
		     !option.empty() ? option.c_str() : "");

bot->logLastComMessage(theClient, Message);

return;
}

} // namespace cf

} // namespace gnuworld
