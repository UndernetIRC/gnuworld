/**
 * msg_SQ.cc
 */

#include	<string>

#include	<cstring>

#include	"server.h"
#include	"events.h"
#include	"Network.h"
#include	"iServer.h"
#include	"ELog.h"
#include	"xparameters.h"

const char msg_SQ_cc_rcsId[] = "$Id: msg_SQ.cc,v 1.2 2001/03/24 01:31:42 dan_karrels Exp $" ;

namespace gnuworld
{

using std::string ;
using std::endl ;

/**
 * SQUIT message handler.
 * :ripper_ SQ ripper.ufl.edu 0 :squitting the services server
 * :ripper_ SQ maniac.krushnet.org 954445164 :remote server squit
 * 0 SQ Auckland.NZ.KrushNet.Org 957468458 :Ping timeout
 * kAm SQ Luxembourg.LU.EU.KrushNet.Org 957423889 :byyyyyye
 *
 * The following example is gnuworld being squitted:
 * OAO SQ Asheville-R.NC.US.KrushNet.Org 0 :I bid you adoo
 * asheville-r.* was gnuworld's uplink
 * 0 SQ Asheville-R.NC.US.KrushNet.Org 0 :Ping timeout
 * Az SQ Seattle-R.WA.US.KrushNet.Org 0 :Ping timeout
 */
int xServer::MSG_SQ( xParameters& Param )
{

if( Param.size() < 2 )
	{
	elog	<< "xServer::MSG_SQ> Invalid number of parameters"
		<< endl ;
	return -1 ;
	}

iServer* squitServer = 0 ;
if( strchr( Param[ 1 ], '.' ) != NULL )
	{
	// Full server name specified
	squitServer = Network->findServerName( Param[ 1 ] ) ;
	}
else
	{
	// Numeric
	squitServer = Network->findServer( Param[ 1 ] ) ;
	}

if( NULL == squitServer )
	{
	elog	<< "xServer::MSG_SQ> Unable to find server: "
		<< Param[ 1 ]
		<< endl ;
	return -1 ;
	}

if( squitServer->getIntYY() == Uplink->getIntYY() )
	{
	elog	<< "xServer::MSG_SQ> Ive been delinked!!"
		<< endl ;

	// It's my uplink, we have been squit...those bastards!
	OnDisConnect() ;
	}
else
	{

//	elog	<< "xServer::MSG_SQ> " << squitServer->getName()
//		<< " has been squit\n" ;

	// Otherwise, it's just some server.
	Network->OnSplit( squitServer->getIntYY() ) ;

	// Remove this server from the network tables.
	Network->removeServer( squitServer->getIntYY(), true ) ;

	}

string source( Param[ 0 ] ) ;
string reason( Param[ 3 ] ) ;

PostEvent( EVT_NETBREAK,
	static_cast< void* >( squitServer ),
	static_cast< void* >( &source ),
	static_cast< void* >( &reason ) ) ;

// Deallocate
delete squitServer ;

return 0 ;
}

} // namespace gnuworld
