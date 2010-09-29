/**
 * STATSCommand.cc
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
 * Give the user various statistics
 */

#include "gnuworld_config.h"
#include "levels.h"
#include "nickserv.h"
#include "responses.h"
#include "Stats.h"

namespace gnuworld
{

namespace ns
{

using std::string;

bool STATSCommand::Exec(iClient* theClient, const string& )
{
bot->theStats->incStat("NS.CMD.STATS");

sqlUser* theUser = bot->isAuthed(theClient);

if(!theUser || (theUser->getLevel() < level::admin::stats)) {
  bot->Notice(theClient, responses::noAccess);
  return true;
}

Stats::statsMapType::const_iterator statsBegin = bot->theStats->getStatsMapBegin();
Stats::statsMapType::const_iterator statsEnd   = bot->theStats->getStatsMapEnd();

for(Stats::statsMapType::const_iterator ptr = statsBegin; ptr != statsEnd; ptr++) {
  string stat = ptr->first;
  unsigned int amount = ptr->second;
  bot->Notice(theClient, "%s: %u", stat.c_str(), amount);
}

return true;
} // STATSCommand

} // namespace ns

} // namespace gnuworld
