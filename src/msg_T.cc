/**
 * msg_T.cc
 */

#include	"server.h"
#include	"xparameters.h"

const char msg_T_cc_rcsId[] = "$Id: msg_T.cc,v 1.2 2001/05/18 15:27:10 dan_karrels Exp $" ;
const char xParameters_h_rcsId[] = __XPARAMETERS_H ;
const char server_h_rcsId[] = __SERVER_H ;

namespace gnuworld
{

// Channel topics currently are not tracked.
// kAI T #omniplex :-=[ Washington.DC.US.Krushnet.Org / Luxembourg.
// LU.EU.KrushNet.Org
// Admin Channel ]=-
int xServer::MSG_T( xParameters& )
{
return 0 ;
}


} // namespace gnuworld
