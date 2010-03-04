/**
 * CANFIXCommand.cc
 *
 * 03/04/2006 - Neil Spierling <sirvulcan@gmail.com>
 * Initial Version
 *
 * Shows a list of accounts who can issue fixes for their channel
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
 * $Id: CANFIXCommand.cc,v 1.2 2010/03/04 04:24:11 hidden1 Exp $
 */

#include "gnuworld_config.h"
#include "Network.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlChannel.h"
#include "sqlChanOp.h"

RCSTAG("$Id: CANFIXCommand.cc,v 1.2 2010/03/04 04:24:11 hidden1 Exp $");

namespace gnuworld
{
namespace cf
{

void CANFIXCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

sqlChanOp* curOp = 0;

chanfix::chanOpsType myOps = bot->getMyOps(st[1]);
int minFix = bot->getMinCanFixScore();
int maxFix = 0;
int maxScore = 0;
bool highscore = false;
unsigned int opCount = 0;

for (chanfix::chanOpsType::iterator opPtr = myOps.begin();
     opPtr != myOps.end(); opPtr++) {
  curOp = *opPtr;
  opCount++;

  if ((curOp->getPoints() + curOp->getBonus()) > maxScore)
    maxScore = curOp->getPoints() + curOp->getBonus();

  if (!strcasecmp(curOp->getAccount(), theClient->getAccount())) {
    maxFix = curOp->getPoints() + curOp->getBonus();
    if (maxFix < minFix)
      highscore = false;
    else
      highscore = true;
  }
}

/* Opers can view the list anyway despite having
 * potentially no score. */
if (theClient->isOper())
  highscore = true;

if (highscore == false) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
	 	               language::can_fix_not_enough_points,
		   	       std::string("You do not have a high enough score for %s to check canfix list.")).c_str(),
					   st[1].c_str());
  return;
}

if (myOps.empty()) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::no_scores_for_chan,
                              std::string("There are no scores in the database for %s.")).c_str(),
                                          st[1].c_str());
  return;
}

unsigned int oCnt = myOps.size();

/**
 * Technically, if there are all 0 scores, it will get to this point.
 * We don't want a notice saying 0 accounts.
 */
if (oCnt == 0) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::no_scores_for_chan,
                              std::string("There are no scores in the database for %s.")).c_str(),
	      st[1].c_str());
  return;
}

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::canfix_op_accounts,
                            std::string("Accounts who can issue fixes in channel %s:")).c_str(),
                                        st[1].c_str());

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::canfix_rank_score_acc_header,
			    std::string("Account -- Time first opped / Time last opped")).c_str());

std::string firstop;
std::string lastop;

opCount = 0;
int pScore = 0;
int percent = 0;
int wScore = 0;

for (chanfix::chanOpsType::iterator opPtr = myOps.begin();
     opPtr != myOps.end(); opPtr++) {
  curOp = *opPtr;
  opCount++;
  firstop = bot->tsToDateTime(curOp->getTimeFirstOpped(), false);
  lastop = bot->tsToDateTime(curOp->getTimeLastOpped(), true);

  pScore = curOp->getPoints() + curOp->getBonus();
  percent = static_cast<int>((static_cast<float>(pScore) / static_cast<float>(maxScore)) * 100);
  /* Config value is within X percent, so we need to minus it from 100 to make sure
     its within the X percent specified in the config */
  wScore = 100 - percent;
  if ((static_cast<float>(wScore) <= bot->getTopOpPercent()) && (static_cast<float>(pScore) >= bot->getMinFixScore())) {
    bot->SendTo(theClient, "%s -- %s / %s",
		curOp->getAccount().c_str(), firstop.c_str(),
		lastop.c_str());
  }

  pScore = 0;
  percent = 0;
  wScore = 0;
}

bot->logAdminMessage("%s (%s) CANFIX %s",
		     theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		     theClient->getRealNickUserHost().c_str(),
		     st[1].c_str());

return;
}

} //namespace cf
} //namespace gnuworld
