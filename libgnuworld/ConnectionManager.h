/**
 * ConnectionManager.h
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
 * $Id: ConnectionManager.h,v 1.8 2005/01/12 04:36:43 dan_karrels Exp $
 */

#ifndef __CONNECTIONMANAGER_H
#define __CONNECTIONMANAGER_H "$Id: ConnectionManager.h,v 1.8 2005/01/12 04:36:43 dan_karrels Exp $"

#include	<sys/types.h>

#include	<iostream>
#include	<string>
#include	<list>
#include	<map>
#include	<set>

#include	<ctime>

#include	"Connection.h"
#include	"ConnectionHandler.h"

namespace gnuworld
{

/**
 * The purpose of this class it to manage multiple incoming and
 * outgoing connections on behalf of ConnectionHandler's.
 * This class is intended for systems which support multiple client
 * classes which might require one or more incoming and outgoing
 * connections, but may be used to manage a single connection
 * equally as well.  Any client that wishes to create a connection
 * should subclass ConnectionHandler, and overload the method(s)
 * in which it is interested.
 * This class will support all connections for any number of
 * different ConnectionHandlers (note that a pointer to the
 * ConnectionHandler is passed as the first argument to many
 * methods).  Each Connection is associated with a particular
 * ConnectionHandler, and no other ConnectionHandlers may view
 * or modify Connections which it does not own.
 * Any opening/closing of Connections is performed within this
 * class, so it is not necessary to perform any direct maintenance
 * of a particular Connection.  Simply remember to call the
 * appropriate methods based on different situations (Disconnect(),
 * for example), and the ConnectionManager will update the state
 * of those Connections.
 */
class ConnectionManager
{

	/**
	 * This type is used to store Connection's in a weak sorted
	 * manner.
	 * There is a single instance of a connectionMapType for
	 * each handler which has one or more Connection's registered.
	 */
	typedef std::set< Connection* > connectionMapType ;

	/**
	 * This type is used as convenience to define an iterator
	 * type for the connectionMap.
	 */
	typedef connectionMapType::iterator	connectionMapIterator ;

	/**
	 * A const iterator used to iterate through a connectionMap.
	 */
	typedef connectionMapType::const_iterator constConnectionMapIterator ;

	/**
	 * This map stores connectionMapType's, keyed by the handler
	 * which registered the connection(s).
	 */
	typedef std::map< ConnectionHandler*, connectionMapType > 
		handlerMapType ;

	/**
	 * This type is used as convenience to define an iterator
	 * type for the handlerMap.
	 */
	typedef handlerMapType::iterator	handlerMapIterator ;

	/**
	 * A const iterator used to iterate through the handlerMap.
	 */
	typedef handlerMapType::const_iterator	constHandlerMapIterator ;

	/**
	 * The type is used to store Connection objects to be erased.
	 * This structure stores an iterator to each Connection
	 * to be removed.  Removal is synchronous to prevent
	 * iterator invalidation of the handlerMap.
	 * Because more than one item may be associated with any
	 * given key, we need a multimap here.  While that may
	 * be a useful trait for handling more than one Connection
	 * for any given ConnectionHandler, it also has the unfortunate
	 * side effect of allowing a Connection to be scheduled for
	 * closure more than once.  The scheduleErasure() method is
	 * used to prevent multiple entries of the same Connection
	 * into the eraseMap.
	 */
	typedef std::multimap< ConnectionHandler*, connectionMapType::iterator >
		eraseMapType ;

	/**
	 * This type is used as convenience to define an iterator
	 * type for the eraseMap.
	 */
	typedef eraseMapType::iterator		eraseMapIterator ;

public:
	/**
	 * Default constructor.  The first argument which (may) be
	 * supplied is the default timeout value for establishing
	 * individual connections (in seconds).  If the (outgoing)
	 * connection is not established within that time period,
	 * then the connection's handler is notified via OnTimeout(),
	 * and the Connection scheduled for erasure.  Note that
	 * listening Connections do not have a timeout.
	 * Delimiter is the line delimiter for determining when a
	 * full line/command has been read from each connection.
	 * Once this delimiter is encountered, the line/command is
	 * passed to OnRead().
	 */
	ConnectionManager( const time_t defaultTimeout = 10,
		const char defaultDelimiter = '\n' ) ;

	/**
	 * The destructor will deallocate any memory and close all
	 * connections.
	 * Handlers for the given connections will *not* be
	 * notified.
	 */
	virtual ~ConnectionManager() ;

