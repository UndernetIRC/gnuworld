/* WHOAMI - Test command mainly */

#include "nickserv.h"

const char WHOAMICommand_cc_rcsId[] = "$Id: WHOAMICommand.cc,v 1.2 2002/08/16 21:37:32 jeekay Exp $";

namespace gnuworld
{

namespace ns
{

using std::string;

bool WHOAMICommand::Exec(iClient* theClient, const string& Message)
{
string myString("You are you!");
bot->Notice(theClient, myString);

return true;
}

} // namespace ns

} // namespace gnuworld
