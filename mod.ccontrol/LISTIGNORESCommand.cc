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

const char LISTIGNORESCommand_cc_rcsId[] = "$Id: LISTIGNORESCommand.cc,v 1.3 2001/07/23 10:28:51 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;
using std::endl ;
using std::strstream ;
using std::ends ;

namespace uworld
{

bool LISTIGNORESCommand::Exec( iClient* theClient, const string& Message)
{
bot->listIgnores(theClient);
return true;
}

}
} // namespace gnuworld