	/**
	 * This method will set the timeout duraction for connection 
	 * attempts to the new value given to the method.
	 */
	void	setTimeoutDuration( const time_t newTimeoutDuration )
			{ timeoutDuration = newTimeoutDuration ; }

	/**
	 * Return a string with (host)'s IP address in numbers
	 * and dots format.
	 * Returns an empty string if no IP address found.
	 */
	static std::string	ipAddrOf( const std::string& host ) ;

	/**
	 * Return true if the given string is in the proper
	 * IP format of numbers and dots.
	 */
	static bool	isIpAddress( const std::string& host ) ;

	/**
	 * Connect() will attempt to establish a new connection to
	 * the given host, on the given port.  The (host) field may
	 * be the canonical host name, or the IP in the form of
	 * numbres and dots.
	 * This method creates a non-blocking socket with which to
	 * connect to the remote host, thus a return value of non-NULL
	 * does not necessarily mean that the connection is valid,
	 * just that the attempt is in progress.
	 * A return value of NULL indicates that some part of the
	 * connection process failed, and errno is set appropriately.
	 * UDP sockets are currently not implemented fully, and will
	 * not work properly.
	 * The ConnectionHandler must be non-NULL.
	 */
	virtual Connection*	Connect(
				ConnectionHandler*,
				const std::string& host,
				const unsigned short int remotePort ) ;

	/**
	 * Connect to a file instead of a network host.  This method
	 * has the same semantics as Connect(), except that OnConnect()
	 * may be called during the call to ConnectToFile().
	 */
	virtual Connection*	ConnectToFile( ConnectionHandler*,
					const std::string& ) ;

	/**
	 * Attempt to establish a listening Connection on the
	 * given port number.  If successful, the Connection is
	 * returned.  The Connection returned is of no functional
	 * use except to indicate success or failure.  That
	 * Connection is the actual listening socket, and cannot
	 * be written to or read from directly.
	 * The ConnectionHandler must be non-NULL.
	 */
	virtual Connection*	Listen(
				ConnectionHandler*,
				const unsigned short int localPort ) ;

	/**
	 * DisConnect() forces the connection(s) associated with
	 * hostname/ports to be disconnected and deallocated.
	 * By specifying the optional localPort, only outgoing
	 * connections which match all 3 search criteria will be
	 * removed; not specifying a third parameter will cause all
	 * connections which match the first two search criteria to
	 * be removed.
	 * OnDisconnect() is NOT called.
	 * The ConnectionHandler must be non-NULL.
	 * The Connection will not be removed if the ConnectionHandler
	 * does not own the particular Connection.
	 * This method schedules the given Connection to be disconnected
	 * in the next call to Poll().
	 * To close a listening Connection, pass an empty hostname, in
	 * which case the remotePort will be ignored.
	 */
	virtual bool	DisconnectByHost( ConnectionHandler*,
			const std::string& hostname,
			const unsigned short int remotePort,
			const unsigned short int localPort = 0 ) ;

	/**
	 * DisConnect() forces the connection(s) associated with
	 * IP/ports to be disconnected and deallocated.
	 * By specifying the optional localPort, only outgoing
	 * connections which match all 3 search criteria will be
	 * removed; not specifying a third parameter will cause all
	 * connections which match the first two search criteria to
	 * be removed.
	 * OnDisconnect() is NOT called.
	 * The ConnectionHandler must be non-NULL.
	 * The Connection will not be removed if the ConnectionHandler
	 * does not own the particular Connection.
	 * This method schedules the given Connection to be disconnected
	 * in the next call to Poll().
	 * To close a listening Connection, pass an empty hostname, in
	 * which case the remotePort will be ignored.
	 */
	virtual bool	DisconnectByIP( ConnectionHandler*,
			const std::string& IP,
			const unsigned short int remotePort,
			const unsigned short int localPort = 0 ) ;

	/**
	 * Disconnect the given Connection from ConnectionHandler's
	 * list of Connections.
	 * The Connection will not be removed if the ConnectionHandler
	 * does not own the particular Connection.
	 * Return true if removal was successful, false if the
	 * Connection was not found under ConnectionHandler's control.
	 * Both the ConnectionHandler and the Connection must
	 * be non-NULL.
	 * This method schedules the given Connection to be disconnected
	 * in the next call to Poll().
	 * Passing NULL as the Connection* will remove ALL connections
	 * belonging to the ConnectionHandler.
	 */
	virtual bool	Disconnect( ConnectionHandler*,
				Connection* ) ;

	/**
	 * Return the number of outstanding connections for the given
	 * ConnectionHandler.
	 * If anyone figures out why gcc won't let me declare
	 * the ConnectionHandler* as const here please let me know.
	 */
	virtual size_t	numConnections( ConnectionHandler* ) const ;

