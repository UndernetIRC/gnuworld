/**
 * ISREGCommand.cc
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
 * $Id: ISREGCommand.cc,v 1.8 2003/06/28 01:21:20 dan_karrels Exp $
 */

#include	<string>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"responses.h"

const char ISREGCommand_cc_rcsId[] = "$Id: ISREGCommand.cc,v 1.8 2003/06/28 01:21:20 dan_karrels Exp $" ;

namespace gnuworld
{

using std::string ;

bool ISREGCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.ISREG");

StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

sqlUser* theUser = bot->isAuthed(theClient, false);
sqlChannel* theChan = bot->getChannelRecord(st[1]);

if (theChan)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::is_reg,
			string("%s is registered.")).c_str(),
		theChan->getName().c_str());
	}
else
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::is_not_reg,
			string("%s is not registered.")).c_str(),
		st[1].c_str());
	}

return true ;
}

} // namespace gnuworld.

