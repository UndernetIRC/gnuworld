/* FORCECommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"
#include	"responses.h"

const char FORCECommand_cc_rcsId[] = "$Id: FORCECommand.cc,v 1.1 2001/01/02 07:55:12 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool FORCECommand::Exec( iClient* theClient, const string& Message )
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
	 *  Check this user doesn't already have access on this channel.
	 *  If they do, simply bump their access level to the level they
	 *  have on *.
	 */

	sqlLevel* newLevel;
	newLevel = bot->getLevelRecord(theUser, theChan);
	int level = newLevel ? newLevel->getAccess() : 0;

	if (level != 0) // If they already have some access..
	{ 
		// Set this channel access to their admin level.
		// getLevelRecord will have cached this entry, we simply don't commit it to
		// make it temporary.
		newLevel->setAccess(admLevel); 
		newLevel->setForced(true);
		bot->Notice(theClient, "Temporarily increased your access on channel %s to %i", theChan->getName().c_str(), admLevel);
		return true;
	}
 
	/*
	 *  Manually create a new sqlLevel entry, set the members
	 *  and add it to the cache.
	 */

	newLevel = new sqlLevel(bot->SQLDb);
	newLevel->setChannelId(theChan->getID());
	newLevel->setUserId(theUser->getID());
	newLevel->setAccess(admLevel); 
	newLevel->setForced(true);

	/*
	 *  Add it to the cache.
	 */

	pair<int, int> thePair; 
	thePair = make_pair(theUser->getID(), theChan->getID()); 
	bot->sqlLevelCache.insert(cservice::sqlLevelHashType::value_type(thePair, newLevel));
	bot->Notice(theClient, "Gave you temporary access of %i on channel %s", admLevel, theChan->getName().c_str());
	return true ;
} 

} // namespace gnuworld.

