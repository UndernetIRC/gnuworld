/**
 * SETCommand.cc
 *
 * 07/23/2005 - Reed Loden <reed@reedloden.com>
 * Initial Version
 *
 * Sets <option> to value <value>
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
 * $Id: SETCommand.cc,v 1.4 2006/12/09 00:29:19 buzlip01 Exp $
 */

#include	<string>

#include	"gnuworld_config.h"
#include	"StringTokenizer.h"

#include	"chanfix.h"
#include	"responses.h"
#include	"Network.h"

RCSTAG("$Id: SETCommand.cc,v 1.4 2006/12/09 00:29:19 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void SETCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

std::string option = string_upper(st[1]);
std::string value = string_upper(st[2]);

/*
 * This code removes underscores from the option string.
 * Please do NOT add underscores to the if statements below, 
 * as it will break the checking code. -reed
 */
std::string::size_type pos;
while ((pos = option.find('_', 0)) != std::string::npos)
  option.erase(pos, 1);

bot->logAdminMessage("%s (%s) SET %s %s",
		     theUser->getUserName().c_str(),
		     theClient->getRealNickUserHost().c_str(),
		     option.c_str(), value.c_str());

if (option == "NUMSERVERS") {
  int setting = atoi(value.c_str());
  if (setting < 0) {
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::num_server_syntax,
                                std::string("Please use SET NUM_SERVERS <integer number>.")).c_str());
    return;
  }

  bot->setNumServers(setting);
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::num_servers_set,
                              std::string("NUM_SERVERS is now %d.")).c_str(), setting);
  bot->logDebugMessage("%s (%s) changed NUM_SERVERS to %d.",
		       theUser->getUserName().c_str(),
		       theClient->getRealNickUserHost().c_str(),
		       setting);
  bot->checkNetwork();
  return;
}

if (option == "ENABLEAUTOFIX") {
  if (value == "ON" || value == "1") {
    bot->setDoAutoFix(true);
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::enabled_autofix,
                                std::string("Enabled autofix.")).c_str());
    bot->logDebugMessage("%s (%s) enabled autofix.",
			 theUser->getUserName().c_str(),
			 theClient->getRealNickUserHost().c_str());
    return;
  } else if (value == "OFF" || value == "0") {
    bot->setDoAutoFix(false);
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::disabled_autofix,
                                std::string("Disabled autofix.")).c_str());
    bot->logDebugMessage("%s (%s) disabled autofix.",
			 theUser->getUserName().c_str(),
			 theClient->getRealNickUserHost().c_str());
    return;
  } else {
    bot->SendTo(theClient,
              bot->getResponse(theUser,
                                language::enable_autofix_syntax,
                                std::string("Please use SET ENABLE_AUTOFIX <on/off>.")).c_str());
    return;
  }
}

if (option == "ENABLECHANFIX") {
  if (value == "ON" || value == "1") {
    bot->setDoChanFix(true);
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::enabled_manual_chanfix,
                                std::string("Enabled manual chanfix.")).c_str());
    bot->logDebugMessage("%s (%s) enabled manual chanfix.",
			 theUser->getUserName().c_str(),
			 theClient->getRealNickUserHost().c_str());
    return;
  } else if (value == "OFF" || value == "0") {
    bot->setDoChanFix(false);
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::disabled_manual_chanfix,
                                std::string("Disabled manual chanfix.")).c_str());
    bot->logDebugMessage("%s (%s) disabled manual chanfix.",
			 theUser->getUserName().c_str(),
			 theClient->getRealNickUserHost().c_str());
    return;
  } else {
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::enable_chanfix_syntax,
                                std::string("Please use SET ENABLE_CHANFIX <on/off>.")).c_str());
    return;
  }
}

if (option == "ENABLECHANNELBLOCKING") {
  if (value == "ON" || value == "1") {
    bot->setDoChanBlocking(true);
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::enabled_chan_blocking,
                                std::string("Enabled channel blocking.")).c_str());
    bot->logDebugMessage("%s (%s) enabled channel blocking.",
			 theUser->getUserName().c_str(),
			 theClient->getRealNickUserHost().c_str());
    return;
  } else if (value == "OFF" || value == "0") {
    bot->setDoChanBlocking(false);
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::disabled_chan_blocking,
                                std::string("Disabled channel blocking.")).c_str());
    bot->logDebugMessage("%s (%s) disabled channel blocking.",
			 theUser->getUserName().c_str(),
			 theClient->getRealNickUserHost().c_str());
    return;
  } else {
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::enable_chan_blocking_syntax,
                                std::string("Please use SET ENABLE_CHANNEL_BLOCKING <on/off>.")).c_str());
    return;
  }
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
