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

const char LISTIGNORESCommand_cc_rcsId[] = "$Id: LISTIGNORESCommand.cc,v 1.2 2001/07/17 16:58:27 dan_karrels Exp $";

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

} // namespace gnuworld

