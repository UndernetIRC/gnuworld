/**
 * REGISTERCommand.cc
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
 * This command allows a user to register their AC nick
 */

#include <ctime>

#include "gnuworld_config.h"
#include "netData.h"
#include "nickserv.h"

namespace gnuworld
{

namespace ns
{

using std::string;

bool REGISTERCommand::Exec(iClient* theClient, const string& )
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
