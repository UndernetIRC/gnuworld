#ifndef _NICKSERV_H
#define _NICKSERV_H "$Id: nickserv.h,v 1.8 2002/08/27 20:55:53 jeekay Exp $"

#include "client.h"
#include "EConfig.h"
#include "server.h"

#include "Logger.h"
#include "logTarget.h"
#include "nickservCommands.h"
#include "sqlManager.h"
#include "sqlUser.h"
#include "Stats.h"

namespace gnuworld {

namespace ns {

class nickserv : public xClient, public logging::logTarget {
  public:

    /***********************************************************
     ** O V E R R I D E N   L O G T A R G E T   M E T H O D S **
     ***********************************************************/

    /** Receive a message for logging */
    virtual void log(const logging::events::eventType&, const string&);

    
    /*******************************************************
     ** O V E R R I D E N   X C L I E N T   M E T H O D S **
     *******************************************************/
    
    /** Constructor receives a configuration file name */
    nickserv( const string& ) ;
    
    /** Destructor to remove anything interesting we've done */
    virtual ~nickserv() ;
    
    /** This method is called after server connection */
    virtual int BurstChannels() ;
    
    /** This is called when we have attached to the xServer */
    virtual void ImplementServer( xServer* ) ;

    /** This is called when a network event happens */
    virtual int OnEvent( const eventType&, void* , void*, void*, void* );

    /** This method is called when the bot gets a PRIVMSG */
    virtual int OnPrivateMessage( iClient*, const string&, bool secure = false ) ;
    
    /** This method is called when a timer expires */
    virtual int OnTimer(gnuworld::xServer::timerID, void*);
    
  
    /*********************************
     ** N I C K S E R V   T Y P E S **
     *********************************/
    
    typedef map< string, Command*, noCaseCompare > commandMapType;
    typedef commandMapType::value_type commandPairType;

    typedef vector< iClient* > QueueType;
    
    typedef map< string, sqlUser*, noCaseCompare > sqlUserHashType;

    typedef vector< iClient* > logUsersType;
    
  
    /*************************************
     ** N I C K S E R V   M E T H O D S **
     *************************************/

    /** Register a command */
    virtual bool RegisterCommand(Command*);

    /** Load all users into the user cache */
    void precacheUsers();
    
    /** Insert a nick/sqlUser* pair into the cache */
    void addUserToCache(string, sqlUser*);
    
    /** Add an iClient to the processing queue */
    int addToQueue(iClient*);
    
    /** Remove an iClient from the processing queue */
    int removeFromQueue(iClient*);
    
    /** Process the queue */
    void processQueue();
    
    
    /*********************************
     ** U S E R   R E S O U R C E S **
     *********************************/
    
    /** Check if a given iClient matches a given sqlUser */
    bool isAccountMatch(iClient*, sqlUser*);
    
    /** Returns a sqlUser if the user is registered */
    sqlUser* isAuthed(iClient*); 
    
    /** Returns a sqlUser for a given user name */
    sqlUser* isRegistered(string);

    
    /*******************************
     ** M I S C E L L A N E O U S **
     *******************************/

    /** Holds a reference to our Stats collector */
    Stats::Stats* theStats;

    /** Holds a reference to our Logger instance */
    logging::Logger* theLogger;
   
    
    /*****************************************
     ** N I C K S E R V   V A R I A B L E S **
     *****************************************/

    /** Our sqlManager instance for DB communication */
    sqlManager* theManager;
    
  protected:
    /*********************************************
     ** I N T E R N A L   M A I N T E N A N C E **
     *********************************************/
    
    commandMapType commandMap;


    /*************************************
     ** C O N F I G   V A R I A B L E S **
     *************************************/
    
    /** The frequency with which we process the queue */
    int checkFreq;

    /** Store the console channel name */
    string consoleChannel;

    /** Store the config file pointer */
    EConfig* nickservConfig;
    
    /** How long to wait after linking before processing */
    int startDelay;
    
    /** How frequently to commit to the database */
    int commitFreq;
    

    /*****************
     ** Q U E U E S **
     *****************/
    
    /** List of iClient's that currently need verifying */
    QueueType warnQueue;
    
    /** The cached list of registered users */
    sqlUserHashType sqlUserCache;

    /** The list of log users */
    logUsersType logUsers;
    
    
    /***********************
     ** T I M E R   I D S **
     ***********************/
     
    /** TimerID for processing the queue */
    gnuworld::xServer::timerID processQueue_timerID;

}; // class nickserv

} // namespace ns

} // namespace gnuworld

#endif
