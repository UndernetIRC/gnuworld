#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char SUSPENDOPERCommand_cc_rcsId[] = "$Id: SUSPENDOPERCommand.cc,v 1.4 2001/02/25 19:52:06 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;


bool SUSPENDOPERCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st( Message ) ;
	
if( st.size() < 4 )
	{
	Usage(theClient);
	return true;
	}
	
ccUser *tmpUser = bot->GetUser(st[1]);
if(!tmpUser)
	{
	bot->Notice(theClient,"%s isnt on my access list",st[1].c_str());
	return false;
	}
	
if(tmpUser->getFlags() & isSUSPENDED)
	{
	bot->Notice(theClient,"%s is already suspended",st[1].c_str());
	delete tmpUser;
	return false;
	}
	
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
tmpUser->setSuspendExpires(tmpUser->getSuspendExpires() + time( 0 ));
tmpUser->setSuspendedBy(theClient->getNickUserHost());	    
tmpUser->setFlag(isSUSPENDED);
	
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