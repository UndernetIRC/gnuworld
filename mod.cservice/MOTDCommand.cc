/**
 * MOTDCommand.cc
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
 * $Id: MOTDCommand.cc,v 1.8 2003/06/28 01:21:20 dan_karrels Exp $
 */

#include	<string>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include 	"responses.h"

const char MOTDCommand_cc_rcsId[] = "$Id: MOTDCommand.cc,v 1.8 2003/06/28 01:21:20 dan_karrels Exp $" ;

namespace gnuworld
{
using std::string ;

bool MOTDCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.MOTD");

StringTokenizer st( Message ) ;
if( st.size() != 1 )
	{
	Usage(theClient);
	return true;
	}

	sqlUser* theUser = bot->isAuthed(theClient, false);

	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::motd,
			string("No MOTD set.")));

return true ;
}

} // namespace gnuworld.
