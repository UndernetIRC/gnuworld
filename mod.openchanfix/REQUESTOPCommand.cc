/**
 * REQUESTOPCommand.cc
 *
 * 04/04/2006 - Neil Spierling <sirvulcan@gmail.com>
 * Initial Version
 *
 * Allow top ops to fix channels
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
 * $Id: REQUESTOPCommand.cc,v 1.2 2008/01/16 02:03:39 buzlip01 Exp $
 */

#include "gnuworld_config.h"
#include "Network.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"

RCSTAG("$Id: REQUESTOPCommand.cc,v 1.2 2008/01/16 02:03:39 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void REQUESTOPCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

bool alert = false;
bool ishighscore = false;

chanfix::chanOpsType myOps = bot->getMyOps(st[1]);

if (bot->isAllowingTopFix()) {
  sqlChanOp* curOp = 0;
  int maxScore = 0;
  unsigned int opCount = 0;
  unsigned int pScore;
  unsigned int percent;
  unsigned int wScore;

  /* First off we need the top score from the channel */
  for (chanfix::chanOpsType::iterator opPtr = myOps.begin();
       opPtr != myOps.end(); opPtr++) {
    curOp = *opPtr;
    opCount++;

    if (curOp->getPoints() > maxScore)
      maxScore = curOp->getPoints();
  }

  opCount = 0;

  /* Secondly we need to make sure that the person's score who is requesting
     REQUESTOP is within X% of the top score, specified in the config file.
     If it isnt then we reject them soon */
  for (chanfix::chanOpsType::iterator opPtr = myOps.begin();
       opPtr != myOps.end(); opPtr++) {
    curOp = *opPtr;
    opCount++;

    pScore = curOp->getPoints();
    percent = static_cast<int>((static_cast<float>(pScore) / static_cast<float>(maxScore)) * 100);
    wScore = 100 - percent;
    if ((static_cast<float>(wScore) <= bot->getTopOpPercent()) && (static_cast<float>(pScore) >= bot->getMinFixScore())) {
      if (string_lower(curOp->getAccount()) == string_lower(theClient->getAccount()))
        ishighscore = true;
    }
  }
}

Channel* netChan = Network->findChannel(st[1]);
if (!netChan) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::no_such_channel,
                              std::string("No such channel %s.")).c_str(), st[1].c_str());
  return;
}

if (!ishighscore) {
  if (bot->isAllowingTopFix()) {
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::score_not_high_enough,
                                std::string("You score for %s is not high enough to issue a fix.")).c_str(),
                                            netChan->getName().c_str());
  }
  return;
}

/* This could be done simpler however if we decide to add
   more parameters to REQUESTOP then its as simple as
   adding a couple of lines to this section. (Plus the
   other nessarcery code */
if (bot->isAllowingTopOpAlert()) {
  if (st.size() > 2) {
    unsigned int pos = 2;
    while(pos < st.size()) {
      if (!strcasecmp(st[pos],"CONTACT"))
        alert = true;

      pos++;
    }
  }
}

/* Check if manual chanfix has been disabled in the config. */
if (!bot->doChanFix()) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::canfix_manual_fix_disabled,
                              std::string("Sorry, I cant fix channels at the moment.")).c_str());
  return;
}

/* If not enough servers are currently linked, bail out. */
if (bot->getState() != chanfix::RUN) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::not_enough_servers_non_oper,
                              std::string("Sorry, chanfix cannot fix channels at the moment, please try again soon.")).c_str());
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

/* Only allow chanfixes for channels that are in the database. */
if (myOps.empty()) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::no_scores_for_chan,
                              std::string("There are no scores in the database for %s.")).c_str(),
                                          netChan->getName().c_str());
  return;
}

/* Don't fix a channel being chanfixed. */
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

unsigned int mintime = 0;
unsigned int curtime = 0;
unsigned int lastfix = 0;

curtime = bot->currentTime();
lastfix = bot->getLastFix(theChan);
mintime = (curtime - lastfix);
if (mintime < bot->getMinRequestOpTime()) {
  unsigned int mint = (bot->getMinRequestOpTime() - mintime);
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::requestop_must_wait,
                              std::string("You must wait %d seconds before %s can be fixed again with this command, or contact a help channel.")).c_str(),
                                          mint, netChan->getName().c_str());
  return;
}

/* Check if the highest score is high enough for a fix. */
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

/* Don't fix a channel being autofixed. */
if (bot->isBeingAutoFixed(netChan)) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::channel_being_auto_fixed_noper,
                              std::string("The channel %s is being automatically fixed. You cannot issue another fix at the moment.")).c_str(),
                                          netChan->getName().c_str());
  return;
}

/* Don't fix a blocked or alerted channel. */
if ((bot->isTempBlocked(theChan->getChannel())) || (theChan->getFlag(sqlChannel::F_BLOCKED)) || 
    (theChan->getFlag(sqlChannel::F_ALERT))) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::channel_blocked_non_oper,
                              std::string("The channel %s cannot be fixed, please contact a help channel for further information.")).c_str(),
                                          theChan->getChannel().c_str());
  return;
}

/* Add the channel to the SQL database if it hasn't already been added */
if (!theChan->useSQL())
  theChan->Insert(bot->getLocalDBHandle());

/* Alert top ops out of the channel if wanted */
if (alert)
  bot->msgTopOps(netChan);

/* Fix the channel */
bot->manualFix(netChan);

/* Add note to the channel about this manual fix */
theChan->addNote(bot->getLocalDBHandle(), sqlChannel::EV_REQUESTOP, theClient, "");

/* Log the chanfix */
bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::requestop_ack,
                            std::string("Fixing %s, please wait.")).c_str(),
                                        netChan->getName().c_str());

bot->logAdminMessage("%s (%s) REQUESTOP %s %s",
		     theUser ? theUser->getUserName().c_str() : theClient->getUserName().c_str(),
		     theClient->getRealNickUserHost().c_str(), netChan->getName().c_str(),
		     (alert) ? " [alert]" : "");

bot->logLastComMessage(theClient, Message);

return;
}

} // namespace cf
} // namespace gnuworld
