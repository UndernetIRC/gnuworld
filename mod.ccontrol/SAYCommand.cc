/*
 * SAYCommand.cc
 *
 * Forces the bot to quote a command
 *
 */

#include	<string>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"

const char SAYCommand_cc_rcsId[] = "$Id: SAYCommand.cc,v 1.2 2002/08/27 19:22:06 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool SAYCommand::Exec( iClient* theClient, const string& Message)
{	 

StringTokenizer st( Message ) ;
string Numeric;
string Target;
if(st.size() < 4)
	{
	Usage(theClient);
	return true;
	}

bot->MsgChanLog("SAY %s\n",st.assemble(1).c_str());
if(!strcasecmp(st[1].c_str(),"-s"))
	{
	Numeric = bot->getUplink()->getCharYY();
	}
else if(!strcasecmp(st[1].c_str(),"-b"))
	{
	Numeric = bot->getCharYYXXX();
	}
else
	{
	bot->Notice(theClient,"First argument must be -s for server , or -b for bot");
	return true;
	}

if(!strcasecmp(st[2].substr(0,1),"#"))
	{
	if(!Network->findChannel(st[2]))
		{
		bot->Notice(theClient,"Sorry, but i cant find channel %s"
			    ,st[2].c_str());
		return true;
		}
	else
		{
		Target = st[2];
		}
	}
else	
	{
	iClient* tClient = Network->findNick(st[2]);
	if(!tClient)
		{
		bot->Notice(theClient,"Sorry, but i cant find Nick %s"
			    ,st[2].c_str());
		return true;
		}
	else
		{
		Target = tClient->getCharYYXXX();
		}
	}


bot->Write("%s P %s :%s",Numeric.c_str(),Target.c_str(),st.assemble(3).c_str());
return true ;
}

}
}

