/**
 * UNSUSPENDCommand.cc
 * Unsuspend an oper
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
 * $Id: UNSUSPENDCommand.cc,v 1.13 2005/01/12 03:50:29 dan_karrels Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: UNSUSPENDCommand.cc,v 1.13 2005/01/12 03:50:29 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool UNSUSPENDCommand::Exec( iClient* theClient, const string& Message)
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

//Fetch the user record from the database	
//ccUser *tmpUser = bot->GetUser(st[1]);
ccUser* tmpUser = bot->GetOper(bot->removeSqlChars(st[1]));

if(!tmpUser)
	{
	bot->Notice(theClient,"%s isnt on my access list",st[1].c_str());
	return false;
	}
	
ccUser* tmpAuth = bot->IsAuth(theClient->getCharYYXXX());
if(!tmpAuth)
	{ //we should never get here
	return false;
	}
bot->MsgChanLog("UNSUSPEND %s\n",st.assemble(1).c_str());

unsigned int AdFlag = tmpAuth->getType(); //Get the admin flag
unsigned int OpFlag = tmpUser->getType(); //Get the oper flag
bool Admin = AdFlag < operLevel::SMTLEVEL;

if((Admin) && (AdFlag <= OpFlag))
	{
	bot->Notice(theClient,"You cant unsuspend a user who got higher/equal level than yours");
	return false;
	}
else if(AdFlag < OpFlag)
	{
	bot->Notice(theClient,"You cant unsuspend a user who got higher level than yours");
	return false;
	}
if((Admin) && (strcasecmp(tmpAuth->getServer().c_str(),tmpUser->getServer().c_str())))
	{
	bot->Notice(theClient,"You can only unsuspend a user who's associated to the same server as you");
	return false;
	}
if(tmpUser->getSuspendLevel() > AdFlag)
	{
	bot->Notice(theClient,"The suspend level is set to a higher level than yours");
	return false;
	}
	
if(!(bot->isSuspended(tmpUser)))
	{
	bot->Notice(theClient,"%s is not suspended",st[1].c_str());
	return false;
	}

//Remove the suspention and update the database	
tmpUser->setSuspendExpires(0);
tmpUser->setIsSuspended(false);
tmpUser->setSuspendedBy("");
tmpUser->setSuspendReason("");
tmpUser->setSuspendLevel(0);	
if(tmpUser->Update())
	{
	bot->Notice(theClient,"%s has been unsuspended",st[1].c_str());
	return true;
	}
else
	{
	bot->Notice(theClient,"Error while unsuspendeding %s",st[1].c_str());
	return false;
	}

}

}
}
