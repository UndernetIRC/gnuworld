/*
 * COMMANDSCommand.cc
 *
 * Changes all kind of commands options
 *     
 */
 
#include	<string>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include        "ccUser.h"
#include	"AuthInfo.h"
#include	"misc.h"

const char COMMANDSCommand_cc_rcsId[] = "$Id: COMMANDSCommand.cc,v 1.1 2001/07/26 20:12:40 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool COMMANDSCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st( Message ) ;

if( st.size() < 3 )
	{
	Usage(theClient);
	return true;
	}

Command* Comm = bot->findCommandInMem(st[2]);
if(!Comm)
	{
	bot->Notice(theClient,"Can't find command name %s",st[2].c_str());
	return false;
	}
if(!strcasecmp(st[1],"-ds"))
	{
	Comm->Disable();
	if(!bot->updateCommand(Comm))
		{
		bot->Notice(theClient,"Error while disabling command");
		return false;
		}
	else
		{
		bot->Notice(theClient,"Command has been disabled");
		return true;
		}
	}
else if(!strcasecmp(st[1],"-en"))
	{
	Comm->Enable();
	if(!bot->updateCommand(Comm))
		{
		bot->Notice(theClient,"Error while enabling command");
		return false;
		}
	else
		{
		bot->Notice(theClient,"Command has been enabled");
		return true;
		}
	}
else if(!strcasecmp(st[1],"-no"))
	{
	if(st.size() < 4)
		{
		bot->Notice(theClient,"-no must get a on/off option");
		return false;
		}
	if(!strcasecmp(st[3],"on"))
		{
		Comm->setNeedOp(true);
		if(!bot->updateCommand(Comm))
			{
			bot->Notice(theClient,"error while setting need op");
			return false;
			}
		else
			{
			bot->Notice(theClient,"needop has been enabled");
			return true;
			}
		
		}
	else if(!strcasecmp(st[3],"off"))
		{
		Comm->setNeedOp(false);
		if(!bot->updateCommand(Comm))
			{
			bot->Notice(theClient,"error while removing need op");
			return false;
			}
		else
			{
			bot->Notice(theClient,"needop has been disabled");
			return true;
			}
		
		}
	else
		{
		bot->Notice(theClient,"unknown option for -no must be on/off");
		return false;
		}
	}
else if(!strcasecmp(st[1],"-nl"))
	{
	if(st.size() < 4)
		{
		bot->Notice(theClient,"-nl must get a on/off option");
		return false;
		}
	if(!strcasecmp(st[3],"on"))
		{
		Comm->setNoLog(true);
		if(!bot->updateCommand(Comm))
			{
			bot->Notice(theClient,"error while setting no log");
			return false;
			}
		else
			{
			bot->Notice(theClient,"no log has been enabled");
			return true;
			}
		
		}
	else if(!strcasecmp(st[3],"off"))
		{
		Comm->setNoLog(false);
		if(!bot->updateCommand(Comm))
			{
			bot->Notice(theClient,"error while removing no log");
			return false;
			}
		else
			{
			bot->Notice(theClient,"no log has been disabled");
			return true;
			}
		
		}

	else
		{
		bot->Notice(theClient,"unknown option for -nl must be on/off");
		return false;
		}
	}

else if(!strcasecmp(st[1],"-na"))
	{
	if(st.size() < 4)
		{
		bot->Notice(theClient,"-na must get a new command name");
		return false;
		}
	if(!strcasecmp(Comm->getName(),st[3]))
		{
		bot->Notice(theClient,"the command is already called like that");
		return false;
		}
	Comm->setName(string_upper(st[3]));
	if(!bot->updateCommand(Comm))
		{
		bot->Notice(theClient,"error while changing command name");
		return false;
		}
	else
		{
		bot->Notice(theClient,"name has been changed");
		return true;
		}
	}
else
	{
	bot->Notice(theClient,"Unknown option for commands, must be -en,-ds,-no,-nl");			
	}
	
return true;
}
}
}
