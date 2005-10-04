/**
 * USERINFOCommand.cc
 * Shows all kind of stuff about a user
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 * $Id: USERINFOCommand.cc,v 1.11 2005/10/04 15:08:40 kewlio Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"commLevels.h"
#include	"misc.h"
#include	"match.h"
#include	"Network.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: USERINFOCommand.cc,v 1.11 2005/10/04 15:08:40 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool USERINFOCommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
	

if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

bot->MsgChanLog("USERINFO %s\n",st.assemble(1).c_str());

ccUser* tempUser;
string Name;
string Level;
string Email;
string Server;
bool Suspended;
char GetLogs[4];
char NeedOp[4];
string SuspendedBy;
string SuspendReason;
time_t SuspendExpires;
unsigned int Id;
unsigned int SuspendLevel;
string SLevel;
ccontrol::usersConstIterator ptr;
for(ptr = bot->usersMap_begin();ptr != bot->usersMap_end();++ptr)
	{
	tempUser = ptr->second;
	if(!(match(st[1],tempUser->getUserName())) || 
	!(match(st[1],tempUser->getServer())))
		{
		Name.assign(tempUser->getUserName());
		Server.assign(tempUser->getServer());
		Email.assign(tempUser->getEmail());
		SuspendExpires = tempUser->getSuspendExpires();
		if((tempUser->getIsSuspended()) && (SuspendExpires > ::time(0)))
			{
			Suspended = true;
			SuspendedBy.assign(tempUser->getSuspendedBy());
			SuspendReason.assign(tempUser->getSuspendReason());
			SuspendLevel=tempUser->getSuspendLevel();
			if(SuspendLevel == operLevel::OPERLEVEL)
				{
				SLevel.assign("OPER");
				}
			else if(SuspendLevel == operLevel::ADMINLEVEL)
				{
				SLevel.assign("ADMIN");
				}
			else if(SuspendLevel == operLevel::SMTLEVEL)
				{
				SLevel.assign("SMT");
				}
			else if(SuspendLevel == operLevel::CODERLEVEL)
				{
				SLevel.assign("CODER");
				}
			}
		else
			{
			Suspended = false;
			}
		
		if(tempUser->isOper())
			{
			Level.assign("OPER");
			}
		else if(tempUser->isAdmin())
			{
			Level.assign("ADMIN");
			}
		else if(tempUser->isSmt())
			{
			Level.assign("SMT");
			}
		else 
			Level.assign("CODER");
					
		if(tempUser->getLogs())
			{
			sprintf(GetLogs,"YES");
			}
		else
			{
			sprintf(GetLogs,"NO");
			}

		if(tempUser->getNeedOp())
			{
			sprintf(NeedOp,"YES");
			}
		else	
			{
			sprintf(NeedOp,"NO");
			}
		Id=tempUser->getID();
		if(tempUser->getClient())
			{
			bot->Notice(theClient,"User Name : %s , Currently logged in from : %s"
				    ,Name.c_str(),tempUser->getClient()->getRealNickUserHost().c_str());
			}
		else
			bot->Notice(theClient,"User Name : %s",Name.c_str());
		if(Email == "")
			Email.assign("Not assigned");
		bot->Notice(theClient,"Level : %s , Email : %s",Level.c_str(),Email.c_str());
		if(Server == "")
			Server.assign("Not assigned");
		bot->Notice(theClient,"Server : %s",Server.c_str());
		if(Suspended)
			{
			bot->Notice(theClient,"User was suspended By : %s , Until %s"
			,SuspendedBy.c_str(),bot->convertToAscTime(SuspendExpires));
			bot->Notice(theClient,"Reason : %s",SuspendReason.c_str());
			bot->Notice(theClient,"Level : %s",SLevel.c_str());
			}
		bot->Notice(theClient,"User Flags : GetLogs \002%s\002 NeedOp \002%s\002"
			    ,GetLogs,NeedOp);
		if ((st.size() > 2) && (!strcasecmp(st[2],"-cl")))
		{
			/* commands list requested */
			bot->Notice(theClient,"Commands available to this user:");
			string cmdList = "";
			for (ccontrol::constCommandIterator cmdptr = bot->command_begin();
				cmdptr != bot->command_end(); ++cmdptr)
			{
				int ComLevel = cmdptr->second->getFlags();
				if ((ComLevel & commandLevel::flg_NOLOGIN) || ((tempUser) && (tempUser->gotAccess(cmdptr->second))))
				{
					cmdList += (cmdptr->second->getName() + " ");
					if (cmdList.size() > 80)
					{
						bot->Notice(theClient, "%s", cmdList.c_str());
						cmdList.assign("");
					}
				}
			}
			if (!cmdList.empty())
				bot->Notice(theClient, "%s", cmdList.c_str());
		}
		bot->Notice(theClient,"-----===== End of userinfo for %s =====-----",Name.c_str());
		}
	}
bot->Notice(theClient,"End of userinfo");
return true;
}

}
}
