/**
 * MODUSERCommand.cc
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
 *
 * $Id: MODUSERCommand.cc,v 1.1 2003/08/02 18:17:21 jeekay Exp $
 */

#include "StringTokenizer.h"

#include "levels.h"
#include "dronescan.h"
#include "dronescanCommands.h"
#include "sqlUser.h"

RCSTAG("$Id: MODUSERCommand.cc,v 1.1 2003/08/02 18:17:21 jeekay Exp $");

namespace gnuworld {

namespace ds {

bool MODUSERCommand::Exec( const iClient *theClient, const string& Message, const sqlUser* theUser )
{
	if(theUser->getAccess() < level::moduser) return false;

	StringTokenizer st(Message);
	
	/* Usage:
	 *  MODUSER (ACCESS)
	 */
	
	if(st.size() < 2) {
		Usage(theClient);
		return false;
	}
	
	string Command = string_upper(st[1]);
	
	if("ACCESS" == Command) {
		/* Usage:
		 *  MODUSER ACCESS <user> <level>
		 */
		if(st.size() != 4) {
			Usage(theClient);
			return false;
		}
		
		/* Conditions:
		 *  i) User must exist
		 * ii) New level must be < the modifiers
		 */
		
		/* Does the user exist? */
		
		sqlUser *targetUser = bot->getSqlUser(st[2]);
		if(!targetUser) {
			bot->Reply(theClient, "No such user.");
			return true;
		}
		
		/* Is the level less than the modifiers? */
		unsigned int newLevel = atoi(st[3].c_str());
		if(newLevel < 1 || newLevel >= theUser->getAccess()) {
			bot->Reply(theClient, "You cannot grant a user equal or higher access than yourself.");
			return true;
		}
		
		/* Conditions passed. Modify, update. */
		targetUser->setAccess(newLevel);
		if(targetUser->commit()) {
			bot->Reply(theClient, "Successfully changed %s's access to %u.",
				targetUser->getUserName().c_str(),
				newLevel
				);
			return true;
		} else {
			bot->Reply(theClient, "There was a problem changing the access level.");
			return true;
		}
	}
	
	return true;
	
} // MODUSERCommand::Exec(iClient*, const string&, const sqlUser*)

} // namespace ds

} // namespace gnuworld
