/**
 * ConnectionHandler.h
 * Author: Daniel Karrels (dan@karrels.com)
 * $Id: ConnectionHandler.h,v 1.1 2002/05/27 17:22:02 dan_karrels Exp $
 */

#ifndef __CONNECTIONHANDLER_H
#define __CONNECTIONHANDLER_H "$Id: ConnectionHandler.h,v 1.1 2002/05/27 17:22:02 dan_karrels Exp $"

#include	<sys/types.h>

#include	<string>
#include	<list>
#include	<map>

#include	<ctime>

#include	"Connection.h"

using std::string ;
using std::list ;
using std::multimap ;

class ConnectionManager ;

/**
 * The purpose of this class is to provide a concrete base class
 * for interfacing to a ConnectionManager instance.  This class
 * is designed to receive events on Connections for which the
 * ConnectionHandler is responsible.  This class is intended to
 * be subclassed, and the methods overridden by any class which
 * chooses to use a ConnectionManager instance.
 */
class ConnectionHandler
{

public:

	/**
	 * Create a ConnectionHandler instance with default
	 * arguments.
	 */
	ConnectionHandler() ;

	/**
	 * Destroy this ConnectionHandler instance.
	 * This does *not* destroy any associated Connections
	 * which may be in any associated ConnectionManager
	 * objects.
	 */
	virtual ~ConnectionHandler() ;

	/**
	 * This method is called when a host connection succeeds.
	 * The method is in this class hierarchy for now, but may
	 * move to a separate class or registered handler depending
	 * on how it functions here.
	 * This method is NOOP in the base class, and should NOT
	 * be called by clients of this class.
	 */
	virtual void	OnConnect( Connection* ) ;

	/**
	 * This method is called when the given connection attempt
	 * fails.  This could be for either an incoming or outgoing
	 * attempt -- see the Connection's flags for determining
	 * which it is.
	 * The internal state of the ConnectionManager will update
	 * itself after calling this method.
	 */
	virtual void	OnConnectFail( Connection* ) ;

	/**
	 * This method is called when a string of data is available
	 * from the remote connection referred to the Connection.
	 * The string of data (line) is passed as determined by
	 * the delimiter passed to the constructor of this class.
	 * Clients of this class should NOT call this method.
	 */
	virtual void	OnRead( Connection*, const string& ) ;

	/**
	 * This is a handler method called when a connection is
	 * established but is then closed by the remote end.  Note
	 * that connections terminated with Disconnect() are NOT
	 * reflected by calling OnDisconnect().
	 * This method is also called when a listening (incoming)
	 * socket is no longer valid.  If this occurs, the Connection
	 * flags will have F_INCOMING set, and that further listening
	 * on the given port will no longer proceed.
	 * The internal state of the ConnectionManager will update
	 * itself after calling this method.
	 */
	virtual void	OnDisconnect( Connection* ) ;

	/**
	 * This method is called if a connection timeout occurs.
	 * The internal state of the ConnectionManager will update
	 * itself after calling this method.
	 */
	virtual void	OnTimeout( Connection* ) ;

protected:

} ;

#endif // __CONNECTIONHANDLER_H
