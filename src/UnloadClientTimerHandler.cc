/* UnloadClientTiemrHandler.cc */

#include	"UnloadClientTimerHandler.h"
#include	"server.h"
#include	"ELog.h"

const char UnloadClientTimerHandler_h_rcsId[] = __UNLOADCLIENTTIMERHANDLER_H ;
const char UnloadClientTimerHandler_cc_rcsId[] = "$Id: UnloadClientTimerHandler.cc,v 1.1 2001/06/14 22:14:13 dan_karrels Exp $" ;
const char ELog_h_rcsId[] = __ELOG_H ;

namespace gnuworld
{

int UnloadClientTimerHandler::OnTimer( timerID, void* )
{
elog	<< "UnloadClientTimerHandler::OnTimer("
	<< moduleName
	<< ")"
	<< endl ;

theServer->DetachClient( moduleName ) ;

delete this ;
return 0 ;
}

} // namespace gnuworld
