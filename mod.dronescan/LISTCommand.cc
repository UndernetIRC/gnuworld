/*
 * LISTCommand.cc
 *
 * Display information about things.
 *
 * 2003-06-20	GK@NG	Initial writing
 */

#include "Network.h"
#include "StringTokenizer.h"

#include "dronescan.h"
#include "dronescanCommands.h"

namespace gnuworld {

namespace ds {

bool LISTCommand::Exec( const iClient *theClient, const string& Message )
{
	/* Usage: LIST <name> */

	StringTokenizer st(Message);
	
	if(st.size() < 2) {
		Usage(theClient);
		return true;
	}
	
	string Name = string_upper(st[1]);
	
	if("ACTIVE" == Name) {
		bot->Reply(theClient, "Active drone channels:");
		
		for(dronescan::droneChannelsType::const_iterator itr =
		    bot->droneChannels.begin() ; itr != bot->droneChannels.end()
		    ; ++itr) {
			bot->Reply(theClient, "  %s",
				(*itr).c_str()
				);
		}
	}
	
	return true;
} // LISTCommand::Exec(iClient*, const string&)

} // namespace ds

} // namespace gnuworld
