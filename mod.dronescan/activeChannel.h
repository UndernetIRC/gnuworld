/**
 * activeChannel.h
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
 * $Id: activeChannel.h,v 1.3 2003/11/26 23:30:22 dan_karrels Exp $
 */

#ifndef ACTIVECHANNEL_H
#define ACTIVECHANNEL_H "$Id: activeChannel.h,v 1.3 2003/11/26 23:30:22 dan_karrels Exp $"

#include <string>
#include <ctime>

namespace gnuworld {

namespace ds {

class activeChannel {
public:
	/**
	 * Constructor to set up initial state.
	 *
	 * @param _name		the name of the active channel
	 * @param _lastjoin	the time of the last join to this channel
	 */
	inline activeChannel( const std::string& _name, const time_t& _lastjoin ) :
		name(_name),
		lastjoin(_lastjoin)
		{ }
	
	/** Empty destructor as we allocate no memory. */
	inline ~activeChannel() { } ;
	
	/************************
	 ** A C C E S S O  R S **
	 ************************/
	
	inline const std::string& getName() const
		{ return name; }
	
	inline const time_t& getLastJoin() const
		{ return lastjoin; }
	
	/*********************
	 ** M U T A T O R S **
	 *********************/
	
	inline void setLastJoin( const time_t& _lastjoin )
		{ lastjoin = _lastjoin; }
	
protected:
	std::string	name;
	time_t		lastjoin;

}; // class activeChannel

} // namespace ds

} // namespace gnuworld

#endif
