/**
 * msg_EA.cc
 */

#include	<stack>

#include	<cstring>

#include	"server.h"
#include	"Network.h"
#include	"events.h"
#include	"ELog.h"
#include	"iServer.h"

const char server_h_rcsId[] = __SERVER_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char events_h_rcsId[] = __EVENTS_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char iServer_h_rcsId[] = __ISERVER_H ;
const char msg_EA_cc_rcsId[] = "$Id: msg_EA.cc,v 1.3 2002/04/28 16:11:23 dan_karrels Exp $" ;

using std::string ;
using std::endl ;
using std::stack ;

namespace gnuworld
{

// Q EA
// Q: Remote server numeric
// EA: End Of Burst Acknowledge
// Our uplink server has acknowledged our EB
int xServer::MSG_EA( xParameters& Param )
{
if( !strcmp( Param[ 0 ], Uplink->getCharYY() ) )
	{
	// My uplink! :)
	// Reset EOB just to be sure
	bursting = false ; // ACKNOWLEDGE! :)
	}

if( !bursting )
	{
	iServer* theServer = Network->findServer( Param[ 0 ] ) ;
	if( NULL == theServer )
		{
		elog	<< "xServer::MSG_EA> Unable to find server: "
			<< Param[ 0 ] << endl ;
		return -1 ;
		}

	PostEvent( EVT_BURST_ACK, static_cast< void* >( theServer ) );
	}
return( 0 ) ;

}

} // namespace gnuworld
