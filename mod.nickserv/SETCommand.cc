/* 
 * SETCommand.cc
 *
 * Allow a user to set various flags on themselves
 */

#include "StringTokenizer.h"

#include "nickserv.h"

const char SETCommand_cc_rcsId[] = "$Id: SETCommand.cc,v 1.3 2002/08/25 23:12:28 jeekay Exp $";

namespace gnuworld
{

namespace ns
{

using std::string;

bool SETCommand::Exec(iClient* theClient, const string& Message)
{
bot->theStats->incStat("NS.CMD.SET");

/* Is this user logged in? */
sqlUser* theUser = bot->isAuthed(theClient);

if(!theUser) {
  bot->Notice(theClient, "You must log in and register before attempting to use commands.");
  return true;
}

StringTokenizer st(Message);

/* Format:
 * SET property status
 *  0      1       2
 */
if(st.size() != 3) {
  Usage(theClient);
  return true;
}

/* Extract our strings from the tokenized message */
string property = string_upper(st[1]);
string status = string_upper(st[2]);

/* Make some sense out of what we've been given for a status */
bool bStatus;

if("ON" == status) {
  bStatus = true;
} else if("OFF" == status) {
  bStatus = false;
} else {
  bot->Notice(theClient, "The value of STATUS must be ON or OFF.");
  return true;
}

/* Find out what flag we are trying to alter */
sqlUser::flagType theFlag = 0;
if("AUTOKILL" == property) theFlag = sqlUser::F_AUTOKILL;
if("RECOVER" == property) theFlag = sqlUser::F_RECOVER;

if(0 == theFlag) {
  bot->Notice(theClient, "Unknown property %s.", property.c_str());
  return true;
}

if(bStatus) {
  theUser->setFlag(theFlag);
} else {
  theUser->removeFlag(theFlag);
}

theUser->commit();

bot->Notice(theClient, "%s set to %s", property.c_str(), status.c_str());

return true;
} // bool SETCommand::Exec

} // namespace ns

} // namespace gnuworld
