/**
 * LoadClientTimerHandler.h
 */

#ifndef __LOADCLIENTTIMERHANDLER_H
#define __LOADCLIENTTIMERHANDLER_H "$Id: LoadClientTimerHandler.h,v 1.2 2002/04/27 14:54:49 dan_karrels Exp $"

#include	<string>

#include	"ServerTimerHandlers.h"

namespace gnuworld
{

using std::string ;

class xServer ;

/**
 * The purpose of this class is to load xClient's at a particular time,
 * it is used to perform xClient unload/loads while still allowing the
 * event system to give the xClient time to unload before reloading a
 * new instance.
 */
class LoadClientTimerHandler : public ServerTimerHandler
{

protected:

	/// The name of the module to be loaded
	string		moduleName ;

	/// The configuration file name to pass to the client constructor
	string		configFileName ;

public:

	/**
	 * Constructor receives:
	 * - The xServer instance for the system
	 * - The xClient's module name
	 * - The configuration file name for the xClient.
	 */
	LoadClientTimerHandler( xServer* theServer,
		 const string& _moduleName,
		 const string& _configFileName )
	: ServerTimerHandler( theServer, 0),
	  moduleName( _moduleName ),
	  configFileName( _configFileName )
	{}

	/**
	 * Default destructor, nothing to do here atm.
	 */
	virtual ~LoadClientTimerHandler()
	{}

	/**
	 * The method that is called by the server when this handler's
	 * time to perform has arrived.
	 */
	virtual	int	OnTimer( timerID, void* ) ;

} ;

} // namespace gnuworld

#endif // __LOADCLIENTTIMERHANDLER_H
