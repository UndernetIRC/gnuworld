#include	<string>
#include	<cstdlib>
#include        <iomanip.h>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char DEAUTHCommand_cc_rcsId[] = "$Id: DEAUTHCommand.cc,v 1.3 2001/02/23 20:19:43 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

bool DEAUTHCommand::Exec( iClient* theClient, const string& Message)
{
AuthInfo* tmpUser = bot->IsAuth(theClient->getCharYYXXX());
if (!tmpUser) 
	{
	bot->Notice(theClient,"Your are not authenticated");
	return false;
	}
if(bot->deAuthUser(theClient->getCharYYXXX()))
	{
	bot->Notice(theClient,"DeAuthentication successfull");
	return true;
	}
else
	{
	bot->Notice(theClient,"DeAuthentication failed");
	return false;
	}
}	
}
