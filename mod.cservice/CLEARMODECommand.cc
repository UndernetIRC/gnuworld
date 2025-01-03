/**
 * CLEARMODECommand.cc
 *
 * 16/02/2001 - David Henriksen <david@itwebnet.dk>
 * Initial version.
 *
 * Clears all channel modes.
 *
 * Caveats: None.
 *
 * Todo: Support ircu2.10.11's CLEARMODE feature.
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
 * $Id: CLEARMODECommand.cc,v 1.13 2005/11/17 19:08:08 kewlio Exp $
 */

#include	<string>
#include	<sstream>
#include	<iostream>

#include	"StringTokenizer.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"
#include	"Network.h"

namespace gnuworld
{
using std::endl ;
using std::ends ;
using std::string ;
using std::stringstream ;

using namespace gnuworld;

bool CLEARMODECommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.CLEARMODE");

StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

sqlUser* theUser = bot->isAuthed(theClient, true);
if(!theUser)
	{
	return false;
	}

sqlChannel* theChan = bot->getChannelRecord(st[1]);
if(!theChan)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::chan_not_reg).c_str(),
		st[1].c_str());
	return false;
	}

/* Check the bot is in the channel. */

if (!theChan->getInChan())
	{
	bot->Notice(theClient, bot->getResponse(theUser,
		language::i_am_not_on_chan, "I'm not in that channel!"));
	return false;
	}

int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
if(level < level::clearmode)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::insuf_access).c_str(),
		st[1].c_str());
	return false;
	}

// Cannot clear modes, if E hasn't joined.

Channel* tmpChan = Network->findChannel(theChan->getName());
if(!tmpChan)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::chan_is_empty).c_str(),
		st[1].c_str());
	return false;
	}

bot->ClearMode( tmpChan, string( "mstnipklrDcC" ), false ) ;

bot->Notice(theClient,
    bot->getResponse(theUser,
	    language::modeclear_done,
	    string("%s: Cleared channel modes.")).c_str(),
		theChan->getName().c_str());

// Send action opnotice to channel if OPLOG is enabled
if (theChan->getFlag(sqlChannel::F_OPLOG))
{
	bot->NoticeChannelOps(theChan->getName(),
		"%s (%s) cleared channel modes.",
		theClient->getNickName().c_str(), theUser->getUserName().c_str());
}

return true ;
}

} // namespace gnuworld.
