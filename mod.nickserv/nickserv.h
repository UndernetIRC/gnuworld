#ifndef _NICKSERV_H
#define _NICKSERV_H "$Id: nickserv.h,v 1.2 2002/08/10 19:54:51 jeekay Exp $"

#include "EConfig.h"

#include "sqlManager.h"
#include "sqlUser.h"

namespace gnuworld {

namespace ns {

class nickserv : public xClient {
  public:
    
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
    virtual int OnTimer(xServer::timerID, void*);
    
  
    /*********************************
     ** N I C K S E R V   T Y P E S **
     *********************************/
    
    typedef map< string, sqlUser*, noCaseCompare > sqlUserHashType;
    
    typedef vector< iClient* > QueueType;
    
  
    /*************************************
     ** N I C K S E R V   M E T H O D S **
     *************************************/

    /** Load all users into the user cache */
    void precacheUsers();
    
    /** Add an iClient to the processing queue */
    int addToQueue(iClient*);
    
    /** Remove an iClient from the processing queue */
    int removeFromQueue(iClient*);
    
    /** Process the queue */
    void processQueue();

    
    /*****************************************
     ** N I C K S E R V   V A R I A B L E S **
     *****************************************/

    /** Our sqlManager instance for DB communication */
    sqlManager* theManager;

  protected:
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
    
    
    /*****************
     ** Q U E U E S **
     *****************/
    
    /** List of iClient's that currently need verifying */
    QueueType warnQueue;
    
    /** The cached list of registered users */
    sqlUserHashType sqlUserCache;
    
    
    /***********************
     ** T I M E R   I D S **
     ***********************/
     
     /** TimerID for processing the queue */
     xServer::timerID processQueue_timerID;
     
     
     /*************************
      ** S T A T I S T I C S **
      *************************/
}; // class nickserv

} // namespace ns

} // namespace gnuworld

#endif
