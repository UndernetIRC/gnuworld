/**
 * LoadClientTimerHandler.h
 */

#ifndef __LOADCLIENTTIMERHANDLER_H
#define __LOADCLIENTTIMERHANDLER_H "$Id: LoadClientTimerHandler.h,v 1.1 2001/06/14 22:14:12 dan_karrels Exp $"

#include	<string>

#include	"ServerTimerHandlers.h"

namespace gnuworld
{

using std::string ;

class xServer ;

class LoadClientTimerHandler : public ServerTimerHandler
{

protected:
	string		moduleName ;
	string		configFileName ;

public:
	LoadClientTimerHandler( xServer* theServer,
		 const string& _moduleName,
		 const string& _configFileName )
	: ServerTimerHandler( theServer, 0),
	  moduleName( _moduleName ),
	  configFileName( _configFileName )
	{}

	virtual ~LoadClientTimerHandler()
	{}

	virtual	int	OnTimer( timerID, void* ) ;

} ;

} // namespace gnuworld

#endif // __LOADCLIENTTIMERHANDLER_H
