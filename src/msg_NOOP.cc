/**
 * msg_NOOP.cc
 */

#include	"server.h"
#include	"xparameters.h"

const char server_h_rcsId[] = __SERVER_H ;
const char xparameters_h_rcsId[] = __XPARAMETERS_H ;
const char msg_NOOP_cc_rcsId[] = "$Id: msg_NOOP.cc,v 1.2 2002/04/28 16:11:23 dan_karrels Exp $" ;

namespace gnuworld
{

int xServer::MSG_NOOP( xParameters& Param )
{
return 0 ;
}

} // namespace gnuworld
