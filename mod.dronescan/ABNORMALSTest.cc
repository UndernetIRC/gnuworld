/*
 * ABNORMALSTest.cc
 *
 * 2003-05-06	GK@NG	Initial writing
 *
 */

#include "dronescan.h"
#include "dronescanTests.h"

namespace gnuworld {

namespace ds {

bool ABNORMALSTest::isNormal( const Channel *theChannel )
{
	Channel::const_userIterator chanItr =
		theChannel->userList_begin();
	
	unsigned int abnormals = 0;
	
	for( ; chanItr != theChannel->userList_end() ; ++chanItr )
		{
		iClient *theClient = chanItr->second->getClient();
		
		if(!bot->isNormal(theClient)) ++abnormals;
		}
	
	if((double)abnormals / (double)theChannel->size() > bot->channelMargin)
		return false;
	else
		return true;
} // bool ABNORMALSTest::isNormal( const Channel* )

} // namespace ds

} // namespace gnuworld
