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
 * $Id: Connection.h,v 1.7 2004/01/07 18:33:42 dan_karrels Exp $
 */

#ifndef __CONNECTION_H
#define __CONNECTION_H "$Id: Connection.h,v 1.7 2004/01/07 18:33:42 dan_karrels Exp $"

#include	<sys/types.h>
#include	<netinet/in.h>
#include	<arpa/inet.h> // sockaddr
#include	<sys/socket.h>
#include	<netdb.h>

#include	<string>
#include	<iostream>

#include	"Buffer.h"
#include	"ELog.h"

namespace gnuworld
{

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
	 * The hostname may be empty if it represents a listening
	 * socket.  Otherwise, the hostname will contain either
	 * a valid hostname, or a string representation of the
	 * Connection's IP address.
	 */
	inline const std::string&	getHostname() const
		{ return hostname ; }

	/**
	 * Return the IP for this connection.
	 * The IP may be empty if it represents a listening
	 * socket.
	 */
	inline const std::string&	getIP() const
		{ return IP ; }

	/**
	 * Return the local port number for this Connection.
	 */
	inline unsigned short int	getLocalPort() const
		{ return localPort ; }

	/**
	 * Return the remote port number for this Connection.
	 */
	inline unsigned short int	getRemotePort() const
		{ return remotePort ; }

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
	 * connection created by a remote host connecting to the
	 * localhost.
	 * The F_INCOMING flag exists throughout the life ot
	 * the Connection.
	 */
	static const flagType		F_INCOMING ;

	/**
	 * This flag is true if this Connection represents a
	 * socket which is listening for incoming connections.
	 * This flag exists for the life of this connection.
	 * A listening Connection is never connected.
	 */
	static const flagType		F_LISTEN ;

	/**
	 * This flag is true when the Connection represents a
	 * connection to a file, rather than a network connection.
	 */
	static const flagType		F_FILE ;

	/**
	 * This flag will be set if the next write is to flush all
	 * data from the output buffer.
	 * The flag will be reset after the send().
	 */
	static const flagType		F_FLUSH ;

	/**
	 * Append a string to the Connection's output buffer.
	 */
	virtual void		Write( const std::string& ) ;

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
	 * incoming connections, as a result of calling
	 * ConnectionManager::Listen().
	 * Note that this flag remains true for the life of the
	 * connection.
	 */
	inline bool	isListening() const
		{ return (flags & F_LISTEN ) ; }

	/**
	 * Return true if this Connection is a file connection, false
	 * otherwise.
	 */
	inline bool	isFile() const
		{ return (flags & F_FILE) ; }

	/**
	 * Return true if the flush flag is set, indicating that the
	 * next send() should write all available data, blocking
	 * if necessary.
	 */
	inline bool	isFlush() const
		{ return (flags & F_FLUSH) ; }

	/**
	 * Return the total number of bytes read from this
	 * Connection.
	 */
	inline size_t	getBytesRead() const
		{ return bytesRead ; }

	/**
	 * Return the total number of bytes successfully written
	 * to this Connection (does not count amount currently
	 * in the output buffer).
	 */
	inline size_t	getBytesWritten() const
		{ return bytesWritten ; }

	/**
	 * Return the time at which this Connection object established
	 * connection.  This value is 0 for listening sockets.
	 */
	inline time_t	getConnectTime() const
		{ return connectTime ; }

	/**
	 * Return the number of bytes currently in the output
	 * buffer.
	 */
	inline size_t	getOutputBufferSize() const
		{ return outputBuffer.size() ; }

	/**
	 * Return the number of bytes currently in the input
	 * buffer.
	 */
	inline size_t	getInputBufferSize() const
		{ return inputBuffer.size() ; }

	/**
	 * Set the F_FLUSH flag to true.
	 */
	inline void	Flush()
		{ setFlag( F_FLUSH ) ; }

