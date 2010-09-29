/**
 * LEARNNETCommand.cc
 * Causes the bot to automatically update its server db
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
 * $Id: LEARNNETCommand.cc,v 1.16 2006/09/26 17:35:58 kewlio Exp $
 */

#include	<new>
#include	<string>

#include	<cassert>
#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include        "iServer.h"
#include	"Network.h"
#include        "server.h"
#include	"ccUser.h"
#include	"misc.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: LEARNNETCommand.cc,v 1.16 2006/09/26 17:35:58 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool LEARNNETCommand::Exec( iClient* theClient, const string& Message )
{

bot->MsgChanLog("LEARNNET \n");

unsigned int AddedServers = 0;
StringTokenizer st(Message);

bot->MsgChanLog("Learning network status at the request of: %s\n",
	theClient->getNickName().c_str());

xNetwork::serverIterator ptr = Network->servers_begin();
xNetwork::serverIterator end = Network->servers_end();

for( ; ptr != end ; ptr++ )
	{
	iServer* CurServer = ptr->second ;
	if( NULL == CurServer)
		{
		continue ;
		}

	// NOTE: Changed from isJuped()
	if((0 == Network->findFakeServer(CurServer)) && 
		(strcmp(CurServer->getName().c_str(),
			bot->getUplinkName().c_str())))
		{
		if(!bot->getServer(CurServer->getName()))
			{
			//If the server isnt in the database , update it
			ccServer* NewServer = new (std::nothrow)
				ccServer(bot->SQLDb);
			assert( NewServer != 0 ) ;

			// TODO: Shouldn't all of this be in the
			// ccServer constructor?
			NewServer->setName(CurServer->getName());
			NewServer->setUplink(Network->findServer(CurServer->getUplinkIntYY())->getName());
			NewServer->setLastNumeric(CurServer->getCharYY());
			NewServer->setLastConnected(CurServer->getConnectTime());
			NewServer->setLastSplitted(0);
			NewServer->setAddedOn(::time(0));
			NewServer->setLastUpdated(::time(0));
			NewServer->setNetServer(CurServer);

			bot->addServer(NewServer);
			bot->Write("%s V :%s\n",
				bot->getCharYYXXX().c_str(),
				CurServer->getCharYY().c_str());

			if(NewServer->Insert())
				{
				AddedServers++;
				}	
			else
				{
				bot->MsgChanLog("Error while learning "
					"server: %s\n",
					NewServer->getName().c_str());
				}	
			}
		}
	}

bot->MsgChanLog( "Finished learning the network. Learned a total of %d "
	"servers\n", AddedServers ) ;
bot->Notice( theClient, "Finished learning the network. Learned a "
	"total of %d servers\n", AddedServers ) ;

return true;
}

} // namespace uworld

} // namespace gnuworld

