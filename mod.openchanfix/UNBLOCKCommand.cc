/**
 * UNBLOCKCommand.cc
 *
 * 07/19/2005 - Reed Loden <reed@reedloden.com>
 * Initial Version
 *
 * Removes the block on a channel
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
 * $Id: UNBLOCKCommand.cc,v 1.5 2008/01/16 02:03:39 buzlip01 Exp $
 */

#include "gnuworld_config.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlChannel.h"
#include "sqlcfUser.h"

RCSTAG("$Id: UNBLOCKCommand.cc,v 1.5 2008/01/16 02:03:39 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void UNBLOCKCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

/* Check if channel blocking has been disabled in the config. */
if (!bot->doChanBlocking()) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::channel_blocking_disabled,
                              std::string("Channel blocking is disabled.")).c_str());
  return;
}
	
sqlChannel* theChan = bot->getChannelRecord(st[1]);
if (!theChan) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::no_entry_in_db,
                              std::string("There is no entry in the database for %s.")).c_str(),
                                          st[1].c_str());
  return;
}

if (!theChan->getFlag(sqlChannel::F_BLOCKED)) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::channel_not_blocked,
                              std::string("The channel %s is not blocked.")).c_str(),
                                          theChan->getChannel().c_str());
  return;
}

theChan->removeFlag(sqlChannel::F_BLOCKED);

if (!theChan->useSQL())
  theChan->Insert(bot->getLocalDBHandle());
else
  theChan->commit(bot->getLocalDBHandle());

/* Add note to the channel about this command */
theChan->addNote(bot->getLocalDBHandle(), sqlChannel::EV_UNBLOCK, theClient, "");

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::channel_unblocked,
                            std::string("Channel %s has been unblocked.")).c_str(),
                                        theChan->getChannel().c_str());

/* Log command */
bot->logAdminMessage("%s (%s) UNBLOCK %s",
		     theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		     theClient->getRealNickUserHost().c_str(),
		     theChan->getChannel().c_str());

bot->logLastComMessage(theClient, Message);

return;
}
} // namespace cf
} // namespace gnuworld
