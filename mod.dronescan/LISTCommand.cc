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
 * $Id: LISTCommand.cc,v 1.6 2003/08/02 18:17:21 jeekay Exp $
 *
 * Display information about things.
 *
 * 2003-06-20	GK@NG	Initial writing
 */

#include "Network.h"
#include "StringTokenizer.h"

#include "dronescan.h"
#include "dronescanCommands.h"
#include "sqlUser.h"

namespace gnuworld {

namespace ds {

bool LISTCommand::Exec( const iClient *theClient, const string& Message , const sqlUser* )
{
	/* Usage: LIST <name> */

	StringTokenizer st(Message);
	
	if(st.size() < 2) {
		Usage(theClient);
		return true;
	}
	
	string Name = string_upper(st[1]);
	
	if("ACTIVE" == Name) {
		if(bot->droneChannels.empty()) {
			bot->Reply(theClient, "There are currently no active drone channels.");
			return true;
		}
	
		bot->Reply(theClient, "Active drone channels:");
		
		for(dronescan::droneChannelsType::const_iterator itr =
		    bot->droneChannels.begin() ; itr != bot->droneChannels.end()
		    ; ++itr) {
			/* Does this channel still exist? */
			Channel *theChannel = Network->findChannel(*itr);
			
			if(theChannel) {
				bot->Reply(theClient, "  [%3u] %s",
					theChannel->size(),
					theChannel->getName().c_str()
					);
			} else {
				bot->Reply(theClient, "  [N/A] %s",
					(*itr).c_str()
					);
			}
		}
	}
	
	if("JOINFLOOD" == Name) {
		if(bot->jcChanMap.empty()) {
			bot->Reply(theClient, "There are currently no channels being join flooded.");
			return true;
		}
	
		bot->Reply(theClient, "Currently joinflooded channels:");
		
		for(dronescan::jcChanMapType::const_iterator itr =
		    bot->jcChanMap.begin() ; itr != bot->jcChanMap.end()
		    ; ++itr) {
		    	if(itr->second >= bot->jcCutoff)
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
	
	return true;
} // LISTCommand::Exec(iClient*, const string&)

} // namespace ds

} // namespace gnuworld
