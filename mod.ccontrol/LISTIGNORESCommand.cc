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

const char LISTIGNORESCommand_cc_rcsId[] = "$Id: LISTIGNORESCommand.cc,v 1.4 2001/12/23 09:07:57 mrbean_ Exp $";

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
ccUser* tmpUser = bot->IsAuth(theClient);
if(tmpUser)
        bot->MsgChanLog("(%s) - %s : LISTIGNORES\n",tmpUser->getUserName().c_str()
                        ,theClient->getNickUserHost().c_str());
bot->listIgnores(theClient);
return true;
}

}
} // namespace gnuworld

