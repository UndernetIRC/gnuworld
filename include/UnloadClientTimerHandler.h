/**
 * UnloadClientTimerHandler.h
 */

#ifndef __UNLOADCLIENTTIMERHANDLER_H
#define __UNLOADCLIENTTIMERHANDLER_H "$Id: UnloadClientTimerHandler.h,v 1.3 2002/04/27 14:54:49 dan_karrels Exp $"

#include	<string>

#include	"ServerTimerHandlers.h"

namespace gnuworld
{

using std::string ;

class xServer ;

/**
 * This class is responsible for unloading an xClient at a particular time.
 * This delayed unload allows the xClient to finish cleanup work as
 * requested by the server event system.
 */
class UnloadClientTimerHandler : public ServerTimerHandler
{

protected:

	/// The xClient module name
	string		moduleName ;

	/// The reason for the unload, will be delivered to the xClient
	string		reason ;

public:

	/**
	 * The constructor receives the follow arguments:
	 * - A pointer to the global xServer instance
	 * - The xClient module name
	 * - The reason for unloading the client.
	 */
	UnloadClientTimerHandler( xServer* theServer,
		const string& _moduleName,
		const string& _reason )
	: ServerTimerHandler( theServer, 0),
	  moduleName( _moduleName ),
	  reason( _reason )
	{}

	/**
	 * Destructor, not much to talk about here.
	 */
	virtual ~UnloadClientTimerHandler()
	{}

	/**
	 * The method that is called when it's time to unload the client.
	 */
	virtual	int	OnTimer( timerID, void* ) ;

} ;

} // namespace gnuworld

#endif // __UNLOADCLIENTTIMERHANDLER_H
