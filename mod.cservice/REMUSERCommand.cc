/* 
 * REMUSERCommand.cc 
 *
 * 27/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Removes a users access from a particular channel.
 *
 * Caveats: None
 * 
 *
 * $Id: REMUSERCommand.cc,v 1.7 2001/01/30 01:16:15 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"
#include	"libpq++.h"

const char REMUSERCommand_cc_rcsId[] = "$Id: REMUSERCommand.cc,v 1.7 2001/01/30 01:16:15 gte Exp $" ;
 
namespace gnuworld
{

using namespace gnuworld;
 
bool REMUSERCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() < 3 )
	{
		Usage(theClient);
		return true;
	}

	static const char* queryHeader = "DELETE FROM levels WHERE "; 

	strstream theQuery; 
	ExecStatusType status;
 
 	/*
	 *  First, check the channel is registered.
	 */
 
	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if (!theChan) {
		bot->Notice(theClient, "Sorry, %s isn't registered with me.", st[1].c_str());
		return false;
	} 

	/*
	 *  Fetch the sqlUser record attached to this client. If there isn't one,
	 *  they aren't logged in - tell them they should be.
	 */

	sqlUser* theUser = bot->isAuthed(theClient, true);
	if (!theUser) return false; 
 
	/*
	 *  Check the user has sufficient access on this channel.
	 */
	sqlUser* targetUser = bot->getUserRecord(st[2]);

	int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
	if ((level < level::remuser) && ((targetUser) && targetUser != theUser))
	{
		bot->Notice(theClient, "You have insufficient access to perform that command.");
		return false;
	} 
 
	/*
	 *  Check the person we're trying to remove actually exists.
	 */
 
	if (!targetUser)
	{
		bot->Notice(theClient, "Sorry, I don't know who %s is.", st[2].c_str());
		return false; 
	}
 
	/*
	 *  Check this user has access on this channel.
	 */

	sqlLevel* tmpLevel = bot->getLevelRecord(targetUser, theChan);

	if (!tmpLevel)
	{
		bot->Notice(theClient, "%s doesn't appear to have access in %s.", targetUser->getUserName().c_str(), theChan->getName().c_str());
		return false;
	}

	int targetLevel = tmpLevel->getAccess();

	/*
	 *  Check we aren't trying to remove someone with access higher than ours.
	 *  Unless they are trying to remove themself.. in which case its ok ;)
	 */

	if ((level <= targetLevel) && (targetUser != theUser))
	{
		bot->Notice(theClient, "Cannot remove a user with equal or higher access than your own");
		return false;
	} 

	if ((targetLevel == 500) && (targetUser == theUser))
	{
		bot->Notice(theClient, "You can't remove yourself from a channel you own");
		return false;
	} 

	/*
	 *  Now, build up the SQL query & execute it!
	 */

	theQuery << queryHeader 
	<< "channel_id = " << theChan->getID()
	<< " AND user_id = " << targetUser->getID() 
	<< ";" << ends;
 
	elog << "sqlQuery> " << theQuery.str() << endl; 

	if ((status = bot->SQLDb->Exec(theQuery.str())) == PGRES_COMMAND_OK)
	{
		bot->Notice(theClient, "Removed user %s from %s", targetUser->getUserName().c_str(), theChan->getName().c_str());
	} else {
		bot->Notice(theClient, "Something went wrong: %s", bot->SQLDb->ErrorMessage()); // Log to msgchan here.
 	}
 
	/* Remove tmpLevel from the cache. (It has to be there, we just got it even if it wasnt..)
	 * If its a forced record, don't bother.. */

	if (!tmpLevel->getFlag(sqlLevel::F_FORCED))
	{ 
		pair<int, int> thePair;
		thePair = make_pair(tmpLevel->getUserId(), tmpLevel->getChannelId());
		bot->sqlLevelCache.erase(thePair);
	}

	return true ;
} 

} // namespace gnuworld.
