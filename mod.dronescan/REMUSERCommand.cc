/**
 * REMUSERCommand.cc
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
 * 2003-08-02	Jeekay	Initial writing
 */

#include "StringTokenizer.h"

#include "levels.h"
#include "dronescan.h"
#include "dronescanCommands.h"
#include "sqlUser.h"

namespace gnuworld {

namespace ds {

void REMUSERCommand::Exec( const iClient *theClient, const string& Message, const sqlUser* theUser )
{
	if(theUser->getAccess() < level::remuser) return ;

	StringTokenizer st(Message);

	/* Usage:
	 *  REMUSER <username>
	 */

	if(st.size() != 2) {
		Usage(theClient);
		return ;
	}

	/* Conditions:
	 *  i) The target user must exist
	 * ii) The removee's access must be < the removers access
	 */

	/* Does the user exist? */
	sqlUser *targetUser = bot->getSqlUser(st[1]);
	if(!targetUser) {
		bot->Reply(theClient, "The user %s does not exist.",
			st[1].c_str()
			);
		return ;
	}

	/* Is the removees access lower than the removers? */
	if(targetUser->getAccess() >= theUser->getAccess()) {
		bot->Reply(theClient, "You cannot remove someone with higher or equal access.");
		return ;
	}

	/* Conditions satisfied.
	 *  i) Remove user from the database
	 * ii) Flush cache
	 */

	/* Remove the user from the database */
	if(targetUser->remove()) {
		bot->Reply(theClient, "User %s successfully removed.",
			targetUser->getUserName().c_str());
	} else {
		bot->Reply(theClient, "There was a problem removing the user.");
	}

	/* Flush the cache */
	bot->preloadUserCache();

	return ;

} // REMUSERCommand::Exec(iClient*, const string&, const sqlUser*)

} // namespace ds

} // namespace gnuworld
