/*
 * SUSPENDCommand.cc
 *
 * Set oper as suspended
 *
 */

#include	<string>
#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char SUSPENDCommand_cc_rcsId[] = "$Id: SUSPENDCommand.cc,v 1.2 2001/08/19 15:58:28 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool SUSPENDCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st( Message ) ;
	
if( st.size() < 4 )
	{
	Usage(theClient);
	return true;
	}
//Fetch the oper record from the database	
ccUser* tmpUser = bot->GetOper(st[1]);

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
	bot->Notice(theClient,"You cant suspend a user who got higher/equal level than yours");
	delete tmpUser;
	return false;
	}
else if(AdFlag < OpFlag)
	{
	bot->Notice(theClient,"You cant suspend a user who got higher level than yours");
	delete tmpUser;
	return false;
	}
if((Admin) && (strcasecmp(tmpAuth->getServer().c_str(),tmpUser->getServer().c_str())))
	{
	bot->Notice(theClient,"You can only suspend a user who's associated to the same server as you");
	return false;
	}

if(bot->isSuspended(tmpUser))
	{
	bot->Notice(theClient,"%s is already suspended",st[1].c_str());
	delete tmpUser;
	return false;
	}
//Fill in the suspendtion period according to the user entry	
if(!strcasecmp(st[3].c_str(),"s"))
	tmpUser->setSuspendExpires(atoi(st[2].c_str()));
else if(!strcasecmp(st[3].c_str(),"m"))
	tmpUser->setSuspendExpires(atoi(st[2].c_str()) * 60);
else if(!strcasecmp(st[3].c_str(),"h"))
	tmpUser->setSuspendExpires(atoi(st[2].c_str()) * 3600);
else if(!strcasecmp(st[3].c_str(),"d"))
	tmpUser->setSuspendExpires(atoi(st[2].c_str()) * 3600*24);
else
	{
	bot->Notice(theClient,"%s is not a proper time refrence");
	delete tmpUser;
	return false;
	}
//Set the suspention and update the db
tmpUser->setSuspendExpires(tmpUser->getSuspendExpires() + time( 0 ));
tmpUser->setSuspendedBy(theClient->getNickUserHost());	    
tmpUser->setIsSuspended(true);
	
if(tmpUser->Update())
	{
	bot->Notice(theClient,"%s has been suspended",st[1].c_str());
	bot->UpdateAuth(tmpUser);
	delete tmpUser;
	return true;
	}
else
	{
	bot->Notice(theClient,"Error while suspendeding %s",st[1].c_str());
	delete tmpUser;
	return false;
	}

}

}
} // namespace gnuworld

