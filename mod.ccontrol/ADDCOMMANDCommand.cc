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

const char ADDCOMMANDCommand_cc_rcsId[] = "$Id: ADDCOMMANDCommand.cc,v 1.10 2001/07/20 09:09:31 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

bool ADDCOMMANDCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st( Message ) ;

if( st.size() < 3 )
	{
	Usage(theClient);
	return true;
	}

// Fetch the oper record from the db
ccUser* theUser = bot->GetOper(st[1]);
	
if( !theUser )
	{	
	bot->Notice( theClient,
		"I cant find oper %s",
		st[1].c_str());
	return false;
	}
	
int CommandLevel = bot->getCommandLevel(st[2]);

if( CommandLevel == -1 )
	{
	bot->Notice( theClient,
		"Command %s does not exist!",
		st[2].c_str());

	// TODO: Bug?  Even if it's not, this is an obvious design
	// error.
	delete theUser;

	return false;	        
	}

//CommandLevel &= ~flg_NOLOG;
CommandLevel = bot->getTrueAccess(CommandLevel);	
AuthInfo *AClient = bot->IsAuth( theClient );
if( NULL == AClient )
	{
	bot->Notice( theClient, "You must first authenticate" ) ;
	return true ;
	}

// Only allow opers who have access to that command to add it to new opers
if(!(AClient->getAccess() & CommandLevel ))
	{
	bot->Notice( theClient,
		"You must have access to a command inorder to add it");
	delete theUser;
	return false;
	}
	
//else if(theUser->getAccess() & CommandLevel)
else if(theUser->gotAccess(CommandLevel))	
	{
	bot->Notice( theClient,
		"%s already got access for %s",
		st[1].c_str(),
		st[2].c_str());
	delete theUser;
	return false;	        
	}

//Add the command and update the user db record	
theUser->addCommand(CommandLevel);
theUser->setLast_Updated_By(theClient->getNickUserHost());
if(theUser->Update())
	{
	bot->Notice( theClient,
		"Successfully added the command for %s",
		st[1].c_str());

	// If the user is authenticated update his authenticate entry
	bot->UpdateAuth(theUser); 
	delete theUser;
	return true;
	}
else
	{
	bot->Notice( theClient,
		"Error while adding command for %s",
		st[1].c_str());
	delete theUser;
	return false;
	}
	
}	
}
