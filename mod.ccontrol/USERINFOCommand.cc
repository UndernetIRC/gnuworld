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
#include	"AuthInfo.h"
#include	"Network.h"

const char USERINFOCommand_cc_rcsId[] = "$Id: USERINFOCommand.cc,v 1.2 2001/09/30 20:26:44 mrbean_ Exp $";

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

static const char Main[] = "SELECT user_name,password,server,isSuspended "
		",Suspend_Expires,Suspended_By,Suspend_Reason,isUHS,isOPER"
		",isADMIN,isSMT,isCODER,getLogs,NeedOp,Email,user_id,suspend_level from opers";

strstream theQuery;
theQuery	<< Main
		<< ends;


elog << theQuery.str() << endl;
ExecStatusType status = bot->SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if(PGRES_TUPLES_OK != status) 
	{
	elog	<< "USERINFO> SQL Error: "
		<< bot->SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}
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
AuthInfo* tmpAuth;
unsigned int Id;
unsigned int SuspendLevel;
string SLevel;
for(int i = 0;i< bot->SQLDb->Tuples();++i)
	{
	if(!(match(st[1],bot->SQLDb->GetValue(i,0))) || 
	!(match(st[1],bot->SQLDb->GetValue(i,2))))
		{
		Name.assign(bot->SQLDb->GetValue(i,0));
		Server.assign(bot->SQLDb->GetValue(i,2));
		Email.assign(bot->SQLDb->GetValue(i,14));
		SuspendExpires = atoi(bot->SQLDb->GetValue(i,4));
		if((!strcasecmp(bot->SQLDb->GetValue(i,3),"t")) && (SuspendExpires > ::time(0)))
			{
			Suspended = true;
			SuspendedBy.assign(bot->SQLDb->GetValue(i,5));
			SuspendReason.assign(bot->SQLDb->GetValue(i,6));
			SuspendLevel=atoi(bot->SQLDb->GetValue(i,16));
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
		
		if(!strcasecmp(bot->SQLDb->GetValue(i,8),"t"))
			{
			Level.assign("OPER");
			}
		else if(!strcasecmp(bot->SQLDb->GetValue(i,9),"t"))
			{
			Level.assign("ADMIN");
			}
		else if(!strcasecmp(bot->SQLDb->GetValue(i,10),"t"))
			{
			Level.assign("SMT");
			}
		else 
			Level.assign("CODER");
					
		if(!strcasecmp(bot->SQLDb->GetValue(i,12),"t"))
			{
			sprintf(GetLogs,"YES");
			}
		else
			{
			sprintf(GetLogs,"NO");
			}

		if(!strcasecmp(bot->SQLDb->GetValue(i,13),"t"))
			{
			sprintf(NeedOp,"YES");
			}
		else	
			{
			sprintf(NeedOp,"NO");
			}
		Id=atoi(bot->SQLDb->GetValue(i,15));
		tmpAuth = bot->IsAuth(Id);
		if(tmpAuth)
			{
			bot->Notice(theClient,"User Name : %s , Currently logged in from : %s"
				    ,Name.c_str(),(Network->findClient(tmpAuth->getNumeric())->getNickUserHost()).c_str());
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
