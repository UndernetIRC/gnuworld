/* 
 * ADDUSERCommand.cc 
 *
 * 26/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Adds a new user to a channel, obeying common sense.
 *
 * Caveats: None
 *
 * $Id: ADDUSERCommand.cc,v 1.6 2001/01/14 23:12:09 gte Exp $
 */
 
#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"
#include	"libpq++.h"

const char ADDUSERCommand_cc_rcsId[] = "$Id: ADDUSERCommand.cc,v 1.6 2001/01/14 23:12:09 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool ADDUSERCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() < 4 )
	{
		Usage(theClient);
		return true;
	}

	static const char* queryHeader = "INSERT INTO levels (channel_id, user_id, access, flags, added, added_by, last_modif, last_modif_by, last_updated) "; 

	strstream theQuery; 
	ExecStatusType status;
	int targetAccess = atoi(st[3].c_str());

	/*
	 *  Fetch the sqlUser record attached to this client. If there isn't one,
	 *  they aren't logged in - tell them they should be.
	 */

	sqlUser* theUser = bot->isAuthed(theClient, true);
	if (!theUser) return false; 

 	/*
	 *  First, check the channel is registered.
	 */
 
	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if (!theChan) {
		bot->Notice(theClient, "Sorry, %s isn't registered with me.", st[1].c_str());
		return false;
	} 
 
	/*
	 *  Check the user has sufficient access on this channel.
	 */

	int level = bot->getAccessLevel(theUser, theChan);
	if (level < level::adduser)
	{
		bot->Notice(theClient, "Sorry, you have insufficient access to perform that command.");
		return false;
	} 

	/*
	 *  Check we aren't trying to add someone with access higher than ours.
	 */

	if (level <= targetAccess)
	{
		bot->Notice(theClient, "Cannot add a user with equal or higher access than your own.");
		return false;
	}

	if ((targetAccess <= 0) || (targetAccess > 999))
	{
		bot->Notice(theClient, "Invalid access level.");
		return false;
	}

	/*
	 *  Check the person we're trying to add is actually registered.
	 */

	sqlUser* targetUser = bot->getUserRecord(st[2]);
	if (!targetUser)
	{
		bot->Notice(theClient, "Sorry, I don't know who %s is.", st[2].c_str());
		return false; 
	}
 
	/*
	 *  Check this user doesn't already have access on this channel.
	 */

	sqlLevel* newLevel;
	newLevel = bot->getLevelRecord(targetUser, theChan);
	int levelTest = newLevel ? newLevel->getAccess() : 0;

	if (levelTest != 0)
	{
		/*
		 *  If the current access is Forced via FORCE, then allow the addition anyway..
		 */
		
		if (!newLevel->getFlag(sqlLevel::F_FORCED))
		{ 
			bot->Notice(theClient, "%s is already added to %s with access level %i.", targetUser->getUserName().c_str(), theChan->getName().c_str(), levelTest);
			return false;
		}

		sqlLevel testLevel(bot->SQLDb);
		if (testLevel.loadData(targetUser->getID(), theChan->getID()))
		{
			// If a forced person is trying to add themselves and they are already in the DB..
			bot->Notice(theClient, "%s is already added to %s with access level %i.", targetUser->getUserName().c_str(), theChan->getName().c_str(), levelTest);
			return false;
		}

		if(newLevel->getFlag(sqlLevel::F_FORCED))
		{
			/*
			 *  If the access is forced and is now actually being added, 
			 *  update the cache.
			 */
			 newLevel->setAccess(targetAccess);
		}
	}
 
	/*
	 *  Now, build up the SQL query & execute it!
	 */

	theQuery << queryHeader << "VALUES (" 
	<< theChan->getID() << ","
	<< targetUser->getID() << ","
	<< targetAccess << ","
	<< "0" << ","
	<< ::time(NULL) << ","
	<< "'" << theClient->getNickUserHost() << "',"
	<< ::time(NULL) << ","
	<< "'" << theClient->getNickUserHost() << "'," 
	<< ::time(NULL)
	<< ");"
	<< ends; 
 
	elog << "sqlQuery> " << theQuery.str() << endl; 

	if ((status = bot->SQLDb->Exec(theQuery.str())) == PGRES_COMMAND_OK)
	{
		bot->Notice(theClient, "Added user %s to %s with access level %i", targetUser->getUserName().c_str(), theChan->getName().c_str(), targetAccess);
	} else {
		bot->Notice(theClient, "Something went wrong: %s", bot->SQLDb->ErrorMessage()); // Log to msgchan here.
 	}

	return true ;
} 

} // namespace gnuworld.
