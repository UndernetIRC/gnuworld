/* ServerTimerHandlers.cc */

#include	"ServerTimerHandlers.h"
#include	"server.h"
#include	"ELog.h"

#include	<ctime>

const char ServerTimerHandlers_h_rcsId[] = __SERVERTIMERHANDLERS_H ;
const char ServerTimerHandlers_cc_rcsId[] = "$Id: ServerTimerHandlers.cc,v 1.3 2001/03/31 01:26:10 dan_karrels Exp $" ;
const char server_h_rcsId[] = __SERVER_H ;
const char ELog_h_rcsId[] = __ELOG_H ;

namespace gnuworld
{

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

int PINGTimer::OnTimer( timerID, void* data )
{
xServer* theServer = static_cast< xServer* >( data ) ;
if( NULL == theServer )
	{
	elog	<< "PINGTimer::OnTimer> NULL Server pointer"
		<< endl ;
	return -1 ;
	}

string writeMe( theServer->getCharYY() ) ;
writeMe += " G " ;
writeMe += ":I am the King, bow before me!\n" ;

theServer->RegisterTimer( ::time( 0 ) + 60,
	this,
	data ) ;

// Write to the network, even during bursting
return theServer->WriteDuringBurst( writeMe ) ;

}

} // namespace gnuworld
