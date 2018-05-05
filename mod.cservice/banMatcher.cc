/**
 * banMatcher.cc
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
 * banMatcher.cc, v2.0 2013.10.28 -- Seven
 */

#include	"banMatcher.h"
#include	"misc.h"

using std::string ;

namespace gnuworld
{
	bool banMatch(const string& banMask, const string& address)
	{
		/* If we don't have at least a user@hostip at both side
		 * it means we don't match a ban
		 */
		if ((!isUserHost(banMask)) || (!isUserHost(address)))
			return false;

		if (match(banMask, address))
			return false;

		return true;
	}

	bool banMatch(const string& banMask, const iClient* theClient)
	{
		if (!isUserHost(banMask))
			return false;

		if (!match(banMask, theClient))
			return true;

		string authmask = theClient->getNickName() + "!" + theClient->getUserName();
		authmask += "@" + theClient->getAccount() + theClient->getHiddenHostSuffix();
		if (theClient->isModeR() && !theClient->isModeX())
		{
			/* client is authed, check our constructed hidden host against them */
			if (match(banMask, authmask) == 0)
				return true;
		}
		return false;
	}
} //namespace gnuworld
