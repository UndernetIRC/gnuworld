/**
 * COMMONREALTest.cc
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
 * Checks to see whether the members of a channel have a common realname.
 */

#include	<string>
#include	<sstream>
#include	<map>

#include "gnuworld_config.h"
#include "dronescan.h"
#include "dronescanTests.h"

namespace gnuworld {

namespace ds {

bool COMMONREALTest::isNormal( const Channel *theChannel )
{
	typedef std::map<std::string, unsigned short> realnameMapType;
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


const std::string COMMONREALTest::getVariable() const
{
	return "realCutoff";
}


bool COMMONREALTest::setVariable( const std::string& var,
	const std::string& value)
{
	if("REALCUTOFF" != var) return false;

	realCutoff = atoi(value.c_str());

	return true;
} // bool COMMONREALTest::setVariable( const std::string&,const std::string& )


const std::string COMMONREALTest::getStatus() const
{
	std::stringstream status;
	status << "realCutoff: " << realCutoff;
	return status.str();
}

} // namespace ds

} // namespace gnuworld
