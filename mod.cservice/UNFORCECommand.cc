/* UNFORCECommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"
#include	"responses.h"

const char UNFORCECommand_cc_rcsId[] = "$Id: UNFORCECommand.cc,v 1.4 2001/01/16 01:31:40 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool UNFORCECommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}

	/*
	 *  Fetch the sqlUser record attached to this client. If there isn't one,
	 *  they aren't logged in - tell them they should be.
	 */

	sqlUser* theUser = bot->isAuthed(theClient, true);
	if (!theUser) return false; 
 
	sqlChannel* admChan = bot->getChannelRecord("*");

	int admLevel = bot->getAccessLevel(theUser, admChan);
	if (admLevel < level::force)
	{
		bot->Notice(theClient, "Sorry, you have insufficient access to perform that command.");
		return false;
	} 

 	/*
	 *  First, check the channel is registered.
	 */
 
	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if (!theChan) 
	{
		bot->Notice(theClient, "Sorry, %s isn't registered with me.", st[1].c_str());
		return false;
	} 
 
	/*
	 *  Look in the user level cache to see if this user is present.
	 *  If they are, check the isForced() status, and if true
	 *  drop them from the cache.
	 */

	pair<int, int> thePair; 
	thePair = make_pair(theUser->getID(), theChan->getID()); 
 
	cservice::sqlLevelHashType::iterator ptr = bot->sqlLevelCache.find(thePair);
	if(ptr != bot->sqlLevelCache.end()) // Found something!
	{ 
		if (ptr->second->getFlag(sqlLevel::F_FORCED)) // Forced access, drop it.
		{
			bot->sqlLevelCache.erase(thePair);
			bot->Notice(theClient, "Removed your temporary access of %i from channel %s", admLevel, theChan->getName().c_str());
//			bot->logAdminMessage("%s has removed their forced access on %s", 
//				theUser->getUserName().c_str(), theChan->getName().c_str());
			return true;
		}
	} 

	bot->Notice(theClient, "You don't appear to have a forced access in %s, perhaps it expired?", theChan->getName().c_str()); 
	return true ;
} 

} // namespace gnuworld.

