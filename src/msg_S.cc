/**
 * msg_S.cc
 */

#include	<new>
#include	<string>

#include	<cassert>

#include	"server.h"
#include	"events.h"
#include	"Network.h"
#include	"iServer.h"
#include	"ELog.h"
#include	"xparameters.h"

const char msg_S_cc_rcsId[] = "$Id: msg_S.cc,v 1.3 2001/05/18 15:27:10 dan_karrels Exp $" ;
const char server_h_rcsId[] = __SERVER_H ;
const char events_h_rcsId[] = __EVENTS_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char iServer_h_rcsId[] = __ISERVER_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char xParameters_h_rcsId[] = __XPARAMETERS_H ;

namespace gnuworld
{

using std::string ;
using std::endl ;

/**
 * New server joined the network.
 * Q S irc.dynmc.net 2 0 948159347 P10 BD] 0 :[209.0.37.10]
 * [209.0.37.10] Dynamic Networking Solutions
 *
 * B S EUWorld1.test.net 3 0 947284938 P10 OD] 0 :[128.227.184.152]
 * EUWorld Undernet Service
 * B: Uplink Server numeric
 * S: SERVER message
 * EUWorld1.test.net: server name
 * 3: hopcount
 * 0: creation time
 * 947284938: link time
 * P10: Protocol
 * B: Server numeric
 * D]: Last used nick number for clients
 * 0: Unused
 * EUWorld Undernet Server: description
 * As always, the second token, the command, is not
 * included in the xParameters passed here.
 */
int xServer::MSG_S( xParameters& params )
{

// We need at least 9 tokens
if( params.size() < 9 )
	{
	elog	<< "xServer::MSG_S> Not enough parameters"
		<< endl ;
	return -1 ;
	}

int uplinkIntYY = base64toint( params[ 0 ] ) ;
iServer* uplinkServer = Network->findServer( uplinkIntYY ) ;

if( NULL == uplinkServer )
	{
	elog	<< "xServer::MSG_S> Unable to find uplink server"
		<< endl ;
	return -1 ;
	}

const string serverName( params[ 1 ] ) ;
// Don't care about hop count
// Don't care about start time
time_t connectTime = static_cast< time_t >( atoi( params[ 4 ] ) ) ;
// Don't care about version

int serverIntYY = 0 ;
if( 5 == strlen( params[ 6 ] ) )
	{
	// n2k
	serverIntYY = base64toint( params[ 6 ], 2 ) ;
	}
else
	{
	// yxx
	serverIntYY = base64toint( params[ 6 ], 1 ) ;
	}

// Does the new server's numeric already exist?
if( NULL != Network->findServer( serverIntYY ) )
	{
	elog	<< "xServer::MSG_S> Server numeric collision, numeric: "
		<< params[ 6 ]
		<< ", old name: "
		<< Network->findServer( serverIntYY )->getName()
		<< ", new name: "
		<< serverName
		<< endl ;
	delete Network->removeServer( serverIntYY ) ;
	}

// Dun really care about the server description

iServer* newServer = new (nothrow) iServer( uplinkIntYY,
		params[ 6 ], // yxx
		serverName,
		connectTime ) ;
assert( newServer != 0 ) ;

Network->addServer( newServer ) ;
//elog << "Added server: " << *newServer ;

// TODO: Post message
PostEvent( EVT_NETJOIN,
	static_cast< void* >( newServer ),
	static_cast< void* >( uplinkServer ) ) ;

return 0 ;

}


} // namespace gnuworld
