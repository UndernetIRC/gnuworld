/**
 * ADDUSERCommand.cc
 *
 * 26/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * 01/03/01 - Daniel Simard <svr@undernet.org>
 * Fixed Language module stuff.
 *
 * Adds a new user to a channel, obeying common sense.
 *
 * Caveats: None
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 * $Id: ADDUSERCommand.cc,v 1.29 2010/04/10 18:56:06 danielaustin Exp $
 */

#include	<map>
#include	<string>
#include	<sstream>
#include	<iostream>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"levels.h"
#include	"dbHandle.h"
#include	"responses.h"
#include	"cservice_config.h"

namespace gnuworld
{
using std::pair ;
using std::endl ;
using std::ends ;
using std::string ;
using std::stringstream ;

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
			language::chan_not_reg).c_str(),
		st[1].c_str()
	);
	return false;
	}

/*
 *  Check the user has sufficient access on this channel.
 */

int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
if ((level < level::adduser) || (( st[1] == "*" ) && (level < adminlevel::adduser)))
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::insuf_access).c_str()
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
			language::access_higher).c_str()
	);
	return false;
	}

if ((targetAccess <= 0) || (targetAccess > 999))
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::inval_access).c_str()
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
			language::not_registered).c_str(),
		st[2].c_str()
	);
	return false;
	}

/*
 *  Check this user doesn't already have access on this channel.
 *  (Note: If they're forced, this will only be shown in
 *  getEffectiveAccess, not by looking at level records).
 */

sqlLevel* newLevel = bot->getLevelRecord(targetUser, theChan);
int levelTest = newLevel ? newLevel->getAccess() : 0 ;

if (levelTest != 0)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::already_in_list).c_str(),
		targetUser->getUserName().c_str(),
		theChan->getName().c_str(),
		levelTest);
	return false;
	}

/* check if this user wants to be added ! */
if (targetUser->getFlag(sqlUser::F_NOADDUSER))
{
	/* this user has elected not to be added to channels at this time */
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::no_adduser,
			string("%s does not wish to be added to channels at this time.")).c_str(),
		targetUser->getUserName().c_str());
	return false;
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

string lastModifMask = "(" + theUser->getUserName() + ") " + theClient->getNickUserHost();

stringstream theQuery;
theQuery	<< queryHeader
		<< "VALUES ("
		<< theChan->getID() << ","
		<< targetUser->getID() << ","
		<< targetAccess << ","
		<< targetFlags << ","
		<< bot->currentTime() << ","
		<< "'" << escapeSQLChars(lastModifMask) << "',"
		<< bot->currentTime() << ","
		<< "'" << escapeSQLChars(lastModifMask) << "',"
		<< bot->currentTime()
		<< ");"
		<< ends;

if( bot->SQLDb->Exec(theQuery ) )
{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::add_success).c_str(),
		targetUser->getUserName().c_str(),
		theChan->getName().c_str(),
		targetAccess);

	if (targetUser != theUser)
		bot->NoteAllAuthedClients(targetUser,bot->getResponse(targetUser,
			language::acc_new,
			string("You have been added to channel %s with access level %i")).c_str(),
				theChan->getName().c_str(), targetAccess);
	/*
	 * Add this new record to the level cache.
	 */

	sqlLevel* newLevel = new (std::nothrow) sqlLevel(bot);
	newLevel->setChannelId(theChan->getID());
	newLevel->setUserId(targetUser->getID());
	newLevel->setAccess(targetAccess);
	newLevel->setFlag(targetFlags);
	newLevel->setAdded(bot->currentTime());
	newLevel->setAddedBy("(" + theUser->getUserName() + ") " + theClient->getNickUserHost());
	newLevel->setLastModif(bot->currentTime());
	newLevel->setLastModifBy("(" + theUser->getUserName() + ") " + theClient->getNickUserHost());

	bot->insertLevelCache(newLevel);

	/*
	 *  "If they where added to *, set their invisible flag" (Ace).
	 */
	if (theChan->getName() == "*")
	{
		targetUser->setFlag(sqlUser::F_INVIS);
		targetUser->commit(theClient);
	}
	// Announce the manager about the new access change
	if (level < 500)
	{
		string theMessage = TokenStringsParams("%s added %s to channel %s with access level %i",
				theUser->getUserName().c_str(), targetUser->getUserName().c_str(), theChan->getName().c_str(), targetAccess);
		bot->NoteChannelManager(theChan, theMessage.c_str());
	}
}
else
	{
	LOG( ERROR, "ADDUSER SQL Error:") ;
	LOGSQL_ERROR( bot->SQLDb ) ;
	bot->dbErrorMessage( theClient ) ;
	}

return true ;
}

} // namespace gnuworld.
