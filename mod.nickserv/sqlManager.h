#ifndef _SQLMANAGER_H
#define _SQLMANAGER_H "$Id"

#include <vector>

#include "libpq++.h"

namespace gnuworld {

namespace ns {

class sqlManager {
  public:
    /**
     * Implement sqlManager as a singleton
     * Only way to get a reference to the manager is through this method
     */
    static sqlManager* getInstance(const string&);
    
    /** Allow checking out of database connections */
    PgDatabase* getConnection();
  
    /** Allow checking in of database connections */
    void removeConnection(PgDatabase*); 
    
    /** Add a statement to the commit queue */
    void queueCommit(const string&);
  
  protected:
    /**
     * Disable the default constructor so that instances can only be gotten
     * through getInstance()
     * @see #getInstance
     */
    sqlManager(const string&);
    
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
    typedef commitQueueType::const_iterator constCommitQueueItr;
    
    /** Our commit queue */
    commitQueueType commitQueue;
    
    /** The current instance of sqlManager */
    static sqlManager* theManager;
}; // class sqlManager

} // namespace ns

} // namespace gnuworld

#endif
