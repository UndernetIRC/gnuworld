/* FORCECommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"
#include	"responses.h"

const char FORCECommand_cc_rcsId[] = "$Id: FORCECommand.cc,v 1.10 2001/02/22 19:09:34 gte Exp $" ;

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
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::insuf_access,
				string("Sorry, you have insufficient access to perform that command.")));
		return false;
	} 

 	/*
	 *  First, check the channel is registered/real.
	 */

	if ( (st[1][0] != '#') )
	{
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::inval_chan_name,
				string("Invalid channel name.")));
		return false;
	} 
	 
	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if (!theChan) 
	{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::chan_not_reg,
				string("Sorry, %s isn't registered with me.")).c_str(),
			st[1].c_str());
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
		/*
		 * Set this channel access to their admin level.
		 * getLevelRecord will have cached this entry, we simply don't commit it to
		 * make it temporary.
		 */
 
		newLevel->setForcedAccess(admLevel);
		newLevel->setFlag(sqlLevel::F_FORCED);
		bot->logAdminMessage("%s (%s) is getting access on %s", 
			theClient->getNickName().c_str(), theUser->getUserName().c_str(), theChan->getName().c_str()); 
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::temp_inc_access,
				string("Temporarily increased your access on channel %s to %i")).c_str(), 
			theChan->getName().c_str(), admLevel); 
		bot->writeChannelLog(theChan, theClient, sqlChannel::EV_FORCE, "");
		return true;
	}
 
	/*
	 *  Manually create a new sqlLevel entry, set the members
	 *  and add it to the cache.
	 */

	newLevel = new sqlLevel(bot->SQLDb);
	newLevel->setChannelId(theChan->getID());
	newLevel->setUserId(theUser->getID());
	newLevel->setForcedAccess(admLevel);
	newLevel->setAccess(501);
	newLevel->setFlag(sqlLevel::F_FORCED); 
	newLevel->setSuspendExpire(0);

	/*
	 *  Add it to the cache.
	 */

	pair<int, int> thePair; 
	thePair = make_pair(theUser->getID(), theChan->getID()); 
	bot->sqlLevelCache.insert(cservice::sqlLevelHashType::value_type(thePair, newLevel));
	bot->logAdminMessage("%s (%s) is getting access on %s", 
		theClient->getNickName().c_str(), theUser->getUserName().c_str(), theChan->getName().c_str());
	bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::temp_inc_access,
			string("Temporarily increased your access on channel %s to %i")).c_str(), 
		theChan->getName().c_str(), admLevel);
	bot->writeChannelLog(theChan, theClient, sqlChannel::EV_FORCE, "");
	return true ;
} 

} // namespace gnuworld.

