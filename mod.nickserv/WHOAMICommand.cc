/**
 * WHOAMICommand.cc
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
 */

#include	<sstream>

#include	"gnuworld_config.h"
#include	"nickserv.h"

using std::stringstream ;
using std::ends ;

namespace gnuworld
{

namespace ns
{

using std::string;

bool WHOAMICommand::Exec(iClient* theClient, const string&)
{
bot->theStats->incStat("NS.CMD.WHOAMI");

sqlUser* theUser = bot->isAuthed(theClient);

stringstream theResponse;
theResponse << "Nick: " << theClient->getNickName()
  << "; Account: " << theClient->getAccount();

if(theUser) {
  theResponse << "; Level: " << theUser->getLevel()
    << "; Flags: " << theUser->getFlags()
    << "; LogMask: " << theUser->getLogMask()
    ;
}

theResponse << ends;

bot->Notice(theClient, theResponse.str());

return true;
}

} // namespace ns

} // namespace gnuworld
