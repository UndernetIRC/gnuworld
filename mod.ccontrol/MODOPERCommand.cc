#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char MODOPERCommand_cc_rcsId[] = "$Id: MODOPERCommand.cc,v 1.3 2001/02/23 20:19:43 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;



bool MODOPERCommand::Exec( iClient* theClient, const string& Message)
{	 
StringTokenizer st( Message ) ;
	
if( st.size() < 4 )
	{
	Usage(theClient);
	return true;
	}

User *tmpUser = bot->GetUser(st[1]);
if(!tmpUser)
	{
        bot->Notice(theClient,"%s isnt on my access list",st[1].c_str());
        return false;
	}
	
AuthInfo* tmpAuth = bot->IsAuth(theClient->getCharYYXXX());
if(tmpAuth->Flags < tmpUser->Flags)
	{
	bot->Notice(theClient,"You cant modify a user who got higher level than yours");
	delete tmpUser;
	return false;
	}
else if(!strcasecmp(st[2].c_str(),"newpass"))
	{
	tmpUser->Password = bot->CryptPass(st[3]);
	tmpUser->last_updated_by = theClient->getNickUserHost();
	if(bot->UpdateOper(tmpUser))
		{
		bot->Notice(theClient,"Password for %s Changed to %s",st[1].c_str(),st[2].c_str());
		}
	else
		{
		bot->Notice(theClient,"Error while changing password for %s",st[1].c_str());
		}
	}
else if(!strcasecmp(st[2].c_str(),"addhost"))
	{
	if(!bot->validUserMask(st[3]))
		{
		bot->Notice(theClient,"Mask %s isnt not a valid mask in the form of *!*@*",st[3].c_str());
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
else if(!strcasecmp(st[2].c_str(),"delhost"))
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
else
	{
	bot->Notice(theClient,"Unknown option",st[3].c_str(),st[1].c_str());
	}
	
delete tmpUser;
return true;
}		    				
}
