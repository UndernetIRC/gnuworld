/**
 * INFOCommand.cc
 *
 * 07/20/2005 - Reed Loden <reed@reedloden.com>
 * Initial Version
 *
 * Shows all notes of this channel, and whether it has been blocked
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
 * $Id: INFOCommand.cc,v 1.7 2008/01/16 02:03:37 buzlip01 Exp $
 */

#include "gnuworld_config.h"
#include "Network.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlChannel.h"
#include "sqlcfUser.h"

RCSTAG("$Id: INFOCommand.cc,v 1.7 2008/01/16 02:03:37 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void INFOCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

bool isBlocked = bot->isTempBlocked(st[1]);

sqlChannel* theChan = bot->getChannelRecord(st[1]);
	
if (!theChan) {
  if (isBlocked) {
    bot->SendTo(theClient,
		bot->getResponse(theUser,
			    language::temporarily_blocked,
			    std::string("%s is temporarily blocked. (Use OVERRIDE to bypass)")).c_str(),
			    st[1].c_str());
    return;
  }
	
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::no_info_for_chan,
                              std::string("No information on %s in the database.")).c_str(),
                                          st[1].c_str());
  return;
}

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::information_on,
                            std::string("Information on %s:")).c_str(),
                                        theChan->getChannel().c_str());

if (isBlocked)
  bot->SendTo(theClient,
	    bot->getResponse(theUser,
			    language::temporarily_blocked,
			    std::string("%s is temporarily blocked. (Use OVERRIDE to bypass)")).c_str(),
			    st[1].c_str());

if (theChan->getFlag(sqlChannel::F_BLOCKED))
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::info_chan_blocked,
                              std::string("%s is BLOCKED.")).c_str(),
                                          theChan->getChannel().c_str());
else if (theChan->getFlag(sqlChannel::F_ALERT))
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::info_chan_alerted,
                              std::string("%s is ALERTED.")).c_str(),
                                          theChan->getChannel().c_str());

Channel* netChan = Network->findChannel(st[1]);
if (netChan && bot->isBeingFixed(netChan)) {
  if (bot->isBeingChanFixed(netChan))
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::info_chan_being_fixed,
                                std::string("%s is being chanfixed.")).c_str(),
                                            theChan->getChannel().c_str());
  if (bot->isBeingAutoFixed(netChan))
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::info_chan_being_autofixed,
                                std::string("%s is being autofixed.")).c_str(),
                                            theChan->getChannel().c_str());
  
  if (theChan->getFixStart() > 0)
    bot->SendTo(theClient,
		bot->getResponse(theUser,
				language::info_fix_started,
				std::string("Current fix has been running for %s")).c_str(),
				bot->prettyDuration(theChan->getFixStart()).c_str());
  else
    bot->SendTo(theClient,
		bot->getResponse(theUser,
				language::info_fix_waiting,
				std::string("Current fix is on hold (waiting for ops to join)")).c_str());
}
else
{
	unsigned int lastfix = bot->getLastFix(theChan);
	
	std::string dateTimeOfLastFix;
	if (lastfix == 0)
		dateTimeOfLastFix = "Never";
	else
		dateTimeOfLastFix = bot->tsToDateTime(lastfix, true);
		
	bot->SendTo(theClient, "Last fix: %s (%s ago)", dateTimeOfLastFix.c_str(), bot->prettyDuration(lastfix).c_str());
	
}

if (!theChan->useSQL()) {
  bot->logAdminMessage("%s (%s) INFO %s",
		       theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		       theClient->getRealNickUserHost().c_str(),
		       theChan->getChannel().c_str());
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			       language::end_of_information,
			       std::string("End of information.")).c_str());
  return;
}

/* Get a connection instance to our backend */
dbHandle* cacheCon = bot->getLocalDBHandle();

/*
 * Perform a query to list all notes belonging to this channel.
 */
std::stringstream allNotesQuery;
allNotesQuery	<< "SELECT notes.id, notes.ts, notes.user_name, notes.event, notes.message "
 		<< "FROM notes "
		<< "WHERE notes.channelID = "
		<< theChan->getID()
		<< " ORDER BY notes.ts DESC"
		;

if (!cacheCon->Exec(allNotesQuery.str(),true)) {
  elog	<< "INFOCommand> SQL Error: "
	<< cacheCon->ErrorMessage()
	<< std::endl;

  bot->SendTo(theClient,
		bot->getResponse(theUser,
				language::error_occured_notes,
				std::string("An unknown error occurred while reading this channel's notes.")).c_str());

  /* Dispose of our connection instance */
  //bot->theManager->removeConnection(cacheCon);

  return ;
}

unsigned int noteCount = cacheCon->Tuples();

if (noteCount > 0) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::info_notes_count,
                              std::string("Notes (%d):")).c_str(), noteCount);

  for (unsigned int i = 0; i < noteCount; i++) {
    unsigned int note_id = atoi(cacheCon->GetValue(i,0));
    unsigned int when = atoi(cacheCon->GetValue(i,1));
    std::string from = cacheCon->GetValue(i,2);
    unsigned short event = atoi(cacheCon->GetValue(i,3));
    std::string theMessage = cacheCon->GetValue(i,4);

    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::info_notes,
                                std::string("[%d:%s] %s \002%s\002%s%s")).c_str(),
                                            note_id, from.c_str(),
                                            bot->tsToDateTime(when, true).c_str(),
                                            bot->getEventName(event).c_str(),
                                            (!theMessage.empty()) ? " " : "", theMessage.c_str());
  }
}

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::end_of_information,
                            std::string("End of information.")).c_str());

/* Dispose of our connection instance */
//bot->theManager->removeConnection(cacheCon);

bot->logAdminMessage("%s (%s) INFO %s",
		     theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		     theClient->getRealNickUserHost().c_str(),
		     theChan->getChannel().c_str());

bot->logLastComMessage(theClient, Message);

return;
}

} // namespace cf
} // namespace gnuworld
