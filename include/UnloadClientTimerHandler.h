/**
 * UnloadClientTimerHandler.h
 */

#ifndef __UNLOADCLIENTTIMERHANDLER_H
#define __UNLOADCLIENTTIMERHANDLER_H "$Id: UnloadClientTimerHandler.h,v 1.2 2001/08/18 14:49:49 dan_karrels Exp $"

#include	<string>

#include	"ServerTimerHandlers.h"

namespace gnuworld
{

using std::string ;

class xServer ;

class UnloadClientTimerHandler : public ServerTimerHandler
{

protected:
	string		moduleName ;
	string		reason ;

public:
	UnloadClientTimerHandler( xServer* theServer,
		const string& _moduleName,
		const string& _reason )
	: ServerTimerHandler( theServer, 0),
	  moduleName( _moduleName ),
	  reason( _reason )
	{}

	virtual ~UnloadClientTimerHandler()
	{}

	virtual	int	OnTimer( timerID, void* ) ;

} ;

} // namespace gnuworld

#endif // __UNLOADCLIENTTIMERHANDLER_H
