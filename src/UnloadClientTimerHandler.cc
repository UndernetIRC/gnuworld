/* UnloadClientTiemrHandler.cc */

#include	<iostream>

#include	"UnloadClientTimerHandler.h"
#include	"server.h"
#include	"ELog.h"

const char UnloadClientTimerHandler_h_rcsId[] = __UNLOADCLIENTTIMERHANDLER_H ;
const char UnloadClientTimerHandler_cc_rcsId[] = "$Id: UnloadClientTimerHandler.cc,v 1.3 2002/04/28 16:11:23 dan_karrels Exp $" ;
const char ELog_h_rcsId[] = __ELOG_H ;

namespace gnuworld
{

int UnloadClientTimerHandler::OnTimer( timerID, void* )
{
elog	<< "UnloadClientTimerHandler::OnTimer("
	<< moduleName
	<< ")"
	<< std::endl ;

theServer->DetachClient( moduleName, reason ) ;

delete this ;
return 0 ;
}

} // namespace gnuworld
