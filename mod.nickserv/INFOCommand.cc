/* 
 * INFOCommand.cc
 *
 * Allows an administrator to see what attributes a registered user has.
 */

#include "StringTokenizer.h"

#include "levels.h"
#include "nickserv.h"
#include "responses.h"

const char INFOCommand_cc_rcsId[] = "$Id: INFOCommand.cc,v 1.2 2002/11/26 03:33:24 jeekay Exp $";

namespace gnuworld {

namespace ns {

bool INFOCommand::Exec(iClient* theClient, const string& Message)
{
bot->theStats->incStat("NS.CMD.INFO");

sqlUser* theUser = bot->isAuthed(theClient);

if(!theUser || (theUser->getLevel() < level::admin::info)) {
  bot->Notice(theClient, responses::noAccess);
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
  bot->Notice(theClient, responses::noSuchUser);
  return true;
}

bot->Notice(theClient, "Nick: %s", targetUser->getName().c_str());
bot->Notice(theClient, "Level: %u; Flags: %u; LogMask: %u",
  targetUser->getLevel(), targetUser->getFlags(), targetUser->getLogMask());

return true;
}

} // namespace ns

} // namespace gnuworld
