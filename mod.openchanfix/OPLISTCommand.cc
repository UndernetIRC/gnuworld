/**
 * OPLISTCommand.cc
 *
 * 07/21/2005 - Jimmy Lipham <music0m@alltel.net>
 * Initial Version
 *
 * Shows a list of accounts plus their score of the top ops of this channel
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
 * $Id: OPLISTCommand.cc,v 1.8 2010/03/04 04:24:11 hidden1 Exp $
 */

#include "gnuworld_config.h"
#include "Network.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlChannel.h"
#include "sqlChanOp.h"

RCSTAG("$Id: OPLISTCommand.cc,v 1.8 2010/03/04 04:24:11 hidden1 Exp $");

namespace gnuworld
{
namespace cf
{

void OPLISTCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

bool all = false;
bool days = false;

unsigned int pos = 2;
while (pos < st.size()) {
  if (!strcasecmp(st[pos],"-all"))
    all = true;

  if (!strcasecmp(st[pos],"all"))
    all = true;

  if (!strcasecmp(st[pos],"!"))
    all = true;

  if (!strcasecmp(st[pos],"-days"))
    days = true;

  pos++;
}

chanfix::chanOpsType myOps = bot->getMyOps(st[1]);
if (myOps.empty()) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::no_scores_for_chan,
                              std::string("There are no scores in the database for %s.")).c_str(),
                                          st[1].c_str());
	
	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if (theChan) {
	  bot->SendTo(theClient, "Notes: %d", theChan->countNotes(bot->getLocalDBHandle(),0));

	  if (bot->isTempBlocked(theChan->getChannel()))
	    bot->SendTo(theClient,
			bot->getResponse(theUser,
					language::info_chan_temp_blocked,
					std::string("%s is TEMPBLOCKED.")).c_str(),
						    theChan->getChannel().c_str());
	  else if (theChan->getFlag(sqlChannel::F_BLOCKED))
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
	}
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
	
	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if (theChan) {
	  bot->SendTo(theClient, "Notes: %d", theChan->countNotes(bot->getLocalDBHandle(),0));

	  if (bot->isTempBlocked(theChan->getChannel()))
	    bot->SendTo(theClient,
			bot->getResponse(theUser,
					language::info_chan_temp_blocked,
					std::string("%s is TEMPBLOCKED.")).c_str(),
						    theChan->getChannel().c_str());
	  else if (theChan->getFlag(sqlChannel::F_BLOCKED))
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
	}
  return;
}

if (oCnt > OPCOUNT && !all)
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::top_unique_op_accounts,
                              std::string("Top %d unique op accounts in channel %s:")).c_str(),
                                          OPCOUNT, st[1].c_str());
else
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::found_unique_op_accounts,
                              std::string("Found %d unique op accounts in channel %s:")).c_str(),
                                          oCnt, st[1].c_str());

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::rank_score_acc_header,
			     std::string("Rank Score Account -- Time first opped / Time last opped / Nick")).c_str());

sqlChanOp* curOp = 0;
unsigned int opCount = 0;
unsigned int percent = 0;
int cScore;
bool inChan = false;
std::string firstop;
std::string lastop;
std::string nickName;
std::stringstream dayString;
time_t oldestTS = bot->currentTime();
for (chanfix::chanOpsType::iterator chOp = myOps.begin();
     chOp != myOps.end(); chOp++) {
     curOp = *chOp;
    if (curOp->getTimeFirstOpped() < oldestTS)
      oldestTS = curOp->getTimeFirstOpped();
}
for (chanfix::chanOpsType::iterator opPtr = myOps.begin();
     opPtr != myOps.end() && (all || opCount < OPCOUNT); opPtr++) {
  curOp = *opPtr;
  opCount++;
  firstop = bot->tsToDateTime(curOp->getTimeFirstOpped(), false);
  lastop = bot->tsToDateTime(curOp->getTimeLastOpped(), true);
  inChan = bot->accountIsOnChan(st[1], curOp->getAccount());
  if (inChan)
    nickName = bot->getChanNickName(st[1], curOp->getAccount());

  if (days) {
    dayString.str("");

    for (int i = 1; i <= DAYSAMPLES; i++) {
      cScore = curOp->getDay((currentDay + i) % DAYSAMPLES);
      percent = static_cast<unsigned int>(((static_cast<float>(cScore) / static_cast<float>(86400 / POINTS_UPDATE_TIME)) * 100) + 0.5);

      if (!cScore)
	dayString << "."; // no score (.)
      else if (percent <= 10)
	dayString << "0"; // 0%-10% (0)
      else if ((percent >= 11) && (percent <= 20))
	dayString << "1"; // 11%-20% (1)
      else if ((percent >= 21) && (percent <= 30))
	dayString << "2"; // 21%-30% (2)
      else if ((percent >= 31) && (percent <= 40))
	dayString << "3"; // 31%-40% (3)
      else if ((percent >= 41) && (percent <= 50))
	dayString << "4"; // 41%-50% (4)
      else if ((percent >= 51) && (percent <= 60))
	dayString << "5"; // 51%-60% (5)
      else if ((percent >= 61) && (percent <= 70))
	dayString << "6"; // 61%-70% (6)
      else if ((percent >= 71) && (percent <= 80))
	dayString << "7"; // 71%-80% (7)
      else if ((percent >= 81) && (percent <= 90))
	dayString << "8"; // 81%-90% (8)
      else if ((percent >= 91))
	dayString << "9"; // 91%-100% (9)
    }
    dayString << std::ends;
  }

  bot->SendTo(theClient, "%3d. %4d  %s -- %s / %s%s%s%s%s%s",
	      opCount, 
          (curOp->getPoints() + curOp->getBonus()),
	      curOp->getAccount().c_str(), firstop.c_str(),
	      lastop.c_str(), inChan ? " / " : "",
	      inChan ? nickName.c_str() : "",
	      (days) ? " [" : "",
	      (days) ? dayString.str().c_str() : "",
	      (days) ? "]" : "");
}

sqlChannel* theChan = bot->getChannelRecord(st[1]);
if (theChan) {
  bot->SendTo(theClient, "Notes: %d", theChan->countNotes(bot->getLocalDBHandle(), 0));

  if (bot->isTempBlocked(theChan->getChannel()))
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::info_chan_temp_blocked,
                                std::string("%s is TEMPBLOCKED.")).c_str(),
                                            theChan->getChannel().c_str());
  else if (theChan->getFlag(sqlChannel::F_BLOCKED))
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
}

bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::end_of_oplist,
			    std::string("End of OPLIST for %s")).c_str(),
			    st[1].c_str());

bot->logAdminMessage("%s (%s) OPLIST %s %s",
		     theUser ? theUser->getUserName().c_str() : "!NOT-LOGGED-IN!",
		     theClient->getRealNickUserHost().c_str(),
		     st[1].c_str(),
		     (st.size() > 2) ? st.assemble(2).c_str() : "");

bot->logLastComMessage(theClient, Message);

return;
}

} //namespace cf
} //namespace gnuworld
