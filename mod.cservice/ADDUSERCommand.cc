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
 * $Id: ADDUSERCommand.cc,v 1.13 2001/02/22 19:09:34 gte Exp $
 */
 
#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"
#include	"libpq++.h"
#include	"responses.h"

const char ADDUSERCommand_cc_rcsId[] = "$Id: ADDUSERCommand.cc,v 1.13 2001/02/22 19:09:34 gte Exp $" ;

namespace gnuworld
{

using std::string ;

static const char* queryHeader = "INSERT INTO levels (channel_id, user_id, access, flags, added, added_by, last_modif, last_modif_by, last_updated) "; 
 
bool ADDUSERCommand::Exec( iClient* theClient, const string& Message )
{ 
StringTokenizer st( Message ) ;
if( st.size() < 4 )
	{
	Usage(theClient);
	return true;
	}

/*
 *  Fetch the sqlUser record attached to this client. If there isn't one,
 *  they aren't logged in - tell them they should be.
 */

sqlUser* theUser = bot->isAuthed(theClient, true);
if (!theUser)
	{
	return false; 
	}

/*
 *  First, check the channel is registered.
 */
 
sqlChannel* theChan = bot->getChannelRecord(st[1]);
if (!theChan)
	{
	bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::chan_not_reg,
			string("Sorry, %s isn't registered with me.")).c_str(),
		st[1].c_str()
	);
	return false;
	} 
 
/*
 *  Check the user has sufficient access on this channel.
 */

int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
if (level < level::adduser)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::insuf_access,
			string("You have insufficient access to perform that command."))
	);
	return false;
	} 

/*
 *  Check we aren't trying to add someone with access higher than ours.
 */
int targetAccess = atoi(st[3].c_str());

if (level <= targetAccess)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::access_higher,
			string("Cannot add a user with equal or higher access than your own."))
	);
	return false;
	}

if ((targetAccess <= 0) || (targetAccess > 999))
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::inval_access,
			string("Invalid access level."))
	);
	return false;
	}

/*
 *  Check the person we're trying to add is actually registered.
 */

sqlUser* targetUser = bot->getUserRecord(st[2]);
if (!targetUser)
	{
	bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::not_registered,
			string("Sorry, I don't know who %s is.")).c_str(),
		st[2].c_str()
	);
	return false; 
	}
 
/*
 *  Check this user doesn't already have access on this channel.
 */

sqlLevel* newLevel = bot->getLevelRecord(targetUser, theChan);
int levelTest = newLevel ? newLevel->getAccess() : 0 ;

if (levelTest != 0)
	{
	/*
	 * If the current access is Forced via FORCE, then
	 * allow the addition anyway..
	 */
		
	if (!newLevel->getFlag(sqlLevel::F_FORCED))
		{ 
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::already_in_list,
				string("%s is already added to %s with access level %i.")).c_str(),
			targetUser->getUserName().c_str(),
			theChan->getName().c_str(),
			levelTest);
		return false;
		}

	sqlLevel testLevel(bot->SQLDb);
	if (testLevel.loadData(targetUser->getID(), theChan->getID()))
		{
		// If a forced person is trying to add themselves and they are already in the DB..
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::already_in_list,
				string("%s is already added to %s with access level %i.")).c_str(),
			targetUser->getUserName().c_str(),
			theChan->getName().c_str(),
			levelTest);
		return false;
		}

	if(newLevel->getFlag(sqlLevel::F_FORCED))
		{
		/*
		 *  If the access is forced and is now actually being added, 
		 *  update the cache.
		 */
		 newLevel->setAccess(targetAccess);
		 // Record is on the DB now.
		 newLevel->setFlag(sqlLevel::F_ONDB);
		}
	}

/*
 *  Work out the flags this user should default to.
 */
 
unsigned short targetFlags = 0;

if (theChan->getUserFlags() == 1) targetFlags = sqlLevel::F_AUTOOP;
if (theChan->getUserFlags() == 2) targetFlags = sqlLevel::F_AUTOVOICE;

/*
 *  Now, build up the SQL query & execute it!
 */

strstream theQuery; 
theQuery << queryHeader << "VALUES (" 
	<< theChan->getID() << ","
	<< targetUser->getID() << ","
	<< targetAccess << ","
	<< targetFlags << ","
	<< bot->currentTime() << ","
	<< "'" << theClient->getNickUserHost() << "',"
	<< bot->currentTime() << ","
	<< "'" << theClient->getNickUserHost() << "'," 
	<< bot->currentTime()
	<< ");"
	<< ends; 
 
elog << "ADDUSER::sqlQuery> " << theQuery.str() << endl; 

ExecStatusType status = bot->SQLDb->Exec(theQuery.str()) ;
if( PGRES_COMMAND_OK == status )
	{
	bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::add_success,
			string("Added user %s to %s with access level %i")).c_str(),
		targetUser->getUserName().c_str(),
		theChan->getName().c_str(),
		targetAccess);


		/*
		 *  "If they where added to *, set their invisible flag" (Ace).
		 */

		if (theChan->getName() == "*")
		{
			targetUser->setFlag(sqlUser::F_INVIS);
			targetUser->commit();
		}
	}
else
	{
	bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::its_bad_mmkay,
			string("Something went wrong: %s")).c_str(),
		bot->SQLDb->ErrorMessage());
	// TODO: Log to msgchan here.
 	}

delete[] theQuery.str() ;

return true ;
} 

} // namespace gnuworld.
