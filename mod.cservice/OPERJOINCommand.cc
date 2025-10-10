/**
 * OPERJOINCommand.cc
 *
 * 10/02/2001 - David Henriksen <david@itwebnet.dk>
 * Initial Version. Written, and finished.
 *
 * Allows an oper to make cmaster join a registered channel.
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
 * $Id: OPERJOINCommand.cc,v 1.16 2008/04/16 20:34:39 danielaustin Exp $
 */

#include	<string>
#include	<sstream>
#include	<iostream>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"
#include	"Network.h"
#include	"cservice_config.h"

namespace gnuworld
{
using std::endl ;
using std::ends ;
using std::string ;
using std::stringstream ;

bool OPERJOINCommand::Exec( [[maybe_unused]] iClient* theClient, [[maybe_unused]] const string& Message )
{
#ifdef USE_OPERPARTJOIN

StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

/*
 *  Check the user is an oper.
 */
sqlUser* theUser = bot->isAuthed(theClient, false);

if(!theClient->isOper())
	{
	bot->Notice(theClient, bot->getResponse(theUser,
		language::ircops_only_cmd,
		"This command is reserved to IRC Operators"));
	return true;
	}

/*
 *  Check the channel is actually registered.
 */

Channel* tmpChan = Network->findChannel(st[1]);
sqlChannel* theChan = bot->getChannelRecord(st[1]);
if (!theChan)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::chan_not_reg,
		"The channel %s doesn't appear to be registered").c_str(),
		st[1].c_str());
	return false;
	}

/* Check the bot isn't in the channel. */
if (theChan->getInChan())
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::already_on_chan,
		"I'm already in that channel!"));
	return false;
	}

// Tell the world.

bot->Wallops( "An IRC Operator is asking me to join channel %s",
	theChan->getName().c_str() ) ;

bot->logAdminMessage("%s is asking me to join channel %s",
		theClient->getNickUserHost().c_str(),
		theChan->getName().c_str());

bot->writeChannelLog(theChan, theClient, sqlChannel::EV_OPERJOIN, "");

theChan->setInChan(true);
bot->Join(theChan->getName(),
	theChan->getChannelMode(),
	theChan->getChannelTS(),
	true);
bot->incrementJoinCount();

if (tmpChan)
	{
	if(theChan->getFlag(sqlChannel::F_NOOP))
		{
		bot->deopAllOnChan(tmpChan);
		}
	if(theChan->getFlag(sqlChannel::F_STRICTOP))
		{
		bot->deopAllUnAuthedOnChan(tmpChan);
		}
	}

#endif // USE_OPERPARTJOIN

return true;

}

} // namespace gnuworld.
