/*
 * HASOPTest.cc
 *
 * Checks to see whether a channel has any ops in it.
 *
 * 2003-05-06	GK@NG	Initial writing
 */

#include "dronescan.h"

namespace gnuworld {

namespace ds {

bool HASOPTest::isNormal( const Channel *theChannel )
{
	Channel::const_userIterator chanItr = theChannel->userList_begin();
	
	for( ; chanItr != theChannel->userList_end() ; ++chanItr )
		if(chanItr->second->isModeO()) return true;
	
	return false;
}

} // namespace ds

} // namespace gnuworld
