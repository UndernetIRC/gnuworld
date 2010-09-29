/**
 * ACCESSCommand.cc
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
 * Return the access level of the current or specified user.
 *
 * 2003-06-15	GK@NG	Initial writing
 */

#include <time.h>

#include "Network.h"
#include "StringTokenizer.h"

#include "levels.h"
#include "dronescan.h"
#include "dronescanCommands.h"
#include "sqlUser.h"

namespace gnuworld {

namespace ds {

void ACCESSCommand::Exec( const iClient *theClient, const string& Message, const sqlUser* theUser )
{
	if(theUser->getAccess() < level::access) return ;

	StringTokenizer st(Message);

	/* Usage:
	 *  ACCESS
	 *  ACCESS <username>
	 */

	const sqlUser *targetUser = theUser;

	if(st.size() == 2) {
		targetUser = bot->getSqlUser(st[1]);
		if(!targetUser) {
			bot->Reply(theClient, "No such user %s",
				st[1].c_str()
				);
			return ;
		}
	}

	time_t lastseen = targetUser->getLastSeen();
	char lastseen_r[21];
	struct tm *lastseen_b = gmtime(&lastseen);
	strftime(lastseen_r, 20, "%F %H:%M:%S", lastseen_b);

	/* Give the client information about the targetUser */
	bot->Reply(theClient, "Username : %-10s Access: %4u",
		targetUser->getUserName().c_str(),
		targetUser->getAccess()
		);
	bot->Reply(theClient, "Last Seen: %s",
		lastseen_r
		);

	lastseen = targetUser->getLastUpdated();
	lastseen_b = gmtime(&lastseen);
	strftime(lastseen_r, 20, "%F %H:%M:%S", lastseen_b);
	bot->Reply(theClient, "Last Updated: %s (by %s)",
		lastseen_r,
		targetUser->getLastUpdatedBy().c_str()
		);

	return ;
} // ACCESSCommand::Exec(iClient*, const string&)

} // namespace ds

} // namespace gnuworld
