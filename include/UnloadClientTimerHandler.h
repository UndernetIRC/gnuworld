/**
 * UnloadClientTimerHandler.h
 */

#ifndef __UNLOADCLIENTTIMERHANDLER_H
#define __UNLOADCLIENTTIMERHANDLER_H "$Id: UnloadClientTimerHandler.h,v 1.1 2001/06/14 22:14:12 dan_karrels Exp $"

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

public:
	UnloadClientTimerHandler( xServer* theServer,
		 const string& _moduleName )
	: ServerTimerHandler( theServer, 0),
	  moduleName( _moduleName )
	{}

	virtual ~UnloadClientTimerHandler()
	{}

	virtual	int	OnTimer( timerID, void* ) ;

} ;

} // namespace gnuworld

#endif // __UNLOADCLIENTTIMERHANDLER_H
