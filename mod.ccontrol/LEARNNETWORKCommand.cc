/*
 * LEARNNETWORKCommand.cc
 *
 * Causes the bot to automatically update its server db
 *
 */

#include	<string>
//#include	<cstdlib>
//#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include        "iServer.h"
#include	"Network.h"
#include        "stdlib.h"

const char LEARNNETWORKCommand_cc_rcsId[] = "$Id: LEARNNETWORKCommand.cc,v 1.2 2001/05/01 18:44:39 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;


bool LEARNNETWORKCommand::Exec( iClient* theClient, const string& Message )
{
ccServer* NewServer = new ccServer(bot->SQLDb);
assert(NewServer != NULL);
unsigned int AddedServers = 0;

string wallopMe = "Learning network status at the request of  : " ;
wallopMe+= theClient->getCharYYXXX();
bot->Wallops( wallopMe ) ;

xNetwork::serverIterator ptr = Network->server_begin();
xNetwork::serverIterator end = Network->server_end();

iServer* CurServer;
for( ; ptr != end ; ptr++ )
	{
	CurServer = *ptr;
	if( NULL == CurServer)
		{
		continue ;
		}
	else
		{
		if(!NewServer->loadData(CurServer->getName().c_str()))
			{ //If the server isnt in the database , update it
			NewServer->set_Name(CurServer->getName());
			NewServer->set_Uplink(Network->findServer(CurServer->getUplinkIntYY())->getName());
			NewServer->set_LastNumeric(CurServer->getCharYY());
			NewServer->set_LastConnected(CurServer->getConnectTime());
			NewServer->set_LastSplitted(0);
			if(NewServer->Insert())
				{
				wallopMe = "Successfully learned server : " ;
				wallopMe+=  NewServer->get_Name().c_str(); 
				bot->Wallops( wallopMe ) ;
				AddedServers++;
				}	
				else
				{
				wallopMe = "Error while learning server : " ;
				wallopMe+=  NewServer->get_Name().c_str(); 
				bot->Wallops( wallopMe ) ;
				}	
			}
		}
	}
delete NewServer;
char tWall[256];
sprintf(tWall,"Finished learning the network, Learned a total of %d servers\n",AddedServers);
bot->Wallops( tWall ) ;
return true;
}
}