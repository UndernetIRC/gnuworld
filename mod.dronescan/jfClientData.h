/**
 * jfClientData.h
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

#ifndef JFCLIENTDATA_H
#define JFCLIENTDATA_H

#include <string>
#include <ctime>
#include <list>

namespace gnuworld {

namespace ds {

class jfClientData {
public:
	
	jfClientData() :
	    numOfJoins(0),numOfParts(0),seenOper(false),seenLoggedInUser(false) {} 
	unsigned int numOfJoins;
	unsigned int numOfParts;
	bool seenOper;
	bool seenLoggedInUser;
	std::list<std::string> numerics;
	std::list<std::pair<std::string,std::string> > userNames;

}; // class jfClientData

} // namespace ds

} // namespace gnuworld

#endif
