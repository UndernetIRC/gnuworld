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
 * $Id: OPLISTCommand.cc,v 1.2 2006/03/21 22:49:14 buzlip01 Exp $
 */

#include "gnuworld_config.h"
#include "Network.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlChannel.h"
#include "sqlChanOp.h"

RCSTAG("$Id: OPLISTCommand.cc,v 1.2 2006/03/21 22:49:14 buzlip01 Exp $");

namespace gnuworld
{

void OPLISTCommand::Exec(iClient* theClient, sqlUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

bool all = false;
if (st.size() > 2) {
  const std::string flag = string_upper(st[2]);
  if ((flag == "ALL") || (flag == "-ALL") || (flag == "!"))
    all = true;
}

sqlChanOp* curOp = 0;

chanfix::chanOpsType myOps = bot->getMyOps(st[1]);
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
                            std::string("Rank Score Account -- Time first opped / Time last opped")).c_str());

unsigned int opCount = 0;
std::string firstop;
std::string lastop;
for (chanfix::chanOpsType::iterator opPtr = myOps.begin();
     opPtr != myOps.end() && (all || opCount < OPCOUNT); opPtr++) {
  curOp = *opPtr;
  opCount++;
  firstop = bot->tsToDateTime(curOp->getTimeFirstOpped(), false);
  lastop = bot->tsToDateTime(curOp->getTimeLastOpped(), true);
  if (bot->accountIsOnChan(st[1],curOp->getAccount()))
    bot->SendTo(theClient, "%3d. \002%4d  %s -- %s / %s\002", opCount,
		curOp->getPoints(), curOp->getAccount().c_str(),
		firstop.c_str(), lastop.c_str());
  else
    bot->SendTo(theClient, "%3d. %4d  %s -- %s / %s", opCount,
		curOp->getPoints(), curOp->getAccount().c_str(),
		firstop.c_str(), lastop.c_str());
}

return;
}

}
