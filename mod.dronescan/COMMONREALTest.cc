/*
 * COMMONREALTest.cc
 *
 * Checks to see whether the members of a channel have a common realname.
 *
 * 2003-06-15	GK@NG	Initial writing
 */

#include <map>

#include "dronescan.h"
#include "dronescanTests.h"

namespace gnuworld {

namespace ds {

bool COMMONREALTest::isNormal( const Channel *theChannel )
{
	typedef map<string, unsigned short> realnameMapType;
	realnameMapType realnameMap;
	
	Channel::const_userIterator chanItr = theChannel->userList_begin();
	
	for( ; chanItr != theChannel->userList_end() ; ++chanItr ) {
		iClient *theClient = chanItr->second->getClient();
		
		realnameMap[theClient->getDescription()]++;
	}
	
	for( realnameMapType::const_iterator rnItr = realnameMap.begin() ;
	     rnItr != realnameMap.end() ; ++rnItr) {
		if(rnItr->second >= realCutoff) return false;
	}
	
	return true;
} // bool COMMONREALTest::isNormal(const Channel*)


bool COMMONREALTest::setVariable( const string& var, const string& value)
{
	if("REALCUTOFF" != var) return false;
	
	realCutoff = atoi(value.c_str());
	
	return true;
} // bool COMMONREALTest::setVariable( const string&, const string& )

} // namespace ds

} // namespace gnuworld
