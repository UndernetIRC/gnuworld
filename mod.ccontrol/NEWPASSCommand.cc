#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"
namespace gnuworld
{

using std::string ;


bool NEWPASSCommand::Exec( iClient* theClient, const string& Message)
{
    	StringTokenizer st( Message ) ;
	
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}

	AuthInfo *tmpUser = bot->IsAuth(theClient->getCharYYXXX());
	
	if(!tmpUser)
	{
	    bot->Notice(theClient,"You have to be logged in to use this command");
	    return false;
	}
	
	User* theUser = bot->GetUser(tmpUser->Name);

	theUser->Password = bot->CryptPass(st[1]);
	if(bot->UpdateOper(theUser))
	{
	    bot->Notice(theClient,"Password changed!");
	    delete tmpUser;
	    return true;
	}
	else
	{
	    bot->Notice(theClient,"Error while changing password");
	    delete tmpUser;
	    return true;
	}
}	
}
