/*
 * CHECKNETCommand.cc
 *
 * Delete a server for the bot database
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"

const char CHECKNETCommand_cc_rcsId[] = "$Id: CHECKNETCommand.cc,v 1.6 2001/12/23 09:07:57 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool CHECKNETCommand::Exec( iClient* theClient, const string& Message )
{

static const char* queryHeader
	= "SELECT * FROM servers";

if(!dbConnected)
        {
        bot->Notice(theClient,"Sorry, but the db connection is down now, please try again alittle later");
        return false;
        }

strstream theQuery;
theQuery	<< queryHeader 
		<< ends;

ExecStatusType status = bot->SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "CHECKNETWORK> SQL Error: "
		<< bot->SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}

// SQL Query succeeded

char Msg[512];
iServer* CurServer;
unsigned int TServers = 0;

bot->Notice(theClient,"Checking network status as for %d\n",::time(NULL));
ccUser* tmpUser = bot->IsAuth(theClient);
if(tmpUser)
	bot->MsgChanLog("(%s) - %s : CHECKNET\n",tmpUser->getUserName().c_str()
		    ,theClient->getNickUserHost().c_str());
		    
for (int i = 0 ; i < bot->SQLDb->Tuples(); i++)
	{ //For each server on the database check if its connected
	CurServer = Network->findServerName(bot->SQLDb->GetValue(i, 0));
	if(!CurServer) //If the server isnt connected , compose the message and wallop it 
		{
		strcpy(Msg,"Server \002");
		strcat(Msg,bot->SQLDb->GetValue(i, 0)); 
		strcat(Msg,"\002 Is missing, ");
		strcat(Msg,"Last split time : ");
		if(atoi(bot->SQLDb->GetValue(i,3)) != 0)
			{
			strcat(Msg,bot->SQLDb->GetValue(i,3));
			strcat(Msg," Split Reason:");
			strcat(Msg,bot->SQLDb->GetValue(i,5));
			}
		else
			strcat(Msg," unknown ");
			
		strcat(Msg," Last connection time : ");
		
		if(bot->SQLDb->GetValue(i,2) != NULL)
			strcat(Msg,bot->SQLDb->GetValue(i,2));
		else
			strcat(Msg," unknown ");
		bot->Notice(theClient,Msg);
		TServers++;
		}
	}


bot->Notice(theClient,"Finished checking the network status\n");
bot->Notice(theClient,"Found a total of %d missing servers\n",TServers);
return true;

}

}
}
