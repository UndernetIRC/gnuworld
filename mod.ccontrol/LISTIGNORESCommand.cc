/*
 * LISTIGNORESCommand.cc 
 *
 * Shows all ignored masks
 
 */

#include	<string>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"

const char LISTIGNORESCommand_cc_rcsId[] = "$Id: LISTIGNORESCommand.cc,v 1.6 2002/05/23 17:43:11 dan_karrels Exp $";

namespace gnuworld
{

using std::string ;
using std::endl ;

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

