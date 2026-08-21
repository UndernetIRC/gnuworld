/**
 * DELSCORECommand.cc
 *
 * Deletes an account's chanfix score from a channel
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
 */
#include "gnuworld_config.h"
#include "chanfix.h"
#include "responses.h"
#include "StringTokenizer.h"
#include "sqlChanOp.h"
#include "sqlcfUser.h"

namespace gnuworld {
namespace cf {

void DELSCORECommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message) {
    StringTokenizer st(Message);

    if (st[1][0] != '#') {
        bot->SendTo(theClient,
                    bot->getResponse(theUser, language::invalid_channel_name,
                                     std::string("%s is an invalid channel name."))
                        .c_str(),
                    st[1].c_str());
        return;
    }

    sqlChanOp* targetOp = bot->findChanOp(st[1], st[2]);
    if (!targetOp) {
        bot->SendTo(theClient,
                    bot->getResponse(theUser, language::no_score_for_account,
                                     std::string("No score for account %s on channel %s."))
                        .c_str(),
                    st[2].c_str(), st[1].c_str());
        return;
    }

    std::string account = targetOp->getAccount();
    unsigned int deletedPoints = targetOp->getPoints() + targetOp->getBonus();

    if (!bot->deleteChanOp(st[1], account)) {
        bot->SendTo(
            theClient,
            bot->getResponse(theUser, language::error_deleting_score,
                             std::string("Error deleting score for account %s on channel %s."))
                .c_str(),
            account.c_str(), st[1].c_str());
        return;
    }

    bot->SendTo(
        theClient,
        bot->getResponse(theUser, language::score_deleted,
                         std::string("Deleted score for account %s in channel %s (was %u points)."))
            .c_str(),
        account.c_str(), st[1].c_str(), deletedPoints);

    bot->logAdminMessage("%s (%s) DELSCORE %s %s",
                         theUser ? theUser->getUserName().c_str()
                                 : theClient->getNickName().c_str(),
                         theClient->getRealNickUserHost().c_str(), st[1].c_str(), account.c_str());

    bot->logLastComMessage(theClient, Message);

    return;
} // DELSCORECommand::Exec

} // namespace cf
} // namespace gnuworld
