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
 * $Id: ConnectionManager.h,v 1.8 2002/06/06 02:29:00 dan_karrels Exp $
 */

#ifndef __CONNECTIONMANAGER_H
#define __CONNECTIONMANAGER_H "$Id: ConnectionManager.h,v 1.8 2002/06/06 02:29:00 dan_karrels Exp $"

#include	<sys/types.h>
#include	<pthread.h>

#include	<iostream>
#include	<sstream>
#include	<string>
#include	<list>
#include	<set>

#include	<ctime>

#include	"Connection.h"

namespace gnuworld
{

using std::set ;
using std::stringstream ;
using std::string ;
using std::list ;

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
	typedef set< Connection* > connectionMapType ;

	/**
	 * This type is used as convenience to define an iterator
	 * type for the connectionMap.
	 */
	typedef connectionMapType::iterator	connectionMapIterator ;

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
	typedef list< connectionMapIterator > eraseMapType ;

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
	static string	ipAddrOf( const string& host ) ;

	/**
	 * Return true if the given string is in the proper
	 * IP format of numbers and dots.
	 */
	static bool	isIpAddress( const string& host ) ;

	/**
	 * Connect() will attempt to establish a new connection to
	 * the given host, on the given port.  The (host) field may
	 * be the canonical host name, or the IP in the form of
	 * numbres and dots. If TCP is true (or
	 * not given to the method, thus default), then a TCP
	 * connection will be made, UDP otherwise.
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
	virtual Connection*	Connect( const string& host,
				const unsigned short int remotePort ) ;

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
	 * The Connection will not be removed if the ConnectionHandler
	 * does not own the particular Connection.
	 * This method schedules the given Connection to be disconnected
	 * in the next call to Poll().
	 * To close a listening Connection, pass an empty hostname, in
	 * which case the remotePort will be ignored.
	 */
	virtual bool	DisconnectByHost( const string& hostname,
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
	 * The Connection will not be removed if the ConnectionHandler
	 * does not own the particular Connection.
	 * This method schedules the given Connection to be disconnected
	 * in the next call to Poll().
	 * To close a listening Connection, pass an empty hostname, in
	 * which case the remotePort will be ignored.
	 */
	virtual bool	DisconnectByIP( const string& IP,
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
	 */
	virtual bool	Disconnect( Connection* ) ;

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

protected:

	/// The duration to wait for outgoing connections to be 
	/// established
	time_t		timeoutDuration ;

	/// The line delimiter, 0 if none
	char		delimiter ;

	pthread_cond_t	condWait ;
	pthread_mutex_t	condMutex ;

	connectionMapType	connectionMap ;

	/// Allow for asynchronous calls to Disconnect()
	/// This structure contains Connections to be removed from
	/// the connection tables.
	eraseMapType	eraseMap ;

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

	/// Return -1 on error
	int		openSocket() ;

	/// Close a socket
	void		closeSocket( int ) ;

	/// Set the options for a socket
	/// Return true is all options were set properly, false
	/// otherwise.
	bool		setSocketOptions( int ) ;

	/**
	 * Attempt to read()/recv() from the given Connection.
	 * Post event(s) OnRead() or OnDisconnect().
	 * Return true if the read was successful, false if
	 * connection is no longer valid.
	 */
	bool		handleRead( Connection* ) ;

	/**
	 * Attempt to write()/send() to the given Connection.
	 * Post event OnDisconnect() if necessary.
	 * Return true if the write was successful, false if
	 * the connection is no longer valid.
	 */
	bool		handleWrite( Connection* ) ;

	/**
	 * Attempt to complete the connection to the given Connection.
	 * Post event(s) OnConnect() or OnConnectFail().
	 * Return true if connect was successful, false if Connection
	 * is invalid.
	 */
	bool		finishConnect( Connection* ) ;

	/**
	 * Attempt to complete an incoming connection creation.
	 * The ConnectionHandler will be notified either by a call
	 * to OnConnect() or OnConnectFail(), depending if the incoming
	 * connection attempt was successful or unsuccessful,
	 * respectively.
	 * Return true if connection attempt was successful, false
	 * otherwise.
	 */
	bool		finishAccept( Connection* ) ;

	void		scheduleErasure( connectionMapIterator ) ;

} ;

} // namespace gnuworld

#endif // __CONNECTIONMANAGER_H
