/*
 * LISTIGNORESCommand.cc 
 *
 * Shows all ignored masks
 
 */

#include	<string>
#include	<strstream>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"

const char LISTIGNORESCommand_cc_rcsId[] = "$Id: LISTIGNORESCommand.cc,v 1.1 2001/05/31 18:17:59 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;
using std::endl ;
using std::strstream ;
using std::ends ;


bool LISTIGNORESCommand::Exec( iClient* theClient, const string& Message)
{
bot->listIgnores(theClient);
return true;
}
}