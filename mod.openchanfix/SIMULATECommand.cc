/**
 * SIMULATECommand.cc
 *
 * 28/09/2006 - Neil Spierling
 * Initial Version
 *
 * Simulate a fix on a channel
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
 * $Id: SIMULATECommand.cc,v 1.2 2008/01/16 02:03:39 buzlip01 Exp $
 */

#include "gnuworld_config.h"
#include "Network.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlChannel.h"
#include "sqlcfUser.h"

RCSTAG("$Id: SIMULATECommand.cc,v 1.2 2008/01/16 02:03:39 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void SIMULATECommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

bool autof = false;

if (st.size() > 2) {
  unsigned int pos = 2;
  while(pos < st.size()) {
    if (!strcasecmp(st[pos],"AUTO"))
      autof = true;

    if (!strcasecmp(st[pos],"MANUAL"))
      autof = false;

    pos++;
  }
}

/* If not enough servers are currently linked, bail out. */
if (bot->getState() != chanfix::RUN) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::not_enough_servers,
                              std::string("Sorry, chanfix is currently disabled because not enough servers are linked.")).c_str());
  return;
}

Channel* netChan = Network->findChannel(st[1]);
if (!netChan) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::no_such_channel,
                              std::string("No such channel %s.")).c_str(), st[1].c_str());
  return;
}

if (!bot->canScoreChan(netChan) || netChan->getMode(Channel::MODE_REG)) {
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			       language::registered_channel,
			       std::string("%s is a registered channel.")).c_str(),
			       netChan->getName().c_str());
  return;
}

chanfix::chanOpsType myOps = bot->getMyOps(netChan);
if (myOps.empty()) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::no_scores_for_chan,
                              std::string("There are no scores in the database for %s.")).c_str(),
                                          netChan->getName().c_str());
  return;
}

if (bot->isBeingChanFixed(netChan)) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::already_being_man_fixed,
                              std::string("The channel %s is already being manually fixed.")).c_str(),
                                          netChan->getName().c_str());
  return;
}

sqlChannel* theChan = bot->getChannelRecord(st[1]);
if (!theChan) theChan = bot->newChannelRecord(st[1]);

if (myOps.begin() != myOps.end())
  theChan->setMaxScore((*myOps.begin())->getPoints());

if (theChan->getMaxScore() <= 
    static_cast<int>(static_cast<float>(FIX_MIN_ABS_SCORE_END) * MAX_SCORE)) 
{
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::highscore_channel,
                              std::string("The highscore in channel %s is %d which is lower than the minimum score required (%.2f * %d = %d).")).c_str(),
                                          theChan->getChannel().c_str(), theChan->getMaxScore(),
                                          FIX_MIN_ABS_SCORE_END, MAX_SCORE,
                                          static_cast<int>(static_cast<float>(FIX_MIN_ABS_SCORE_END) 
                                          * MAX_SCORE));
  return;
}

if (theChan->getFlag(sqlChannel::F_BLOCKED)) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::channel_blocked,
                              std::string("The channel %s is BLOCKED.")).c_str(),
                                          theChan->getChannel().c_str());
  return;
}

if (bot->isTempBlocked(theChan->getChannel())) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::channel_temp_blocked,
                              std::string("The channel %s is TEMPBLOCKED.")).c_str(),
                                          theChan->getChannel().c_str());
  return;
}

if (!theChan->useSQL())
  theChan->Insert(bot->getLocalDBHandle());

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::manual_simulate_starting,
                            std::string("Simulate for %s (%s) starting at next fixing round (Current C time %s).")).c_str(),
                                        netChan->getName().c_str(), ((autof == true) ? "AUTO" : "MANUAL"), 
                                        bot->tsToDateTime(bot->currentTime(), true).c_str());

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::manual_simulate_estimate,
                            std::string("NOTE: This is only an estimate, if ops with points join or part it could affect who gets opped.")).c_str());

sqlChannel* sqlChan = bot->getChannelRecord(st[1]);
bot->simulateFix(sqlChan, autof, theClient, theUser);

theChan->addNote(bot->getLocalDBHandle(), sqlChannel::EV_SIMULATE, theClient, (autof) ? "[auto]" : "[manual]");

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::manual_simulate_complete,
                            std::string("Simulate complete for %s")).c_str(),
                                        netChan->getName().c_str());

bot->logAdminMessage("%s (%s) SIMULATE %s",
		     theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		     theClient->getRealNickUserHost().c_str(),
		     netChan->getName().c_str());

bot->logLastComMessage(theClient, Message);

return;
}

} // namespace cf
} // namespace gnuworld
