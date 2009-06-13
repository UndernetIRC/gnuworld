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
 * $Id: REMCOMMANDCommand.cc,v 1.16 2009/06/13 06:43:34 hidden1 Exp $
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

RCSTAG( "$Id: REMCOMMANDCommand.cc,v 1.16 2009/06/13 06:43:34 hidden1 Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool REMCOMMANDCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st( Message ) ;
ccUser* theUser;
if( st.size() < 3 )
	{
	Usage(theClient);
	return true;
	}

ccUser *AClient = bot->IsAuth( theClient );

if( NULL == AClient )
	{
	bot->Notice( theClient, "You must be authenticated to use this command." ) ;
	return true ;
	}

if(st[1].size() > 64)
	{
	bot->Notice(theClient,"Oper name can't be more than 64 characters");
	return false;
	}
//Fetch the user record from the database

bool AllOpers = false;
bool AllAdmins = false;
bool AllSmts = false;
bool AllCoders = false;
if(!strcasecmp(st[1],"-allopers"))
	{
	if (AClient->getType() < operLevel::CODERLEVEL) 
		{
		bot->Notice(theClient, "-allopers: This is coder level");
		return true;
		}
	AllOpers = true;
	}

else if(!strcasecmp(st[1],"-alladmins"))
	{
	if (AClient->getType() < operLevel::CODERLEVEL) 
		{
		bot->Notice(theClient, "-alladmins: This is coder level");
		return true;
		}
	AllAdmins = true;
	}

else if(!strcasecmp(st[1],"-allsmts"))
	{
	if (AClient->getType() < operLevel::CODERLEVEL) 
		{
		bot->Notice(theClient, "-allsmts: This is coder level");
		return true;
		}
	AllSmts = true;
	}

else if(!strcasecmp(st[1],"-allcoders"))
	{
	if (AClient->getType() < operLevel::CODERLEVEL) 
		{
		bot->Notice(theClient, "-allcoders: This is coder level");
		return true;
		}
	AllCoders = true;
	}
	
//ccUser* theUser = bot->GetUser(st[1]);
else
	theUser = bot->GetOper(bot->removeSqlChars(st[1]));
	
if (!AllOpers && !AllAdmins && !AllSmts && !AllCoders && !theUser)
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

bot->MsgChanLog("REMCOMMAND %s\n",st.assemble(1).c_str());
if(!strcasecmp(AClient->getUserName(),st[1]))
	{
	bot->Notice(theClient,"You can't remove your own command access!");
	return false;
	}

list<ccUser*> ccList;
if (!AllOpers && !AllAdmins && !AllSmts && !AllCoders)
	{
	ccList.push_back(theUser);
	}
else
	{
	ccontrol::usersConstIterator uItr = bot->usersMap_begin();
	for (; uItr != bot->usersMap_end(); uItr++)
		{
		if (AllCoders)
			{
			ccList.push_back(uItr->second);
			}
		else if ((AllSmts) && (uItr->second->getType() <= operLevel::SMTLEVEL))
			{
			ccList.push_back(uItr->second);
			}
		else if ((AllAdmins) && (uItr->second->getType() <= operLevel::ADMINLEVEL))
			{
			ccList.push_back(uItr->second);
			}
		else if ((AllOpers) && (uItr->second->getType() <= operLevel::OPERLEVEL))
			{
			ccList.push_back(uItr->second);
			}
		}
	}

int count = 0;
bool sentOnce = false;
for (list<ccUser*>::iterator Itr = ccList.begin(); Itr != ccList.end(); Itr++)
	{
    theUser = *Itr;

	bool Admin = (AClient->getType()  < operLevel::SMTLEVEL);

	if((Admin) && (AClient->getType() <= theUser->getType()))
		{
		bot->Notice(theClient,"You can't modify a user who has an equal or higher "
			"access level to your own.");
		return false;
		}
	else if(!(Admin) && (AClient->getType() < theUser->getType()))
		{
		bot->Notice(theClient,"You can't modify a user who has a higher access "
			"level than you.");
		return false;
		}
	if((Admin) && (strcasecmp(AClient->getServer(),theUser->getServer())))
		{
	bot->Notice(theClient,"You can only modify a user who is associated with the same server as you");
		return false;
		}
	if ((!strcasecmp(st[2],"ADDCOMMAND")) && (theUser->getType() == operLevel::CODERLEVEL))
		{
		if (!sentOnce)
			bot->Notice(theClient, "You can't remove the ADDCOMMAND command from a coder.");
		sentOnce = true;
		if (!AllOpers && !AllAdmins && !AllSmts && !AllCoders)
			return false;
		else
			continue;
		}
	

	if(!(theUser->gotAccess(Comm)))
		{
		if (!AllOpers && !AllAdmins && !AllSmts && !AllCoders)
			{
			bot->Notice(theClient,"%s doesn't have access to %s",theUser->getUserName().c_str(),st[2].c_str());
			return false;
			}
		else
			continue;
		}	
	//Remove the command 	
	theUser->removeCommand(Comm);
	theUser->setLast_Updated_By(bot->removeSqlChars(theClient->getRealNickUserHost()));
	if(theUser->Update())
		{
		bot->Notice(theClient,"Successfully removed the command from %s",theUser->getUserName().c_str());
		count++;
		if (!AllOpers && !AllAdmins && !AllSmts && !AllCoders)
			return true;
		else
			continue;
		}
	else
		{
		bot->Notice(theClient,"Error while removing command from %s",theUser->getUserName().c_str());
		if (!AllOpers && !AllAdmins && !AllSmts && !AllCoders)
			return false;
		else
			continue;
		}
	
	}
bot->Notice(theClient, "Removed %s access from %d users", Comm->getName().c_str(), count);
return true;

}

}
}
