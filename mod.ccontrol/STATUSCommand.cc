/*
 * STATUSCommand.cc
 *
 * Shows debug status
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
//#include	"StringTokenizer.h"

const char STATUSCommand_cc_rcsId[] = "$Id: STATUSCommand.cc,v 1.4 2002/03/01 18:27:36 mrbean_ Exp $";

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

