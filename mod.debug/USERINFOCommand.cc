/**
 * USERINFOCommand.cc
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
#include <vector>

#include "Network.h"
#include "StringTokenizer.h"
#include "gnuworld_config.h"
#include "ip.h"
#include "misc.h"

#include "debug.h"
#include "debug-commands.h"

namespace gnuworld {
namespace {

bool isControlCode(unsigned char c) {
    return (c > 1 && c < 4) || c == 15 || (c > 27 && c < 33) || c == 22 || c == 160 ||
           (c > 252 && c <= 254);
}

std::string formatChannelEntry(Channel* theChannel, const iClient* Target) {
    std::string tChannel = theChannel->getName();
    const ChannelUser* theChannelUser = theChannel->findUser(Target);

    if (theChannelUser->getMode(ChannelUser::MODE_V))
        tChannel.insert(tChannel.begin(), '+');
    if (theChannelUser->getMode(ChannelUser::MODE_O))
        tChannel.insert(tChannel.begin(), '@');
    if (theChannel->getMode(Channel::MODE_S) || theChannel->getMode(Channel::MODE_P))
        tChannel.insert(tChannel.begin(), '!');

    std::string curChannel;
    bool hasCC = false;
    for (unsigned char c : tChannel) {
        if (isControlCode(c)) {
            hasCC = true;
            curChannel += "\x16^";
            curChannel += std::to_string(c);
            curChannel += "\x16";
        } else {
            curChannel += static_cast<char>(c);
        }
    }

    if (hasCC) {
        curChannel.insert(curChannel.begin(), '*');
    }
    return curChannel;
}

void noticeChannelList(debug* bot, const iClient* theClient,
                       const std::vector<std::string>& channels) {
    std::string chanNames;
    for (std::size_t i = 0; i < channels.size(); ++i) {
        if (!chanNames.empty() && (chanNames.size() + channels[i].size()) > 410) {
            bot->Notice(theClient, "On channels: %s", chanNames.c_str());
            chanNames.clear();
        }
        if (!chanNames.empty()) {
            chanNames += ", ";
        }
        chanNames += channels[i];
    }
    bot->Notice(theClient, "On channels: %s", chanNames.c_str());
}

void dumpUserInfo(debug* bot, const iClient* theClient, iClient* Target) {
    const std::string& nick = Target->getNickName();

    iServer* targetServer = Network->findServer(Target->getIntYY());
    if (targetServer == nullptr) {
        bot->Notice(theClient, "Unable to find server for numeric: %d", Target->getIntYY());
        return;
    }

    if (Target->isModeX() && Target->isModeR()) {
        bot->Notice(theClient, "%s is %s (%s) [%s]", nick.c_str(),
                    Target->getNickUserHost().c_str(), Target->getRealNickUserHost().c_str(),
                    xIP(Target->getIP()).GetNumericIP().c_str());
    } else {
        bot->Notice(theClient, "%s is %s [%s]", nick.c_str(), Target->getNickUserHost().c_str(),
                    xIP(Target->getIP()).GetNumericIP().c_str());
    }

    bot->Notice(theClient, "Realname: %s", Target->getDescription().c_str());
    bot->Notice(theClient, "Visible host: %s", Target->getInsecureHost().c_str());
    bot->Notice(theClient, "Real host: %s", Target->getRealInsecureHost().c_str());

#ifdef ASUKA
    if (!Target->getSetHost().empty()) {
        bot->Notice(theClient, "Set host: %s", Target->getSetHost().c_str());
    }
#endif

#ifdef SRVX
    if (!Target->getFakeHost().empty()) {
        bot->Notice(theClient, "Fake host: %s", Target->getFakeHost().c_str());
    }
#endif

    if (Target->getNickTS() != Target->getFirstNickTS()) {
        bot->Notice(theClient, "%s has used its current nickname for %s [since %ld]", nick.c_str(),
                    prettyDuration(Target->getNickTS()).c_str(), Target->getNickTS());
    }

    bot->Notice(theClient, "%s has been connected for %s [since %ld]", nick.c_str(),
                prettyDuration(Target->getFirstNickTS()).c_str(), Target->getFirstNickTS());

    if (Target->isModeR()) {
        std::string accountFlags;
        if (Target->getAccountFlag(iClient::X_TOTP_REQ_IPR) ||
            Target->getAccountFlag(iClient::X_TOTP_ENABLED)) {
            accountFlags +=
                Target->getAccountFlag(iClient::X_TOTP_REQ_IPR) ? "TOTP_REQ_IPR " : "TOTP ";

            std::string disabled;
            if (Target->getAccountFlag(iClient::X_WEB_DISABLE_TOTP))
                disabled += disabled.empty() ? "WEB" : ",WEB";
            if (Target->getAccountFlag(iClient::X_CERT_DISABLE_TOTP))
                disabled += disabled.empty() ? "CERT" : ",CERT";
            if (!disabled.empty())
                accountFlags += "(DISABLE=" + disabled + ") ";
        }

        if (Target->getAccountFlag(iClient::X_GLOBAL_SUSPEND))
            accountFlags += "SUSPENDED ";
        if (Target->getAccountFlag(iClient::X_FRAUD))
            accountFlags += "FRAUD ";
        if (Target->getAccountFlag(iClient::X_CERTONLY))
            accountFlags += "CERTONLY ";

        bot->Notice(theClient, "%s is authed as [%s]", nick.c_str(), Target->getAccount().c_str());
        bot->Notice(theClient, "Account ID: %u", Target->getAccountID());
        bot->Notice(theClient, "Account flags: 0x%04x", Target->getAccountFlags());
        if (!accountFlags.empty())
            bot->Notice(theClient, "Account flags (decoded): %s", accountFlags.c_str());
    } else {
        bot->Notice(theClient, "%s is not authed", nick.c_str());
    }

    bot->Notice(theClient, "Numeric: %s, UserModes: %s, Server Numeric: %s (%s)",
                Target->getCharYYXXX().c_str(), Target->getStringModes().c_str(),
                targetServer->getCharYY().c_str(), targetServer->getName().c_str());

    if (Target->isModeZ()) {
        bot->Notice(theClient, "%s is connected using TLS", nick.c_str());
        if (Target->hasTlsFingerprint())
            bot->Notice(theClient, "   Fingerprint: %s",
                        compactToCanonical(Target->getTlsFingerprint()).c_str());
    }

    if (Target->isOper())
        bot->Notice(theClient, "%s is an IRC operator", nick.c_str());

    if (Target->isFake())
        bot->Notice(theClient, "%s is a fake client", nick.c_str());

    if (Target->getMode(iClient::MODE_SERVICES)) {
        bot->Notice(theClient, "%s is a service agent", nick.c_str());
        return;
    }

    std::vector<std::string> channels;
    channels.reserve(Target->channels_size());
    for (Channel* theChannel : Target->channels()) {
        channels.push_back(formatChannelEntry(theChannel, Target));
    }

    if (channels.empty()) {
        bot->Notice(theClient, "On channels: (none)");
        return;
    }

    noticeChannelList(bot, theClient, channels);
    bot->Notice(theClient, "* - Channel contains control codes");
    bot->Notice(theClient, "! - Channel is +s or +p");
}

} // namespace

void USERINFOCommand::Exec(const iClient* theClient, const std::string& Message) {
    StringTokenizer st(Message);
    if (st.size() < 2) {
        Usage(theClient);
        return;
    }

    bool byNumeric = false;
    std::size_t argPos = 1;
    if (!strcasecmp(st[1], "-num")) {
        byNumeric = true;
        argPos = 2;
    }

    if (st.size() <= argPos) {
        Usage(theClient);
        return;
    }

    const std::string& target = st[argPos];
    iClient* Target = byNumeric ? Network->findClient(target) : Network->findNick(target);
    if (Target == nullptr) {
        bot->Notice(theClient, byNumeric ? "Unable to find numeric: %s" : "Unable to find nick: %s",
                    target.c_str());
        return;
    }

    dumpUserInfo(bot, theClient, Target);
}

} // namespace gnuworld
