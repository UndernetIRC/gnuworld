/*
 * MAXCHANSTest.cc
 *
 * Checks whether any member of a channel is in more than maxChans channels.
 *
 * 2003-05-08	GK@NG	Initial writing
 */

#include "dronescan.h"

namespace gnuworld {

namespace ds {

bool MAXCHANSTest::isNormal( const Channel *theChannel )
{
	unsigned int normals = 0;

	Channel::const_userIterator chanItr = theChannel->userList_begin();
	
	for( ; chanItr != theChannel->userList_end() ; ++chanItr )
		{
		iClient *theClient = chanItr->second->getClient();
		if(theClient->channels_size() >= bot->maxChans) ++normals;
		if(normals >= 3) return true;
		}
	
	return false;
	
} // bool MAXCHANSTest::isNormal( const Channel* )

} // namespace ds

} // namespace gnuworld
