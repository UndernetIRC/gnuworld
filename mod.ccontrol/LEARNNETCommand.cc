/*
 * LEARNNETCommand.cc
 *
 * Causes the bot to automatically update its server db
 *
 */

#include	<string>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include        "iServer.h"
#include	"Network.h"
#include        "stdlib.h"
#include        "server.h"
#include	"ccUser.h"
#include	"AuthInfo.h"

const char LEARNNETCommand_cc_rcsId[] = "$Id: LEARNNETCommand.cc,v 1.2 2001/09/26 13:58:28 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool LEARNNETCommand::Exec( iClient* theClient, const string& Message )
{
ccServer* NewServer = new ccServer(bot->SQLDb);
assert(NewServer != NULL);
unsigned int AddedServers = 0;

StringTokenizer st(Message);

if((st.size() > 1) && (!strcasecmp(st[1],"-r")))
	{
	AuthInfo *tmpAuth = bot->IsAuth(theClient->getCharYYXXX());
	if(!tmpAuth)
		{ //donno how we got to here .. but what the hell 
		return false;
		}
	if(tmpAuth->getFlags() < operLevel::CODERLEVEL)
		{
		bot->Notice(theClient,"Only coders can specify the -r flag");
		return false;
		}
	if(bot->CleanServers())
		{
		bot->MsgChanLog("Cleaned the server database at the request of %s\n"
		,theClient->getNickName().c_str());
		bot->Notice(theClient,"Successfully cleaned the database");
		}		
	}
			
bot->MsgChanLog("Learning network status at the request of : %s\n",theClient->getNickName().c_str());

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
			NewServer->setName(CurServer->getName());
			//NewServer->setUplink(Network->findServer(CurServer->getUplinkIntYY())->getName());
			NewServer->setLastNumeric(CurServer->getCharYY());
			NewServer->setLastConnected(CurServer->getConnectTime());
			NewServer->setLastSplitted(0);
			if(NewServer->Insert())
				{
				AddedServers++;
				}	
				else
				{
				bot->MsgChanLog("Error while learning server : %s\n",NewServer->getName().c_str());
				}	
			}
		}
	}
delete NewServer;
bot->MsgChanLog("Finished learning the network, Learned a total of %d servers\n",AddedServers);
bot->Notice(theClient,"Finished learning the network, Learned a total of %d servers\n",AddedServers);
return true;
}

}
} // namespace gnuworld

