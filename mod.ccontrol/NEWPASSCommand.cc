/**
 * NEWPASSCommand.cc
 * Changes the user password
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 * $Id: NEWPASSCommand.cc,v 1.22 2005/01/12 03:50:29 dan_karrels Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"
#include	"Constants.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: NEWPASSCommand.cc,v 1.22 2005/01/12 03:50:29 dan_karrels Exp $" ) ;

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
                        		,theClient->getRealNickUserHost().c_str());
			return true;
			}
		else
			{
			bot->Notice(theClient,"Error while changing password");
        		bot->MsgChanLog("Error while changing password for (%s) - %s\n",theUser->getUserName().c_str()
                        		,theClient->getRealNickUserHost().c_str(),st.assemble(1).c_str());
			return true;
			}
		}
	}
return true;	
}	

}
}
