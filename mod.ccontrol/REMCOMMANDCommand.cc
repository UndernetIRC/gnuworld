/*
 * REMCOMMANDCommand.cc
 *
 * Removes a command from oper access list
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include 	"ccUser.h"
#include	"misc.h"

const char REMCOMMANDCommand_cc_rcsId[] = "$Id: REMCOMMANDCommand.cc,v 1.10 2002/03/01 18:27:36 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool REMCOMMANDCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st( Message ) ;

if(!dbConnected)
        {
        bot->Notice(theClient,"Sorry, but the db connection is down now, please try again alittle later");
        return false;
        }

if( st.size() < 3 )
	{
	Usage(theClient);
	return true;
	}

if(st[1].size() > 64)
	{
	bot->Notice(theClient,"Oper name can't be more than 64 chars");
	return false;
	}
//Fetch the user record from the database
//ccUser* theUser = bot->GetUser(st[1]);
ccUser* theUser = bot->GetOper(bot->removeSqlChars(st[1]));
	
if(!theUser)
	{	
	bot->Notice(theClient,"I cant find oper %s",st[1].c_str());
	return false;
	}
	
if(st[2].size() > 128)
	{
	bot->Notice(theClient,"Command name can't be more than 128 chars");
	return false;
	}
Command* Comm = bot->findCommandInMem(st[2]);
if(!Comm)
	{
	bot->Notice(theClient,"Command %s does not exists!",st[2].c_str());
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
	bot->Notice(theClient,"I dont know about you, but i for one dont think removing your own command is such a good idea ... ");
	return false;
	}

bool Admin = (tempUser->getType()  < operLevel::SMTLEVEL);

if((Admin) && (tempUser->getType() <= theUser->getType()))
	{
	bot->Notice(theClient,"You cant modify user who have a equal/higher access than you");
	return false;
	}
else if(!(Admin) && (tempUser->getType() < theUser->getType()))
	{
	bot->Notice(theClient,"You cant modify user who have a higher access than you");
	return false;
	}
if((Admin) && (strcasecmp(tempUser->getServer(),theUser->getServer())))
	{
	bot->Notice(theClient,"You can only modify a user who is associated with the same server as you");
	return false;
	}
	

if(!(theUser->gotAccess(Comm)))
	{
	bot->Notice(theClient,"%s doest have access for %s",st[1].c_str(),st[2].c_str());
	return false;	        
	}	
//Remove the command 	
theUser->removeCommand(Comm);
theUser->setLast_Updated_By(bot->removeSqlChars(theClient->getNickUserHost()));
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
