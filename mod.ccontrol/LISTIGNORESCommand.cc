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

const char LISTIGNORESCommand_cc_rcsId[] = "$Id: LISTIGNORESCommand.cc,v 1.5 2002/03/01 18:27:36 mrbean_ Exp $";

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
bot->MsgChanLog("LISTIGNORES\n");
bot->listIgnores(theClient);
return true;
}

}
} // namespace gnuworld

