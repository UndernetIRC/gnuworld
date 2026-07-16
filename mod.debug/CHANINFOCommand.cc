/**
 * CHANINFOCommand.cc
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

#include <string>

#include "Network.h"
#include "StringTokenizer.h"
#include "gnuworld_config.h"
#include "misc.h"

#include "debug.h"
#include "debug-commands.h"

namespace gnuworld {

void CHANINFOCommand::Exec(const iClient* theClient, const std::string& Message) {
    StringTokenizer st(Message);
    if (st.size() < 2) {
        Usage(theClient);
        return;
    }

    Channel* theChan = Network->findChannel(st[1]);
    if (theChan == nullptr) {
        bot->Notice(theClient, "Unable to find channel {}", st[1]);
        return;
    }

    bot->Notice(theClient, "Channel: {}", theChan->getName());
    bot->Notice(theClient, "Modes: {} ({:#x})", theChan->getModeString(), theChan->getModes());
    bot->Notice(theClient, "Created at time: {} ({} ago)", theChan->getCreationTime(),
                prettyDuration(theChan->getCreationTime()));

    if (theChan->getMode(Channel::MODE_K) || !theChan->getKey().empty()) {
        bot->Notice(theClient, "Key: {}", theChan->getKey());
    }

    if (theChan->getMode(Channel::MODE_L) || theChan->getLimit() != 0) {
        bot->Notice(theClient, "Limit: {}", theChan->getLimit());
    }

    if (theChan->getMode(Channel::MODE_A) || !theChan->getApass().empty()) {
        bot->Notice(theClient, "Apass: {}", theChan->getApass());
    }

    if (theChan->getMode(Channel::MODE_U) || !theChan->getUpass().empty()) {
        bot->Notice(theClient, "Upass: {}", theChan->getUpass());
    }

#ifdef TOPIC_TRACK
    bot->Notice(theClient, "Topic: {}", theChan->getTopic());
    if (theChan->getTopicTS() != 0) {
        bot->Notice(theClient, "Topic set {} ago [{}] by {}", prettyDuration(theChan->getTopicTS()),
                    theChan->getTopicTS(), theChan->getTopicWhoSet());
    }
#endif

    int totalOps = 0;
    int totalVoice = 0;

    bot->Notice(theClient, "Users:");
    for (const auto& [id, theUser] : theChan->users()) {
        (void)id;
        if (theUser->isModeO())
            ++totalOps;
        if (theUser->isModeV())
            ++totalVoice;

        const char* tmpMode = "none: ";
        if (theUser->isModeO() && theUser->isModeV())
            tmpMode = "+o+v: ";
        else if (theUser->isModeO())
            tmpMode = "+o:   ";
        else if (theUser->isModeV())
            tmpMode = "+v:   ";

        bot->Notice(theClient, "  {}{}!{}@{} (numeric: {})", tmpMode, theUser->getNickName(),
                    theUser->getUserName(), theUser->getHostName(), theUser->getCharYYXXX());
    }

    bot->Notice(theClient, "Number of channel users: {} ({} ops, {} voice)", theChan->size(),
                totalOps, totalVoice);

    if (theChan->banList_size() == 0) {
        bot->Notice(theClient, "Ban list: (empty)");
        return;
    }

    bot->Notice(theClient, "Ban list ({}):", theChan->banList_size());
    std::string banLine;
    for (auto banItr = theChan->banList_begin(); banItr != theChan->banList_end(); ++banItr) {
        if (!banLine.empty() && (banLine.size() + banItr->size()) > 400) {
            bot->Notice(theClient, "  {}", banLine);
            banLine.clear();
        }
        if (!banLine.empty()) {
            banLine += ", ";
        }
        banLine += *banItr;
    }
    if (!banLine.empty()) {
        bot->Notice(theClient, "  {}", banLine);
    }
}

} // namespace gnuworld
