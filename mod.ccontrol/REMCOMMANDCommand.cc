/**
 * REMCOMMANDCommand.cc
 * Removes a command from oper access list
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
 * $Id: REMCOMMANDCommand.cc,v 1.15 2006/09/26 17:36:00 kewlio Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include 	"ccUser.h"
#include	"misc.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: REMCOMMANDCommand.cc,v 1.15 2006/09/26 17:36:00 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool REMCOMMANDCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st( Message ) ;

if( st.size() < 3 )
	{
	Usage(theClient);
	return true;
	}

if(st[1].size() > 64)
	{
	bot->Notice(theClient,"Oper name can't be more than 64 characters");
	return false;
	}
//Fetch the user record from the database
//ccUser* theUser = bot->GetUser(st[1]);
ccUser* theUser = bot->GetOper(bot->removeSqlChars(st[1]));
	
if(!theUser)
	{	
	bot->Notice(theClient,"I can't find oper %s",st[1].c_str());
	return false;
	}
	
if(st[2].size() > 128)
	{
	bot->Notice(theClient,"Command name can't be more than 128 characters");
	return false;
	}
Command* Comm = bot->findCommandInMem(st[2]);
if(!Comm)
	{
	bot->Notice(theClient,"Command %s does not exist!",st[2].c_str());
	return false;	        
	}

ccUser* tempUser = bot->IsAuth(theClient);

if(!tempUser)
	{ //we should never get here
	return false;
	}
bot->MsgChanLog("REMCOMMAND %s\n",st.assemble(1).c_str());
if(!strcasecmp(tempUser->getUserName(),st[1]))
	{
	bot->Notice(theClient,"You can't remove your own command access!");
	return false;
	}

bool Admin = (tempUser->getType()  < operLevel::SMTLEVEL);

if((Admin) && (tempUser->getType() <= theUser->getType()))
	{
	bot->Notice(theClient,"You can't modify a user who has an equal or higher "
		"access level to your own.");
	return false;
	}
else if(!(Admin) && (tempUser->getType() < theUser->getType()))
	{
	bot->Notice(theClient,"You can't modify a user who has a higher access "
		"level than you.");
	return false;
	}
if((Admin) && (strcasecmp(tempUser->getServer(),theUser->getServer())))
	{
	bot->Notice(theClient,"You can only modify a user who is associated with the same server as you");
	return false;
	}
	

if(!(theUser->gotAccess(Comm)))
	{
	bot->Notice(theClient,"%s doesn't have access to %s",st[1].c_str(),st[2].c_str());
	return false;	        
	}	
//Remove the command 	
theUser->removeCommand(Comm);
theUser->setLast_Updated_By(bot->removeSqlChars(theClient->getRealNickUserHost()));
if(theUser->Update())
	{
	bot->Notice(theClient,"Successfully removed the command from %s",st[1].c_str());
	return true;
	}
else
	{
	bot->Notice(theClient,"Error while removing command from %s",st[1].c_str());
	return false;
	}
	
}	

}
}
