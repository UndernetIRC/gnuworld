/* msg_CM. cc */

#include	"server.h"
#include	"xparameters.h"

namespace gnuworld
{

/**
 * CLEARMODE message handler.
 * ZZAAA CM #channel obv
 * The above message would remove all ops, bans, and voice modes
 *  from channel #channel.
 */
int xServer::MSG_CM( xParameters& param )
{
return 0 ;
}

} // namespace gnuworld
