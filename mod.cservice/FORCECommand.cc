/**
 * FORCECommand.cc
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
 * $Id: FORCECommand.cc,v 1.16 2005/12/05 17:32:15 kewlio Exp $
 */

#include	<string>
#include	<utility>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"

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

	int admLevel = bot->getAdminAccessLevel(theUser);
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

	// Check for NOFORCE flag
	if ((theChan->getFlag(sqlChannel::F_NOFORCE)) && (admLevel < level::immune::noforce))
	{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::chan_noforce,
				string("The NOFORCE flag is set on %s")).c_str(),
			st[1].c_str());
		return false;
	}

 	/*
	 * Add an entry to this channel records 'Force List'.
	 * This list is checked in getEffectiveAccess(), and will return
	 * the relevant amount of access the admin has forced too.
	 */

	theChan->forceMap.insert(sqlChannel::forceMapType::value_type(
		theUser->getID(),
		std::make_pair(admLevel, theUser->getUserName())
	));

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

} // namespace gnuworld.
