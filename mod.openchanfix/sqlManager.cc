/**
 * sqlManager.cc
 *
 * Author: Rasmus Hansen <jeekay@netgamers.org>
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
 * $Id: sqlManager.cc,v 1.6 2009/01/15 08:31:07 denspike Exp $
 */

#include <new>
#include <iostream>

#include <cassert>
#include <cstdlib>

#include "gnuworld_config.h"
#include "ELog.h"
#include "sqlManager.h"

namespace gnuworld
{

namespace cf
{

/* Initialize our static data members */
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
sqlManager* sqlManager::getInstance(const std::string& _dbString)
{
if(theManager) return theManager;

/* There is currently no sqlManager instance */
return new sqlManager(_dbString);
} // static sqlManager* sqlManager::getInstance(const std::string&)



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
  << dbString << std::endl;

dbHandle* tempCon = new (std::nothrow) dbHandle(dbString);
assert(tempCon != 0);

if(tempCon->ConnectionBad()) {
  elog << "*** [sqlManager:getConnection] Unable to connect to DB: "
    << tempCon->ErrorMessage() << std::endl;
  ::exit(0);
} else {
  elog << "*** [sqlManager:getConnection] Connection established to DB."
    << std::endl;
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

elog << "*** [sqlManager:removeConnection] Removing DB connection." << std::endl;
delete tempCon; tempCon = 0;
}

/**
 * This is our destructor that ensures the proper teardown of
 * our DB link and clears out any memory currently in use by
 * objects
 */
void sqlManager::removeManager()
{
/* Destruct our DB object */
elog << "*** [sqlManager] Shutting down DB communications." << std::endl;
removeConnection(SQLDb);
delete theManager; theManager = 0;
} // sqlManager::removeManager()


/*****************************************************
 ** C O N S T R U C T O R   &   D E S T R U C T O R **
 *****************************************************/

/**
 * This is our constructor that initialises DB communications.
 * It is only ever called from initialize().
 */
sqlManager::sqlManager(const std::string& _dbString)
{
/* Construct our DB object */
dbString = _dbString;
SQLDb = getConnection();
} // sqlManager::sqlManager(const std::string&)

/**
 * This is a dummy destructor.
 * The real code is in sqlManager::removeManager().
 */
sqlManager::~sqlManager()
{
} // sqlManager::~sqlManager()


} // namespace cf

} // namespace gnuworld
