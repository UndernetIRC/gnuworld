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

const char SAYCommand_cc_rcsId[] = "$Id: SAYCommand.cc,v 1.1 2002/08/16 15:20:34 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool SAYCommand::Exec( iClient* theClient, const string& Message)
{	 

StringTokenizer st( Message ) ;
string Numeric;
if(st.size() < 3)
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
	bot->Notice(theClient,"First argument must be -s for server quote, or -b for bot quote");
	return true;
	}
	
bot->Write("%s %s",Numeric.c_str(),st.assemble(2).c_str());
return true ;
}

}
}

