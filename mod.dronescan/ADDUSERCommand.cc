/**
 * ADDUSERCommand.cc
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

void ADDUSERCommand::Exec( const iClient *theClient, const string& Message, const sqlUser* theUser )
{
	if(theUser->getAccess() < level::adduser) return ;

	StringTokenizer st(Message);

	/* Usage:
	 *  ADDUSER <username> <level>
	 */

	if(st.size() != 3) {
		Usage(theClient);
		return ;
	}

	/* Conditions:
	 *  i) The user must not already exist
	 * ii) A user cannot add a user with >= their own access
	 */

	/* Check if the user already exists */
	string newUser = st[1];

	sqlUser *targetUser = bot->getSqlUser(newUser);
	if(targetUser) {
		bot->Reply(theClient, "The user %s has already been added.",
			targetUser->getUserName().c_str()
			);
		return ;
	}

	/* Check the new level is not >= the user adding them */
	unsigned int newAccess = atoi(st[2].c_str());
	if(newAccess <= 0 || newAccess >= theUser->getAccess()) {
		bot->Reply(theClient, "Please choose a sensible access level.");
		return ;
	}

	/* User doesn't exist, access level is sane. */
	/* NB: targetUser here is empty because of the earlier check */

	targetUser = new sqlUser(bot->getSqlDb());

	targetUser->setUserName(newUser);
	targetUser->setCreated(::time(0));
	targetUser->setLastSeen(0);
	targetUser->setLastUpdatedBy(theClient->getRealNickUserHost());
	targetUser->setLastUpdated(::time(0));
	targetUser->setFlags(0);
	targetUser->setAccess(newAccess);

	if(targetUser->insert()) {
		bot->Reply(theClient, "User %s successfully added.",
			newUser.c_str()
			);
	} else {
		bot->Reply(theClient, "An error occured whilst adding %s.",
			newUser.c_str()
			);
	}

	/* Flush the cache */
	bot->preloadUserCache();

	return ;


} // ADDUSERCommand::Exec(iClient*, const string&, const sqlUser*)

} // namespace ds

} // namespace gnuworld
