/*
 * LISTCommand.cc
 *
 * Gives list of all kind of stuff to the oper
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char LISTCommand_cc_rcsId[] = "$Id: LISTCommand.cc,v 1.10 2003/04/28 09:44:20 mrbean_ Exp $";

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

bot->MsgChanLog("LIST %s\n",st.assemble(1).c_str());
if(!strcasecmp(st[1].c_str(),"glines"))
	{
	bot->listGlines(theClient);
	}
else if(!strcasecmp(st[1].c_str(),"suspended"))
	{
	bot->listSuspended(theClient);
	}
else if(!strcasecmp(st[1].c_str(),"servers"))
	{
	bot->listServers(theClient);
	}

else if(!strcasecmp(st[1].c_str(),"badchannels"))
	{
	bot->listBadChannels(theClient);
	}
else if(!strcasecmp(st[1].c_str(),"exceptions"))
	{
	bot->listExceptions(theClient);
	}
else
	{
	bot->Notice(theClient,"Unknown list");
	}

return true ;
}

}
}

