/*
 * CHECKNETWORKCommand.cc
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

const char CHECKNETWORKCommand_cc_rcsId[] = "$Id: CHECKNETWORKCommand.cc,v 1.1 2001/05/01 18:44:39 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;


bool CHECKNETWORKCommand::Exec( iClient* theClient, const string& Message )
{

static const char* queryHeader
	= "SELECT * FROM servers";

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
			strcat(Msg,bot->SQLDb->GetValue(i,3));
		else
			strcat(Msg," unknown ");
			
		strcat(Msg," Last connection time : ");
		
		if(bot->SQLDb->GetValue(i,2) != NULL)
			strcat(Msg,bot->SQLDb->GetValue(i,2));
		else
			strcat(Msg," unknown ");
		bot->Notice(theClient,Msg);
		}
	}
return true;
}
}