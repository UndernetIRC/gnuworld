/**
 * SERVNOTICECommand.cc
 *
 * 03/01/2001 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Sends a server notice to a channel.
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
 * $Id: SERVNOTICECommand.cc,v 1.5 2003/06/28 01:21:20 dan_karrels Exp $
 */

#include	<string>
#include	<map>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"

const char SERVNOTICECommand_cc_rcsId[] = "$Id: SERVNOTICECommand.cc,v 1.5 2003/06/28 01:21:20 dan_karrels Exp $" ;

namespace gnuworld
{
using std::map ;

bool SERVNOTICECommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.SERVNOTICE");

StringTokenizer st( Message ) ;
if( st.size() < 3 )
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
 *  Check the user has sufficient admin access to do this.
 */

int admLevel = bot->getAdminAccessLevel(theUser);
if (admLevel < level::servnotice)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::insuf_access).c_str());
	return false;
	}

Channel* tmpChan = Network->findChannel(st[1]);
if (!tmpChan)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::chan_is_empty).c_str(),
		st[1].c_str());
	return false;
	}

string theMessage = st.assemble(2);
bot->serverNotice(tmpChan, "%s", theMessage.c_str());

return true ;
}

} // namespace gnuworld.