	/**
	 * This friend operator allows for the easy output of a
	 * Connection object to a given output stream.
	 */
	friend std::ostream& operator<<( std::ostream& out, const 
			Connection& con )
		{
		out	<< "Host: " << con.getHostname()
			<< ", IP: " << con.getIP()
			<< ", localPort: " << con.getLocalPort()
			<< ", remotePort: " << con.getRemotePort()
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

	/**
	 * This friend operator allows for the easy output of a
	 * Connection object to a given ELog output stream.
	 */
	friend ELog& operator<<( ELog& out, const Connection& con )
		{
		out	<< "Host: " << con.getHostname()
			<< ", IP: " << con.getIP()
			<< ", localPort: " << con.getLocalPort()
			<< ", remotePort: " << con.getRemotePort()
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
	 * host (may be empty), the IP, remote port number.
	 */
	Connection( const std::string& host,
			const unsigned short int remotePort,
			const char delimiter ) ;

	/**
	 * Create a new instance of this class, set all variables
	 * to default initial state.
	 */
	Connection( const char delimiter ) ;

	/**
	 * Set an arbitrary flag for this connection
	 */
	inline void	setFlag( const flagType& whichFlag )
		{ flags |= whichFlag ; }

	/**
	 * Remove an arbitrary flag for this connection
	 */
	inline void	removeFlag( const flagType& whichFlag )
		{ flags &= ~whichFlag ; }

	/**
	 * Set this Connection to fully connected state, which also
	 * removes the PENDING state
	 */
	inline void	setConnected()
		{ removeFlag( F_PENDING ) ; setFlag( F_CONNECTED ) ; }

	/**
	 * Set this Connection to the pending state, and remove
	 * the CONNECTED state
	 */
	inline void	setPending()
		{ removeFlag( F_CONNECTED ) ; setFlag( F_PENDING ) ; }

	/**
	 * Mark that this Connection as an incoming connection
	 */
	inline void	setIncoming()
		{ setFlag( F_INCOMING ) ; }

	/**
	 * Mark that this Connection is a listening for connections
	 */
	inline void	setListen()
		{ setFlag( F_LISTEN ) ; }

	/**
	 * Mark that this Connection represents a file
	 */
	inline void	setFile()
		{ setFlag( F_FILE ) ; }

	/**
	 * Set this connection's local port number
	 */
	inline void	setLocalPort(
				const unsigned short int newLocalPort )
		{ localPort = newLocalPort ; }

	/**
	 * Set this connection's remote port number
	 */
	inline void	setRemotePort(
				const unsigned short int newRemotePort )
		{ remotePort = newRemotePort ; }

	/**
	 * Return the socket (file) descriptor for this connection
	 */
	inline int	getSockFD() const
		{ return sockFD ; }

	/**
	 * Return a pointer to the socket address structure
	 */
	inline struct sockaddr_in* getAddr()
		{ return &addr ; }

	/**
	 * Set the Connection's IP to the new IP
	 */
	inline void	setIP( const std::string& newIP )
		{ IP = newIP ; }

	/**
	 * Set the Connection's hostname to newHost
	 */
	inline void	setHostname( const std::string& newHost )
		{ hostname = newHost ; }

	/**
	 * Set the Connection's FD to the new FD
	 */
	inline void	setSockFD( int newSockFD )
		{ sockFD = newSockFD ; }

	/**
	 * Set the Connection absTimeout to the new absTimeOut
	 */
	inline void	setAbsTimeout( const time_t newAbsTimeout )
		{ absTimeout = newAbsTimeout ; }

	/**
	 * Return the time at which this connection attempt will
	 * be terminated (its absolute timeout value).
	 */
	inline time_t	getAbsTimeout() const
		{ return absTimeout ; }

	/**
	 * The remote hostname of this connection
	 * This variable is empty() if this Connection is a listener
	 */
	std::string		hostname ;

	/**
	 * The local port number of this connection
	 */
	unsigned short int	localPort ;

	/**
	 * The remote port number of this connection
	 */
	unsigned short int	remotePort ;

	/**
	 * The input buffer for this connection
	 */
	Buffer			inputBuffer ;

	/**
	 * The output buffer for this connection
	 */
	Buffer			outputBuffer ;

	/**
	 * The remote IP of this connection
	 * This variable is empty() if this Connection is a listener
	 */
	std::string		IP ;

	/**
	 * The socket (file) descriptor for the socket of this
	 * connection.
	 */
	int			sockFD ;

	/**
	 * The flags associated with this connection
	 */
	flagType		flags ;

	/**
	 * The socket address structure for this connection
	 */
	struct sockaddr_in	addr ;

	/**
	 * The time at which this connection attempt began
	 */
	time_t			absTimeout ;

	/**
	 * The time at which this Connection completed connection,
	 * 0 for listening socket.
	 */
	time_t			connectTime ;

	/**
	 * The total number of bytes read
	 */
	size_t			bytesRead ;

	/**
	 * The total number of bytes written
	 */
	size_t			bytesWritten ;

} ;

} // namespace gnuworld

#endif // __CONNECTION_H
