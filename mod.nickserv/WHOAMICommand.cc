/* WHOAMI - Test command mainly */

#include	<sstream>

#include "nickserv.h"

using std::stringstream ;
using std::ends ;

const char WHOAMICommand_cc_rcsId[] = "$Id: WHOAMICommand.cc,v 1.5 2002/09/05 16:02:42 dan_karrels Exp $";

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
