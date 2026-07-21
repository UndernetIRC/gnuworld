/*
 * HELLOCommand.cc
 *
 * Allows an opered user to authenticate with an account name
 * using UserLogin().
 *
 * This command is only available if enableHello is set to 1 in
 * the configuration file.
 */

#include "StringTokenizer.h"
#include "dronescan.h"
#include "dronescanCommands.h"

namespace gnuworld {

namespace ds {

void HELLOCommand::Exec(const iClient* theClient, const string& Message, const sqlUser*) {
    /* Check if HELLO command is enabled */
    if (!bot->isHelloEnabled()) {
        bot->Reply(theClient, "The HELLO command is not enabled.");
        return;
    }

    StringTokenizer st(Message);

    /* Usage:
     *  HELLO <account>
     */

    if (st.size() != 2) {
        bot->Reply(theClient, "Usage: HELLO <account>");
        return;
    }

    const string accountName = st[1];

    /* Check if the user is already authenticated */
    if (!theClient->getAccount().empty()) {
        bot->Reply(theClient, "You are already authenticated as %s", 
                   theClient->getAccount().c_str());
        return;
    }

    /* Perform the authentication using UserLogin */
    bot->getUplink()->UserLogin(
        const_cast<iClient*>(theClient),
        accountName,
        0,
        0,
        bot
    );

    bot->Reply(theClient, "Successfully authenticated as %s", accountName.c_str());

    return;
}

} // namespace ds

} // namespace gnuworld
