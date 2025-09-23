/**
 * JOINCommand.cc
 *
 * 10/02/2001 - David Henriksen <david@itwebnet.dk>
 * Initial Version. Written, and finished.
 *
 * Makes cmaster join a registered channel.
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
 * $Id: JOINCommand.cc,v 1.15 2008/04/16 20:34:39 danielaustin Exp $
 */
#include	<string>
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"
#include	"Network.h"

namespace gnuworld
{
using std::string ;

bool JOINCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.JOIN");

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
if (!theUser)
	{
	return false;
	}

/*
 *  Check the channel is actually registered.
 */

sqlChannel* theChan = bot->getChannelRecord(st[1]);
if (!theChan)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::chan_not_reg).c_str(),
		st[1].c_str());
	return false;
	}

/*
 *  Check the user has sufficient access on this channel.
 */

int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
if (level < level::join)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::insuf_access).c_str());
	return false;
	}

/* Check the bot isn't in the channel. */
if (theChan->getInChan())
	{
	bot->Notice(theClient, bot->getResponse(theUser,
		language::already_on_chan, "I'm already in that channel!"));
	return false;
	}

bot->writeChannelLog(theChan, theClient, sqlChannel::EV_JOIN, "");

theChan->setInChan(true);
/* force setting mode +R - it is probably set already, but doesn't hurt to check */
bot->Join(theChan->getName(),
	"+R",
	theChan->getChannelTS(),
	true);
bot->incrementJoinCount();

return true;
}

} // namespace gnuworld
