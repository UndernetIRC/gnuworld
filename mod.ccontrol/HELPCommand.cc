/**
 * HELPCommand.cc
 * Shows help text on commands
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
 * $Id: HELPCommand.cc,v 1.19 2003/06/28 01:21:19 dan_karrels Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"commLevels.h"

const char HELPCommand_cc_rcsId[] = "$Id: HELPCommand.cc,v 1.19 2003/06/28 01:21:19 dan_karrels Exp $";

namespace gnuworld
{

using std::string ;


// help [command]
namespace uworld
{

bool HELPCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;

if(!dbConnected)
        {
        bot->Notice(theClient,"Sorry, but the db connection is down now, please try again alittle later");
        return false;
        }

ccUser *tmpAuth = bot->IsAuth(theClient);
//if(!tmpAuth)
//	return false;
string banner = "--- Help Menu for " ;
banner += bot->getNickName() + " --- (Showing commands which are available for you)" ;

bot->Notice( theClient, "%s", banner.c_str() ) ;

int ComLevel;
// Check if the user didnt supply a command 
if( 1 == st.size() )
	{
	// Spit out all commands
	string Show;
	int Num = 0;
	for( ccontrol::constCommandIterator ptr = bot->command_begin() ;
		ptr != bot->command_end() ; ++ptr )
		{
		ComLevel = ptr->second->getFlags();
		//ComLevel &= ~flg_NOLOG; 
		if((ComLevel & commandLevel::flg_NOLOGIN) || ((tmpAuth) && (tmpAuth->gotAccess(ptr->second))) )
			{
			Show += (ptr->second->getName() + " ");
			++Num;
			if(Show.size() > 80)
				{
				bot->Notice( theClient, "%s", Show.c_str()) ;
				Num = 0;
				Show.assign("");
				}
			}
		}
	if(!Show.empty())
		bot->Notice( theClient, "%s", Show.c_str()) ;
	bot->Notice(theClient,"End of command list");
	}
else //Supplied a command, show only the help for that command (if it exists)
	{
	ccontrol::constCommandIterator ptr =
		bot->findCommand( string_upper( st[ 1 ] ) ) ;
	if( ptr == bot->command_end() )
		{
		bot->Notice( theClient, "Command not found" ) ;
		}
	else
		{
			bot->GetHelp(theClient,st[1]);
		}
	}

return true ;
}

}
}
