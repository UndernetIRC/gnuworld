/**
 * CHANFIXCommand.cc
 *
 * 01/01/2004 - Reed Loden <reed@reedloden.com>
 * Initial Version
 *
 * Manually fix a channel
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
 * $Id: CHANFIXCommand.cc,v 1.3 2006/04/05 02:37:34 buzlip01 Exp $
 */

#include "gnuworld_config.h"
#include "Network.h"

#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"

RCSTAG("$Id: CHANFIXCommand.cc,v 1.3 2006/04/05 02:37:34 buzlip01 Exp $");

namespace gnuworld
{
namespace cf
{

void CHANFIXCommand::Exec(iClient* theClient, sqlUser* theUser, const std::string& Message)
{
StringTokenizer st(Message);

bool override = false;

if (st.size() > 2) {
  const std::string flag = string_upper(st[2]);
  if ((flag == "OVERRIDE") || (flag == "NOW") || (flag == "YES") || (flag == "!"))
    override = true;
}

/* Check if manual chanfix has been disabled in the config. */
if (!bot->doChanFix()) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::manual_fix_disabled,
                              std::string("Sorry, manual chanfixes are currently disabled.")).c_str());
  return;
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

if (!bot->canScoreChan(netChan)) {
  bot->SendTo(theClient,
	      bot->getResponse(theUser,
			       language::registered_channel,
			       std::string("%s is a registered channel.")).c_str(),
			       netChan->getName().c_str());
  return;
}

/* Only allow chanfixes for channels that are in the database. */
chanfix::chanOpsType myOps = bot->getMyOps(netChan);
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

/* Don't fix a channel being autofixed without OVERRIDE flag. */
if (bot->isBeingAutoFixed(netChan)) {
  if (!override) {
    bot->SendTo(theClient,
                bot->getResponse(theUser,
                                language::channel_being_auto_fixed,
                                std::string("The channel %s is being automatically fixed. Append the OVERRIDE flag to force a manual fix.")).c_str(),
                                            netChan->getName().c_str());
    return;
  } else {
    /* We're going to manually fix this instead of autofixing it,
     * so remove this channel from the autofix queue. */
    bot->removeFromAutoQ(netChan);
  }
}

/* Don't fix a blocked channel. */
if (theChan->getFlag(sqlChannel::F_BLOCKED)) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::channel_blocked,
                              std::string("The channel %s is BLOCKED.")).c_str(),
                                          theChan->getChannel().c_str());
  return;
}

/* Don't fix an alerted channel without the OVERRIDE flag. */
if (theChan->getFlag(sqlChannel::F_ALERT) && !override) {
  bot->SendTo(theClient,
              bot->getResponse(theUser,
                              language::channel_has_notes,
                              std::string("Alert: The channel %s has notes. Use \002INFO %s\002 to read them. Append the OVERRIDE flag to force a manual fix.")).c_str(),
                                          theChan->getChannel().c_str(), theChan->getChannel().c_str());
  return;
}

/* Add the channel to the SQL database if it hasn't already been added */
if (!theChan->useSQL())
  theChan->Insert();

/* Fix the channel */
bot->manualFix(netChan);

/* Add note to the channel about this manual fix */
theChan->addNote(sqlChannel::EV_CHANFIX, theUser, (override) ? "[override]" : "");

/* Log the chanfix */
bot->SendTo(theClient,
            bot->getResponse(theUser,
                            language::manual_chanfix_ack,
                            std::string("Manual chanfix acknowledged for %s")).c_str(),
                                        netChan->getName().c_str());
bot->logAdminMessage("%s (%s) CHANFIX %s%s",
		     theUser->getUserName().c_str(),
		     theClient->getRealNickUserHost().c_str(),
		     netChan->getName().c_str(),
		     (override) ? " [override]" : "");

return;
}

} // namespace cf
} // namespace gnuworld
