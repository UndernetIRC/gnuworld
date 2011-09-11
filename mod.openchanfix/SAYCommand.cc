/**
 * SAYCommand.cc
 *
 * 08/16/2006 - Jimmy Lipham <music0m@alltel.net>
 * Initial Version
 *
 * Displays <text> in <#channel>
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
 * $Id: SAYCommand.cc,v 1.1 2006/12/09 00:29:19 buzlip01 Exp $
 */

#include	<string>

#include	"gnuworld_config.h"
#include	"StringTokenizer.h"

#include	"chanfix.h"
#include	"responses.h"
#include	"Network.h"


namespace gnuworld
{
namespace cf
{

void SAYCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

std::string option = st[1];
std::string value = st.assemble(2);

bot->logAdminMessage("%s (%s) SAY %s %s",
		     theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		     theClient->getRealNickUserHost().c_str(),
		     option.c_str(), value.c_str());

bot->logLastComMessage(theClient, Message);

Channel* thisChan = Network->findChannel(option);

if (!thisChan)
	bot->SendTo(theClient,
		"The channel %s does not exist on the network.",
		option.c_str());
else
	bot->Message(thisChan,value);

return;
}

} // namespace cf
} // namespace gnuworld
