/**
 * HASALLOPTest.cc
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
 * Checks to see whether a channel has all ops in it.
 */

#include "gnuworld_config.h"
#include "dronescan.h"
#include "dronescanTests.h"

namespace gnuworld {

namespace ds {

bool HASALLOPTest::isNormal( const Channel *theChannel )
{
	Channel::const_userIterator chanItr = theChannel->userList_begin();

	for( ; chanItr != theChannel->userList_end() ; ++chanItr )
		if(!chanItr->second->isModeO()) return true;

	return false;
} // bool HASALLOPTest::isNormal(const Channel*)

} // namespace ds

} // namespace gnuworld
