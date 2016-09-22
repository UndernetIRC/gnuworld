/**
 * ANALYSECommand.cc
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
 * Display information about a given channel.
 */

#include <sys/types.h>
#include <regex.h>

#include "gnuworld_config.h"
#include "Network.h"
#include "StringTokenizer.h"

#include "dronescan.h"
#include "dronescanCommands.h"
#include "levels.h"
#include "sqlUser.h"

namespace gnuworld {

namespace ds {

void ANALYSECommand::Exec( const iClient *theClient, const string& Message, const sqlUser* theUser )
{
	if(theUser->getAccess() < level::analyse) return ;

	StringTokenizer st(Message);

	/* ANALYSE <#channel>
	 */
	if(st.size() != 2) {
		Usage(theClient);
		return ;
	}

	Channel *theChannel = Network->findChannel(st[1]);

	if(!theChannel) {
		bot->Reply(theClient, "Unable to find channel: %s",
			st.assemble(1).c_str()
			);
		return ;
	}



	return ;
} // ANALYSECommand::Exec(iClient*, const string&)

} // namespace ds

} // namespace gnuworld
