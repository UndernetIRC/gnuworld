/* ServerTimerHandlers.cc */

#include	"ServerTimerHandlers.h"
#include	"server.h"
#include	"ELog.h"

#include	<ctime>

namespace gnuworld
{

const char ServerTimerHandlers_h_rcsId[] = __SERVERTIMERHANDLERS_H ;
const char ServerTimerHandlers_cc_rcsId[] = "$Id: ServerTimerHandlers.cc,v 1.1 2001/01/28 19:27:35 dan_karrels Exp $" ;

int GlineUpdateTimer::OnTimer( timerID, void* data )
{
xServer* theServer = static_cast< xServer* >( data ) ;
if( NULL == theServer )
	{
	elog	<< "GlineUpdateTimer::OnTimer> NULL Server pointer"
		<< endl ;
	return -1 ;
	}

theServer->updateGlines() ;

// TODO: Move the update interval to the conf file
theServer->RegisterTimer( ::time( 0 ) + 1, // every second
	this,
	data ) ;

return 0 ;
}

} // namespace gnuworld
