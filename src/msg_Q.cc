/**
 * msg_Q.cc
 */

#include	"server.h"
#include	"events.h"
#include	"iClient.h"
#include	"Network.h"
#include	"ELog.h"

const char msg_Q_cc_rcsId[] = "$Id: msg_Q.cc,v 1.1 2001/02/02 18:10:30 dan_karrels Exp $" ;

using std::endl ;

namespace gnuworld
{

/**
 * A client has quit.
 * QAE Q :Signed off
 */
int xServer::MSG_Q( xParameters& Param )
{

// xNetwork::removeClient will remove user<->channel associations
iClient* theClient = Network->removeClient( Param[ 0 ] ) ;
if( NULL == theClient )
	{
	elog	<< "xServer::MSG_Q> Unable to find client: "
		<< Param[ 0 ] << endl ;
	return -1 ;
	}

PostEvent( EVT_QUIT, static_cast< void* >( theClient ) ) ;

// xNetwork::removeClient() will remove channel->user associations.
delete theClient ;

return 0 ;
}

} // namespace gnuworld
