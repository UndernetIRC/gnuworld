/**
 * msg_T.cc
 */

#include	"server.h"
#include	"xparameters.h"
#include	"Network.h"

const char msg_T_cc_rcsId[] = "$Id: msg_T.cc,v 1.3 2002/02/24 21:36:41 mrbean_ Exp $" ;
const char xParameters_h_rcsId[] = __XPARAMETERS_H ;
const char server_h_rcsId[] = __SERVER_H ;

namespace gnuworld
{

// Channel topics currently are not tracked.
// kAI T #omniplex :-=[ Washington.DC.US.Krushnet.Org / Luxembourg.
// LU.EU.KrushNet.Org
// Admin Channel ]=-
int xServer::MSG_T( xParameters& Param)
{
#ifdef TOPIC_TRACK
Channel* Chan = Network->findChannel(Param[1]);
if(!Chan)
	{
	elog << "Cant find channel for setting topic - " << Param[1] << endl;
	return 0;
	}
//char *Topic = Param[3];
//Topic++;
Chan->setTopic(Param[2]);
	
#endif

return 0 ;
}


} // namespace gnuworld
