/**
 * SAYCommand.cc
 *
 * 13/01/2001 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Command to keep Kev happy :)
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
 * $Id: SAYCommand.cc,v 1.9 2005/12/01 04:04:59 kewlio Exp $
 */

#include	<string>

#include	"StringTokenizer.h"
#include	"cservice.h"
#include	"levels.h"
#include	"Network.h"
#include	"responses.h"

const char SAYCommand_cc_rcsId[] = "$Id: SAYCommand.cc,v 1.9 2005/12/01 04:04:59 kewlio Exp $" ;

namespace gnuworld
{
using std::string ;
using namespace level;

bool SAYCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.SAY");

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

sqlUser* theUser = bot->isAuthed(theClient, false);
if (!theUser)
	{
	return false;
	}

int admLevel = bot->getAdminAccessLevel(theUser);
if (admLevel < level::say)
	{
/* CSC get far too many questions as to how to use these commands
 * so they prefer it kept quiet about 'em.  Fair enough too.
 *
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::insuf_access,
			string("Sorry, you have insufficient access to perform that command.")));
 */
	return false;
	}
/*
 *  First, check the channel is registered.
 */

sqlChannel* theChan = bot->getChannelRecord(st[1]);
if (!theChan)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::chan_not_reg,
			string("Sorry, %s isn't registered with me.")).c_str(),
			st[1].c_str());
	return false;
	}

Channel* tmpChan = Network->findChannel(theChan->getName());
/* is this a "say" or "do" command? */
if (tmpChan)
{
	if (!match("SAY", st[0]))
		bot->Message(tmpChan, st.assemble(2));
	else
		bot->Message(tmpChan, "%cACTION %s%c",
			1, st.assemble(2).c_str(), 1);
}

return true;
}

} // namespace gnuworld.

