/*
 * MODOPERCommand.cc
 *
 * Modify an oper database entry
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char MODOPERCommand_cc_rcsId[] = "$Id: MODOPERCommand.cc,v 1.6 2001/05/05 19:53:20 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;



bool MODOPERCommand::Exec( iClient* theClient, const string& Message)
{	 
StringTokenizer st( Message ) ;
	
if( ((st.size() < 4) && (strcasecmp(st[2].c_str(),"getlogs") != 0)) 
|| ((st.size() < 3) && (!strcasecmp(st[2].c_str(),"getlogs"))))
	{
	Usage(theClient);
	return true;
	}
//Fetch the oper data base entry
ccUser *tmpUser = bot->GetUser(st[1]);
if(!tmpUser)
	{
        bot->Notice(theClient,"%s isnt on my access list",st[1].c_str());
        return false;
	}
//Check if the user got a higher or equal flags than the one he's trying to edit	
AuthInfo* tmpAuth = bot->IsAuth(theClient->getCharYYXXX());
if((tmpAuth->Flags & ~getLOGS) < (tmpUser->getFlags() & ~getLOGS))
	{
	bot->Notice(theClient,"You cant modify a user who got higher level than yours");
	delete tmpUser;
	return false;
	}
else if(!strcasecmp(st[2].c_str(),"newpass")) //Trying to change the password ?
	{
	tmpUser->setPassword(bot->CryptPass(st[3]));
	tmpUser->setLast_Updated_By(theClient->getNickUserHost());
	if(tmpUser->Update())
		{
		bot->Notice(theClient,"Password for %s Changed to %s",st[1].c_str(),st[3].c_str());
		}
	else
		{
		bot->Notice(theClient,"Error while changing password for %s",st[1].c_str());
		}
	}
else if(!strcasecmp(st[2].c_str(),"addhost")) //Trying to add a new host ?
	{
	if(!bot->validUserMask(st[3]))
		{
		bot->Notice(theClient,"Mask %s is not a valid mask in the form of *!*@*",st[3].c_str());
		}
	else if(bot->UserGotHost(tmpUser,st[3]) || bot->UserGotMask(tmpUser,st[3]))
		{
		bot->Notice(theClient,"%s already got the host %s covered by other mask",st[1].c_str(),st[3].c_str());
		}
	else if(bot->AddHost(tmpUser,st[3]))
		{
		bot->Notice(theClient,"Mask %s added for %s",st[3].c_str(),st[1].c_str());
		}
	else
		{
		bot->Notice(theClient,"Error while adding mask %s  for %s",st[3].c_str(),st[1].c_str());
		}
	}
else if(!strcasecmp(st[2].c_str(),"delhost")) //Trying to delete an host ?
	{
	if(!bot->UserGotHost(tmpUser,st[3]))
		{
		bot->Notice(theClient,"%s doesnt havee the host %s in my access list",st[1].c_str(),st[3].c_str());
		}
	else if(bot->DelHost(tmpUser,st[3]))
		{
		bot->Notice(theClient,"Mask %s was deleted from %s access list",st[3].c_str(),st[1].c_str());
		}
	else
		{
		bot->Notice(theClient,"Error while deleting mask %s from %s access list",st[3].c_str(),st[1].c_str());
		}
	}	    
else if(!strcasecmp(st[2].c_str(),"getlogs")) //Trying to toggle the get of logs
	{
	if(!tmpUser->gotFlag(getLOGS))
		{
		tmpUser->setFlags(getLOGS);
		bot->Notice(theClient,"getLogs have been turned on for %s",st[1].c_str());
		}
	else
		{
		tmpUser->removeFlag(getLOGS);
		bot->Notice(theClient,"getLogs have been turned off for %s",st[1].c_str());
		}
	tmpUser->Update();
	bot->UpdateAuth(tmpUser);
	}	
else
	{
	bot->Notice(theClient,"Unknown option %s",st[2].c_str());
	}
	
delete tmpUser;
return true;
}		    				
}
