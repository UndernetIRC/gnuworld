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
/* 20110830 - Modified the REQUESTOP command to support the new CFV that was passed for criteria on Undernet
 * A REQUESTOP succeeds if the following criteria are met:
 * 1. The requester has a score of >= 1000
 * 2. There is nobody opped on the channel with a high score >= 2000
 * 3. There is nobody opped on the channel with a higher score than the requester
 * 4. There is nobody opped on the channel with a 'first opped date' within 3 days of the oldest first opped date.
 * 5. The requester is in the top 10 high scores for the channel
 */
void REQUESTOPCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

bool isRequesterScoreHighEnough = false;
int pScore, requesterScore = 0;
bool alert = false;
bool inChan = false;
sqlChanOp* curOp = 0;

// If the bot is not allowing top op fixes (REQUESTOP), ignore the request
if (!bot->isAllowingTopFix()) return;


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

// Does the channel exist on the network? If not, discontinue the request
Channel* netChan = Network->findChannel(st[1]);
if (!netChan) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::no_such_channel,
                              std::string("No such channel %s.")).c_str(), st[1].c_str());
  return;
}

chanfix::chanOpsType myOps = bot->getMyOps(st[1]);

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

// Next lets make sure the requester has a score of >= 1000
for (chanfix::chanOpsType::iterator opPtr = myOps.begin();
   opPtr != myOps.end(); opPtr++) {
  curOp = *opPtr;

  pScore = curOp->getPoints() + curOp->getBonus();
  //bot->SendTo(theClient, "Current Op: %s -- You: %s -- pScore: %d", curOp->getAccount().c_str(), theClient->getAccount().c_str(), pScore);
  if (string_lower(curOp->getAccount()) == string_lower(theClient->getAccount()) && pScore >= 1000)
  {
	isRequesterScoreHighEnough = true;
	requesterScore = pScore;
	break;
  }
}
if (!isRequesterScoreHighEnough)
{
  bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::score_not_high_enough,
                                std::string("You score for %s is not high enough to issue a fix.")).c_str(),
                                            netChan->getName().c_str());
  return;
}

// Is there anyone opped on the chan with a score >= 2000?
std:string nickName;
for (chanfix::chanOpsType::iterator opPtr = myOps.begin();
     opPtr != myOps.end(); opPtr++) {
  curOp = *opPtr;
  pScore = curOp->getPoints() + curOp->getBonus();
  inChan = bot->accountIsOnChan(st[1], curOp->getAccount());
  if (inChan && pScore >= 2000)
  {
    nickName = bot->getChanNickName(st[1], curOp->getAccount());
	// Are they opped?
	if (nickName[0] == '@')
	{
		bot->SendTo(theClient, "Sorry, there is somebody currently opped on %s with a high score.", st[1].c_str());
		return;
	}
  }
}

//Is there anyone opped on the channel with a higher score than the requester?
for (chanfix::chanOpsType::iterator opPtr = myOps.begin();
     opPtr != myOps.end(); opPtr++) {
  curOp = *opPtr;
  pScore = curOp->getPoints() + curOp->getBonus();
  inChan = bot->accountIsOnChan(st[1], curOp->getAccount());
  if (inChan && pScore >= requesterScore)
  {
    nickName = bot->getChanNickName(st[1], curOp->getAccount());
	// Are they opped?
	if (nickName[0] == '@')
	{
		bot->SendTo(theClient, "Sorry, there is somebody currently opped on %s with a higher score than you.", st[1].c_str());
		return;
	}
  }
}

// Is there anyone opped on the channel with a 'first opped date' within 3 days of the oldest first opped date?
// First, get the oldest first opped TS and add 3 days to it
time_t ts_firstOpped = 0;
for (chanfix::chanOpsType::iterator opPtr = myOps.begin();
     opPtr != myOps.end(); opPtr++) {
  curOp = *opPtr;
  if (curOp->isOldestOp())
  {
    ts_firstOpped = (curOp->getTimeFirstOpped() + (3 * 86400));
  }
}
// Got the first opped TS, now we make sure there's not an op with a first_opped_ts within 3 days of this
for (chanfix::chanOpsType::iterator opPtr = myOps.begin();
     opPtr != myOps.end(); opPtr++) {
  curOp = *opPtr;
  //bot->SendTo(theClient, "curOp: %s Time First Opped: %d ts_firstOpped: %d", curOp->getAccount().c_str(), curOp->getTimeFirstOpped(), ts_firstOpped);
  inChan = bot->accountIsOnChan(st[1], curOp->getAccount());
  if (inChan)
  {
    nickName = bot->getChanNickName(st[1], curOp->getAccount());
    if (nickName[0] == '@' && !curOp->isOldestOp() && curOp->getTimeFirstOpped() <= ts_firstOpped)
    {
      bot->SendTo(theClient, "Sorry, there is somebody opped on %s that has been opped longer.", st[1].c_str());
	  return;
    }
  }
}

// Make sure the requester is in the top 10 scores list
int opCount = 0;
for (chanfix::chanOpsType::iterator opPtr = myOps.begin();
     opPtr != myOps.end(); opPtr++) {
  curOp = *opPtr;
  opCount++;
  
  if (string_lower(curOp->getAccount()) == string_lower(theClient->getAccount()) && opCount > 10)
  {
	bot->SendTo(theClient, "Sorry, you are not within the top 10 channel ops in %s.", st[1].c_str());
	return;
  }
}

/* Don't fix a channel being chanfixed. */
if (bot->isBeingChanFixed(netChan)) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::already_being_man_fixed,
                              std::string("The channel %s is already being fixed.")).c_str(),
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
                              std::string("You must wait %s seconds before %s can be fixed again with this command, or contact a help channel.")).c_str(),
                                          bot->prettyDuration(mint).c_str(), netChan->getName().c_str());
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

}

} // namespace cf
} // namespace gnuworld
