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
 *
 * $Id: ACCESSCommand.cc,v 1.2 2003/06/19 22:58:30 dan_karrels Exp $
 */

#include "config.h"

#include "Network.h"
#include "StringTokenizer.h"

#include "levels.h"
#include "dronescan.h"
#include "dronescanCommands.h"

RCSTAG("$Id: ACCESSCommand.cc,v 1.2 2003/06/19 22:58:30 dan_karrels Exp $");

namespace gnuworld {

namespace ds {

bool ACCESSCommand::Exec( const iClient *theClient, const string& Message )
{
	/* Does this user have access to this command? */
	if(bot->getAccess(theClient) < level::access) return false;

	StringTokenizer st(Message);
	
	/* Usage:
	 *  ACCESS
	 *  ACCESS <user>
	 */

	/* If there are no arguments, return the access of theClient */
	if(st.size() == 1) {
		bot->Reply(theClient, "Your access level is %u",
			bot->getAccess(theClient)
			);
		return true;
	}
	
	/* Attempt to find specified user */
	iClient *targetUser = Network->findNick(st[1]);
	if(targetUser) {
		bot->Reply(theClient, "%s's access level is %u",
			targetUser->getNickName().c_str(),
			bot->getAccess(targetUser)
			);
		return true;
	}
	
	return false;
} // ACCESSCommand::Exec(iClient*, const string&)

} // namespace ds

} // namespace gnuworld
