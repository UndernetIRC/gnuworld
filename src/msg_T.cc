/**
 * msg_T.cc
 */

#include	<iostream>

#include	"server.h"
#include	"xparameters.h"
#include	"Network.h"
#include	"ELog.h"
#include	"config.h"
#include	"Channel.h"

const char msg_T_cc_rcsId[] = "$Id: msg_T.cc,v 1.4 2002/04/28 16:11:23 dan_karrels Exp $" ;
const char xParameters_h_rcsId[] = __XPARAMETERS_H ;
const char server_h_rcsId[] = __SERVER_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char Channel_h_rcsId[] = __CHANNEL_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char config_h_rcsId[] = __CONFIG_H ;

namespace gnuworld
{

using std::endl ;

// Channel topics currently are not tracked.
// kAI T #omniplex :-=[ Washington.DC.US.Krushnet.Org / Luxembourg.
// LU.EU.KrushNet.Org
// Admin Channel ]=-
int xServer::MSG_T( xParameters& Param )
{
if( Param.size() < 4 )
	{
	elog	<< "MSG_T> Invalid number of arguments"
		<< endl ;
	return -1 ;
	}

#ifdef TOPIC_TRACK
  Channel* Chan = Network->findChannel( Param[ 1 ] ) ;
  if(!Chan)
	{
	elog	<< "MSG_T> Unable to locate channel: "
		<< Param[ 1 ]
		<< endl;
	return 0;
	}

  Chan->setTopic( Param[ 2 ] ) ;
#endif // TOPIC_TRACK

return 0 ;
}

} // namespace gnuworld
