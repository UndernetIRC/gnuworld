/**
 * msg_EB.cc
 */

#include	<sys/types.h>
#include	<sys/time.h>

#include	"server.h"
#include	"iServer.h"
#include	"events.h"
#include	"Network.h"
#include	"ELog.h"
#include	"xparameters.h"

const char msg_EB_cc_rcsId[] = "$Id: msg_EB.cc,v 1.2 2001/02/05 18:58:12 dan_karrels Exp $" ;

using std::endl ;

namespace gnuworld
{

// Q EB
// Q: Remote server numeric
// EB: End Of Burst
int xServer::MSG_EB( xParameters& params )
{

if( !strcmp( params[ 0 ], Uplink->getCharYY() ) )
	{
	// It's my uplink
	burstEnd = ::time( 0 ) ;

	// Signal that all Write()'s should write to the
	// normal output buffer
	useBurstBuffer = false ;

	// Burst our clients
	BurstClients() ;

	// Burst our channels
	BurstChannels() ;

	// We are no longer bursting
	bursting = false ;

	// For some silly reason, EB must come before EA
	// *shrug*

	// Send our EB
	Write( "%s EB\n", charYY ) ;

	// Acknowledge their end of burst
	Write( "%s EA\n", charYY ) ;

	// Is the burstOutputBuffer empty?
	if( !burstOutputBuffer.empty() )
		{
		// It has data, concatenate this data
		// onto the normal outputBuffer
		outputBuffer += burstOutputBuffer ;
		burstOutputBuffer.clear() ;

		elog	<< "xServer::MSG_EB> Adding "
			<< burstOutputBuffer.size()
			<< " bytes from burstOutputBuffer to "
			<< "outputBuffer"
			<< endl ;

		}

	elog	<< "*** Completed net burst"
		<< endl ;
	}

if( !bursting )
	{
	iServer* theServer = Network->findServer( params[ 0 ] ) ;
	if( NULL == theServer )
		{
		elog	<< "xServer::MSG_EB> Unable to find server: "
			<< params[ 0 ]
			<< endl ;
		return -1 ;
		}

	PostEvent( EVT_BURST_CMPLT, static_cast< void* >( theServer ) ) ;
	}

return 0 ;
}

} // namespace gnuworld
