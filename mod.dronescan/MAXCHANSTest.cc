/**
 * MAXCHANSTest.cc
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
 * $Id: MAXCHANSTest.cc,v 1.7 2003/12/29 23:59:37 dan_karrels Exp $
 *
 * Checks whether any member of a channel is in more than maxChans channels.
 */

#include	<sstream>

#include "config.h"
#include "dronescan.h"
#include "dronescanTests.h"

RCSTAG("$Id: MAXCHANSTest.cc,v 1.7 2003/12/29 23:59:37 dan_karrels Exp $");

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


const string MAXCHANSTest::getVariable() const
{
	return "maxChans";
}


bool MAXCHANSTest::setVariable( const string& var, const string& value )
{
	if("MAXCHANS" != var) return false;
	
	maxChans = atoi(value.c_str());
	
	return true;
}


const string MAXCHANSTest::getStatus() const
{
	std::stringstream status;
	status << "maxChans: " << maxChans;
	return status.str();
}

} // namespace ds

} // namespace gnuworld
