/* 
 * INFOCommand.cc
 *
 * Allows an administrator to see what attributes a registered user has.
 */

#include "StringTokenizer.h"

#include "levels.h"
#include "nickserv.h"

const char INFOCommand_cc_rcsId[] = "$Id: INFOCommand.cc,v 1.1 2002/11/25 03:56:15 jeekay Exp $";

namespace gnuworld {

namespace ns {

bool INFOCommand::Exec(iClient* theClient, const string& Message)
{
bot->theStats->incStat("NS.CMD.INFO");

sqlUser* theUser = bot->isAuthed(theClient);

if(!theUser || (theUser->getLevel() < level::admin::info)) {
  bot->Notice(theClient, "Sorry, you do not have access to this command.");
  return true;
}

StringTokenizer st(Message);

// INFO nick
if(st.size() != 2) {
  Usage(theClient);
  return true;
}

sqlUser* targetUser = bot->isRegistered(st[1]);

if(!targetUser) {
  bot->Notice(theClient, "Unable to find nick.");
  return true;
}

bot->Notice(theClient, "Nick: %s", targetUser->getName().c_str());
bot->Notice(theClient, "Level: %u; Flags: %u; LogMask: %u",
  targetUser->getLevel(), targetUser->getFlags(), targetUser->getLogMask());

return true;
}

} // namespace ns

} // namespace gnuworld
