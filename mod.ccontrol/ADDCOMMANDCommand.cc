/**
 * ADDCOMMANDCommand.cc
 * Adds a new command to an oper
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
 * $Id: ADDCOMMANDCommand.cc,v 1.31 2009/07/25 18:12:34 hidden1 Exp $
 */
 
#include	<string>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include        "ccUser.h"
#include	"misc.h"

RCSTAG( "$Id: ADDCOMMANDCommand.cc,v 1.31 2009/07/25 18:12:34 hidden1 Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool ADDCOMMANDCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st( Message ) ;

if( st.size() < 3 )
	{
	Usage(theClient);
	return true;
	}

StringTokenizer::size_type pos = 1;
ccUser *AClient = bot->IsAuth( theClient );

if( NULL == AClient )
	{
	bot->Notice( theClient, "You must be authenticated to use this command." ) ;
	return true ;
	}

bool Forced = false;
if(!strcasecmp(st[pos],"-fr"))
	{
	Forced = true;
	pos++;
	if(st.size() < 4)
		{
		Usage(theClient);
		}
	}

bool AllOpers = false;
bool AllAdmins = false;
bool AllSmts = false;
bool AllCoders = false;
while ((!strcasecmp(st[pos],"-allopers")) || (!strcasecmp(st[pos],"-alladmins")) || (!strcasecmp(st[pos],"-allsmts")) || (!strcasecmp(st[pos],"-allcoders"))) {
if(!strcasecmp(st[pos],"-allopers"))
	{
	if (AClient->getType() < operLevel::CODERLEVEL) 
		{
		bot->Notice(theClient, "-allopers: This is coder level");
		return true;
		}
	pos++;
	AllOpers = true;
	if ((st.size() - pos) < 1)
		{
		Usage(theClient);
		}
	}

if(!strcasecmp(st[pos],"-alladmins"))
	{
	if (AClient->getType() < operLevel::CODERLEVEL) 
		{
		bot->Notice(theClient, "-alladmins: This is coder level");
		return true;
		}
	pos++;
	AllAdmins = true;
	if ((st.size() - pos) < 1)
		{
		Usage(theClient);
		}
	}

if(!strcasecmp(st[pos],"-allsmts"))
	{
	if (AClient->getType() < operLevel::CODERLEVEL) 
		{
		bot->Notice(theClient, "-allsmts: This is coder level");
		return true;
		}
	pos++;
	AllSmts = true;
	if ((st.size() - pos) < 1)
		{
		Usage(theClient);
		}
	}

if(!strcasecmp(st[pos],"-allcoders"))
	{
	if (AClient->getType() < operLevel::CODERLEVEL) 
		{
		bot->Notice(theClient, "-allcoders: This is coder level");
		return true;
		}
	pos++;
	AllCoders = true;
	if ((st.size() - pos) < 1)
		{
		Usage(theClient);
		}
	}
}

// Fetch the oper record from the db
ccUser* theUser;
if (!AllOpers && !AllAdmins && !AllSmts && !AllCoders)
	{	
	if (st[pos].size() > 64)
		{
		bot->Notice(theClient,"Oper name can't be more than 64 characters in length.");
		return false;
		}

	theUser = bot->GetOper(st[pos]);
	if( !theUser )
		{	
		bot->Notice( theClient,
			"I can't find oper %s",
			st[pos].c_str());
		return false;
		}
	pos++;
	}
else
	theUser = 0;

//int CommandLevel = bot->getCommandLevel(st[pos]);
if(st[pos].size() > 128)
	{
	bot->Notice(theClient,"Command name can't be more than 128 characters in length.");
	return false;
	}
Command* Comm = bot->findCommandInMem(st[pos]);
if( !Comm )
	{
	bot->Notice( theClient,
		"Command '%s' does not exist!",
		st[pos].c_str());
	return false;	        
	}

bot->MsgChanLog("ADDCOMMAND %s\n",st.assemble(1).c_str());

// Only allow opers who have access to that command to add it to new opers
if (!AClient->gotAccess(Comm) && AClient->getType() != operLevel::CODERLEVEL) // Coders can add any command
	{
	bot->Notice( theClient,
		"You must have access to a command in order to add it.");
	return false;
	}

list<ccUser*> ccList;
if (!AllOpers && !AllAdmins && !AllSmts && !AllCoders)
	{
	ccList.push_back(theUser);
	}
else
	{
	if (AllOpers)
		{
		if (AllAdmins)
			{
			bot->Notice(theClient, "No need to specify -alladmins if you specified -allopers");
			AllAdmins = false;
			}
		if (AllSmts)
			{
			bot->Notice(theClient, "No need to specify -allsmts if you specified -allopers");
			AllSmts = false;
			}
		if (AllCoders)
			{
			bot->Notice(theClient, "No need to specify -allcoders if you specified -allopers");
			AllCoders = false;
			}			
		}
	if (AllAdmins)
		{
		if (AllSmts)
			{
			bot->Notice(theClient, "No need to specify -allsmts if you specified -alladmins");
			AllSmts = false;
			}
		if (AllCoders)
			{
			bot->Notice(theClient, "No need to specify -allcoders if you specified -alladmins");
			AllCoders = false;
			}
		}
	if (AllSmts)
		{
		if (AllCoders)
			{
			bot->Notice(theClient, "No need to specify -allcoders if you specified -allsmts");
			AllCoders = false;
			}
		}

	ccontrol::usersConstIterator uItr = bot->usersMap_begin();
	for (; uItr != bot->usersMap_end(); uItr++)
		{
		if (AllOpers)
			{
			ccList.push_back(uItr->second);
			}
		else if ((AllAdmins) && (uItr->second->getType() >= operLevel::ADMINLEVEL))
			{
			ccList.push_back(uItr->second);
			}
		else if ((AllSmts) && (uItr->second->getType() >= operLevel::SMTLEVEL))
			{
			ccList.push_back(uItr->second);
			}
		else if ((AllCoders) && (uItr->second->getType() >= operLevel::CODERLEVEL))
			{
			ccList.push_back(uItr->second);
			}
		}
	}

bool sentOnce = false;
int count = 0;
for (list<ccUser*>::iterator Itr = ccList.begin(); Itr != ccList.end(); Itr++)
	{
    theUser = *Itr;

	bool Admin = false ;
	if(AClient->getType()  < operLevel::SMTLEVEL)
		Admin = true;
	else
		Admin = false;

	if((Admin) && (AClient->getType() <= theUser->getType()))
		{
		bot->Notice(theClient,"You can't modify a user who has an "
			"equal or higher access level than you");
		return false;
		}
	else if(!(Admin) && (AClient->getType() < theUser->getType()))
		{
		bot->Notice(theClient,"You can't modify a user who has a higher "
			"access level than you");
		return false;
		}
	if((Admin) && (strcasecmp(AClient->getServer(),theUser->getServer())))
		{
		bot->Notice(theClient,"You can only modify a user who is "
			"associated with the same server as you");
		return false;
		}
	if(Forced)
		{
		if((AClient->getType() < operLevel::SMTLEVEL) && ((bot->findCommandInMem(st[pos]))->getMinLevel() > theUser->getType()))
			{
			bot->Notice(theClient,"Only SMT+ can force the addition of a command");
			return false;
			}
		}
	else if (Comm->getMinLevel() > theUser->getType())
		{
		if (!sentOnce)
			{
			if (AClient->getType() >= operLevel::SMTLEVEL)
				bot->Notice(theClient,
					"The minimum level required to use this command "
					"is higher than the one the oper has, use "
					"\002-fr\002 if you still want to add it");
			else
				bot->Notice(theClient,
					"The minimum level required to use this command "
					"is higher than the one the oper has");
			sentOnce = true;
			}
		if (!AllOpers && !AllAdmins && !AllSmts && !AllCoders)
			return false;
		else
			continue;
		}
		
	if (theUser->gotAccess(Comm))
		{
		if (!AllOpers && !AllAdmins && !AllSmts && !AllCoders)
			{
			bot->Notice( theClient,
				"%s already has access to '%s'",
				theUser->getUserName().c_str(),
				st[pos].c_str());
				return false;
			}
		else
			continue;
		}

	//Add the command and update the user db record	
	theUser->addCommand(Comm);
	theUser->setLast_Updated_By(theClient->getRealNickUserHost());
	if(theUser->Update())
		{
		bot->Notice( theClient,
			"Successfully added the command for %s",
			theUser->getUserName().c_str());
		if(Forced)
			bot->MsgChanLog("%s is using -fr to add '%s' to %s"
					,theClient->getNickName().c_str(),st[pos].c_str()
					,theUser->getUserName().c_str());
		// If the user is authenticated update his authenticate entry
		count++;
		if (!AllOpers && !AllAdmins && !AllSmts && !AllCoders)
			return true;
		else
			continue;
		}

	bot->Notice( theClient, "Error while adding command for %s",
		theUser->getUserName().c_str());
	return false;
	}
bot->Notice(theClient, "Gave %s access to %d users", Comm->getName().c_str(), count);
return true;
}	
}
}
