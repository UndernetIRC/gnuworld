/* WHOAMI - Test command mainly */

#include "nickserv.h"

const char WHOAMICommand_cc_rcsId[] = "$Id: WHOAMICommand.cc,v 1.4 2002/08/27 20:55:53 jeekay Exp $";

namespace gnuworld
{

namespace ns
{

using std::string;

bool WHOAMICommand::Exec(iClient* theClient, const string& Message)
{
bot->theStats->incStat("NS.CMD.WHOAMI");

sqlUser* theUser = bot->isAuthed(theClient);

stringstream theResponse;
theResponse << "Nick: " << theClient->getNickName()
  << "; Account: " << theClient->getAccount();

if(theUser) {
  theResponse << "; Level: " << theUser->getLevel()
    << "; Flags: " << theUser->getFlags()
    << "; LogMask: " << theUser->getLogMask()
    ;
}

theResponse << ends;

bot->Notice(theClient, theResponse.str());

return true;
}

} // namespace ns

} // namespace gnuworld
