/*
 * MODUSERCommand.cc
 *
 * Allow an administrator to alter the level of another user
 */

#include "StringTokenizer.h"

#include "levels.h" 
#include "nickserv.h"
#include "responses.h"
#include "sqlUser.h"

const char MODUSERCommand_cc_rcsId[] = "$Id: MODUSERCommand.cc,v 1.1 2002/11/26 03:33:24 jeekay Exp $";

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
  unsigned int newLevel = atoi(st[3].c_str());
  
  if(newLevel < 0 || newLevel > 999) {
    bot->Notice(theClient, "Sorry, level must be between between 0 and 999, inclusive.");
    return true;
  }
  
  if(newLevel >= theUser->getLevel()) {
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
