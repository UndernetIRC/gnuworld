/* msg_AC.cc */

#include	"server.h"
#include	"xparameters.h"
#include	"Channel.h"
#include	"Network.h"
#include	"iClient.h"

const char server_h_rcsId[] = __SERVER_H ;
const char xparameters_h_rcsId[] = __XPARAMETERS_H ;
const char Channel_h_rcsId[] = __CHANNEL_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char msg_AC_cc_rcsId[] = "$Id: msg_AC.cc,v 1.2 2002/04/28 16:11:23 dan_karrels Exp $" ;

namespace gnuworld
{

/**
 * ACCOUNT message handler.
 * SOURCE AC TARGET ACCOUNT
 * Eg:
 * AXAAA AC BQrTd Gte
 */
int xServer::MSG_AC( xParameters& Param )
{
/*
 * First, update this users information.
 */

iClient* theClient = Network->findClient(Param[1]);
if( !theClient )
	{
	return 0;
	}

theClient->setAccount( Param[ 2 ] ) ;

PostEvent( EVT_ACCOUNT, static_cast< void* >( theClient ) ) ;
return 0;
}

} // namespace gnuworld
