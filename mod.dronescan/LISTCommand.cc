/*
 * LISTCommand.cc
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
 * Display information about things.
 *
 * 2003-06-20	GK@NG	Initial writing
 */

#include "Network.h"
#include "StringTokenizer.h"

#include "activeChannel.h"
#include "dronescan.h"
#include "dronescanCommands.h"
#include "sqlFakeClient.h"
#include "sqlUser.h"

namespace gnuworld {

namespace ds {

void LISTCommand::Exec( const iClient *theClient, const string& Message , const sqlUser* )
{
	/* Usage: LIST <name> */

	StringTokenizer st(Message);

	if(st.size() < 2) {
		Usage(theClient);
		return ;
	}

	string Name = string_upper(st[1]);

	if("ACTIVE" == Name) {
		if(bot->droneChannels.empty()) {
			bot->Reply(theClient, "There are currently no active drone channels.");
			return ;
		}

		bot->Reply(theClient, "Active drone channels:");

		for(dronescan::droneChannelsType::const_iterator itr =
		    bot->droneChannels.begin() ; itr != bot->droneChannels.end()
		    ; ++itr) {
			/* Does this channel still exist? */
			Channel *theChannel = Network->findChannel(itr->first);

			if(theChannel) {
				bot->Reply(theClient, "  [%4u] (%u) %s",
					theChannel->size(),
					itr->second->getLastJoin(),
					theChannel->getName().c_str()
					);
			} else {
				bot->Reply(theClient, "  [   0] (%u) %s",
					itr->second->getLastJoin(),
					itr->first.c_str()
					);
			}
		}
	}

	if("FAKECLIENTS" == Name) {
		if(bot->fakeClients.empty()) {
			bot->Reply(theClient, "There are currently no fake clients.");
			return ;
		}

		bot->Reply(theClient, "Fake clients:");

		for(dronescan::fcMapType::const_iterator itr =
		    bot->fakeClients.begin() ; itr != bot->fakeClients.end() ;
		    ++itr ) {
			/* List the fake clients back to the user */
			sqlFakeClient *theFake = itr->second;

			bot->Reply(theClient, "  (%02u %1s) %s",
				theFake->getId(),
				theFake->getFlagsString().c_str(),
				theFake->getNickUserHost().c_str()
				);
		}
	}

	if("JOINFLOOD" == Name) {
		if(bot->jcChanMap.empty()) {
			bot->Reply(theClient, "There are currently no channels being join flooded.");
			return ;
		}

		bot->Reply(theClient, "Currently joinflooded channels:");

		for(dronescan::jcChanMapType::const_iterator itr =
		    bot->jcChanMap.begin() ; itr != bot->jcChanMap.end()
		    ; ++itr) {
		    	if(itr->second->getNumOfJoins() >= bot->jcCutoff)
				bot->Reply(theClient, "  %s (%u)",
					itr->first.c_str(),
					itr->second
					);
		}
	}

	if("USERS" == Name) {
		bot->Reply(theClient, "Users:");
		for(dronescan::userMapType::const_iterator itr = bot->userMap.begin() ;
		    itr != bot->userMap.end() ; ++itr) {
			bot->Reply(theClient, "Username: %-10s Access: %4u",
				itr->second->getUserName().c_str(),
				itr->second->getAccess()
				);
		}
	}
	
	if("EXCEPTIONALCHANNELS" == Name) {
		if(bot->exceptionalChannels.empty()) {
			bot->Reply(theClient,"There are currently no exceptional channels");
			return;
		}
		bot->Reply(theClient,"Exceptional channels:");
		for(dronescan::exceptionalChannelsType::const_iterator itr = 
		    bot->exceptionalChannels.begin(); itr != bot->exceptionalChannels.end();++itr) {
			bot->Reply(theClient,"Channel: %s",(*itr).c_str());
		}
	}    

	return ;
} // LISTCommand::Exec(iClient*, const string&)

} // namespace ds

} // namespace gnuworld
