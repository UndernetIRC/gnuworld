/**
 * msg_W.cc
 */

#include	"server.h"
#include	"xparameters.h"
#include	"ELog.h"
#include	"Network.h"
#include	"iClient.h"
#include	"client.h"

const char msg_W_cc_rcsId[] = "$Id: msg_W.cc,v 1.3 2001/05/18 15:27:10 dan_karrels Exp $" ;
const char server_h_rcsId[] = __SERVER_H ;
const char xParameters_h_rcsId[] = __XPARAMETERS_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char iClient_h_rcsId[] = __ICLIENT_H ;
const char client_h_rcsId[] = __CLIENT_H ;

namespace gnuworld
{

// ABAG7 W Az :Gte-
int xServer::MSG_W( xParameters& Param )
{
if( Param.size() != 3 )
	{
	elog	<< "xServer::MSG_W> Invalid number of parameters"
		<< endl ;
	return -1 ;
	}

iClient* sourceClient = Network->findClient( Param[ 0 ] ) ;
if( NULL == sourceClient )
	{
	elog	<< "xServer::MSG_W> Unable to find source client: "
		<< Param[ 0 ]
		<< endl ;
	return -1 ;
	}

iClient* targetClient = Network->findNick( Param[ 2 ] ) ;
if( NULL == targetClient )
	{
	elog	<< "xServer::MSG_W> Unable to find target client: "
		<< Param[ 2 ]
		<< endl ;
	return -1 ;
	}

// WHOIS must be delivered to all xclients
xNetwork::localClientIterator ptr = Network->localClient_begin() ;
for( ; ptr != Network->localClient_end() ; ++ptr )
	{
	(*ptr)->OnWhois( sourceClient, targetClient ) ;
	}

return 0 ;
}

} // namespace gnuworld
