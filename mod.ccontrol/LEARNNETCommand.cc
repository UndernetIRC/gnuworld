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

const char LEARNNETCommand_cc_rcsId[] = "$Id: LEARNNETCommand.cc,v 1.8 2001/12/23 09:07:57 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool LEARNNETCommand::Exec( iClient* theClient, const string& Message )
{

if(!dbConnected)
        {
        bot->Notice(theClient,"Sorry, but the db connection is down now, please try again alittle later");
        return false;
        }

ccUser* tmpUser = bot->IsAuth(theClient);
if(tmpUser)
        bot->MsgChanLog("(%s) - %s : LEARNNET %s\n",tmpUser->getUserName().c_str()
                        ,theClient->getNickUserHost().c_str());

ccServer* NewServer = new ccServer(bot->SQLDb);
assert(NewServer != NULL);
unsigned int AddedServers = 0;

StringTokenizer st(Message);

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
	else if(!server->isJuped(CurServer))
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

