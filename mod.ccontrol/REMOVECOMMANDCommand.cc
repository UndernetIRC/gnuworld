#include	<string>
#include	<cstdlib>
#include        <iomanip.h>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char REMOVECOMMANDCommand_cc_rcsId[] = "$Id: REMOVECOMMANDCommand.cc,v 1.4 2001/02/25 19:52:06 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

bool REMOVECOMMANDCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st( Message ) ;

if( st.size() < 3 )
	{
	Usage(theClient);
	return true;
	}


ccUser* theUser = bot->GetUser(st[1]);
	
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
else if(!(theUser->getAccess() & CommandLevel))
	{
	bot->Notice(theClient,"%s doest have access for %s",st[1].c_str(),st[2].c_str());
	delete theUser;
	return false;	        
	}	
	
theUser->removeCommand(CommandLevel);
theUser->setLast_Updated_By(theClient->getNickUserHost());
if(theUser->Update())
	{
	bot->Notice(theClient,"Successfully removed the command from %s",st[1].c_str());
	bot->UpdateAuth(theUser);
	delete theUser;
	return true;
	}
else
	{
	bot->Notice(theClient,"Error while removing command from %s",st[1].c_str());
	delete theUser;
	return false;
	}
	
}	
}
