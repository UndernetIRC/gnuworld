#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include        "Network.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char REMOVEOPERCommand_cc_rcsId[] = "$Id $";

namespace gnuworld
{

using std::string ;

bool REMOVEOPERCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}
User* theUser = bot->GetUser(st[1]);
if (!theUser) 
	{ 
	bot->Notice(theClient,"Oper %s does not exists db," 
	"check your handle and try again",st[1].c_str());
	return false;
	}	    
	
if(bot->DeleteOper(string_lower(st[1])))     
	{    
	bot->Notice(theClient,"Successfully Deleted Oper %s ",st[1].c_str());
	AuthInfo *TDeauth = bot->IsAuth(theUser->Id);
	if(TDeauth)
		{
		iClient *TClient = Network->findClient(TDeauth->Numeric); 
		if(TClient)
			bot->Notice(TClient,"You have been removed from my access list");
		bot->deAuthUser(TDeauth->Numeric);
		}	
	return true;	
	}
else
	{    
	bot->Notice(theClient,"Error While Deleting Oper %s ",st[1].c_str());
	return false;	
	}
}
}