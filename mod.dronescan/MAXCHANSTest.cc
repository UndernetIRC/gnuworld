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
	Channel::const_userIterator chanItr = theChannel->userList_begin();
	
	for( ; chanItr != theChannel->userList_end() ; ++chanItr )
		{
		iClient *theClient = chanItr->second->getClient();
		if(theClient->channels_size() >= bot->maxChans) return true;
		}
	
	return false;
	
} // bool MAXCHANSTest::isNormal( const Channel* )

} // namespace ds

} // namespace gnuworld
