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
#include	"misc.h"

const char COMMANDSCommand_cc_rcsId[] = "$Id: COMMANDSCommand.cc,v 1.6 2001/12/14 08:00:19 mrbean_ Exp $";

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

if(!dbConnected)
        {
        bot->Notice(theClient,"Sorry, but the db connection is down now, please try again alittle later");
        return false;
        }

if(st[1].size() > 128)
	{
	bot->Notice(theClient,"Command name can't be more than 128 chars");
	return false;
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
	if(st[3].size() > 128)
	{
		bot->Notice(theClient,"Command name can't be more than 128 chars");
		return false;
	}
	if(!strcasecmp(Comm->getName(),st[3]))
		{
		bot->Notice(theClient,"the command is already called like that");
		return false;
		}
	Comm->setName(string_upper(bot->removeSqlChars(st[3])));
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
else if(!strcasecmp(st[1],"-ml"))
	{
	unsigned int MINLEVEL;
	if(st.size() < 4)
		{
		bot->Notice(theClient,"-ml must get a new min level");
		return false;
		}
	if(!strcasecmp(st[3],"CODER"))
		{
		MINLEVEL = operLevel::CODERLEVEL;
		}
	else if(!strcasecmp(st[3],"SMT"))
		{
		MINLEVEL = operLevel::SMTLEVEL;
		}
	else if(!strcasecmp(st[3],"ADMIN"))
		{
		MINLEVEL = operLevel::ADMINLEVEL;
		}
	else if(!strcasecmp(st[3],"OPER"))
		{
		MINLEVEL = operLevel::OPERLEVEL;
		}
	else if(!strcasecmp(st[3],"UHS"))
		{
		MINLEVEL = operLevel::UHSLEVEL;
		}
	else
		{
		bot->Notice(theClient,"Unknown level, must be CODER,SMT,ADMIN,OPER,UHS");
		return false;
		}
	Comm->setMinLevel(MINLEVEL);
	if(!bot->updateCommand(Comm))
		{
		bot->Notice(theClient,"error while changing command min level");
		return false;
		}
	else
		{
		bot->Notice(theClient,"command min level has been changed");
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
