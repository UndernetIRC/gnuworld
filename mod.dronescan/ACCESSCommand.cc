/*
 * ACCESSCommand.cc
 *
 * Return the access level of the current or specified user.
 *
 * 2003-06-15	GK@NG	Initial writing
 */

#include "Network.h"
#include "StringTokenizer.h"

#include "levels.h"
#include "dronescan.h"
#include "dronescanCommands.h"

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
