/**
 * Connection.h
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
 * $Id: Connection.h,v 1.2 2002/05/27 17:53:04 dan_karrels Exp $
 */

#ifndef __CONNECTION_H
#define __CONNECTION_H "$Id: Connection.h,v 1.2 2002/05/27 17:53:04 dan_karrels Exp $"

#include	<sys/types.h>
#include	<netinet/in.h>
#include	<arpa/inet.h> // sockaddr
#include	<sys/socket.h>
#include	<netdb.h>

#include	<string>
#include	<iostream>

#include	"Buffer.h"

/// Import the C++ standard string class
using std::string ;

using std::ostream ;
using std::endl ;

using gnuworld::Buffer ;

/// Forward declaration of the manager class
class ConnectionManager ;

/**
 * This class represents an individual connection to be used with
 * the ConnectionManager class.
 * It is basically just a data storage class.
 */
class Connection
{

	/// Allow the manager class to have access to this class's
	/// protected members
	friend class ConnectionManager ;

public:

	/**
	 * The destructor does nothing since this is a helper class
	 * of ConnectionManager.
	 */
	virtual ~Connection() ;

	/**
	 * Return the hostname which this connection represents.
	 * The hostname may be empty.
	 */
	inline const string&	getHostname() const
		{ return hostname ; }

	/**
	 * Return the IP for this connection.
	 */
	inline const string&	getIP() const
		{ return IP ; }

	/**
	 * Return the port number for this connection.
	 */
	inline unsigned short int	getPort() const
		{ return port ; }

	/**
	 * Return true if this connection is TCP, false if UDP.
	 */
	inline bool		isTCP() const
		{ return TCP ; }

	/**
	 * Return true if this connection is UDP, false if TCP.
	 */
	inline bool		isUDP() const
		{ return !isTCP() ; }

	/**
	 * This type is used to represent flags of this connection,
	 * both for this class and any subclasses.
	 */
	typedef unsigned int flagType ;

	/**
	 * This flag is true if the connection is currently fully
	 * connected.
	 */
	static const flagType		F_CONNECTED ;

	/**
	 * This flag is true if the connection is still pending,
	 * not yet complete.
	 */
	static const flagType		F_PENDING ;

	/**
	 * This flag is true if this Connection represents a
	 * connection received by ConnectionManager::Listen(),
	 * and exists throughout the life of the connection.
	 */
	static const flagType		F_INCOMING ;

	/**
	 * This flag is true if this Connection represents a
	 * socket which is listening for incoming connections.
	 * This flag exists for the life of this connection.
	 */
	static const flagType		F_LISTEN ;

	/**
	 * Return the flags of this connection.
	 */
	inline const flagType&		getFlags() const
		{ return flags ; }

	/**
	 * Check if an individual flag is present on this connection.
	 */
	inline bool	hasFlag( const flagType& whichFlag ) const
		{ return (whichFlag == (flags & whichFlag)) ; }

	/**
	 * Return true if this is object represents a fully-connected
	 * socket, false otherwise.
	 * Note that this flag is mutually exclusive with F_PENDING.
	 */
	inline bool	isConnected() const
		{ return (flags & F_CONNECTED) ; }

	/**
	 * Return true if this connection is still pending.
	 * Note that this flag is mutually exclusive with F_CONNECTED.
	 */
	inline bool	isPending() const
		{ return (flags & F_PENDING) ; }

	/**
	 * Return true if this connection was received by a listening
	 * Connection.
	 * Note that this flag remains true for the life of the
	 * connection, and is not present for connection established
	 * with ConnectionManager::Connect().
	 */
	inline bool	isIncoming() const
		{ return (flags & F_INCOMING) ; }

	/**
	 * Return true if this connection is listening for new
	 * incoming connections, as a resulf of calling
	 * ConnectionManager::Listen().
	 * Note that this flag remains true for the life of the
	 * connection.
	 */
	inline bool	isListening() const
		{ return (flags & F_LISTEN ) ; }

