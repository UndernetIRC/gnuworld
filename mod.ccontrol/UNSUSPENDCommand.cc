/*
 * UNSUSPENDCommand.cc
 *
 * Unsuspend an oper
 *
*/

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char UNSUSPENDCommand_cc_rcsId[] = "$Id: UNSUSPENDCommand.cc,v 1.5 2001/11/11 16:05:51 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool UNSUSPENDCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st( Message ) ;
	
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

//Fetch the user record from the database	
//ccUser *tmpUser = bot->GetUser(st[1]);
ccUser* tmpUser = bot->GetOper(bot->removeSqlChars(st[1]));

if(!tmpUser)
	{
	bot->Notice(theClient,"%s isnt on my access list",st[1].c_str());
	return false;
	}
	
AuthInfo* tmpAuth = bot->IsAuth(theClient->getCharYYXXX());
unsigned int AdFlag = tmpAuth->getFlags(); //Get the admin flag
unsigned int OpFlag = tmpUser->getType(); //Get the oper flag
bool Admin = AdFlag < operLevel::SMTLEVEL;

if((Admin) && (AdFlag <= OpFlag))
	{
	bot->Notice(theClient,"You cant unsuspend a user who got higher/equal level than yours");
	delete tmpUser;
	return false;
	}
else if(AdFlag < OpFlag)
	{
	bot->Notice(theClient,"You cant unsuspend a user who got higher level than yours");
	delete tmpUser;
	return false;
	}
if((Admin) && (strcasecmp(tmpAuth->getServer().c_str(),tmpUser->getServer().c_str())))
	{
	bot->Notice(theClient,"You can only unsuspend a user who's associated to the same server as you");
	delete tmpUser;
	return false;
	}
if(tmpUser->getSuspendLevel() > AdFlag)
	{
	bot->Notice(theClient,"The suspend level is set to a higher level than yours");
	delete tmpUser;
	return false;
	}
	
if(!(bot->isSuspended(tmpUser)))
	{
	bot->Notice(theClient,"%s is not suspended",st[1].c_str());
	delete tmpUser;
	return false;
	}

//Remove the suspention and update the database	
tmpUser->setSuspendExpires(0);
tmpUser->setIsSuspended(false);
tmpUser->setSuspendedBy("");
tmpUser->setSuspendReason("");
tmpUser->setSuspendLevel(0);	
if(tmpUser->Update())
	{
	bot->Notice(theClient,"%s has been unsuspended",st[1].c_str());
	bot->UpdateAuth(tmpUser);
	delete tmpUser;
	return true;
	}
else
	{
	bot->Notice(theClient,"Error while unsuspendeding %s",st[1].c_str());
	delete tmpUser;
	return false;
	}

}

}
}
