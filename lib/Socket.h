/* Socket.h
 */

#ifndef __SOCKET_H
#define __SOCKET_H "$Id: Socket.h,v 1.7 2001/07/29 22:44:06 dan_karrels Exp $"

#include	<new>
#include	<iostream>
#include	<fstream>
#include	<strstream>
#include	<string>
#include	<vector>

#include	<sys/types.h>
#include	<netinet/in.h>
#include	<arpa/inet.h> // sockaddr
#include	<sys/socket.h>
#include	<netdb.h>

#include	<cstdio>
#include	<cstring>
#include	<cerrno>
#include	<cstdlib>

namespace gnuworld
{

/**
 * This class provides basic functionality for communications
 * using a blocking TCP connection.
 */
class Socket
{

protected:

	/**
	 * This is the actual file (socket) descriptor to the
	 * remote machine, -1 if not connected.
	 */
	int			fd ;

	/**
	 * This is the socket address structure containing
	 * information about the remote machine to which we
	 * are connected, NULL if not connected.
	 */
	struct sockaddr_in	addr ;

	/**
	 * This is the remote port number to which this Socket
	 * is connected, -1 if not connected.
	 */
	unsigned short int	portNum ;

public:

	/**
	 * Construct a Socket with default values, and not connected.
	 */
	Socket() ;

	/**
	 * Allow Socket's to be copied.
	 */
	Socket( const Socket& rhs ) ;

	/**
	 * The destructor for the class Socket doesn't do much.
	 * Check the destructor for SocketInfo, and be sure to
	 * close the connection manually (for now) before
	 * destroying instances of class Socket.
	 */
	virtual ~Socket() {}

	/**
	 * Close the currently open socket connection, and
	 * reinitialize state variables to default values.
	 * Note that destructors do NOT call this method, it
	 * must be called explicitly before destroying the
	 * Socket instance.  This is to permit safe copying
	 * of the SocketInfo structure.
	 */
	inline virtual int close() ;

	/**
	 * Send no more than (nb) bytes of the unsigned character
	 * array pointer to by (b) to the socket.
	 * Returns number of bytes written, -1 on error.
	 */
	virtual int send( const unsigned char* b, size_t nb ) ;

	/**
	 * Send as much of the C NULL terminated string (s)
	 * to the socket connection as possible.
	 * Returns the number of bytes written, or -1 on error.
	 */
	virtual int send( const char* s ) ;

	/**
	 * Send as much of the C++ string object to the
	 * socket connection as possible.
	 * Returns number of bytes written, or -1 on error.
	 */
	virtual int send( const std::string& val, const size_t ) ;

	/**
	 * Receive no more than (nb) bytes into the unsigned
	 * character array pointer to by (b).
	 * The number of bytes read is returned, or -1 on error.
	 */
	virtual int recv( unsigned char* b, size_t nb ) ;

	/**
	 * Attempt to read numBytes bytes from the connection.
	 */
	virtual int recv( std::string&, size_t numBytes ) ;

	/**
	 * Return this Socket's file descriptor.
	 */
	virtual int getFD() const
		{ return fd ; }

	/**
	 * Return the IP address of the given host name.
	 * If an IP is not found, an empty string is
	 * returned.
	 */
	static std::string ipAddrOf( const std::string& host ) ;

	/**
	 * Return the hostname of the local machine.
	 */
	static std::string hostname() ;

	/**
	 * Returns true if the C++ string is a dotted decimal
	 * IP address.  This does not check that it is an existing
	 * IP address, just that it is in the proper format.
	 */
	static bool isIPAddress( const std::string& h ) ;

	/**
	 * This method returns the port number and IP
	 * address of the remote connection in C++ string
	 * format.
	 */
	virtual std::string description() const ;

	/**
	 * Return the number of bytes available for immediate
	 * read, or -1 on error.
	 */
	virtual int available() const ;

	/**
	 * Return the size of the TCP receive buffer window,
	 * or -1 on error.
	 */
	virtual int recvBufSize() const ;

	/**
	 * Return the size of the TCP send buffer window,
	 * or -1 on error.
	 */
	virtual int sendBufSize() const ;

	/**
	 * Return 1 if the Socket has pending IO to complete,
	 * 0 if no data to read/write,
	 * -1 on error.
	 */
	virtual int pendingIO() const ;

	/**
	 * Return 1 if data can be read without blocking,
	 * 0 if no data is available to read,
	 * -1 on error.
	 */
	virtual int readable() const ;
 
	/**
	 * Return 1 if the socket can be written to without blocking,
	 * 0 if the socket output buffer is full,
	 * -1 on error.
	 */
	virtual int writable() const ;
 
protected:

	/**
	 * This method opens and configures the socket.
	 */
	bool setSocket() ;


} ;

} // namespace gnuworld

#endif // __SOCKET_H
