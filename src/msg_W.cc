/**
 * msg_W.cc
 */

#include	"server.h"
#include	"xparameters.h"
#include	"ELog.h"
#include	"Network.h"
#include	"iClient.h"
#include	"client.h"

const char msg_W_cc_rcsId[] = "$Id: msg_W.cc,v 1.1 2001/02/04 16:43:28 dan_karrels Exp $" ;

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

// Only deliver the command to the services clients if the source
// client is an oper
if( !sourceClient->isOper() )
	{
	// Exit silently
	return 0 ;
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
