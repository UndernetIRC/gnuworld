/**
 * UNALERTCommand.cc
 *
 * 07/19/2005 - Reed Loden <reed@reedloden.com>
 * Initial Version
 *
 * Unsets the ALERT flag of this channel
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
 * $Id: UNALERTCommand.cc,v 1.5 2008/01/16 02:03:39 buzlip01 Exp $
 */

#include "gnuworld_config.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlChannel.h"
#include "sqlcfUser.h"

RCSTAG("$Id: UNALERTCommand.cc,v 1.5 2008/01/16 02:03:39 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void UNALERTCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

sqlChannel* theChan = bot->getChannelRecord(st[1]);
if (!theChan) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::no_entry_in_db,
                              std::string("There is no entry in the database for %s.")).c_str(),
                                          st[1].c_str());
  return;
}

if (!theChan->getFlag(sqlChannel::F_ALERT)) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::no_alert_set,
                              std::string("The channel %s does not have the ALERT flag.")).c_str(),
                                          theChan->getChannel().c_str());
  return;
}

theChan->removeFlag(sqlChannel::F_ALERT);

if (!theChan->useSQL())
  theChan->Insert(bot->getLocalDBHandle());
else
  theChan->commit(bot->getLocalDBHandle());

/* Add note to the channel about this command */
theChan->addNote(bot->getLocalDBHandle(), sqlChannel::EV_UNALERT, theClient, "");

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::alert_removed,
                            std::string("ALERT flag removed from channel %s")).c_str(),
                                        theChan->getChannel().c_str());

/* Log command */
bot->logAdminMessage("%s (%s) UNALERT %s",
		     theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		     theClient->getRealNickUserHost().c_str(),
		     theChan->getChannel().c_str());

bot->logLastComMessage(theClient, Message);

return;
}
} // namespace cf
} // namespace gnuworld
