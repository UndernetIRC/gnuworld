/*
 * RANGETest.cc
 *
 * Checks the range of the entropy of a given channel.
 *
 * 2003-05-06	GK@NG	Initial writing
 */

#include "dronescan.h"
#include "dronescanTests.h"

namespace gnuworld {

namespace ds {

bool RANGETest::isNormal( const Channel *theChannel )
{
	double maxEntropy, minEntropy;
	maxEntropy = minEntropy = 0;
	
	bool hasOp = false;
	
	Channel::const_userIterator chanItr = theChannel->userList_begin();
	
	for( ; chanItr != theChannel->userList_end() ; ++chanItr )
		{
		iClient *targetClient = chanItr->second->getClient();
		
		double userEntropy = bot->calculateEntropy(targetClient);
		
		if(userEntropy < minEntropy || minEntropy == 0) minEntropy = userEntropy;
		if(userEntropy > maxEntropy) maxEntropy = userEntropy;
		
		if(chanItr->second->isModeO()) hasOp = true;
		}
	
	if((maxEntropy - minEntropy) > bot->channelRange)
		return true;
	else
		return false;
}

} // namespace ds

} // namespace gnuworld
