/*
 * ADDUSERCommand.cc
 *
 * Adds a new oper to the bot database
 * 
 * when adding a new oper one must specify access flags
 * 
 * the oper initial commands is defined by these flags 
 *
 * the defualt flags can be found in CControlCommands.h file
 */

#include	<string>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include        "ccUser.h"
#include	"misc.h"
#include	"commLevels.h"

const char ADDUSERCommand_cc_rcsId[] = "$Id: ADDUSERCommand.cc,v 1.4 2001/09/30 20:26:44 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool ADDUSERCommand::Exec( iClient* theClient, const string& Message)
{
 
StringTokenizer st( Message ) ;
if( st.size() < 4 )
	{
	Usage(theClient);
	return true;
	}

if(st[1].size() > 64)
	{
	bot->Notice(theClient,"Oper name can't be more than 64 chars");
	return false;
	}

// Try fetching the user data from the database, note this is
// the new user handle
ccUser* theUser = bot->GetOper(st[1]);

if (theUser)  
	{ 
	bot->Notice(theClient,"Oper %s already exsits in my db," 
		"please change the oper handle and try again",
		theUser->getUserName().c_str());
        delete theUser;
	return false;
	}

unsigned long int NewAccess = 0 ;
unsigned long int NewSAccess = 0 ;

unsigned int NewFlags = 0 ;

// TODO: use std::string::operator=(const char*) const
if(!strcasecmp(st[2],"coder"))
	{
	NewAccess = commandLevel::CODER;
	NewSAccess = commandLevel::SCODER;
	NewFlags = operLevel::CODERLEVEL;
	}
else if(!strcasecmp(st[2],"smt"))
	{
	NewAccess = commandLevel::SMT;
	NewSAccess = commandLevel::SSMT;
	NewFlags = operLevel::SMTLEVEL;
	}

else if(!strcasecmp(st[2],"admin"))
	{
	NewAccess = commandLevel::ADMIN;
	NewSAccess = commandLevel::SADMIN;
        NewFlags = operLevel::ADMINLEVEL;
	}
else if(!strcasecmp(st[2],"oper"))
	{
        NewAccess = commandLevel::OPER;
	NewSAccess = commandLevel::SOPER;
        NewFlags = operLevel::OPERLEVEL;
	}
else
	{
	bot->Notice(theClient,
		"Illegal oper type; types are: oper, admin, smt ,coder");
	return false;
	}	     	

AuthInfo *tOper = bot->IsAuth( theClient );
if( NULL == tOper )
	{
	bot->Notice( theClient,
		"You must first authenticate" ) ;
	return true ;
	}

//Make sure the new oper wont have a command the old one doesnt have enabled
NewAccess &= tOper->getAccess(); 
//NewAccess = bot->getTrueAccess(NewAccess);
//Check if the user doesnt try to add an oper with higher flag than he is
unsigned int OperFlags = tOper->getFlags();
if(OperFlags < operLevel::ADMINLEVEL)
	{
	bot->Notice(theClient,
		"Sorry, but only admins+ can add new opers");
	return false;
	}

if((OperFlags < operLevel::SMTLEVEL) && (OperFlags <= NewFlags))
	{
	bot->Notice(theClient,
		"Sorry, but you can't add an oper with higher or equal access to yours");
	return false;
	}
else if(OperFlags < NewFlags)
	{
	bot->Notice(theClient,
		"Sorry, but you can't add an oper with higher access than yours");
	return false;
	}


//Create the new user and update the database
theUser = new ccUser(bot->SQLDb);
theUser->setUserName(st[1]);
theUser->setPassword(bot->CryptPass(st[3]));
theUser->setAccess(NewAccess);
theUser->setSAccess(NewSAccess);
theUser->setType(NewFlags);
theUser->setLast_Updated_By(theClient->getNickUserHost());
theUser->setServer(tOper->getServer());
theUser->setNeedOp(true);
if(bot->AddOper(theUser) == true)
	bot->Notice(theClient, "Oper successfully Added.");
else
	bot->Notice(theClient, "Error while adding new oper.");
delete theUser;
return true; 
}
}
}