	/**
	 * This friend operator allows for the easy output of a
	 * Connection object to a given output stream.
	 */
	friend ostream& operator<<( ostream& out, const Connection& con )
		{
		out	<< "Host: " << con.getHostname()
			<< ", IP: " << con.getIP()
			<< ", port: " << con.getPort()
			<< ", sockFD: " << con.getSockFD()
			<< ", state: "
			<< (con.isConnected() ? "connected" : "pending") ;
		if( con.isIncoming() )
			{
			out	<< ",incoming" ;
			}
		if( con.isListening() )
			{
			out	<< ",listening" ;
			} 
		return out ;
		}

protected:

	/**
	 * Create a new instance of this class given the remote
	 * host (may be empty), the IP, port number, and TCP/UDP
	 * flag (true if TCP).
	 */
	Connection( const string& host,
			const unsigned short int port,
			const bool TCP,
			const char delimiter ) ;

	/**
	 * Create a new instance of this class, set all variables
	 * to default initial state.
	 */
	Connection( const char delimiter ) ;

	/// Set an arbitrary flag for this connection
	inline void	setFlag( const flagType& whichFlag )
		{ flags |= whichFlag ; }

	/// Remove an arbitrary flag for this connection
	inline void	removeFlag( const flagType& whichFlag )
		{ flags &= ~whichFlag ; }

	/// Set this Connection to fully connected state, which also
	/// removes the PENDING state
	inline void	setConnected()
		{ removeFlag( F_PENDING ) ; setFlag( F_CONNECTED ) ; }

	/// Set this Connection to the pending state, and remove
	/// the CONNECTED state
	inline void	setPending()
		{ removeFlag( F_CONNECTED ) ; setFlag( F_PENDING ) ; }

	/// Mark that this Connection is an incoming connection
	inline void	setIncoming()
		{ setFlag( F_INCOMING ) ; }

	/// Mark that this Connection is a listening for connections
	inline void	setListen()
		{ setFlag( F_LISTEN ) ; }

	/// Set this connection's port number
	inline void	setPort( const unsigned short int newPort )
		{ port = newPort ; }

	/// Set the TCP variable, default value is true
	inline void	setTCP( bool newVal = true )
		{ TCP = newVal ; }

	/// Return the socket (file) descriptor for this connection
	inline int	getSockFD() const
		{ return sockFD ; }

	/// Return a pointer to the socket address structure
	inline struct sockaddr_in* getAddr()
		{ return &addr ; }

	/// Set the Connection's IP to the new IP
	inline void	setIP( const string& newIP )
		{ IP = newIP ; }

	/// Set the Connection's hostname to newHost
	inline void	setHostname( const string& newHost )
		{ hostname = newHost ; }

	/// Set the Connection's FD to the new FD
	inline void	setSockFD( int newSockFD )
		{ sockFD = newSockFD ; }

	/// Set the Connection startTime to the new startTime
	inline void	setStartTime( time_t newStartTime )
		{ startTime = newStartTime ; }

	/// Return the time at which this connection attempt began
	inline time_t	getStartTime() const
		{ return startTime ; }

	/// The remote hostname of this connection, possibly empty
	string			hostname ;

	/// The remote port number of this connection
	unsigned short int	port ;

	/// True if TCP connection, false if UDP
	bool			TCP ;

	/// The input buffer for this connection
	Buffer			inputBuffer ;

	/// The output buffer for this connection
	Buffer			outputBuffer ;

	/// The remote IP of this connection
	string			IP ;

	/// The socket (file) descriptor for the socket of this
	/// connection
	int			sockFD ;

	/// The flags associated with this connection
	flagType		flags ;

	/// The socket address structure for this connection
	struct sockaddr_in	addr ;

	/// The time at which this connection attempt began
	time_t			startTime ;
} ;

#endif // __CONNECTION_H
