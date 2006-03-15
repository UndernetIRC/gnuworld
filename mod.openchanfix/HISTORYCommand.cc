/**
 * HISTORYCommand.cc
 *
 * 08/14/2005 - Reed Loden <reed@reedloden.com>
 * Initial Version
 *
 * Shows the times that <channel> has been manually fixed
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
 * $Id: HISTORYCommand.cc,v 1.1 2006/03/15 02:50:37 buzlip01 Exp $
 */

#include "gnuworld_config.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlChannel.h"
#include "sqlUser.h"

RCSTAG("$Id: HISTORYCommand.cc,v 1.1 2006/03/15 02:50:37 buzlip01 Exp $");

namespace gnuworld
{

void HISTORYCommand::Exec(iClient* theClient, sqlUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

sqlChannel* theChan = bot->getChannelRecord(st[1]);
if (!theChan || !theChan->useSQL()) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::chan_no_manual_fixes,
                              std::string("Channel %s has never been manually fixed.")).c_str(),
                                          st[1].c_str());
  return;
}

/* Get a connection instance to our backend */
PgDatabase* cacheCon = bot->theManager->getConnection();

/*
 * Perform a query to list the dates/times this channel was last chanfixed.
 */
std::stringstream chanfixQuery;
chanfixQuery	<< "SELECT ts "
		<< "FROM notes "
		<< "WHERE channelID = "
		<< theChan->getID()
		<< " AND event = "
		<< sqlChannel::EV_CHANFIX
		<< " ORDER BY ts DESC"
		;

if (!cacheCon->ExecTuplesOk(chanfixQuery.str().c_str())) {
	elog	<< "HISTORYCommand> SQL Error: "
		<< cacheCon->ErrorMessage()
		<< std::endl;

        bot->SendTo(theClient,
                    bot->getResponse(theUser,
                                    language::error_occured_notes,
                                    std::string("An unknown error occurred while reading this channel's notes.")).c_str());
	return ;
	}

unsigned int noteCount = cacheCon->Tuples();

if (noteCount <= 0) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::chan_no_manual_fixes,
                              std::string("Channel %s has never been manually fixed.")).c_str(),
                                          theChan->getChannel().c_str());

  /* Dispose of our connection instance */
  bot->theManager->removeConnection(cacheCon);

  return;
}

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::chan_manually_fix,
                            std::string("Channel %s has been manually fixed on:")).c_str(),
                                        theChan->getChannel().c_str());

for (unsigned int i = 0; i < noteCount; i++)
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::chan_manual_fix,
                              std::string("%s")).c_str(), bot->tsToDateTime(atoi(cacheCon->GetValue(i,0)), true).c_str());

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::end_of_list,
                            std::string("End of list.")).c_str());

/* Dispose of our connection instance */
bot->theManager->removeConnection(cacheCon);

return;
}
} // namespace gnuworld
