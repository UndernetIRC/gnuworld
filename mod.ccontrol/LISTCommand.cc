/*
 * LISTCommand.cc
 *
 * Gives list of all kind of stuff to the oper
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char LISTCommand_cc_rcsId[] = "$Id: LISTCommand.cc,v 1.5 2001/12/23 09:07:57 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool LISTCommand::Exec( iClient* theClient, const string& Message)
{	 

StringTokenizer st( Message ) ;
if(st.size() < 2)
	{
	Usage(theClient);
	return true;
	}
ccUser* tmpUser = bot->IsAuth(theClient);
if(tmpUser)
        bot->MsgChanLog("(%s) - %s : LIST %s\n",tmpUser->getUserName().c_str()
                        ,theClient->getNickUserHost().c_str(),st.assemble(1).c_str());
if(!strcasecmp(st[1].c_str(),"glines"))
	{
	bot->listGlines(theClient);
	}
else if(!strcasecmp(st[1].c_str(),"suspended"))
	{
	bot->listSuspended(theClient);
	}
else
	{
	bot->Notice(theClient,"Unknown list");
	}

return true ;
}

}
}

