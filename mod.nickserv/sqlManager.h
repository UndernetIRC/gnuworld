#ifndef _SQLMANAGER_H
#define _SQLMANAGER_H "$Id: sqlManager.h,v 1.6 2002/08/27 20:55:53 jeekay Exp $"

#include <vector>

#include "libpq++.h"

#include "Logger.h"
#include "Stats.h"

namespace gnuworld {

namespace ns {

class sqlManager {
  public:
    /**
     * Implement sqlManager as a singleton
     * Only way to get a reference to the manager is through this method
     */
    static sqlManager* getInstance(const string&, int);
    
    /** Allow checking out of database connections */
    PgDatabase* getConnection();
  
    /** Allow checking in of database connections */
    void removeConnection(PgDatabase*); 
    
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
    PgDatabase* SQLDb;
    
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