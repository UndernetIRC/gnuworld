/**
 * ALERTCommand.cc
 *
 * 07/19/2005 - Reed Loden <reed@reedloden.com>
 * Initial Version
 *
 * Sets the ALERT flag of this channel
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
 * $Id: ALERTCommand.cc,v 1.5 2008/01/16 02:03:37 buzlip01 Exp $
 */

#include "gnuworld_config.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlChannel.h"
#include "sqlcfUser.h"

RCSTAG("$Id: ALERTCommand.cc,v 1.5 2008/01/16 02:03:37 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void ALERTCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
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

if (theChan->getFlag(sqlChannel::F_ALERT)) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::alert_already_set,
                              std::string("The channel %s already has the ALERT flag.")).c_str(),
                                          theChan->getChannel().c_str());
  return;
}

theChan->setFlag(sqlChannel::F_ALERT);

if (!theChan->useSQL())
  theChan->Insert(bot->getLocalDBHandle());
else
  theChan->commit(bot->getLocalDBHandle());

/* Add note to the channel about this command */
theChan->addNote(bot->getLocalDBHandle(), sqlChannel::EV_ALERT, theClient, (st.size() > 2) ? st.assemble(2) : "");

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::alert_flag_added,
                            std::string("ALERT flag added to channel %s")).c_str(),
                                        theChan->getChannel().c_str());

/* Log command */
bot->logAdminMessage("%s (%s) ALERT %s",
		     theUser ? theUser->getUserName().c_str() : theClient->getNickName().c_str(),
		     theClient->getRealNickUserHost().c_str(),
		     theChan->getChannel().c_str());

bot->logLastComMessage(theClient, Message);

return;
}
} // namespace cf
} // namespace gnuworld
