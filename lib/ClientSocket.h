/* ClientSocket.h 
 */

#ifndef __CLIENTSOCKET_H
#define __CLIENTSOCKET_H "$Id: ClientSocket.h,v 1.2 2000/12/15 00:13:44 dan_karrels Exp $"

#include	"Socket.h" // inherit send, recv, status methods
#include	"cstring"
#include	"limits.h"

/**
 * This class provides basic functionality for a client type socket.
 * It inherits from Socket, and basically just adds the connect()
 * method.
 */
class ClientSocket : public Socket
{
 
public:

	/**
	 * Construct a ClientSocket with default values.
	 */
	ClientSocket()
	{}

	/**
	 * Construct a copy of a ClientSocket, copying all of
	 * its state.
	 */
	inline ClientSocket( const ClientSocket& rhs )
	  : Socket( rhs ), _host( rhs._host ), _ipAddr( rhs._ipAddr )
	{}

	/**
	 * Destroy a ClientSocket.
	 * This does NOT close the socket descriptor of the
	 * ClientSocket, that must be done manually.
	 */
	virtual ~ClientSocket() ;

	/**
	 * Attempt to connect to a given hostname on a given port.
	 * Returns a socket descriptor (socketFd) which is >= 0 if
	 * connection was successful, or -1 on error.
	 * This function blocks.
	 */
	virtual int connect( const string& host,
		unsigned short int portNo ) ;

protected:

	/**
	 * The hostname of the machine to which this ClientSocket is
	 * connected, empty if not connected.
	 */
	string		_host ;

	/**
	 * The IP address, in dotted decimal form, to which this
	 * ClientSocket is connected, empty if not connected.
	 */
	string		_ipAddr ;

} ;

#endif // __CLIENTSOCKET_H
