/* VOICECommand.cc */ 

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"Network.h"
#include	"levels.h"

const char VOICECommand_cc_rcsId[] = "$Id: VOICECommand.cc,v 1.4 2000/12/24 02:25:49 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
using namespace level;
 
bool VOICECommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}
 
	/* 
	 *  Check the channel exists.
	 */

	sqlChannel* theChan = bot->getChannelRecord(st[1]);

	if (theChan) {

		/*
		 *  Fetch the sqlUser record attached to this client. If there isn't one,
		 *  they aren't logged in.
		 */

		sqlUser* theUser = bot->isAuthed(theClient, true);
		if (!theUser) {
			return false;
		}
 
		// Check this user has access.
		int level = bot->getAccessLevel(theUser, theChan);
		if (level > level::voice) 
		{
			iClient* target = Network->findNick(st[2]);
			if( target == NULL )
			{
				bot->Notice( theClient, "I don't see %s anywhere.", st[2].c_str());
				return true;
			}

			Channel* tmpChan = Network->findChannel(theChan->getName());
			if (tmpChan) {
// TODO:				bot->Voice(tmpChan, target);
			} else {
				bot->Notice(theClient, "Sorry, that channel is empty.");
			} 
		} else {
			bot->Notice(theClient, "Sorry, you have insufficient access to perform that command.");
		}

	} else {
		bot->Notice(theClient, "Sorry, %s isn't registered with me.", st[1].c_str());
	}

	return true ;
} 

} // namespace gnuworld.
 
