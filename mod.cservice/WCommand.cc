/**
 * WCommand.cc
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
 */

#include <string>
#include <sstream>

#include "cservice.h"
#include "StringTokenizer.h"
#include "Network.h"
#include "ELog.h"
#include "responses.h"

#ifdef THERETURN_ENABLED

namespace gnuworld {
using std::endl;
using std::string;
using std::stringstream;

bool WCommand::Exec(iClient* theClient, const string& Message) {
    bot->incStat("COMMANDS.W");

    /* Check if the user is authed. */
    sqlUser* theUser = bot->isAuthed(theClient, true);
    if (!theUser) {
        return false;
    }

    /* Get admin access level. */
    unsigned int admLevel = bot->getAdminAccessLevel(theUser);

    int confLevel = bot->getConfigVar("THERETURN_INTEGRATION")->asInt();
    /* If config is 0, we stay silent. */
    if (confLevel == 0 && admLevel < 750) {
        return false;
    }

    /* if config is 1, we notify the user that W is currently unavailable. */
    else if (confLevel == 1 && admLevel < 1000) {
        bot->Notice(theClient, "%s is currently unavailable.", bot->getConfwNickName().c_str());
        return false;
    }

    StringTokenizer st(Message);
    if (st.size() < 3) {
        Usage(theClient);
        return false;
    }

    /* Check if the channel is registered with X. */
    sqlChannel* theChan = bot->getChannelRecord(st[2]);
    if (!theChan || theChan->getName() == "*") {
        bot->Notice(theClient, bot->getResponse(theUser, language::chan_not_reg).c_str(),
                    st[2].c_str());
        return false;
    }
    /* We don't process channels with a length of more than 45 characters. W is old, remember. */
    if (theChan->getName().length() > 45) {
        bot->Notice(theClient,
                    "Apologise, but %s don't accept channels with more than 45 characters.",
                    bot->getConfwNickName().c_str());
        return false;
    }

    /* Check that W is available. */
    iServer* wServer = Network->findServerName(bot->getConfwServerName());
    iClient* wClient = Network->findNick(bot->getConfwNickName());
    if (!wServer || !wClient) {
        bot->Notice(theClient, "%s is unavailable. Try again later.",
                    bot->getConfwNickName().c_str());
        return false;
    }

    Channel* tmpChan = Network->findChannel(theChan->getName());
    if (!tmpChan) {
        bot->Notice(theClient, bot->getResponse(theUser, language::chan_is_empty).c_str(),
                    theChan->getName().c_str());
        return false;
    }

    ChannelUser* wChanUser = tmpChan->findUser(wClient);
    unsigned int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
    string Command = string_upper(st[1]);

    /* Registration of channel. */
    if (Command == "JOIN") {
        /* Is the channel already registered? Order W to re-join. */
        if (theChan->hasW()) {
            /* Check access level. */
            if (level < 450) {
                bot->Notice(theClient, bot->getResponse(theUser, language::insuf_access).c_str(),
                            theChan->getName().c_str());
                return false;
            }

            /* Is W on the channel? */
            if (wChanUser) {
                bot->Notice(theClient, "%s is already on that channel.",
                            bot->getConfwNickName().c_str());
                return false;
            }

            /* We send a XQ to W. The callback will handle the rest. */
            bot->logPrivAdminMessage("%s (%s) has initiated a re-join of %s to %s",
                                     theClient->getNickName().c_str(),
                                     theClient->getAccount().c_str(),
                                     wClient->getNickName().c_str(), theChan->getName().c_str());

            stringstream xQuery;
            xQuery << bot->getCharYY() << " XQ " << wServer->getCharYY()
                   << " AnyCServiceRouting :JOIN " << theChan->getName() << " "
                   << theClient->getCharYYXXX() << endl;

            bot->Write(xQuery);
            return true;
        } else {
            /* Check access level. */
            if (level < 500) {
                bot->Notice(theClient, bot->getResponse(theUser, language::insuf_access).c_str(),
                            theChan->getName().c_str());
                return false;
            }

            /* We don't accept this command through FORCE. */
            if (bot->isForced(theChan, theUser)) {
                bot->Notice(theClient, "Channel registration in %s cannot be done through FORCE.",
                            bot->getConfwNickName().c_str());
                return false;
            }

            /* We send a XQ to W. The callback will handle the rest. */
            bot->logPrivAdminMessage("%s (%s) has initiated %s-registration of %s",
                                     theClient->getNickName().c_str(),
                                     theClient->getAccount().c_str(),
                                     wClient->getNickName().c_str(), theChan->getName().c_str());

            stringstream xQuery;
            xQuery << bot->getCharYY() << " XQ " << wServer->getCharYY()
                   << " AnyCServiceRouting :REG " << theChan->getName() << " "
                   << theClient->getCharYYXXX() << endl;

            bot->Write(xQuery);
            return true;
        }
    } else if (Command == "PURGE") {
        if (level < 500) {
            bot->Notice(theClient, bot->getResponse(theUser, language::insuf_access).c_str(),
                        theChan->getName().c_str());
            return false;
        }

        /* Is the channel registered? */
        if (!theChan->hasW()) {
            bot->Notice(theClient, "This channel is not registered with %s",
                        wClient->getNickName().c_str());
            return false;
        }

        /* We send a XQ to W. The callback will handle the rest. */
        bot->logPrivAdminMessage("%s (%s) has initiated %s-purge of %s",
                                 theClient->getNickName().c_str(), theClient->getAccount().c_str(),
                                 wClient->getNickName().c_str(), theChan->getName().c_str());

        stringstream xQuery;
        xQuery << bot->getCharYY() << " XQ " << wServer->getCharYY()
               << " AnyCServiceRouting :PURGE " << theChan->getName() << " "
               << theClient->getCharYYXXX() << endl;

        bot->Write(xQuery);

        return true;
    } else if (Command == "PART") {
        if (level < 450) {
            bot->Notice(theClient, bot->getResponse(theUser, language::insuf_access).c_str(),
                        st[1].c_str());
            return false;
        }

        /* Is the channel registered? */
        if (!theChan->hasW()) {
            bot->Notice(theClient, "This channel is not registered with %s",
                        wClient->getNickName().c_str());
            return false;
        }

        /* Is W on the channel? */
        if (!wChanUser) {
            bot->Notice(theClient, bot->getResponse(theUser, language::cant_find_on_chan).c_str(),
                        wClient->getNickName().c_str());
            return false;
        }

        /* We send a XQ to W. The callback will handle the rest. */
        bot->logPrivAdminMessage("%s (%s) has initiated %s-part of %s",
                                 theClient->getNickName().c_str(), theClient->getAccount().c_str(),
                                 wClient->getNickName().c_str(), theChan->getName().c_str());

        if (theChan->getFlag(sqlChannel::F_OPLOG))
            bot->NoticeChannelOps(theChan->getName(), "%s has requested that %s parts the channel",
                                  wClient->getNickName().c_str(), theClient->getNickName().c_str());

        stringstream xQuery;
        xQuery << bot->getCharYY() << " XQ " << wServer->getCharYY() << " AnyCServiceRouting :PART "
               << theChan->getName() << " " << theClient->getCharYYXXX() << endl;

        bot->Write(xQuery);

        return true;
    } else if (Command == "PURGE") {
        /* Admin? */
        if (admLevel < 750) {
            bot->Notice(
                theClient,
                bot->getResponse(theUser, language::insuf_access,
                                 string("You have insufficient access to perform that command.")));
            return false;
        }

        /* Is the channel registered? */
        if (!theChan->hasW()) {
            bot->Notice(theClient, "%s is not registered with %s", theChan->getName().c_str(),
                        wClient->getNickName().c_str());
            return false;
        }

        if (st.size() > 3 && st[3] == "-f") {
            theChan->setW(false);
            bot->Notice(theClient,
                        "%s has been flagged as not registered with %s. I have NOT purged the "
                        "channel with %s and this will no longer be possible through X.",
                        theChan->getName().c_str(), wClient->getNickName().c_str(),
                        wClient->getNickName().c_str());
            return true;
        } else {
            bot->Notice(theClient,
                        "%s is flagged as registered with %s in my records. To manually update my "
                        "records, use /msg X W PURGE %s -f to force unregistration. This should "
                        "ONLY be used if %s is already purged with %s",
                        theChan->getName().c_str(), wClient->getNickName().c_str(),
                        theChan->getName().c_str(), theChan->getName().c_str(),
                        wClient->getNickName().c_str());
            return true;
        }
    } else {
        Usage(theClient);
        return false;
    }

    return true;
}

} // namespace gnuworld

#endif // THERETURN_ENABLED