	/**
	 * This method performs the actual read/write calls for all
	 * sockets.  This method will check for any
	 * pending outgoing connections (and call OnConnect() for
	 * all that succeed), check for read state on all sockets
	 * which are currently connected (and call OnRead() if conditions
	 * are met), check for dead connections (and call OnDisconnect()
	 * for each), and check for connection time outs (and call
	 * OnTimeout() for each).
	 * (seconds) and (milliseconds) is how long to wait for a
	 * state change on any of the Connections, in seconds and
	 * milliseconds, respectively.  The default is to return
	 * immediately (poll).
	 * If -1 is passed for seconds, then the process will block
	 * indefinitely waiting for a state change.
	 */
	virtual void	Poll( const long seconds = 0,
				const long milliseconds = 0 ) ;

protected:

	/**
	 * The duration to wait for outgoing connections to be 
	 * established.
	 */
	time_t		timeoutDuration ;

	/**
	 * The line delimiter, 0 if none
	 */
	char		delimiter ;

	/**
	 * Stores connectionMap's, one for each handler (key)
	 */
	handlerMapType	handlerMap ;

	/**
	 * Allow for asynchronous calls to Disconnect()
	 * This structure contains Connections to be removed from
	 * the connection tables.
	 */
	eraseMapType	eraseMap ;

	/**
	 * The size of the scratch buffer
	 */
	size_t		inputBufferSize ;

	/**
	 * Scratch buffer for reads
	 */
	char*		inputBuffer ;

	/**
	 * Open a socket.
	 * Return -1 on error
	 */
	virtual int	openSocket() ;

	/**
	 * Close a socket
	 */
	virtual void	closeSocket( int ) ;

	/**
	 * Set the options for a socket
	 * Return true is all options were set properly, false
	 * otherwise.
	 */
	virtual bool	setSocketOptions( int ) ;

	/**
	 * Attempt to read()/recv() from the given Connection.
	 * Post event(s) OnRead() or OnDisconnect().
	 * Return true if the read was successful, false if
	 * connection is no longer valid.
	 */
	virtual bool	handleRead( ConnectionHandler*, Connection* ) ;

	/**
	 * Attempt to write()/send() to the given Connection.
	 * Post event OnDisconnect() if necessary.
	 * Return true if the write was successful, false if
	 * the connection is no longer valid.
	 */
	virtual bool	handleWrite( ConnectionHandler*, Connection* ) ;

	/**
	 * Attempt to flush all data from the given Connection.
	 * Reset the F_FLUSH flag.
	 * Post event OnDisconnect() if necessary.
	 * Return true if the write(s) was successful, false if
	 * the connection is no longer valid.
	 */
	virtual bool	handleFlush( ConnectionHandler*, Connection* ) ;

	/**
	 * Attempt to complete the connection to the given Connection.
	 * Post event(s) OnConnect() or OnConnectFail().
	 * Return true if connect was successful, false if Connection
	 * is invalid.
	 */
	virtual bool	finishConnect( ConnectionHandler*, Connection* ) ;

	/**
	 * Attempt to complete an incoming connection creation.
	 * The ConnectionHandler will be notified either by a call
	 * to OnConnect() or OnConnectFail(), depending if the incoming
	 * connection attempt was successful or unsuccessful,
	 * respectively.
	 * Return true if connection attempt was successful, false
	 * otherwise.
	 */
	virtual bool	finishAccept( ConnectionHandler*, Connection* ) ;

	/**
	 * Because the eraseMap is a multimap, it is possible to insert
	 * more than one value for each key.  This is useful for
	 * removing more than a single connection for any single handler,
	 * but it also permits the insertion of the same connection
	 * more than once.  This would create a problem when closing
	 * connections, because the second (and third, fourth, etc)
	 * instance of a particular iterator would be invalidated
	 * once the iterator is removed once -- this would cause
	 * a process crash.
	 * This method performs a simple check to see if the connection
	 * iterator is already present in the eraseMap.  If so, just
	 * return because the connection is already scheduled to be
	 * removed, and this will occur in Poll().  Otherwise, add
	 * the connection to be erased to the eraseMap.
	 */
	virtual void	scheduleErasure( ConnectionHandler*,
				connectionMapType::iterator ) ;

	/**
	 * Call Disconnect() for each Connection owned by the
	 * given ConnectionHandler.
	 */
	virtual bool	disconnectAll( ConnectionHandler* ) ;

} ;

} // namespace gnuworld

#endif // __CONNECTIONMANAGER_H
