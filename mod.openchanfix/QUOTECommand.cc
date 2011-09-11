/**
 * QUOTECommand.cc
 *
 * 18/12/2003 - Reed Loden <reed@reedloden.com>
 * Initial Version
 *
 * Sends raw P10 code to the server
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
 * $Id: QUOTECommand.cc,v 1.4 2006/12/09 00:29:19 buzlip01 Exp $
 */

#include	<string>

#include	"gnuworld_config.h"
#include	"StringTokenizer.h"

#include	"chanfix.h"
#include	"responses.h"

RCSTAG("$Id: QUOTECommand.cc,v 1.4 2006/12/09 00:29:19 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void QUOTECommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
#ifndef ENABLE_QUOTE
return;
#endif

if (!theUser->getNeedOper()) {
  bot->SendTo(theClient,
	bot->getResponse(theUser,
			 language::unknown_command,
			 std::string("Unknown command.")).c_str());
  return;
}

StringTokenizer st(Message);

bot->Write( st.assemble(1) );

bot->logAdminMessage("%s (%s) QUOTE %s",
		     theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		     theClient->getRealNickUserHost().c_str(),
		     st.assemble(1).c_str());

bot->logLastComMessage(theClient, Message);

return;
}

} // namespace cf
} // namespace gnuworld
