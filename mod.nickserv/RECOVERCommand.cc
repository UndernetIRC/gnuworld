/*
 * RECOVERCommand.cc
 *
 * Allows a user to kill off another user that is
 * using their registered nickname.
 */

#include "Network.h"

#include "nickserv.h"

const char RECOVERCommand_cc_rcsId[] = "$Id: RECOVERCommand.cc,v 1.1 2002/08/25 16:21:44 jeekay Exp $";

namespace gnuworld
{

namespace ns
{

using std::string;

bool RECOVERCommand::Exec(iClient* theClient, const string& Message)
{
string authedNick = theClient->getAccount();

iClient* targetClient = Network->findNick(authedNick);

if(!targetClient) {
  bot->Notice(theClient, "Unable to find the nick %s.",
    authedNick.c_str());
  return true;
}

if(theClient == targetClient) {
  bot->Notice(theClient, "Recovering yourself is a bad idea.");
  return true;
}

sqlUser* theUser = bot->isRegistered(authedNick);
if(theUser && !theUser->getFlag(sqlUser::F_RECOVER)) {
  bot->Notice(theClient, "This user has disabled use of the RECOVER command.");
  return true;
}

bot->Kill(targetClient, "Nick recovered by " + theClient->getNickName());

bot->Notice(theClient, "Nick successfully recovered.");

return true;
}

} // namespace ns

} // namespace gnuworld
