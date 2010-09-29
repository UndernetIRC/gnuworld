/**
 * SHUTDOWNCommand.cc
 *
 * 28/12/2001 - Matthias Crauwels <ultimate_@wol.be>
 * Initial Version.
 *
 * Shuts down the bot and squits the server
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
 * $Id: SHUTDOWNCommand.cc,v 1.8 2005/01/08 23:33:42 dan_karrels Exp $
 */

#include	<string>

#include	"StringTokenizer.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"

const char SHUTDOWNCommand_cc_rcsId[] = "$Id: SHUTDOWNCommand.cc,v 1.8 2005/01/08 23:33:42 dan_karrels Exp $" ;
namespace gnuworld
{
using std::string ;
using namespace level;

bool SHUTDOWNCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.SHUTDOWN");

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

sqlUser* theUser = bot->isAuthed(theClient, false);
if (!theUser)
	{
	return false;
	}

int admLevel = bot->getAdminAccessLevel(theUser);
if (admLevel < level::shutdown)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::insuf_access,
			string("Sorry, you have insufficient access to perform that command.")));
	return false;
	}

bot->logAdminMessage("%s issued the shutdown command! So long, and thanks for all the fish!", theClient->getNickName().c_str());

server->Shutdown( st.assemble( 1 ) ) ;

return true;
}

} // namespace gnuworld.
