/*
 * SHUTDOWNCommand.cc
 *
 * Allows an administrator to shut down NickServ cleanly.
 */

#include "StringTokenizer.h"

#include "levels.h"
#include "logTarget.h"
#include "nickserv.h"
#include "sqlUser.h"

const char SHUTDOWNCommand_cc_rcsId[] = "$Id: SHUTDOWNCommand.cc,v 1.1 2002/11/25 03:56:15 jeekay Exp $";

namespace gnuworld {

namespace ns {

bool SHUTDOWNCommand::Exec(iClient* theClient, const string& Message)
{

StringTokenizer st(Message);

if(st.size() < 2) {
  Usage(theClient);
  return true;
}

sqlUser* theUser = bot->isAuthed(theClient);

if(!theUser || (theUser->getLevel() < level::admin::shutdown)) {
  bot->Notice(theClient, "Sorry, you do not have access to this command.");
  return true;
}

stringstream toLog;
toLog << theClient->getNickName() << " ("
      << theUser->getName() << ") "
      << "is asking me to shut down!";

bot->theLogger->log(logging::events::E_ERROR, toLog.str());

stringstream toSQuit;
toSQuit << server->getCharYY() << " SQ "
        << server->getName() << " 0 :("
        << theUser->getName() << ") "
        << st.assemble(1);

stringstream toQuit;
toQuit  << bot->getCharYYXXX() << " Q :"
        << st.assemble(1);

bot->Write(toQuit);
bot->Write(toSQuit);

return true;
} // SHUTDOWNCommand::Exec(iClient*, const string&)

} // namespace ns

} // namespace gnuworld
