/*
 * DEAUTHCommand.cc
 *
 * Deauthenticates a user
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char DEAUTHCommand_cc_rcsId[] = "$Id: DEAUTHCommand.cc,v 1.4 2001/02/26 16:58:05 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

bool DEAUTHCommand::Exec( iClient* theClient, const string& Message)
{
//Fetch the user authenticate entry
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
