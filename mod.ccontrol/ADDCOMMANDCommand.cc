/*
 * ADDCOMMANDCommand.cc
 *
 * Adds a new command to an oper
 *     
 */
 
#include	<string>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include        "ccUser.h"
#include	"AuthInfo.h"
#include	"misc.h"

const char ADDCOMMANDCommand_cc_rcsId[] = "$Id: ADDCOMMANDCommand.cc,v 1.15 2001/07/30 16:58:39 mrbean_ Exp $";

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

unsigned int pos = 1;
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

// Fetch the oper record from the db
if(st[pos].size() > 64)
	{
	bot->Notice(theClient,"Oper name can't be more than 64 chars");
	return false;
	}

ccUser* theUser = bot->GetOper(st[pos]);
	
if( !theUser )
	{	
	bot->Notice( theClient,
		"I cant find oper %s",
		st[1].c_str());
	return false;
	}
pos++;	
//int CommandLevel = bot->getCommandLevel(st[pos]);
if(st[1].size() > 128)
	{
	bot->Notice(theClient,"Command name can't be more than 128 chars");
	return false;
	}
Command* Comm = bot->findCommandInMem(st[pos]);
if( !Comm )
	{
	bot->Notice( theClient,
		"Command %s does not exist!",
		st[pos].c_str());

	delete theUser;

	return false;	        
	}

AuthInfo *AClient = bot->IsAuth( theClient );
if( NULL == AClient )
	{
	bot->Notice( theClient, "You must first authenticate" ) ;
	return true ;
	}

// Only allow opers who have access to that command to add it to new opers
if(!AClient->gotAccess(Comm) )
	{
	bot->Notice( theClient,
		"You must have access to a command inorder to add it");
	delete theUser;
	return false;
	}
	
bool Admin = (AClient->getFlags()  < operLevel::SMTLEVEL);

if((Admin) && (AClient->getFlags() <= theUser->getType()))
	{
	bot->Notice(theClient,"You cant modify user who have a equal/higher access than you");
	return false;
	}
else if(!(Admin) && (AClient->getFlags() < theUser->getType()))
	{
	bot->Notice(theClient,"You cant modify user who have a higher access than you");
	return false;
	}
if((Admin) && (strcasecmp(AClient->getServer(),theUser->getServer())))
	{
	bot->Notice(theClient,"You can only modify a user who is associated with the same server as you");
	return false;
	}
if(Forced)
	{
	if((AClient->getFlags() < operLevel::SMTLEVEL) && ((bot->findCommandInMem(st[pos]))->getMinLevel() > theUser->getType()))
		{
		bot->Notice(theClient,"Only SMT+ can force the add of command");
		return false;
		}
	}
else if(Comm->getMinLevel() > theUser->getType())
	{
	if(AClient->getFlags() >= operLevel::SMTLEVEL)
		bot->Notice(theClient,
			    "The min level required to use this command is higher than the one the oper has, use \002-fr\002 if you stil want to add it");
	else
		bot->Notice(theClient,
			    "The min level required to use this command is higher than the one the oper has");
	return false;
	}
		
else if(theUser->gotAccess(Comm))	
	{
	bot->Notice( theClient,
		"%s already got access for %s",
		st[1].c_str(),
		st[2].c_str());
	delete theUser;
	return false;	        
	}

//Add the command and update the user db record	
theUser->addCommand(Comm);
theUser->setLast_Updated_By(theClient->getNickUserHost());
if(theUser->Update())
	{
	bot->Notice( theClient,
		"Successfully added the command for %s",
		st[pos-1].c_str());
	if(Forced)
		bot->MsgChanLog("%s is using -fr to add %s to %s"
				,theClient->getNickName().c_str(),st[pos].c_str()
				,st[pos-1].c_str());
	// If the user is authenticated update his authenticate entry
	bot->UpdateAuth(theUser); 
	delete theUser;
	return true;
	}
else
	{
	bot->Notice( theClient,
		"Error while adding command for %s",
		st[pos-1].c_str());
	delete theUser;
	return false;
	}
	
}	
}
}
