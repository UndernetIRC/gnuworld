/**
 * sqlManager.h
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

#ifndef SQLMANAGER_H
#define SQLMANAGER_H

#include <string>
#include <vector>

#include "dbHandle.h"

#include "Logger.h"
#include "Stats.h"

namespace gnuworld {

namespace ns {

using std::string ;
using std::vector ;

class sqlManager {
  public:
    /**
     * Implement sqlManager as a singleton
     * Only way to get a reference to the manager is through this method
     */
    static sqlManager* getInstance(const string&, int);

    /** Allow checking out of database connections */
    dbHandle* getConnection();

    /** Allow checking in of database connections */
    void removeConnection(dbHandle*);

    /** Flush the current commit queue to the database */
    void flush();

    /** Add a statement to the commit queue */
    void queueCommit(const string&);

  protected:
    /**
     * Disable the default constructor so that instances can only be gotten
     * through getInstance()
     * @see #getInstance
     */
    sqlManager(const string&, int);

    /**
     * Disable the default destructor so that other objects cannot destruct
     * the instance they have a reference to.
     */
    ~sqlManager();

    /** The string storing our DB connection path */
    string dbString;

    /** Our PgDatabase instance */
    dbHandle* SQLDb;

    /** The type used for the commit queue */
    typedef vector< string > commitQueueType;

    /** Allow iteration over the commit queue */
    typedef commitQueueType::iterator CommitQueueItr;

    /** Our commit queue */
    commitQueueType commitQueue;

    /** Max commit queue size before autocommit */
    unsigned long int commitQueueMax;

    /** The current instance of sqlManager */
    static sqlManager* theManager;

    /** A Stats instance for stats collecting */
    Stats* theStats;

    /** A Logger instance for logging */
    logging::Logger* theLogger;
}; // class sqlManager

} // namespace ns

} // namespace gnuworld

#endif
