#include	<string>
#include	<cstdlib>
#include        <iomanip.h>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char ADDCOMMANDCommand_cc_rcsId[] = "$Id $";

namespace gnuworld
{

using std::string ;

bool ADDCOMMANDCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st( Message ) ;

if( st.size() < 3 )
	{
	Usage(theClient);
	return true;
	}


User* theUser = bot->GetUser(st[1]);
	
if(!theUser)
	{	
	bot->Notice(theClient,"I cant find oper %s",st[1].c_str());
	return false;
	}
	
int CommandLevel = bot->getCommandLevel(st[2]);
	
if(CommandLevel < 0 )
	{
	bot->Notice(theClient,"Command %s does not exists!",st[2].c_str());
	delete theUser;
	return false;	        
	}
	
AuthInfo *AClient = bot->IsAuth(theClient->getCharYYXXX());
	
if(!(AClient->Access & CommandLevel))
	{
	bot->Notice(theClient,"You must have access to a command inorder to add it");
	delete theUser;
	return false;
	}
	
else if(theUser->Access & CommandLevel)
	{
	bot->Notice(theClient,"%s already got access for %s",st[1].c_str(),st[2].c_str());
	delete theUser;
	return false;	        
	}	
	
theUser->Access |= CommandLevel;
theUser->last_updated_by = theClient->getNickUserHost();
if(bot->UpdateOper(theUser))
	{
	bot->Notice(theClient,"Successfully added the command for %s",st[1].c_str());
	bot->UpdateAuth(theUser->Id);
	delete theUser;
	return true;
	}
else
	{
	bot->Notice(theClient,"Error while adding command for %s",st[1].c_str());
	return false;
	}
	
}	
}
