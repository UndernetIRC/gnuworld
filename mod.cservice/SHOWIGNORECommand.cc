/**
 * SHOWIGNORECommand.cc
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
 * $Id: SHOWIGNORECommand.cc,v 1.13 2003/06/28 01:21:20 dan_karrels Exp $
 */

#include	<string>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"responses.h"

const char SHOWIGNORECommand_cc_rcsId[] = "$Id: SHOWIGNORECommand.cc,v 1.13 2003/06/28 01:21:20 dan_karrels Exp $" ;

namespace gnuworld
{
using std::string ;

bool SHOWIGNORECommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.SHOWIGNORE");

StringTokenizer st( Message ) ;
if( st.size() < 1 )
	{
	Usage(theClient);
	return true;
	}

size_t count = 0;

sqlUser* theUser = bot->isAuthed(theClient, false);

// TODO: Violation of encapsulation
if( bot->silenceList.empty() )
	{
	bot->Notice(theClient, bot->getResponse(theUser,
		language::ignore_list_empty,
		string("Ignore list is empty")));
	return true ;
	}

bot->Notice(theClient,
	bot->getResponse(theUser, language::ignore_list_start,
	string("Ignore list:")));

// TODO: Same as above
for( cservice::silenceListType::const_iterator ptr = bot->silenceList.begin() ;
	ptr != bot->silenceList.end() ; ++ptr )
	{
	bot->Notice(theClient, bot->getResponse(theUser,
		language::rpl_ignorelist, "%s for %i minutes").c_str(),
		ptr->first.c_str(),
		(((ptr->second.first - bot->currentTime()) / 60) % 60));
	count++;
	}

bot->Notice(theClient,
	bot->getResponse(theUser, language::ignore_list_end,
	string("-- End of Ignore List")));

return true ;
}

} // namespace gnuworld.
