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

const char STATUSCommand_cc_rcsId[] = "$Id: STATUSCommand.cc,v 1.3 2001/12/28 16:28:47 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool STATUSCommand::Exec( iClient* theClient, const string& Message)
{	 

ccUser* tmpUser = bot->IsAuth(theClient);
if(tmpUser)
        bot->MsgChanLog("(%s) - %s : STATUS\n",tmpUser->getUserName().c_str()
                        ,theClient->getNickUserHost().c_str());
bot->showStatus(theClient);
return true ;
}

}
}

