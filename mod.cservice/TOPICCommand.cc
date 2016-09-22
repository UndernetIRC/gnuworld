/**
 * TOPICCommand.cc
 *
 * 26/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * 30/12/2000 - David Henriksen <david@itwebnet.dk>
 * Wrote TOPIC Command.
 *
 * Sets a topic in the channel.
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
 * $Id: TOPICCommand.cc,v 1.13 2003/06/28 01:21:20 dan_karrels Exp $
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

const char TOPICCommand_cc_rcsId[] = "$Id: TOPICCommand.cc,v 1.13 2003/06/28 01:21:20 dan_karrels Exp $" ;

namespace gnuworld
{
using std::string ;
using std::endl ;
using std::ends ;
using std::stringstream ;

bool TOPICCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.TOPIC");

StringTokenizer st( Message ) ;
if( st.size() < 3 )
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
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::i_am_not_on_chan,
			string("I'm not in that channel!")));
	return false;
	}

int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
if(level < level::topic)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::insuf_access).c_str(),
		st[1].c_str());
	return false;
	}

// Cannot set topic, if the bot hasn't joined.

Channel* tmpChan = Network->findChannel(theChan->getName());
if(!tmpChan)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::chan_is_empty).c_str(),
		st[1].c_str());
	return false;
	}

// Done with the checking.

string topic = st.assemble(2);

// Default ircu TOPICLEN - maxusername?
// TODO: Put into config somewhere
if( topic.size() > 145 )
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
		language::topic_max_len,
		string("ERROR: Topic cannot exceed 145 chars")));
	return false;
        }

stringstream s;
s	<< bot->getCharYYXXX()
	<< " T "
	<< st[1]
	<< " :("
	<< theUser->getUserName()
	<< ") "
	<< topic
	<< ends;

bot->Write( s );

return true ;
}

} // namespace gnuworld.
