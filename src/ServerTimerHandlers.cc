/* ServerTimerHandlers.cc */

#include	"ServerTimerHandlers.h"
#include	"server.h"
#include	"ELog.h"

#include	<ctime>

const char ServerTimerHandlers_h_rcsId[] = __SERVERTIMERHANDLERS_H ;
const char ServerTimerHandlers_cc_rcsId[] = "$Id: ServerTimerHandlers.cc,v 1.4 2001/05/17 21:32:57 dan_karrels Exp $" ;
const char server_h_rcsId[] = __SERVER_H ;
const char ELog_h_rcsId[] = __ELOG_H ;

namespace gnuworld
{

int GlineUpdateTimer::OnTimer( timerID, void* )
{

// Remove any expired glines
theServer->updateGlines() ;

// Re-register this timer
theServer->RegisterTimer( ::time( 0 ) + updateInterval, this, 0 ) ;

return 0 ;
}

int PINGTimer::OnTimer( timerID, void* )
{

string writeMe( theServer->getCharYY() ) ;
writeMe += " G " ;
writeMe += ":I am the King, bow before me!\n" ;

theServer->RegisterTimer( ::time( 0 ) + updateInterval, this, 0 ) ;

// Write to the network, even during bursting
return theServer->WriteDuringBurst( writeMe ) ;

}

} // namespace gnuworld
