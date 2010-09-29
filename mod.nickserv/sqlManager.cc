/**
 * sqlManager.cc
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
 */

#include <new>
#include <iostream>

#include <cassert>

#include "gnuworld_config.h"
#include "ELog.h"
#include "sqlManager.h"
#include	"dbHandle.h"
#include <cstdlib>

using std::endl ;

namespace gnuworld {

namespace ns {

/* Initialise our static data members */
sqlManager* sqlManager::theManager = 0;

/*********************************
 ** S T A T I C   M E T H O D S **
 *********************************/

/**
 * Implementation of singleton - this is the only way to get a reference to
 * the sqlManager instance, and it does not allow modification.
 * initialise must be called prior to attempted to obtain an instance.
 * This method is static.
 */
sqlManager* sqlManager::getInstance(const string& _dbString, int _commitQueueMax)
{
if(theManager) return theManager;

/* There is currently no sqlManager instance */
return new sqlManager(_dbString, _commitQueueMax);
} // static sqlManager* sqlManager::getInstance(const string&)

/*********************************
 ** P U B L I C   M E T H O D S **
 *********************************/

/**
 * This method creates and returns a connection to the database.
 * It will use the stored dbString to create the connection.
 */
dbHandle* sqlManager::getConnection()
{
elog << "*** [sqlManager:getConnection] Attempting DB connection to: "
  << dbString << endl;

dbHandle* tempCon = new (std::nothrow) dbHandle(dbString.c_str());
assert(tempCon != 0);

if(tempCon->ConnectionBad()) {
  elog << "*** [sqlManager:getConnection] Unable to connect to DB: "
    << tempCon->ErrorMessage() << endl;
  ::exit(0);
} else {
  elog << "*** [sqlManager:getConnection] Connection established to DB."
    << endl;
}

return tempCon;
}

/**
 * This method will disconnect and destroy a database connection.
 * This method should never be passed a null pointer.
 */
void sqlManager::removeConnection(dbHandle* tempCon)
{
assert(tempCon != 0);

elog << "*** [sqlManager:removeConnection] Removing DB connection." << endl;
delete tempCon;
}

/**
 * This method simply processes all statements in the queue, executing
 * them against the database.
 * TODO: Should this be inside a transaction?
 */
void sqlManager::flush()
{
  theStats->incStat("SM.FLUSH");
  for(CommitQueueItr ptr = commitQueue.begin(); ptr != commitQueue.end(); ++ptr) {
    string statement = *ptr;
    theStats->incStat("SM.EXEC");

#ifdef LOG_SQL
    elog << "*** [sqlManager:flush] Executing: " << statement << endl;
#endif
    theLogger->log(logging::events::E_SQL, "Executing: " + statement);
    if(!SQLDb->Exec(statement)) {
      theStats->incStat("SM.ERROR");
      string error = string(SQLDb->ErrorMessage());
#ifndef LOG_SQL
      /* Make sure people without LOG_SQL still see what statement failed */
      elog << "*** [sqlManager:flush] Executing: " << statement << endl;
#endif
      elog << "*** [sqlManager:flush] Error: " << error << endl;

      theLogger->log(logging::events::E_ERROR, "Executing: " + statement);
      theLogger->log(logging::events::E_ERROR, "Error    : " + error);
      // TODO: Log error
    }
  }

  commitQueue.clear();
}


/**
 * This method allows a caller to add a statement to the commit queue.
 * The statement will be executed against the database when the next commit
 * interval occurs.
 */
void sqlManager::queueCommit(const string& theStatement)
{
  commitQueue.push_back(theStatement);

  if(commitQueue.size() >= commitQueueMax) {
    flush();
  }
}


/*****************************************************
 ** C O N S T R U C T O R   &   D E S T R U C T O R **
 *****************************************************/

/**
 * This is our constructor that initialises DB communications
 * and any of the queues that will be used
 * It is only ever called from initialise()
 */
sqlManager::sqlManager(const string& _dbString, int _commitQueueMax)
{
/* Construct our DB object and initialise queues */
dbString = _dbString;
SQLDb = getConnection();
commitQueueMax = _commitQueueMax;

theStats = Stats::getInstance();
theLogger = logging::Logger::getInstance();
} // sqlManager::sqlManager


/**
 * This is our constructor that ensures the proper teardown of
 * our DB link and clears out any memory currently in use by
 * queues or objects
 */
sqlManager::~sqlManager()
{
/* Destruct our DB object and any queues */
elog << "*** [sqlManager] Shutting down DB communications." << endl;
removeConnection(SQLDb);
} // sqlManager::~sqlManager()


} // namespace ns

} // namespace gnuworld
