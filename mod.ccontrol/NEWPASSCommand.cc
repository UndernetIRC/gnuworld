/*
 * NEWPASSCommand.cc
 *
 * Changes the user password
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"

const char NEWPASSCommand_cc_rcsId[] = "$Id: NEWPASSCommand.cc,v 1.7 2001/05/14 21:26:37 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;


bool NEWPASSCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st( Message ) ;
	
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

//Fetch the user authentication entry
AuthInfo *tmpUser = bot->IsAuth(theClient->getCharYYXXX());

if(!tmpUser)
	{
        bot->Notice(theClient,"You have to be logged in to use this command");
	return false;
	}

//Fetch the user record from the database	
//ccUser* theUser = bot->GetUser(tmpUser->Name);
ccUser* theUser = bot->GetOper(tmpUser->Name);

theUser->setPassword(bot->CryptPass(st[1]));
if(theUser->Update())
	{
	bot->Notice(theClient,"Password changed!");
	delete theUser;
	return true;
	}
	else
		{
		bot->Notice(theClient,"Error while changing password");
		delete tmpUser;
		return true;
		}
	}	
}
