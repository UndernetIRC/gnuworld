/**
 * sqlUser.cc
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
 * Stores a database user
 */

#include <ctime>
#include <sstream>

#include "gnuworld_config.h"
#include "sqlManager.h"
#include "sqlUser.h"
#include	"misc.h"

namespace gnuworld {

namespace ns {

using std::stringstream ;

const sqlUser::flagType sqlUser::F_SUSPEND  = 0x0001;
const sqlUser::flagType sqlUser::F_AUTOKILL = 0x0002;
const sqlUser::flagType sqlUser::F_RECOVER  = 0x0004;

unsigned long int sqlUser::maxUserId = 0;

/**
 * Default constructor.
 * This simply creates an empty, zeroed sqlUser
 */
sqlUser::sqlUser(sqlManager* _myManager) :
  id(0),
  name(""),
  flags(0),
  level(0),
  lastseen_ts(0),
  registered_ts(0),
  logmask(0)
{
  myManager = _myManager;
}

/**
 * Default destructor.
 * This does nothing as we have no heap space allocated
 */
sqlUser::~sqlUser()
{
}

/**
 * This function commits the current state of the sqlUser back to
 * the backend DB
 */
void sqlUser::commit()
{
  /* Use reference to sqlManager to queue a commit request */
  stringstream userCommit;
  userCommit << "UPDATE users SET"
    << " name = '" << name << "'"
    << ", flags = " << flags
    << ", level = " << level
    << ", lastseen_ts = " << lastseen_ts
    << ", registered_ts = " << registered_ts
    << ", logmask = " << logmask
    << " WHERE id = " << id;
  myManager->queueCommit(userCommit.str());
}

/**
 * This function updates this users lastseen back to the DB
 */
void sqlUser::commitLastSeen()
{
  lastseen_ts = time(NULL);
  commit();
}

/**
 * This function simply deletes this users entry from the DB.
 * It is possible to call insertUser() on a sqlUser that has
 * just been deleteUser()'d
 */
void sqlUser::deleteUser()
{
  /* Construct our delete statement */
  stringstream commitStatement;
  commitStatement << "DELETE FROM users WHERE"
    << " id = " << id;
  myManager->queueCommit(commitStatement.str());
}

/**
 * This function inserts a brand new user into the DB.
 * It is a slight fudge, in that it first creates a blank record then
 * calls commit() to update the data fields for that record. This is done
 * so that any new fields added will automatically be dealt with in commit()
 * instead of in 50 different functions.
 */
void sqlUser::insertUser()
{
  /* Grab the next available user id */
  id = ++maxUserId;

  /* Construct our insert statement */
  stringstream commitStatement;
  commitStatement << "INSERT INTO users (id,name) VALUES ("
    << id
    << ", '" << name << "'"
    << ")";
  myManager->queueCommit(commitStatement.str());
  commit();
}

/**
 * This function loads in data from a DB handle,
 * to initialise the object properly
 */
void sqlUser::setAllMembers(dbHandle* theDB, int row)
{
  /* Grab the data, set the members */
id = atoi(theDB->GetValue(row, 0));
name = theDB->GetValue(row, 1);
flags = atoi(theDB->GetValue(row, 2));
level = atoi(theDB->GetValue(row, 3));
lastseen_ts = atoi(theDB->GetValue(row, 4));
registered_ts = atoi(theDB->GetValue(row, 5));
logmask = atoi(theDB->GetValue(row, 6));

if(id > maxUserId) maxUserId = id;
}

} // namespace ns

} // namespace gnuworld
