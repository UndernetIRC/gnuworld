/**
 * QUOTECommand.cc
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
 * $Id: QUOTECommand.cc,v 1.6 2003/06/28 01:21:20 dan_karrels Exp $
 */

#include	<string>

#include	"cservice.h"
//#include	"sqlUser.h"
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"levels.h"
#include	"responses.h"

const char QUOTECommand_cc_rcsId[] = "$Id: QUOTECommand.cc,v 1.6 2003/06/28 01:21:20 dan_karrels Exp $" ;

namespace gnuworld
{

using std::string ;

bool QUOTECommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.QUOTE");

StringTokenizer st( Message ) ;
if( st.size() < 1 )
	{
	Usage(theClient);
	return true;
	}

sqlUser* theUser = bot->isAuthed(theClient, true);
if (!theUser)
	{
	return false;
 	}

int admLevel = bot->getAdminAccessLevel(theUser);
if (admLevel < level::quote)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::insuf_access,
			string("Sorry, you have insufficient access to perform that command.")));
	return false;
	}

bot->Write( st.assemble(1) );

return true ;
}

} // namespace gnuworld.
