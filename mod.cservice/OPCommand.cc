/* OPCommand.cc */ 

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"Network.h"
#include	"levels.h"

const char OPCommand_cc_rcsId[] = "$Id: OPCommand.cc,v 1.5 2000/12/24 02:25:49 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
using namespace level;
 
bool OPCommand::Exec( iClient* theClient, const string& Message )
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
		if (level > level::op) 
		{
			iClient* target = Network->findNick(st[2]);
			if( target == NULL )
			{
				bot->Notice( theClient, "I don't see %s anywhere.", st[2].c_str());
				return true;
			}

			Channel* tmpChan = Network->findChannel(theChan->getName());
			if (tmpChan) 
			{
				ChannelUser* tmpChanUser = tmpChan->findUser(target) ;
				if (tmpChanUser) {
					bot->Op(tmpChan, target);
					elog << "opping" << endl;
				} else {
	 				bot->Notice(theClient, "Sorry, %s isn't on %s.", target->getNickName().c_str(), theChan->getName().c_str()); 
					return false;
				} 
			} else {
				bot->Notice(theClient, "Sorry, that channel is empty.");
				return false;
			} 
		} else {
			bot->Notice(theClient, "Sorry, you have insufficient access to perform that command.");
			return false;
		} 

	} else {
		bot->Notice(theClient, "Sorry, %s isn't registered with me.", st[1].c_str());
		return false;
	}

	return true ;
} 

} // namespace gnuworld.

