/*
 * INVITECommand.cc
 *
 * Invite the user to the console channel.
 */

#include "Network.h"

#include "levels.h" 
#include "nickserv.h"
#include "responses.h"

const char INVITECommand_cc_rcsId[] = "$Id: INVITECommand.cc,v 1.1 2002/11/26 03:33:24 jeekay Exp $";

namespace gnuworld
{

namespace ns
{

using std::string;

bool INVITECommand::Exec(iClient* theClient, const string& Message)
{

bot->theStats->incStat("NS.CMD.INVITE");

sqlUser* theUser = bot->isAuthed(theClient);

if(!theUser || (theUser->getLevel() < level::admin::invite)) {
  bot->Notice(theClient, responses::noAccess);
  return true;
}

Channel* theChannel = Network->findChannel(bot->getConsoleChannel());

if(!theChannel) {
  bot->Notice(theClient, "Could not find the console channel on the network.");
  return true;
}

ChannelUser* theBot = theChannel->findUser(bot->getInstance());
if(!theBot) {
  bot->Notice(theClient, "I am not in the console channel.");
  return true;
}

ChannelUser* theChannelUser = theChannel->findUser(theClient);
if(theChannelUser) {
  bot->Notice(theClient, "You are already in the console channel!");
  return true;
}

bot->Invite(theClient, theChannel);

return true;
} // INVITECommand

} // namespace ns

} // namespace gnuworld
