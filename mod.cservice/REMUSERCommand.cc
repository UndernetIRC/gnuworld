/**
 * REMUSERCommand.cc
 *
 * 27/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Removes a users access from a particular channel.
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
 * $Id: REMUSERCommand.cc,v 1.19 2010/04/10 18:56:06 danielaustin Exp $
 */

#include	<map>
#include	<sstream>
#include	<string>
#include	<utility>
#include	<iostream>
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"levels.h"
#include	"dbHandle.h"
#include	"responses.h"
#include	"Network.h"

namespace gnuworld
{
using std::pair ;
using std::ends ;
using std::endl ;
using std::string ;
using std::stringstream ;

bool REMUSERCommand::Exec( iClient* theClient, const string& Message )
{
	bot->incStat("COMMANDS.REMUSER");

	StringTokenizer st( Message ) ;
	if( st.size() < 3 )
	{
		Usage(theClient);
		return true;
	}

	static const char* queryHeader = "DELETE FROM levels WHERE ";

	stringstream theQuery;

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
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::chan_not_reg,
				string("Sorry, %s isn't registered with me.")).c_str(),
			st[1].c_str());
		return false;
	}


	/*
	 *  Check the user has sufficient access on this channel.
	 */
	sqlUser* targetUser = bot->getUserRecord(st[2]);

	int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
	if (((level < level::remuser) || ((st[1] == "*") && (level < adminlevel::remuser))) &&
		((targetUser) && targetUser != theUser))
	{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::insuf_access,
				string("You have insufficient access to perform that command.")));
		return false;
	}

	/*
	 *  Check the person we're trying to remove actually exists.
	 */


	if (!targetUser)
	{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::not_registered,
				string("Sorry, I don't know who %s is.")).c_str(),
			st[2].c_str());
		return false;
	}

	/*
	 *  Check this user has access on this channel.
	 */

	sqlLevel* tmpLevel = bot->getLevelRecord(targetUser, theChan);

	if (!tmpLevel)
	{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::doesnt_have_access,
				string("%s doesn't appear to have access in %s.")).c_str(),
			targetUser->getUserName().c_str(), theChan->getName().c_str());
		return false;
	}

	int targetLevel = tmpLevel->getAccess();

	/*
	 *  Check we aren't trying to remove someone with access higher than ours.
	 *  Unless they are trying to remove themself.. in which case its ok ;)
	 */

	if ((theChan->getName() == "*") && (targetUser == theUser))
	{
		bot->Notice(theClient,
                        bot->getResponse(theUser,
                                language::cant_rem_higher,
                                string("CSC has your soul! YOU CAN NEVER ESCAPE!")));
                return false;
	}

	if ((targetLevel == 500) && (targetUser == theUser))
	{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::cant_rem_owner_self,
				string("You can't remove yourself from a channel you own")));
		return false;
	}

	if ((level <= targetLevel) && (targetUser != theUser))
	{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::cant_rem_higher,
				string("Cannot remove a user with equal or higher access than your own")));
		return false;
	}



	/*
	 *  Now, build up the SQL query & execute it!
	 */

	theQuery << queryHeader
		<< "channel_id = " << theChan->getID()
		<< " AND user_id = " << targetUser->getID()
		<< ";" << ends;

	if (bot->SQLDb->Exec(theQuery))
	{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::removed_user,
				string("Removed user %s from %s")).c_str(),
			targetUser->getUserName().c_str(), theChan->getName().c_str());
		if (targetUser != theUser)
			bot->NoteAllAuthedClients(targetUser, bot->getResponse(targetUser,language::acc_rem).c_str(), theChan->getName().c_str());
		if ((theChan->getName() == "*") && (targetUser == theUser))
		{
			bot->Notice(theClient,"CSC is You!! YOU CAN NEVER ESCAPE!");
			//bot->Notice(theClient,"I will always remember you!");
		}
	} else {
		LOGSQL_ERROR( bot->SQLDb ) ;
		bot->dbErrorMessage(theClient);
 	}

	/* Remove tmpLevel from the cache. (It has to be there, we just got it even if it wasnt..) */
	bot->removeLevelCache(tmpLevel);
	delete(tmpLevel);
	
	// If strictop is set, the client has no longer right for op
	Channel* tmpChan = Network->findChannel(theChan->getName());
	if ((tmpChan) && (theChan->getFlag(sqlChannel::F_STRICTOP)))
	        bot->deopSuspendedOnChan(tmpChan,targetUser);

	// Announce the manager about the new access change
	if (level < 500)
	{
		string theMessage = TokenStringsParams("%s removed %s from channel %s",
				theUser->getUserName().c_str(), targetUser->getUserName().c_str(), theChan->getName().c_str());
		bot->NoteChannelManager(theChan, theMessage.c_str());
	}

	return true ;
}

} // namespace gnuworld.
