/*
 * USERINFOCommand.cc
 *
 * Shows all kind of stuff about a user
 *
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"misc.h"
#include	"match.h"
#include	"Network.h"

const char USERINFOCommand_cc_rcsId[] = "$Id: USERINFOCommand.cc,v 1.6 2002/03/01 18:27:36 mrbean_ Exp $";

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
				    ,Name.c_str(),tempUser->getClient()->getNickUserHost().c_str());
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
		bot->Notice(theClient,"-----===== End of userinfo for %s =====-----",Name.c_str());
		}
	}
bot->Notice(theClient,"End of userinfo");
return true;
}

}
}
