/*
 * nickserv.cc
 */

#include "libpq++.h"

#include "client.h"
#include "server.h"

#include "nickserv.h"

const char NickServ_cc_rcsId[] = "$Id: nickserv.cc,v 1.1 2002/08/10 13:55:19 jeekay Exp $";

namespace gnuworld
{

namespace ns
{
/*
 *  Exported function used by moduleLoader to gain an
 *  instance of this module.
 */

extern "C"
{
  xClient* _gnuwinit(const string& args)
  { 
    return new nickserv( args );
  }

} 
 
/**
 * This constructor calls the base class constructor.  The xClient
 * constructor will open the configuration file given and retrieve
 * basic client info (nick/user/host/etc).
 * Any additional processing must be done here.
 */
nickserv::nickserv( const string& configFileName )
 : xClient( configFileName )
{
/* Load the config file */
nickservConfig = new (std::nothrow) EConfig(configFileName);
assert(nickservConfig != 0);

/* Load in the values we need from the config file */
consoleChannel = nickservConfig->Require("consoleChannel")->second;

string dbHost = nickservConfig->Require("dbHost")->second;
string dbPort = nickservConfig->Require("dbPort")->second;
string dbDb = nickservConfig->Require("dbDb")->second;
string dbUser = nickservConfig->Require("dbUser")->second;
string dbPass = nickservConfig->Require("dbPass")->second;

string dbString = "host=" + dbHost + " port=" + dbPort + " dbname=" + dbDb
  + " user="+dbUser + " password=" + dbPass;

theManager = sqlManager::getInstance(dbString);
}

nickserv::~nickserv()
{
delete nickservConfig;
}

void nickserv::ImplementServer( xServer* theServer )
{
theServer->RegisterEvent(EVT_KILL, this);
theServer->RegisterEvent(EVT_QUIT, this);
theServer->RegisterEvent(EVT_NICK, this);
theServer->RegisterEvent(EVT_ACCOUNT, this);

xClient::ImplementServer( theServer );
}

int nickserv::OnPrivateMessage( iClient* theClient,
	const string& message, bool )
{
Notice( theClient, "Howdy :)" ) ;
return 0 ;
}

// Burst any channels.
int nickserv::BurstChannels()
{
MyUplink->JoinChannel(this, consoleChannel, nickservConfig->Require("consoleChannelModes")->second);

return xClient::BurstChannels() ;
}

/**
 * This function empties the current user cache and reloads all information from
 * the database. It is only called at startup, as the cache is authoritative.
 */
void nickserv::precacheUsers()
{
  /* Get a connection instance to our backend */
  PgDatabase* cacheCon = theManager->getConnection();
  
  /* Dispose of our connection instance */
  theManager->removeConnection(cacheCon);
}

} // namespace ns

} // namespace gnuworld
