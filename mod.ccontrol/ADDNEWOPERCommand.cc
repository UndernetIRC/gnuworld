#include	<string>
#include	<cstdlib>
#include        <iomanip.h>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char ADDNEWOPERCommand_cc_rcsId[] = "$Id $";

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
 
User* theUser = bot->GetUser(st[1]);
if (theUser) 
	{ 
	bot->Notice(theClient,"Oper %s already exsits in my db," 
	"please change the oper handle and try again",theUser->UserName.c_str());
        return false;
	}	    
int NewAccess;
int NewFlags;
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

if(tOper->Access < NewAccess)
	{
	bot->Notice(theClient,"You can't add an oper with higher access than yours!");
	return false;
	}	     	

theUser = new User;
theUser->UserName = st[1];
theUser->Password = bot->CryptPass(st[3]);
theUser->Access=NewAccess;
theUser->Flags=NewFlags;
theUser->last_updated_by = theClient->getNickUserHost();
if(bot->AddOper(theUser) == true)
	bot->Notice(theClient, "Oper successfully Added.");
else
	bot->Notice(theClient, "Error while adding new oper.");
return true; 
}
}