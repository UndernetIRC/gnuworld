/**
 * ConnectionHandler.h
 * Author: Daniel Karrels (dan@karrels.com)
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 * $Id: ConnectionHandler.h,v 1.6 2002/06/02 23:14:23 dan_karrels Exp $
 */

#ifndef __CONNECTIONHANDLER_H
#define __CONNECTIONHANDLER_H "$Id: ConnectionHandler.h,v 1.6 2002/06/02 23:14:23 dan_karrels Exp $"

#include	<sys/types.h>

#include	<sstream>
#include	<string>

#include	<ctime>

#include	"Connection.h"

namespace gnuworld
{

using std::string ;
using std::stringstream ;

class ConnectionManager ;

/**
 * The purpose of this class is to provide a concrete base class
 * for interfacing to a ConnectionManager instance.  This class
 * is designed to receive events on Connections for which the
 * ConnectionHandler is responsible.
 * Clients should NOT call any of these methods directly.
 * All state information of ConnectionManager is updated internally.
 * These methods are information for the Clients of the ConnectionManager
 * class, and are meant to be one-way.
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
	 * Create a ConnectionHandler instance with the controlling
	 * ConnectionManager as argument.
	 */
	ConnectionHandler( ConnectionManager* ) ;

	/**
	 * Destroy this ConnectionHandler instance.
	 * This does *not* destroy any associated Connections
	 * which may be in any associated ConnectionManager
	 * objects.
	 */
	virtual ~ConnectionHandler() ;

	/**
	 * This method is called when a host connection succeeds.
	 * This method is NOOP in the base class, and should NOT
	 * be called by clients of this class.
	 */
	virtual void	OnConnect( Connection* ) ;

	/**
	 * This method is called when the given connection attempt
	 * fails.  This could be for either an incoming or outgoing
	 * attempt -- see the Connection's flags for determining
	 * which it is.
	 * The given Connection is no longer valid when this method
	 * is called.
	 */
	virtual void	OnConnectFail( Connection* ) ;

	/**
	 * This method is called when a string of data is available
	 * from the remote connection referred to by the Connection.
	 * The string of data (line) is passed as determined by
	 * the delimiter passed to the constructor of the associated
	 * ConnectionManager instance.
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
	 * The given Connection is no longer valid when this method
	 * is called.
	 */
	virtual void	OnDisconnect( Connection* ) ;

	/**
	 * This method is called if a connection timeout occurs.
	 * The given Connection is no longer valid when this method
	 * is called.
	 */
	virtual void	OnTimeout( Connection* ) ;

	/**
	 * Appends data to the given Connection's output buffer,
	 * to be sent during a call to ConnectionManagerPoll().
	 * Connection must be non-NULL.
	 * This object must have a valid (non-NULL) ConnectionManager
	 * object associated with it.
	 */
	virtual void    Write( Connection*, const string& ) ;
  
	/**
	 * Appends data to the given Connection's output buffer,
	 * to be sent during a call to ConnectionManagerPoll().
	 * Connection must be non-NULL.
	 * This object must have a valid (non-NULL) ConnectionManager
	 * object associated with it.
	 */
	virtual void    Write( Connection*, const stringstream& ) ;

	/**
	 * Allow this Handler to be associated with a new (or
	 * initial) ConnectionHandler.
	 * This is something of a blemish in the design, as a
	 * result of fitting it into the rest of te GNUWorld
	 * system.
	 */
	inline void	setManager( ConnectionManager* newCM )
		{ cmPtr = newCM ; }

private:

	/**
	 * A pointer to the ConnectionManager for this Handler.
	 * This may be NULL if default constructor is chosen.
	 * This variable is private, so that subclasses may
	 * not directly access it.  This is done to improve
	 * system coupling, by forcing an adapter in some OO
	 * situations (GNUWorld in particular).
	 */
	ConnectionManager*	cmPtr ;

} ;

} // namespace gnuworld

#endif // __CONNECTIONHANDLER_H
