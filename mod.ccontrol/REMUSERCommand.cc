/**
 * REMUSERCommand.cc
 * Removes an oper from the bot access list
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
 * $Id: REMUSERCommand.cc,v 1.14 2006/09/26 17:36:01 kewlio Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include        "Network.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: REMUSERCommand.cc,v 1.14 2006/09/26 17:36:01 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool REMUSERCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st( Message ) ;

if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}
//Fetch the user record from the database
ccUser* theUser = bot->GetOper(bot->removeSqlChars(st[1]));
if (!theUser) 
	{ 
	bot->Notice(theClient,"Oper %s does not exist in database, " 
		"check your handle and try again",st[1].c_str());
	return false;
	}
ccUser* tempAuth = bot->IsAuth(theClient);

if(!tempAuth)
	{ //we should never get here
	return false;
	}
if((tempAuth->getType() <= theUser->getType()) 
    && (tempAuth->getType() < operLevel::CODERLEVEL))
	{
	bot->Notice(theClient,"You can't remove an oper who has a higher or equal "
		" access level to you.");
	return false;
	}
if(((tempAuth->getType() < operLevel::SMTLEVEL)) &&
    (strcasecmp(tempAuth->getServer(),theUser->getServer())))
	{
	bot->Notice(theClient,"You can't remove opers from other servers");
	return false;
	}
bot->MsgChanLog("REMUSER %s\n",st.assemble(1).c_str());

if(bot->DeleteOper(string_lower(st[1])))     
	{    
	bot->Notice(theClient,"Successfully deleted oper: %s",st[1].c_str());
	
	//Check if the user is authenticate 
	if(theUser->getClient())
		{
		//Get hte user iClient entry from the network , and notify him that he was deleted
		const iClient *TClient = theUser->getClient(); 
		if(TClient)
			bot->Notice(TClient,"You have been removed from my access list, "
				"and have been deauthenticated.");
		//Remove the user authenticate entry
		bot->deAuthUser(theUser);
		}	
	delete theUser;
	return true;	
	}
else
	{    
	bot->Notice(theClient,"Error while deleting oper: %s",st[1].c_str());
	return false;	
	}
}

}
}

