/**
 * jfChannel.cc
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
 */

#include "jfChannel.h"
#include "ip.h"

namespace gnuworld {

namespace ds {


void jfChannel::addJoin(iClient* theClient)
{
	std::string clientIp = xIP(theClient->getIP()).GetNumericIP();
	jfClientData& jfClient = joinPartMap[clientIp];
	jfClient.numOfJoins++;
	std::list<std::string>::iterator begin = jfClient.numerics.begin();
	std::list<std::string>::iterator end = jfClient.numerics.end();

	if(theClient->isOper())
		{
		jfClient.seenOper = true;
		}
	if(theClient->getMode(iClient::MODE_REGISTERED))
		{
		jfClient.seenLoggedInUser = true;
		jfClient.userNames.push_back(std::pair<std::string,std::string>(theClient->getCharYYXXX(),
			theClient->getAccount()));
		}

	bool found = false;
	for(std::list<std::string>::iterator it=begin;it!=end && !found;++it) {
		if(*it == theClient->getCharYYXXX()) {
			found = true;
		}
	}

	if(!found) {
		jfClient.numerics.push_back(theClient->getCharYYXXX());
	}
}
	
void jfChannel::addPart(iClient* theClient)
{
joinPartMap[xIP(theClient->getIP()).GetNumericIP()].numOfParts++;
}
	
	

} // namespace ds

} // namespace gnuworld

