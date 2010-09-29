/**
 * banMatcher.h
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
 * $Id: HostBanMatcher.cc,v 1.1 2009/06/25 19:05:23 mrbean_ Exp $
 */

#include	<string>
#include	"HostBanMatcher.h"
#include	"ip.h"
#include	"match.h"
 
using std::string ;

namespace gnuworld
{ 
 
	
	HostBanMatcher::HostBanMatcher(const string& mask) : banMask(mask)
	{
			
	}
	
	bool HostBanMatcher::matches(iClient* theClient) 
	{
		string authbanmask = theClient->getNickName() + "!" + theClient->getUserName();
		authbanmask += "@" + theClient->getAccount() + theClient->getHiddenHostSuffix();
		if (theClient->isModeR() && !theClient->isModeX())
		{
			/* client is authed, check our constructed hidden host against them */
			if ((match(banMask, authbanmask)) == 0)
				return true;
		}

		string nickuserip = theClient->getNickName() + "!" + theClient->getUserName();
		nickuserip += "@" + xIP(theClient->getIP()).GetNumericIP();
		
		if( (match(banMask,theClient->getNickUserHost()) == 0) ||
				(0 == match( banMask,theClient->getRealNickUserHost())) ||
				(0 == match( banMask, nickuserip)) )
		{
			return true;
		}
		return false;
	} 
}


 

