/* WHOAMI - Test command mainly */

#include "nickserv.h"

const char WHOAMICommand_cc_rcsId[] = "$Id: WHOAMICommand.cc,v 1.1 2002/08/15 20:46:45 jeekay Exp $";

namespace gnuworld
{

namespace ns
{

bool WHOAMICommand::Exec(iClient* theClient, const string& Message)
{
string myString("You are you!");
bot->Notice(theClient, myString);

return true;
}

} // namespace ns

} // namespace gnuworld
