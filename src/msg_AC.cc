/* msg_AC. cc */

#include	"server.h"
#include	"xparameters.h"
#include	"Channel.h"
#include	"Network.h"
#include	"iClient.h"

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
if(!theClient)
	{
		return 0;
	}

theClient->setAccount(Param[2]);

PostEvent( EVT_ACCOUNT, static_cast< void* >( theClient ) ) ;
return 0;
}

} // namespace gnuworld
