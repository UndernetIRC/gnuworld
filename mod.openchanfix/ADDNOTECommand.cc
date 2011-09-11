/**
 * ADDNOTECommand.cc
 *
 * 08/18/2005 - Reed Loden <reed@reedloden.com>
 * Initial Version
 *
 * Adds a note to a channel
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
 * $Id: ADDNOTECommand.cc,v 1.5 2008/01/16 02:03:37 buzlip01 Exp $
 */

#include "gnuworld_config.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlChannel.h"
#include "sqlcfUser.h"

RCSTAG("$Id: ADDNOTECommand.cc,v 1.5 2008/01/16 02:03:37 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{
void ADDNOTECommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

if (st[1][0] != '#') {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::invalid_channel_name,
                              std::string("%s is an invalid channel name.")).c_str(),
                                          st[1].c_str());
  return;
}

sqlChannel* theChan = bot->getChannelRecord(st[1]);
if (!theChan) theChan = bot->newChannelRecord(st[1]);

if (!theChan->useSQL())
  theChan->Insert(bot->getLocalDBHandle());

theChan->addNote(bot->getLocalDBHandle(), sqlChannel::EV_NOTE, theClient, st.assemble(2));

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::note_recorded,
                            std::string("Note recorded for channel %s.")).c_str(),
                                        theChan->getChannel().c_str());

bot->logAdminMessage("%s (%s) ADDNOTE %s %s",
		     theUser ? theUser->getUserName().c_str() : theClient->getNickName().c_str(),
		     theClient->getRealNickUserHost().c_str(),
		     theChan->getChannel().c_str(), st.assemble(2).c_str());

bot->logLastComMessage(theClient, Message);

return;
}
} // namespace cf
} // namespace gnuworld
