/**
 * msg_Server.cc
 */

#include	<new>

#include	<cstring>
#include	<cassert>

#include	"server.h"
#include	"events.h"
#include	"Network.h"
#include	"iServer.h"
#include	"ELog.h"
#include	"xparameters.h"

const char msg_Server_cc_rcsId[] = "$Id: msg_Server.cc,v 1.5 2001/05/18 15:27:10 dan_karrels Exp $" ;
const char server_h_rcsId[] = __SERVER_H ;
const char events_h_rcsId[] = __EVENTS_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char iServer_h_rcsId[] = __ISERVER_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char xParameters_h_rcsId[] = __XPARAMETERS_H ;

namespace gnuworld
{

using std::endl ;

/**
 * New server message
 * SERVER irc-r.mediabit.net 1 934191376 946934706 J10 BD] :MCS MEDIABIT Testnet
 * Routing Server, Padova, Italy
 * SERVER Austin-R.TX.US.KrushNet.Org 1 900000000 958147697 J10 1]] :Large Penis
 *  Support Group IRC Server
 *
 * The following command has been changed to token S
 * B SERVER irc.mediabit.net 2 0 945972199 P10 CD] 0 :[193.76.114.11]
 * [193.76.114.11] MCS MEDIABIT Testnet Server, Padov
 *
 * Declaration of our own server:
 * SERVER ripper.ufl.edu 1 933022556 948162945 J10 QD] :[128.227.184.152]
 * University of Florida
 *
 * Remember that the "SERVER" parameter is removed.
 */
int xServer::MSG_Server( xParameters& Param )
{

burstEnd = 0 ;
burstStart = ::time( 0 ) ;

// Check the hopcount
// 1: It's our uplink
if( Param[ 1 ][ 0 ] == '1' )
	{

//	clog	<< "xServer::MSG_Server> Got Uplink: " << Param[ 0 ] << endl ;

	// It's our uplink
	if( Param.size() < 6 )
		{
		elog	<< "xServer::MSG_Server> Invalid number of parameters\n" ;
		return -1 ;
		}

	// Here's the deal:
	// We are just connecting to the network
	// We have just received the first server command,
	// telling us who our uplink server is.
	// We need to add our uplink to network tables.

	// Assume 3 character numerics
	unsigned int uplinkYY = convert2n[ Param[ 5 ][ 0 ] ] ;

	// Check for n2k, 5 character numerics
	if( strlen( Param[ 5 ] ) == 5 )
		{
		// n2k
		uplinkYY = base64toint( Param[ 5 ], 2 ) ;
		}

	// Our uplink has its own numeric as its uplinkIntYY.
	Uplink = new (nothrow) iServer( 
		uplinkYY,
		Param[ 5 ], // yyxxx
		Param[ 0 ], // name
		atoi( Param[ 3 ] ) ) ; // connect time
	assert( Uplink != 0 ) ;

	// Find this server (me)
	iServer* me = Network->findServer( intYY ) ;
	if( NULL == me )
		{
		elog	<< "xServer::MSG_SERVER> Unable to find myself "
			<< " ("
			<< intYY
			<< ")"
			<< endl ;
		::exit( 0 ) ;
		}

	// Now that I know my uplink, I can set its numeric
	// in my own iServer info
	me->setUplinkIntYY( uplinkYY ) ;

	// We now have a pointer to our own uplink
	// Add it to the tables
	// We maintain a local pointer just for speed reasons
	Network->addServer( Uplink ) ;

//	elog << "Added server: " << *Uplink ;

	}

// Not posting message here because this method is only called once
// using tokenized commands - when the xServer connects
return 0 ;
}


} // namespace gnuworld
