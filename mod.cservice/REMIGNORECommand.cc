/**
 * REMIGNORECommand.cc
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
 * $Id: REMIGNORECommand.cc,v 1.10 2003/06/28 01:21:20 dan_karrels Exp $
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

const char REMIGNORECommand_cc_rcsId[] = "$Id: REMIGNORECommand.cc,v 1.10 2003/06/28 01:21:20 dan_karrels Exp $" ;

namespace gnuworld
{
using std::endl ;
using std::ends ;
using std::string ;
using std::stringstream ;

bool REMIGNORECommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.REMIGNORE");

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

int admLevel = bot->getAdminAccessLevel(theUser);
if (admLevel < level::remignore)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::insuf_access,
			string("Sorry, you have insufficient access to perform that command.")));
	return false;
	}

for( cservice::silenceListType::iterator ptr = bot->silenceList.begin() ;
	ptr != bot->silenceList.end() ; ++ptr )
	{
	if ( string_lower(st[1]) == string_lower(ptr->first.c_str()) )
		{
		stringstream s;
		s	<< bot->getCharYYXXX()
			<< " SILENCE * -"
			<< ptr->first.c_str()
			<< ends;
		bot->Write( s );

		/*
		 * Locate this user by numeric.
		 * If the numeric is still in use, clear the ignored flag.
		 * If someone else has inherited this numeric, no prob,
		 * its cleared anyway.
		 */

		iClient* netClient = Network->findClient(ptr->second.second);
		if (netClient)
			{
			bot->setIgnored(netClient, false);
			}

		bot->silenceList.erase(ptr);
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::unsilenced,
				string("Removed %s from my ignore list")).c_str(),
			st[1].c_str());
		bot->logAdminMessage("%s (%s) has removed ignore: %s",
			theClient->getNickName().c_str(),
			theUser->getUserName().c_str(),
			st[1].c_str());
		return true;
		}

	} // for()

bot->Notice(theClient,
	bot->getResponse(theUser,
		language::couldnt_find_silence,
		string("Couldn't find %s in my silence list")).c_str(),
	st[1].c_str());

return true ;
}

} // namespace gnuworld
