/**
 * chanfix-core.cc
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
 * $Id: chanfix-core.cc,v 1.1 2004/05/18 23:13:12 jeekay Exp $
 */

#include <string>

#include "Channel.h"
#include "ChannelUser.h"
#include "Network.h"

#include "cfChannel.h"
#include "cfChannelUser.h"
#include "chanfix.h"

namespace gnuworld {

namespace chanfix {

using std::string;

cfChannel* chanfix::getChannel(const string& channel)
{
	mapChannels::iterator itr = channels.find(channel);
	
	if( itr != channels.end() ) { return itr->second; }
	
	cfChannel* newChan = new cfChannel(channel);
	
	channels[channel] = newChan;
	
	return newChan;
}


void chanfix::doCountUpdate()
{
	/* Iterator over all available channels */
	log(logging::DEBUG, "Starting count cycle");
	
	for( xNetwork::const_channelIterator itr = Network->channels_begin() ;
	     itr != Network->channels_end() ;
	     ++itr ) {
		Channel *tmpChannel = itr->second;
		log(logging::DEBUG, "Found channel %s",
			tmpChannel->getName().c_str());
		
		cfChannel *cfChan = 0;
		
		for( Channel::userIterator citr = tmpChannel->userList_begin() ;
		     citr != tmpChannel->userList_end() ;
		     ++citr ) {
			ChannelUser *chanUser = citr->second;
			
			log(logging::DEBUG, "  Found client: %s",
				chanUser->getNickName().c_str()
				);
			
			if( ! chanUser->isModeO() ) { continue ; }
			if( ! chanUser->getClient()->isModeR() ) { continue ; }
			if( chanUser->getClient()->isModeK() ) { continue ; }
			
			if( ! cfChan ) { cfChan = getChannel(tmpChannel->getName()); }
			
			cfChannelUser *user = cfChan->getUser(chanUser->getClient()->getAccount());
			
			user->addPoints(confPointsAuth);
			
			log(logging::DEBUG, "  Found opped client: %s",
				chanUser->getNickName().c_str()
				);
		}
	}
}

} // namespace chanfix

} // namespace gnuworld
