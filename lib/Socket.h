/* Socket.h
 */

#ifndef __SOCKET_H
#define __SOCKET_H "$Id: Socket.h,v 1.1 2000/06/30 18:46:06 dan_karrels Exp $"

#include	<new>
#include	<iostream>
#include	<fstream>
#include	<strstream>
#include	<string>
#include	<vector>

#include	"sys/types.h"
#include	"netinet/tcp.h" // TCP_

#include	"cstdio"
#include	"cstring"
#include	"cerrno"
#include	"cstdlib"
#include	"unistd.h"
#include	"netdb.h"
#include	"limits.h"

#include	"strings.h"
#include	"fcntl.h"
#include	"sys/uio.h"
#include	"sys/types.h"
#include	"sys/time.h"
#include	"sys/stat.h"
#include	"sys/socket.h"
#include	"arpa/inet.h"
#include	"netinet/in.h" // IP_

/**
 * A default port number for testing Socket and its
 * children classes.
 */
const int __DefaultTestServerPortNo_ = 57575 ;

/**
 * This structure is responsible for representing the state
 * associated with a connection to a remote machine.
 */
struct SocketInfo
{

	/**
	 * This is the actual file (socket) descriptor to the
	 * remote machine, -1 if not connected.
	 */
	int fd ;

	/**
	 * This is the socket address structure containing
	 * information about the remote machine to which we
	 * are connected, NULL if not connected.
	 */
	struct sockaddr_in* addr ;

	/**
	 * Default construction of the SocketInfo just initializes
	 * the (fd) and (addr) variables to -1 and NULL, respectively.
	 */
	inline SocketInfo()
	  : fd( -1 ), addr( 0 )
	{}

	/**
	 * Copy constructor.
	 * The (fd) is copied using a primitive copy.
	 * The socket address structure (addr) is allocated here,
	 * and the contents of the SocketInfo structure (rhs)'s
	 * (addr) variable is copied into (this->addr).
	 */
	inline SocketInfo( const SocketInfo& rhs )
	  : fd( rhs.fd ), addr( 0 )
		{
		addr = new struct sockaddr_in ;
		memcpy( addr, rhs.addr, sizeof( sockaddr_in ) ) ;
		}

	/**
	 * Destructor for the SocketInfo structure.
	 * This method does NOT close the socket descriptor associated
	 * with this SocketInfo structure, it must be closed manually.
	 * However, this SocketInfo's (addr) variable is deallocated,
	 * and set to NULL (just for good programming style).
	 */
	inline ~SocketInfo()
		{
 		//::close(fd); let close do this, because deep copies may exist while
 		// fd has not been "duped"
 		delete addr ;
		addr = 0 ;
		}

	/**
	 * The close method will close the socket descriptor (fd), and
	 * reset it to -1.
	 */
	int close() ;

} ;

/**
 * This class provides basic functionality for communications
 * using a blocking TCP connection.
 */
class Socket
{
public:

	/**
	 * This is the timeout to be used for polling the socket.
	 * It is public and static to allow outside parties to
	 * alter and query its value.  Note that it being static
	 * makes it inherently thread UNsafe.
	 */
	static float _timeOut ;

	/**
	 * This is the type that will be used for representing
	 * a socket descriptor.
	 */
	typedef int socketFd ;

protected:

	/**
	 * This is the remote port number to which this Socket
	 * is connected, -1 if not connected.
	 */
	int 		_portNo ; 

	/**
	 * This is the SocketInfo structure representing
	 * (holding the state) a particular connection.
	 */
	SocketInfo	_sockinfo ;

public:

	/**
	 * Construct a Socket with default values, and not connected.
	 */
	inline Socket( int port= -1 )
	 : _portNo( port ), _sockinfo()
	{}

	/**
	 * Construct a socket given an existing connection on a
	 * given port.
	 */
	Socket( const SocketInfo& connection, int port ) ;

	/**
	 * Allow Socket's to be copied.
	 */
	inline Socket( const Socket& rhs )
	  : _portNo( rhs._portNo ), _sockinfo( rhs._sockinfo )
	{}

	/**
	 * The destructor for the class Socket doesn't do much.
	 * Check the destructor for SocketInfo, and be sure to
	 * close the connection manually (for now) before
	 * destroying instances of class Socket.
	 */
	inline virtual ~Socket()
	{  _portNo = -1 ; }

	/**
	 * Close the currently open socket connection, and
	 * reinitialize state variables to default values.
	 * Note that destructors do NOT call this method, it
	 * must be called explicitly before destroying the
	 * Socket instance.  This is to permit safe copying
	 * of the SocketInfo structure.
	 */
	inline virtual int close()
	{ _portNo = -1 ; return _sockinfo.close() ; }

	/**
	 * Send no more than (nb) bytes of the unsigned character
	 * array pointer to by (b) to the socket.
	 * Returns number of bytes written, -1 on error.
	 */
	virtual int send( const unsigned char* b, int nb ) ;

	/**
	 * Receive no more than (nb) bytes into the unsigned
	 * character array pointer to by (b).
	 * The number of bytes read is returned, or -1 on error.
	 */
	virtual int recv( unsigned char* b, int nb ) ;

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
	virtual int send( const string& val ) ;

	/**
	 * Returns a const reference to the SocketInfo object
	 * holding the state of this Socket.
	 */
	inline const SocketInfo& getInfo() const
	{ return _sockinfo ; }

	/**
	 * This method opens and configures the socket.
	 */
	int setSocket( SocketInfo& sock ) ;

	/**
	 * This method initializes this Socket to default
	 * values, and calls setSocket.
	 */
	int resetSocket( SocketInfo& sock ) ;

	/**
	 * Return the IP address of the given host name.
	 * If an IP is not found, an empty string is
	 * returned.
	 */
	static string ipAddrOf( const string& host ) ;

	/**
	 * Return the hostname of the local machine.
	 */
	static string hostname() ;

	/**
	 * Returns true if the C++ string is a dotted decimal
	 * IP address.  This does not check that it is an existing
	 * IP address, just that it is in the proper format.
	 */
	static bool isIPAddress( const string& h ) ;

	/**
	 * This method returns the port number and IP
	 * address of the remote connection in C++ string
	 * format.
	 */
	virtual string description() const ;

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
 
	/**
	 * Check a list of socketFd's for pendinginput.
	 * The socket file descriptors are given in the vector
	 * (socFds).  The list of socketFd's available for immediate
	 * recv()'s are put into pendingList.
	 * A timeout value of -1.0 defaults to an infinite wait
	 * socket data to be ready on any of the socketFd's.
	 * Returns the number of socketFd's ready for immediate recv(),
	 * or -1 on error.
	 */
	static int pendingInputList( const vector< socketFd >& socFds,
		vector< socketFd >& pendingList,
		float timeOut = -1.0 ) ;

	/**
	 * Check a list of socketFd's for space in the TCP
	 * output buffer of each.  The vector of socketFd's to
	 * check is (socFds).  The vector of socketFd's whose
	 * TCP output buffer is not completely full is returned
	 * in (writeList).  A timeout value of -1.0 (default)
	 * indicates that the method is to wait indefinitely
	 * for at leaste one of the socket descriptors to become
	 * writable.
	 * Returns the number of socketFd's ready for non-blocking
	 * immediate send(), or -1 on error.
	 */
	static int writableList( const vector< socketFd >& socFds,
		vector< socketFd >& writeList,
		float timeOut = -1.0 ) ; 

} ;

#endif // __SOCKET_H
