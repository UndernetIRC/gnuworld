/**
 * msg_Q.cc
 */

#include	"server.h"
#include	"events.h"
#include	"iClient.h"
#include	"Network.h"
#include	"ELog.h"

const char msg_Q_cc_rcsId[] = "$Id: msg_Q.cc,v 1.3 2001/03/24 01:31:42 dan_karrels Exp $" ;

namespace gnuworld
{

using std::endl ;

/**
 * A client has quit.
 * QAE Q :Signed off
 */
int xServer::MSG_Q( xParameters& Param )
{

if( Param.size() < 1 )
	{
	elog	<< "xServer::MSG_Q> Invalid number of parameters"
		<< endl ;
	return -1 ;
	}

// xNetwork::removeClient will remove user<->channel associations
iClient* theClient = Network->removeClient( Param[ 0 ] ) ;
if( NULL == theClient )
	{
	elog	<< "xServer::MSG_Q> Unable to find client: "
		<< Param[ 0 ]
		<< endl ;
	return -1 ;
	}

PostEvent( EVT_QUIT, static_cast< void* >( theClient ) ) ;

// xNetwork::removeClient() will remove channel->user associations.
delete theClient ;

return 0 ;
}

} // namespace gnuworld
