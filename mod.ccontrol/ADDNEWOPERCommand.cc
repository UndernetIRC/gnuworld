/*
 * ADDNEWOPERCommand.cc
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
#include        <iomanip.h>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char ADDNEWOPERCommand_cc_rcsId[] = "$Id: ADDNEWOPERCommand.cc,v 1.5 2001/02/26 16:58:05 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;


bool ADDNEWOPERCommand::Exec( iClient* theClient, const string& Message)
{
 
StringTokenizer st( Message ) ;
if( st.size() < 4 )
	{
	Usage(theClient);
	return true;
	}

//Try fetching the user data from the database, note this is the new user handle
ccUser* theUser = bot->GetUser(st[1]);
if (theUser)  
	{ 
	bot->Notice(theClient,"Oper %s already exsits in my db," 
	"please change the oper handle and try again",theUser->getUserName().c_str());
        delete theUser;
	return false;
	}	    

unsigned int NewAccess;
unsigned int NewFlags;
if(!strcasecmp(st[2].c_str(),"coder"))
	{
	NewAccess = CODER;
	NewFlags = isCODER;
	}
else if(!strcasecmp(st[2].c_str(),"admin"))
	{
	NewAccess = ADMIN;
        NewFlags = isADMIN;
	}
else if(!strcasecmp(st[2].c_str(),"oper"))
	{
        NewAccess = OPER;
        NewFlags = isOPER;
	}
else
	{
	bot->Notice(theClient,"Illegal oper type, Types are : oper , admin , coder");
	return false;
	}	     	

AuthInfo *tOper = bot->IsAuth(theClient->getCharYYXXX());


//Check if the user doesnt try to add an oper with higher flag than he is
if(tOper->Flags < NewFlags)
	{
	bot->Notice(theClient,"You can't add an oper with higher access than yours!");
	return false;
	}	     	

//Create the new user and update the database
theUser = new ccUser(bot->SQLDb);
theUser->setUserName(st[1]);
theUser->setPassword(bot->CryptPass(st[3]));
theUser->setAccess(NewAccess);
theUser->setFlags(NewFlags);
theUser->setLast_Updated_By(theClient->getNickUserHost());
if(bot->AddOper(theUser) == true)
	bot->Notice(theClient, "Oper successfully Added.");
else
	bot->Notice(theClient, "Error while adding new oper.");
delete theUser;
return true; 
}
}