/**
 * cfChannel.h
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

#include <assert.h>

#include "cfChannel.h"
#include "cfChannelUser.h"

namespace gnuworld {

namespace chanfix {

using std::string;

cfChannel::cfChannel(const string& _name) :
	name(_name)
{
}

cfChannel::~cfChannel()
{
for( mapUsers::iterator itr = users.begin() ;
     itr != users.end() ;
     ++itr ) {
	delete itr->second;
}
}


cfChannelUser* cfChannel::getUser(const string& username)
{
	mapUsers::iterator itr = users.find(username);

	if( itr != users.end() ) { return itr->second; }

	cfChannelUser *tmpUser = new cfChannelUser(username);

	users[username] = tmpUser;

	return tmpUser;
}

} // namespace chanfix

} // namespace gnuworld
