/*
 * MAXCHANSTest.cc
 *
 * Checks whether any member of a channel is in more than maxChans channels.
 *
 * 2003-05-08	GK@NG	Initial writing
 */

#include "dronescan.h"
#include "dronescanTests.h"

namespace gnuworld {

namespace ds {

bool MAXCHANSTest::isNormal( const Channel *theChannel )
{
	unsigned int normals = 0;

	Channel::const_userIterator chanItr = theChannel->userList_begin();
	
	for( ; chanItr != theChannel->userList_end() ; ++chanItr )
		{
		iClient *theClient = chanItr->second->getClient();
		if(theClient->channels_size() >= maxChans) ++normals;
		if(normals >= 3) return true;
		}
	
	return false;
	
} // bool MAXCHANSTest::isNormal( const Channel* )


bool MAXCHANSTest::setVariable( const string& var, const string& value )
{
	if("MAXCHANS" != var) return false;
	
	maxChans = atoi(value.c_str());
	
	return true;
}

} // namespace ds

} // namespace gnuworld
