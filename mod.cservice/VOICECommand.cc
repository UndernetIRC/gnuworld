/* VOICECommand.cc */ 

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"Network.h"
#include	"levels.h"

const char VOICECommand_cc_rcsId[] = "$Id: VOICECommand.cc,v 1.1 2000/12/22 22:22:49 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool VOICECommand::Exec( iClient* theClient, const string& Message )
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
			if (level > 25) {
				iClient* target = Network->findNick(st[2]);
				if( target == NULL )
				{
					bot->Notice( theClient, "I don't see %s anywhere.", st[2].c_str());
					return true;
				}

				// TODO: Update gnuworld internal state - or write a gnuw voice function. :)
				strstream tmp ;
				tmp << bot->getCharYYXXX() << " M " << theChan->getName() << " +v "
					<< target->getCharYYXXX() << ends ;
	
				bot->Write( tmp ) ;
				delete[] tmp.str() ;
	
				bot->Notice(theClient, "Username: %s, Email: %s (Level %i).", theUser->getUserName().c_str(), theUser->getEmail().c_str(), level);
			}
		} else {
			bot->Notice(theClient, "I would.. but.. one teensy problem.  You're not in %s's database.", st[1].c_str());
		} 
	} else {
		bot->Notice(theClient, "I am not on that channel!");
	}

	return true ;
} 

} // namespace gnuworld.

