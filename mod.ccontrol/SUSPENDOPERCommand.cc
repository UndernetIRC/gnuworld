#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char SUSPENDOPERCommand_cc_rcsId[] = "$Id $";

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
	
User *tmpUser = bot->GetUser(st[1]);
if(!tmpUser)
	{
	bot->Notice(theClient,"%s isnt on my access list",st[1].c_str());
	return false;
	}
	
if(tmpUser->Flags & isSUSPENDED)
	{
	bot->Notice(theClient,"%s is already suspended",st[1].c_str());
	delete tmpUser;
	return false;
	}
	
if(!strcasecmp(st[3].c_str(),"s"))
	tmpUser->SuspendExpires = atoi(st[2].c_str());
else if(!strcasecmp(st[3].c_str(),"m"))
	tmpUser->SuspendExpires = atoi(st[2].c_str()) * 60;
else if(!strcasecmp(st[3].c_str(),"h"))
	tmpUser->SuspendExpires = atoi(st[2].c_str()) * 3600;
else if(!strcasecmp(st[3].c_str(),"d"))
	tmpUser->SuspendExpires = atoi(st[2].c_str()) * 3600*24;
else
	{
	bot->Notice(theClient,"%s is not a proper time refrence");
	delete tmpUser;
	}
tmpUser->SuspendExpires += time( 0 );
tmpUser->SuspendedBy = theClient->getNickUserHost();	    
tmpUser->Flags |= isSUSPENDED;
	
if(bot->UpdateOper(tmpUser))
	{
	bot->Notice(theClient,"%s has been suspended",st[1].c_str());
	bot->UpdateAuth(tmpUser->Id);
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