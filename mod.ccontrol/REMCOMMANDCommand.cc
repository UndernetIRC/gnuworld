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

const char REMCOMMANDCommand_cc_rcsId[] = "$Id: REMCOMMANDCommand.cc,v 1.2 2001/07/29 13:33:20 mrbean_ Exp $";

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

//Fetch the user record from the database
//ccUser* theUser = bot->GetUser(st[1]);
ccUser* theUser = bot->GetOper(st[1]);
	
if(!theUser)
	{	
	bot->Notice(theClient,"I cant find oper %s",st[1].c_str());
	return false;
	}
	
Command* Comm = bot->findCommandInMem(st[2]);
if(!Comm)
	{
	bot->Notice(theClient,"Command %s does not exists!",st[2].c_str());
	delete theUser;
	return false;	        
	}
AuthInfo* tempUser = bot->IsAuth(theClient->getCharYYXXX());

if(!strcasecmp(tempUser->getName(),st[1]))
	{
	bot->Notice(theClient,"I dont know about you, but i for one dont think removing your own command is such a good idea ... ");
	delete theUser;
	return false;
	}

bool Admin = (tempUser->getFlags()  < operLevel::SMTLEVEL);

if((Admin) && (tempUser->getFlags() <= theUser->getType()))
	{
	bot->Notice(theClient,"You cant modify user who have a equal/higher access than you");
	return false;
	}
else if(!(Admin) && (tempUser->getFlags() < theUser->getType()))
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
	delete theUser;
	return false;	        
	}	
//Remove the command 	
theUser->removeCommand(Comm);
theUser->setLast_Updated_By(theClient->getNickUserHost());
if(theUser->Update())
	{
	bot->Notice(theClient,"Successfully removed the command from %s",st[1].c_str());
	bot->UpdateAuth(theUser);
	delete theUser;
	return true;
	}
else
	{
	bot->Notice(theClient,"Error while removing command from %s",st[1].c_str());
	delete theUser;
	return false;
	}
	
}	

}
}
