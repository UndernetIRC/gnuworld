/**
 * msg_PASS.cc
 */

#include	"server.h"
#include	"xparameters.h"

const char msg_PASS_cc_rcsId[] = "$Id: msg_PASS.cc,v 1.2 2001/02/05 18:58:12 dan_karrels Exp $" ;

namespace gnuworld
{

int xServer::MSG_PASS( xParameters& )
{
bursting = true ;
useBurstBuffer = true ;
return 0 ;
}


} // namespace gnuworld
