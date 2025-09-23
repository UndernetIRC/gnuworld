/**
 * STATSCommand.cc
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
 * $Id: STATSCommand.cc,v 1.7 2003/08/09 23:15:35 dan_karrels Exp $
 */
#include	<string>
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"


namespace gnuworld
{
using std::string ;

bool STATSCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.STATS");
StringTokenizer st( Message ) ;

sqlUser* theUser = bot->isAuthed(theClient, false);
if (!theUser)
	{
	return false;
	}

if (bot->getAdminAccessLevel(theUser) < level::stats) return false;

if( st.size() >= 2 )
{
	/*
	 * We have a parameter, lets do something interesting with it!
	 */

	if(st[1][0] == '#')
	{
		/*
		 * Dump loads of internal info about this channel.
		 */
		Channel* tmpChan = Network->findChannel(st[1]);
		if(!tmpChan) { bot->Notice(theClient, "That channel doesn't exist on the network :("); return true; }

		cservice::pendingChannelListType::iterator ptr = bot->pendingChannelList.find(tmpChan->getName());

		if(ptr != bot->pendingChannelList.end())
		{
			sqlPendingChannel* tmpPending = ptr->second;
			bot->Notice(theClient, "---- Pending channel stats for %s ----", ptr->first.c_str());
			bot->Notice(theClient, "Channel ID: %i", tmpPending->channel_id);
			bot->Notice(theClient, "Join Count: %i", tmpPending->join_count);
			bot->Notice(theClient, "Unique Join Count: %i", tmpPending->unique_join_count);



			bot->Notice(theClient, "# of entries in supporter list: %i", tmpPending->supporterList.size());

			sqlPendingChannel::supporterListType::iterator sPtr = tmpPending->supporterList.begin();

			for( ; sPtr != tmpPending->supporterList.end() ; ++sPtr )
				{
				bot->Notice(theClient, " - User ID: %i, Join count: %i", sPtr->first, sPtr->second);
				}



			bot->Notice(theClient, "# of entries in IP traffic checker list: %i", tmpPending->trafficList.size());

			sqlPendingChannel::trafficListType::iterator tPtr = tmpPending->trafficList.begin();
			for( ; tPtr != tmpPending->trafficList.end() ; ++tPtr )
				{
				bot->Notice(theClient, " - IP #: %s (Channel ID: %i) (Join Count: %i)",
					tPtr->second->ip_number.c_str(), tPtr->second->channel_id, tPtr->second->join_count);
				}
		} else {
			bot->Notice(theClient, "Channel doesn't have a pending application.");
		}

	} else {
		/*
		 * Have a look to see if this is a server first..
		 */
		 iServer* theServer = Network->findServerName( st[1] ) ;
		 if(theServer)
		 	{
			 bot->Notice(theClient, "Server: %s", theServer->getName().c_str());
			 bot->Notice(theClient, " - Numeric: %s",
				theServer->getCharYY().c_str());
			 bot->Notice(theClient, " - Connect Time: %i", theServer->getConnectTime());
			 bot->Notice(theClient, " - Still Bursting? %s", theServer->isBursting() ? "Yes" : "No");
		 	}
	}

bot->Notice(theClient, "-- End of STATS");
return true;
}


bot->Notice(theClient, "CMaster Command/SQL Query Statistics:");
for( const auto& [key, value] : bot->getStatsMap() )
	{
	bot->Notice( theClient, "%s: %i", key.c_str(), value ) ;
	}

bot->Notice(theClient, "-- End of STATS");

return true ;
}

} // namespace gnuworld.

