/**
 * cmdCHECK.cc
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
 */

#include <string>

#include "StringTokenizer.h"

#include "cfChannel.h"
#include "cfChannelUser.h"
#include "chanfix.h"
#include "chanfix-commands.h"

namespace gnuworld {

namespace chanfix {

void CHECKCommand::Exec( const iClient *theClient,
	const std::string& message )
{
	StringTokenizer st(message);

	/* Usage:
	 *  CHECK #channel
	 */

	if( st.size() != 2 ) {
		Usage(theClient);
		return;
	}

	cfChannel *theChannel = bot->getChannel(st[1], false);

	if( ! theChannel ) {
		bot->Notice(theClient, "Unable to find channel %s",
			st[1].c_str()
			);
		return;
	} else {
		bot->Notice(theClient, "Status for %s:",
			st[1].c_str()
			);
	}


	for(cfChannel::mapUsersConstIterator itr = theChannel->getUsersBegin() ;
		itr != theChannel->getUsersEnd() ;
		++itr
	) {
		cfChannelUser *theCU = itr->second;

		bot->Notice(theClient, "  %s: %u",
			theCU->getName().c_str(),
			theCU->getPoints()
			);
	}
}

} // namespace chanfix

} // namespace gnuworld

