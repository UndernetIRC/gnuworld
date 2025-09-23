/**
 * REGISTERCommand.cc
 *
 * 26/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Registers a channel.
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
 * $Id: REGISTERCommand.cc,v 1.24 2009/07/31 07:29:13 mrbean_ Exp $
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
#include	"Network.h"
#include	"responses.h"

namespace gnuworld
{
using std::pair ;
using std::string ;
using std::endl ;
using std::ends ;
using std::stringstream ;

using namespace gnuworld;

bool REGISTERCommand::Exec( iClient* theClient, const string& Message )
{
	bot->incStat("COMMANDS.REGISTER");

	bool instantReg = false;

	if (bot->getConfRequiredSupporters() == 0)
		instantReg = true;

	/*
	 *  Fetch the sqlUser record attached to this client. If there isn't one,
	 *  they aren't logged in - tell them they should be.
	 */

	sqlUser* theUser = bot->isAuthed(theClient, true);
	if (!theUser) return false;

	/*
	 *  Check the user has sufficient access for this command..
	 */
	int level = bot->getAdminAccessLevel(theUser);
	if ((level < level::registercmd) && (!instantReg))
	{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::insuf_access,
				string("You have insufficient access to perform that command.")));
		return false;
	}

	StringTokenizer st( Message ) ;

	if (st.size() < 3)
	{
		if (level == 0)
			Usage(theClient);
		else
			bot->Notice(theClient, "SYNTAX: REGISTER <#channel> [username]");
		return true;
	}

 	/*
	 *  First, check the channel isn't already registered.
	 */

	sqlChannel* theChan;
	theChan = bot->getChannelRecord(st[1]);
	if (theChan)
	{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::chan_already_reg,
				string("%s is already registered with me.")).c_str(),
			st[1].c_str());
		return false;
	}

	sqlUser* tmpUser;

	// In case of instant channel registration, the target user can be only the issuer user
	if (instantReg)
		tmpUser = theUser;

	if ((st.size() > 2) && (level))
	{
		tmpUser = bot->getUserRecord(st[2]);
		if (!tmpUser)
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::not_registered,
					string("The user %s doesn't appear to be registered.")).c_str(),
			st[2].c_str());
		return true;
		}
	}

	if (!bot->isValidChannel(st[1]))
	{
		if (!bot->validResponseString.empty())
		{
			bot->Notice(theClient, "Cannot register, channel is %s",bot->getCurrentValidResponse().c_str());
			bot->validResponseString.clear();
		}
       	return false;
	}

	string::size_type pos = st[1].find_first_of( ',' ); /* Don't allow comma's in channel names. :) */

	if ( (st[1][0] != '#') || (string::npos != pos))
	{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::inval_chan_name,
				string("Invalid channel name.")));
		return false;
	}

	return bot->sqlRegisterChannel(theClient, tmpUser, st[1]);
}

} // namespace gnuworld.
