/*
 * REGISTERCommand.cc
 *
 * This command allows a user to register their AC nick
 */

#include <ctime>

#include "netData.h"
#include "nickserv.h"

const char REGISTERCommand_cc_rcsId[] = "$Id: REGISTERCommand.cc,v 1.4 2002/08/25 23:12:28 jeekay Exp $";

namespace gnuworld
{

namespace ns
{

using std::string;

bool REGISTERCommand::Exec(iClient* theClient, const string& Message)
{
bot->theStats->incStat("NS.CMD.REGISTER");

/* Is this nick already registered? */
sqlUser* theUser = bot->isRegistered(theClient->getAccount());

if(theUser) {
  bot->Notice(theClient, "The nickname %s has already been registered.",
    theUser->getName().c_str());
  return true;
}

/* This nick is not currently registered. First, create a sqlUser for them */
/* TODO: The sqlManager instance should not be a public variable */
theUser = new sqlUser(bot->theManager);

theUser->setName(theClient->getAccount());

/* Enable AUTOKILL and RECOVER by default */
theUser->setFlag(sqlUser::F_AUTOKILL | sqlUser::F_RECOVER);

theUser->setLastSeenTS(time(NULL));
theUser->setRegisteredTS(time(NULL));
theUser->insertUser();

/* Assign the new user to the iClient */
netData* theData = static_cast< netData* >( theClient->getCustomData(bot) );
theData->authedUser = theUser;

/* Insert the new user into the cache */
bot->addUserToCache(theUser->getName(), theUser);

bot->Notice(theClient, "Your nickname, %s, has been successfully registered.",
  theClient->getAccount().c_str());
bot->Notice(theClient, "Note: AUTOKILL and RECOVER are enabled by default.");

return true;
}

} // namespace ns

} // namespace gnuworld
