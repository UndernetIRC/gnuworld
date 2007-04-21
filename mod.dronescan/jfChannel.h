/**
 * jfChannel.h
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

#ifndef JFCHANNEL_H
#define JFCHANNEL_H

#include <string>
#include <ctime>
#include <map>
#include "jfClientData.h"
#include "iClient.h"

namespace gnuworld {

namespace ds {

class jfChannel {
public:
	
	typedef std::map<std::string,jfClientData> joinPartMapType;
	typedef joinPartMapType::const_iterator joinPartMapIterator;
		
	/**
	 * Constructor to set up initial state.
	 *
	 * @param _name		the name of the active channel
	 * @param _lastjoin	the time of the last join to this channel
	 */
	inline jfChannel( const std::string& _name ) :
		name(_name) 
		{ numOfJoins = 0; 
		  numOfParts = 0; } ;

	/** Empty destructor as we allocate no memory. */
	inline ~jfChannel() { } ;

	/************************
	 ** A C C E S S O  R S **
	 ************************/

	inline const std::string& getName() const
		{ return name; }
	
	inline const bool getJoinFlooded() const 
		{ return joinFlooded; }

	inline const unsigned int getNumOfJoins() const
		{ return numOfJoins; }
		
	inline const unsigned int getNumOfParts() const
		{ return numOfParts; }

	inline const joinPartMapIterator joinPartBegin() const
		{ return joinPartMap.begin(); }
	
	inline const joinPartMapIterator joinPartEnd() const
		{ return joinPartMap.end(); }
		
	/*********************
	 ** M U T A T O R S **
	 *********************/
	
	inline unsigned int advanceChannelJoin() 
		{ return ++numOfJoins; }
	
	inline void resetJoinCount()
		{ numOfJoins = 0; }
	
	inline unsigned int advanceChannelParts()
		{ return ++numOfParts; }
	
	inline void resetPartCount()
		{ numOfParts = 0; }
				 
	inline void setJoinFlooded( bool _joinFlooded )
		{ joinFlooded = _joinFlooded; }

	inline void setPartFlooded( bool _partFlooded )
		{ partFlooded = _partFlooded; }
	
	inline void setLastJoinFlood( time_t _lastJoinFlood)
		{ lastJoinFlood = _lastJoinFlood; }
	
	inline const time_t getLastJoinFlood() const
		{ return lastJoinFlood; }
		
	void addJoin(iClient*);
	
	void addPart(iClient*);
	
protected:
	std::string	name;
	unsigned int	numOfJoins;
	unsigned int 	numOfParts;
	bool		joinFlooded;
	bool		partFlooded;
	joinPartMapType joinPartMap;
	time_t lastJoinFlood;

}; // class jfChannel

} // namespace ds

} // namespace gnuworld

#endif
