/**
 * msg_T.cc
 */

#include	"server.h"
#include	"xparameters.h"

const char msg_T_cc_rcsId[] = "$Id: msg_T.cc,v 1.1 2001/02/02 18:10:30 dan_karrels Exp $" ;

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
