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

const char NEWPASSCommand_cc_rcsId[] = "$Id: NEWPASSCommand.cc,v 1.16 2001/12/23 09:07:57 mrbean_ Exp $";

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
unsigned int passRet = bot->checkPassword(st[1],theUser);
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
        		bot->MsgChanLog("(%s) - %s : Changed Password \n",theUser->getUserName().c_str()
                        		,theClient->getNickUserHost().c_str());
			return true;
			}
		else
			{
			bot->Notice(theClient,"Error while changing password");
        		bot->MsgChanLog("Error while changing password for (%s) - %s\n",theUser->getUserName().c_str()
                        		,theClient->getNickUserHost().c_str(),st.assemble(1).c_str());
			return true;
			}
		}
	}
	
}	

}
}
