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
 * $Id: chanfix-core.cc,v 1.4 2004/06/03 22:18:00 jeekay Exp $
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

cfChannel* chanfix::getChannel(const string& channel, bool create)
{
	mapChannels::iterator itr = channels.find(channel);

	if( itr != channels.end() ) { return itr->second; }
	
	if( ! create ) { return 0; }

	cfChannel* newChan = new cfChannel(channel);

	channels[channel] = newChan;

	return newChan;
}


void chanfix::doCountUpdate()
{
	/* Iterator over all available channels */
	log(logging::DEBUG, "Starting count cycle");

	time_t starttime = ::time(0);

	for( xNetwork::const_channelIterator itr = Network->channels_begin() ;
	     itr != Network->channels_end() ;
	     ++itr ) {
		Channel *tmpChannel = itr->second;

		cfChannel *cfChan = 0;

		for( Channel::userIterator citr = tmpChannel->userList_begin() ;
		     citr != tmpChannel->userList_end() ;
		     ++citr ) {
			ChannelUser *chanUser = citr->second;

			if( ! chanUser->isModeO() ) { continue ; }
			if( ! chanUser->getClient()->isModeR() ) { continue ; }
			if( chanUser->getClient()->isModeK() ) { continue ; }

			if( ! cfChan ) { cfChan = getChannel(tmpChannel->getName(), true); }

			cfChannelUser *user = cfChan->getUser(chanUser->getClient()->getAccount());

			if( user->getPoints() >= confMaxPoints ) { continue ; }

			user->addPoints(confPointsAuth);
			
			if( user->getPoints() >= confMaxPoints ) {
				user->setPoints(confMaxPoints);
			}
		}
	}

	time_t duration = ::time(0) - starttime;

	log(logging::DEBUG, "Duration: %ld", duration);
}

} // namespace chanfix

} // namespace gnuworld
