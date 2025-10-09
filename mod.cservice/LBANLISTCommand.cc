/**
 * LBANLISTCommand.cc
 *
 * 13/01/2001 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Lists internal bot banlist for a channel.
 *
 * Caveats: None.
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
 * $Id: LBANLISTCommand.cc,v 1.15 2007/08/07 21:22:29 kewlio Exp $
 */

#include	<string>
#include	<ctime>
#include	"StringTokenizer.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"
#include	"cservice_config.h"
#include	"time.h"
#include	"banMatcher.h"

namespace gnuworld
{
using std::string ;
using namespace level;

bool LBANLISTCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 3 )
	{
	Usage(theClient);
	return true;
	}

// Is the channel registered?

sqlUser* theUser = bot->isAuthed(theClient, true);
if (!theUser)
	return false;

sqlChannel* theChan = bot->getChannelRecord(st[1]);
if(!theChan)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::chan_not_reg,
			string("Sorry, %s isn't registered with me.")).c_str(),
		st[1].c_str());
	return false;
	}

/* Show all results? */
bool showAll = false;

for( StringTokenizer::const_iterator ptr = st.begin() ; ptr != st.end() ;
	++ptr )
	{
	if (string_lower(*ptr) == "-all")
		{
		sqlUser* tmpUser = bot->isAuthed(theClient, false);
		if ( tmpUser && bot->getAdminAccessLevel(tmpUser) )
			{
			showAll = true;
			}
		continue;
		}
	}

bot->Notice(theClient,
	bot->getResponse(theUser,
		language::lbanlist_for,
		string("\002*** Ban List for channel %s ***\002")).c_str(),
	theChan->getName().c_str());

size_t results = 0;
time_t ban_expires = 0;
time_t ban_expires_d = 0;
time_t ban_expires_f = 0;

for( std::map< int,sqlBan* >::const_iterator ptr = theChan->banList.begin() ; ptr != theChan->banList.end() ; ++ptr )
	{
	const sqlBan* theBan = ptr->second;
	ban_expires = theBan->getExpires();
	/* If its expired.. just don't show it - it'll be removed soon ;) */
	if ((ban_expires >= bot->currentTime()) || (ban_expires == 0))
		{
		if (!match(fixAddress(st[2]), theBan->getBanMask()))
			{
			results++;
			/* escape the loop if we exceed our ban limit */
			if ((results > MAX_LBAN_RESULTS) && !showAll)
			{
				break;
			}
			if (ban_expires > 0)
			{
				ban_expires_d = ban_expires - bot->currentTime();
				ban_expires_f = bot->currentTime() - ban_expires_d;
			}
			else
				ban_expires_f = 0;
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::lban_entry,
					string("%s %s Level: %i")).c_str(),
				theChan->getName().c_str(),
				theBan->getBanMask().c_str(),
				theBan->getLevel());
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::lban_added_by,
					string("ADDED BY: %s (%s)")).c_str(),
				theBan->getSetBy().c_str(),
				theBan->getReason().c_str());
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::lban_since,
					string("SINCE: %s")).c_str(),
				ctime(&theBan->getSetTS()));
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::lban_exp,
					string("EXP: %s")).c_str(),
				prettyDuration(ban_expires_f).c_str());
			}
		}
	} // for()

/* if there are more results than we are configured to show,
   tell the user (in their preferred language */
if( (results > MAX_LBAN_RESULTS) && !showAll)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::more_than_max,
			string("There are more than %d matching entries.")).c_str(),
		MAX_LBAN_RESULTS);
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::restrict_query,
			string("Please restrict your query.")));
	}
else if (results > 0)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::lban_end,
			string("\002*** END ***\002")));
	}
else
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::no_match,
			string("No match!")));
	}

return true ;
}

} // namespace gnuworld.

