/**
 * MODUSERCommand.cc
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
 * Allow an administrator to alter the level of another user
 */

#include "gnuworld_config.h"
#include "StringTokenizer.h"

#include "levels.h"
#include "nickserv.h"
#include "responses.h"
#include "sqlUser.h"

namespace gnuworld
{

namespace ns
{

using std::string;

bool MODUSERCommand::Exec(iClient* theClient, const string& Message)
{

bot->theStats->incStat("NS.CMD.MODUSER");

sqlUser* theUser = bot->isAuthed(theClient);

if(!theUser || (theUser->getLevel() < level::admin::moduser)) {
  bot->Notice(theClient, responses::noAccess);
  return true;
}

// MODUSER user ACCESS level
StringTokenizer st(Message);

if(st.size() != 4) {
  Usage(theClient);
  return true;
}

sqlUser* targetUser = bot->isRegistered(st[1]);
if(!targetUser) {
  bot->Notice(theClient, responses::noSuchUser);
  return true;
}

if(targetUser == theUser) {
  bot->Notice(theClient, "Sorry, you may not modify your own access level.");
  return true;
}

string attribute = string_upper(st[2]);

if("ACCESS" == attribute) {
  int newLevel = atoi(st[3].c_str());

  if(newLevel < 0 || newLevel > 999) {
    bot->Notice(theClient, "Sorry, level must be between between 0 and 999, inclusive.");
    return true;
  }

  if(static_cast< unsigned int >( newLevel ) >= theUser->getLevel()) {
    bot->Notice(theClient, "You cannot raise another user's level above your own.");
    return true;
  }

  if(targetUser->getLevel() >= theUser->getLevel()) {
    bot->Notice(theClient, "You may not modify a user with a higher level than you.");
    return true;
  }

  targetUser->setLevel(newLevel);
  targetUser->commit();

  bot->Notice(theClient, "Successfully set level of %s to %u",
              targetUser->getName().c_str(), newLevel);

  return true;
}

bot->Notice(theClient, "Sorry, unknown attribute %s.", attribute.c_str());

return true;
} // MODUSERCommand

} // namespace ns

} // namespace gnuworld
