/*
 * STATUSCommand.cc
 *
 * Shows debug status
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip>
#include	"ccontrol.h"
#include	"CControlCommands.h"
//#include	"StringTokenizer.h"

const char STATUSCommand_cc_rcsId[] = "$Id: STATUSCommand.cc,v 1.5 2002/05/23 17:43:11 dan_karrels Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool STATUSCommand::Exec( iClient* theClient, const string& Message)
{	 

bot->MsgChanLog("STATUS\n");

bot->showStatus(theClient);
return true ;
}

}
}

