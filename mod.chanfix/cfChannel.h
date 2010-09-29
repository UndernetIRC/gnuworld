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

#ifndef CF_CFCHANNEL_H
#define CF_CFCHANNEL_H "$Id: cfChannel.h,v 1.6 2007/08/28 16:10:08 dan_karrels Exp $"

#include	<string>
#include	<map>

namespace gnuworld {

namespace chanfix {

class cfChannelUser;

class cfChannel {
public:
	cfChannel(const std::string&);
	virtual ~cfChannel();

	/** Return a given user - add a new one if one is not present. */
	cfChannelUser* getUser(const std::string&);

	/** Return the beginning of the users map. */
	typedef std::map< std::string , cfChannelUser* > mapUsers;
	typedef mapUsers::const_iterator mapUsersConstIterator;
	inline mapUsersConstIterator getUsersBegin()
		{ return users.begin(); }
	inline mapUsersConstIterator getUsersEnd()
		{ return users.end(); }

protected:
	std::string name;
	mapUsers users;
}; // class cfChannel

} // namespace chanfix

} // namespace gnuworld

#endif
