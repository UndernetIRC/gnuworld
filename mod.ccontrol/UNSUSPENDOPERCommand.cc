#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char UNSUSPENDOPERCommand_cc_rcsId[] = "$Id: UNSUSPENDOPERCommand.cc,v 1.4 2001/02/25 19:52:06 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;


bool UNSUSPENDOPERCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st( Message ) ;
	
if( st.size() < 2 )
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
	
if(!(tmpUser->getFlags() & isSUSPENDED))
	{
	bot->Notice(theClient,"%s is not suspended",st[1].c_str());
	return false;
	}
	
tmpUser->setSuspendExpires(0);
tmpUser->removeFlag(isSUSPENDED);
tmpUser->setSuspendedBy("");
	
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
	return false;
	}

}
}
