/* WHOAMI - Test command mainly */

#include "nickserv.h"

const char WHOAMICommand_cc_rcsId[] = "$Id: WHOAMICommand.cc,v 1.3 2002/08/25 23:12:28 jeekay Exp $";

namespace gnuworld
{

namespace ns
{

using std::string;

bool WHOAMICommand::Exec(iClient* theClient, const string& Message)
{
bot->theStats->incStat("NS.CMD.WHOAMI");

string myString("You are you!");
bot->Notice(theClient, myString);

return true;
}

} // namespace ns

} // namespace gnuworld
