/**
 * msg_D.cc
 */

#include	<new>
#include	<string>

#include	<cassert>

#include	"server.h"
#include	"iClient.h"
#include	"iServer.h"
#include	"events.h"
#include	"Network.h"

#include	"ELog.h"
#include	"StringTokenizer.h"

const char server_h_rcsId[] = __SERVER_H ;
const char iClient_h_rcsId[] = __ICLIENT_H ;
const char iServer_h_rcsId[] = __ISERVER_H ;
const char events_h_rcsId[] = __EVENTS_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char StringTokenizer_h_rcsId[] = __STRINGTOKENIZER_H ;
const char msg_D_rcsId[] = "$Id: msg_D.cc,v 1.4 2002/04/28 16:11:23 dan_karrels Exp $" ;

namespace gnuworld
{

using std::string ;
using std::endl ;

/**
 * Kill command
 * QAA D BB5 :localhost!_reppir (Im using my super duper clone detecting
 *  skills)
 * G D r[l :NewYork-R.NY.US.Undernet.Org!NewYork-R.NY.US.Undernet.org ...
 * The source of the kill could be a server or a client.
 */
int xServer::MSG_D( xParameters& Param )
{

if( Param.size() < 3 )
	{
	elog	<< "xServer::MSG_D> Invalid number of parameters"
		<< endl ;
	return -1 ;
	}

// See if the client being killed is one of my own.
xClient* myClient = Network->findLocalClient( Param[ 1 ] ) ;

// Is the user being killed on this server?
if( NULL != myClient )
	{
	// doh, yes it is :(
	myClient->OnKill() ;

	// Don't detach the client until it requests so.
	// TODO: Work on this system.

	// Note that the client is still attached to the
	// server.
	return 0 ;
	}

// Otherwise, it's a non-local client.
iClient* source = 0 ;
iServer* serverSource = 0 ;

if( strchr( Param[ 0 ], '.' ) != NULL )
	{
	// Server, by name
	serverSource = Network->findServerName( Param[ 0 ] ) ;
	}
else if( strlen( Param[ 0 ] ) >= 3 )
	{
	// Client, by numeric
	source = Network->findClient( Param[ 0 ] ) ;
	}
else
	{
	// Server, by numeric
	serverSource = Network->findServer( Param[ 0 ] ) ;
	}

if( (NULL == serverSource) && (NULL == source) )
	{
	elog	<< "xServer::MSG_D> Unable to find source: "
		<< Param[ 0 ]
		<< endl ;
	return -1 ;
	}

// Find and remove the client that was just killed.
// xNetwork::removeClient will remove user<->channel associations
iClient* target = Network->removeClient( Param[ 1 ] ) ;

// Make sure we have valid pointers to both source
// and target.
if( NULL == target )
	{
	elog	<< "xServer::MSG_D> Unable to find target client: "
		<< Param[ 1 ]
		<< endl ;
	return -1 ;
	}

// Notify all listeners of the EVT_KILL event.
string reason( Param[ 2 ] ) ;

if( source != NULL )
	{
	PostEvent( EVT_KILL,
		static_cast< void* >( source ),
		static_cast< void* >( target ),
		static_cast< void* >( &reason ) ) ;
	}
else
	{
	PostEvent( EVT_KILL,
		static_cast< void* >( serverSource ),
		static_cast< void* >( target ),
		static_cast< void* >( &reason ) ) ;
	}

// Deallocate the memory associated with this iClient.
delete target ;

return 0 ;
}

} // namespace gnuworld
