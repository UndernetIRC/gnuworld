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
#include	"Constants.h"

const char NEWPASSCommand_cc_rcsId[] = "$Id: NEWPASSCommand.cc,v 1.14 2001/12/13 09:10:35 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool NEWPASSCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st( Message ) ;
	
if(!dbConnected)
        {
        bot->Notice(theClient,"Sorry, but the db connection is down now, please try again alittle later");
        return false;
        }

if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

//Fetch the user authentication entry
ccUser *theUser = bot->IsAuth(theClient);

if(!theUser)
	{
        bot->Notice(theClient,"You have to be logged in to use this command");
	return false;
	}
unsigned int passRet = bot->checkPasswprd(st[1],theUser);
switch(passRet)
	{
	case password::TOO_SHORT:
		bot->Notice(theClient,"Password must be atleast %d chars",password::MIN_SIZE);
		break;
	case password::LIKE_UNAME:
		bot->Notice(theClient,"Password can't be like your username");
		break;
	case password::PASS_OK:
		{		
		theUser->setPassword(bot->CryptPass(st[1]));
		if(theUser->Update())
			{
			bot->Notice(theClient,"Password changed!");
			return true;
			}
		else
			{
			bot->Notice(theClient,"Error while changing password");
			return true;
			}
		}
	}
	
}	

}
}
