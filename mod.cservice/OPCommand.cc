/* OPCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 

const char OPCommand_cc_rcsId[] = "$Id: OPCommand.cc,v 1.1 2000/12/21 22:28:16 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool OPCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}
 
	sqlChannel* theChan = new sqlChannel(bot->SQLDb);

	/* 
	 *  Check the channel exists.
	 */
	if (theChan->loadData(st[1])) {
		sqlUser* theUser = new sqlUser(bot->SQLDb);

		/*
		 *  TODO: theUser comes from the sqlUser theClient is logged in as.
		 */
		if (theUser->loadData(theClient->getNickName())) { 
			// Check this user has access.
			int level = bot->getAccessLevel(theUser, theChan);
			bot->Notice(theClient, "%s and %s (Level %i).", theUser->getUserName().c_str(), theUser->getEmail().c_str(), level);
		} else {
			bot->Notice(theClient, "I would.. but.. one teensy problem.  You're not in %s's database.", st[1].c_str());
		} 
	} else {
		bot->Notice(theClient, "I am not on that channel!");
	}

	return true ;
} 

} // namespace gnuworld.

