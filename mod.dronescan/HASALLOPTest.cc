/*
 * HASALLOPTest.cc
 *
 * Checks to see whether a channel has all ops in it.
 *
 * 2003-06-14	GK@NG	Initial writing
 */

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
