/* ClientSocket.h 
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
 */

#ifndef __CLIENTSOCKET_H
#define __CLIENTSOCKET_H "$Id: ClientSocket.h,v 1.5 2002/05/27 17:18:12 dan_karrels Exp $"

#include	<string>

#include	"Socket.h" // inherit send, recv, status methods
#include	"cstring"
#include	"limits.h"

namespace gnuworld
{

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
	virtual int connect( const std::string& host,
		unsigned short int portNo ) ;

protected:

	/**
	 * The hostname of the machine to which this ClientSocket is
	 * connected, empty if not connected.
	 */
	std::string		_host ;

	/**
	 * The IP address, in dotted decimal form, to which this
	 * ClientSocket is connected, empty if not connected.
	 */
	std::string		_ipAddr ;

} ;

} // namespace gnuworld

#endif // __CLIENTSOCKET_H
