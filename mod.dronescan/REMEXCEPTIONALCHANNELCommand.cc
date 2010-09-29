/**
 * ADDEXCEPTIONALCHANNELCommand.cc
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
 * Removes an exceptional channel from the exceptional channels list
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

void REMEXCEPTIONALCHANNELCommand::Exec( const iClient *theClient, const string& Message, const sqlUser* theUser )
{
	if(theUser->getAccess() < level::remExceptionalChannel) return ;

	StringTokenizer st(Message);

	/* Usage:
	 *  REMEXCEPTIONALCHANNEL <channel name>
	 */
	if( st.size() != 2 )
	{
		Usage(theClient);
		return;
	}
	if( st[1][0] != '#' )
	{
		bot->Reply(theClient, "Channel name must start with #");
		return;
	}
	if(!bot->isExceptionalChannel(st[1]))
	{
		bot->Reply(theClient, "Channel %s isn't an exceptionalChannel",st[1].c_str());
		return;
	}
	if(!bot->remExceptionalChannel(st[1]))
	{
		bot->Reply(theClient, "Failed to remove %s from the exceptional channels list",
			st[1].c_str());
	} else {
		bot->Reply(theClient, "Successfully removed %s from the exceptional channels list",
			st[1].c_str());
	}

	return ;
} // REMEXCEPTIONALCHANNELCommand::Exec(iClient*, const string&)

} // namespace ds

} // namespace gnuworld
