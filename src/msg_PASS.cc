/**
 * msg_PASS.cc
 */

#include	"server.h"
#include	"xparameters.h"

const char msg_PASS_cc_rcsId[] = "$Id: msg_PASS.cc,v 1.3 2001/05/18 15:27:10 dan_karrels Exp $" ;
const char server_h_rcsId[] = __SERVER_H ;
const char xParameters_h_rcsId[] = __XPARAMETERS_H ;

namespace gnuworld
{

int xServer::MSG_PASS( xParameters& )
{
bursting = true ;
useBurstBuffer = true ;
return 0 ;
}


} // namespace gnuworld
