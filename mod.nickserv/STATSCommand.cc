/*
 * STATSCommand.cc
 *
 * Give the user various statistics
 */

#include "levels.h" 
#include "nickserv.h"
#include "responses.h"
#include "Stats.h"

const char STATSCommand_cc_rcsId[] = "$Id: STATSCommand.cc,v 1.3 2002/11/26 03:33:24 jeekay Exp $";

namespace gnuworld
{

namespace ns
{

using std::string;

bool STATSCommand::Exec(iClient* theClient, const string& Message)
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
