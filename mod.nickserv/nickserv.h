#ifndef _NICKSERV_H
#define _NICKSERV_H "$Id: nickserv.h,v 1.1 2002/08/10 13:55:19 jeekay Exp $"

#include "EConfig.h"

#include "sqlManager.h"

namespace gnuworld {

namespace ns {

class nickserv : public xClient {
  public:
    
    /**
     * Constructor receives a configuration file name
     */
    nickserv( const string& ) ;
    
    /**
     * Destructor to remove anything interesting we've done
     */
    virtual ~nickserv() ;
    
    /**
     * This method is called when the bot gets a PRIVMSG
     */
    virtual int OnPrivateMessage( iClient*, const string&, bool secure = false ) ;
    
    /**
     * This method is called after server connection.
     * Here we tell the xServer what channels we want to burst in to
     */
    virtual int BurstChannels() ;
    
    /** This is called when we have attached to the xServer */
    virtual void ImplementServer( xServer* ) ;

    /** Load all users into the user cache */
    void precacheUsers();

    /** Our sqlManager instance for DB communication */
    sqlManager* theManager;

  protected:
    /** Store the config file pointer */
    EConfig* nickservConfig;
    
    /** Store the console channel name */
    string consoleChannel;
}; // class nickserv

} // namespace ns

} // namespace gnuworld

#endif
