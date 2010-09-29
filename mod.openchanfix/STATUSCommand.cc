/**
 * STATUSCommand.cc
 *
 * 04/01/2004 - Reed Loden <reed@reedloden.com>
 * Initial Version
 *
 * Display the status of chanfix
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
 * $Id: STATUSCommand.cc,v 1.7 2010/03/04 04:24:11 hidden1 Exp $
 */

#include	<string>
#include	<sys/resource.h>

#include	"gnuworld_config.h"
#include	"StringTokenizer.h"

#include	"chanfix.h"
#include	"defs.h"
#include	"responses.h"
#include	"Network.h"

RCSTAG("$Id: STATUSCommand.cc,v 1.7 2010/03/04 04:24:11 hidden1 Exp $");

namespace gnuworld
{
namespace cf
{

void STATUSCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{

bot->SendTo(theClient, "[Evilnet Development -- mod.openchanfix v%s]",
	    CHANFIX_VERSION);
bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::status_uptime,
                            std::string("Uptime: \002%s\002")).c_str(),
                                        bot->prettyDuration(bot->getUplink()->getStartTime()).c_str());
bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::status_auto_fixing,
                            std::string("Automatic fixing is: \002%s\002")).c_str(),
                                        bot->doAutoFix() ? "ON" : "OFF");
bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::status_manual_fixing,
                            std::string("Manual fixing is: \002%s\002")).c_str(),
                                        bot->doChanFix() ? "ON" : "OFF");
bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::status_chan_blocking,
                            std::string("Channel blocking is: \002%s\002")).c_str(),
                                        bot->doChanBlocking() ? "ON" : "OFF");
bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::status_servers_amount,
                            std::string("Required amount of servers linked is %u%% of %u, which is a minimum of %u servers.")).c_str(),
                                        bot->getMinServersPresent(), bot->getNumServers(),
                                       ((bot->getMinServersPresent() * bot->getNumServers()) / 100 + 1));
if (bot->getState() == chanfix::SPLIT)
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::status_splitmode_enabled,
                              std::string("Splitmode enabled: only %i servers linked.")).c_str(),
                                          Network->serverList_size());
else
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::status_splitmode_disabled,
                              std::string("Splitmode disabled. There are %i servers linked.")).c_str(),
                                          Network->serverList_size());
if (bot->isChanServLinked())
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::status_channel_service_linked,
			std::string("Channel service linked. New channels will be scored.")).c_str());
else
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			language::status_channel_service_not_linked,
			std::string("Channel service not linked. New channels will not be scored.")).c_str());

bot->SendTo(theClient,
	std::string("Number of channels being autofixed: %i").c_str(),bot->countAutoFixes());

bot->SendTo(theClient,
	std::string("Number of channels being manually fixed: %i").c_str(),bot->countManFixes());

int who = RUSAGE_SELF;
struct rusage usage;
int ret;
ret = getrusage(who, &usage);
bot->SendTo(theClient,
	std::string("Memory Usage (kB): %ld").c_str(), usage.ru_maxrss);


bot->logAdminMessage("%s (%s) STATUS",
		     theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		     theClient->getRealNickUserHost().c_str());

bot->logLastComMessage(theClient, Message);

return;
}

} // namespace cf
} // namespace gnuworld
