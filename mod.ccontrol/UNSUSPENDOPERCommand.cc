#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char UNSUSPENDOPERCommand_cc_rcsId[] = "$Id: UNSUSPENDOPERCommand.cc,v 1.3 2001/02/23 20:19:43 mrbean_ Exp $";

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
	
User *tmpUser = bot->GetUser(st[1]);
if(!tmpUser)
	{
	bot->Notice(theClient,"%s isnt on my access list",st[1].c_str());
	return false;
	}
	
if(!(tmpUser->Flags & isSUSPENDED))
	{
	bot->Notice(theClient,"%s is not suspended",st[1].c_str());
	return false;
	}
	
tmpUser->SuspendExpires = 0;
tmpUser->Flags &= ~isSUSPENDED;
tmpUser->SuspendedBy ="";
	
if(bot->UpdateOper(tmpUser))
	{
	bot->Notice(theClient,"%s has been unsuspended",st[1].c_str());
	bot->UpdateAuth(tmpUser->Id);
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
