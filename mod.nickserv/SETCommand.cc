/**
 * SETCommand.cc
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
 * Allow a user to set various flags on themselves
 */

#include "gnuworld_config.h"
#include "StringTokenizer.h"

#include "levels.h"
#include "nickserv.h"

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

/* First check if it is a non-true/false variable */
if("CONSOLELEVEL" == property) {
  if(theUser->getLevel() < level::admin::consolelevel) {
    bot->Notice(theClient, "Sorry, you do not have access to this command.");
    return true;
  }

  logging::events::eventType newMask = atoi(status.c_str());
  if(newMask < logging::events::E_MIN || newMask > logging::events::E_MAX) {
    bot->Notice(theClient, "LogMask must be between %u and %u.",
      logging::events::E_MIN, logging::events::E_MAX);
    return true;
  }

  bot->setConsoleLevel(newMask);
  bot->Notice(theClient, "Set console level to %u.", newMask);
  bot->Notice(theClient, "REMINDER: This is only effective until the next restart.");

  return true;
} else if("LOGMASK" == property) {
  if(theUser->getLevel() < level::set::logmask) {
    bot->Notice(theClient, "Sorry, you do not have access to this command.");
    return true;
  }

  logging::events::eventType newMask = atoi(status.c_str());
  if(newMask < logging::events::E_MIN || newMask > logging::events::E_MAX) {
    bot->Notice(theClient, "LogMask must be between %u and %u.",
      logging::events::E_MIN, logging::events::E_MAX);
    return true;
  }

  theUser->setLogMask(newMask);
  theUser->commit();

  bot->Notice(theClient, "LogMask set to %u.", newMask);

  return true;
}

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
