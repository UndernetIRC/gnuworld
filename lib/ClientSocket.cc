/* ClientSocket.cc
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

#include	<iostream>
#include	<string>

#include	<unistd.h>

#include	"ClientSocket.h"
#include	"ELog.h"

const char ClientSocket_h_rcsId[] = __CLIENTSOCKET_H ;
const char ClientSocket_cc_rcsId[] = "$Id: ClientSocket.cc,v 1.4 2002/05/27 17:18:12 dan_karrels Exp $" ;
const char ELog_h_rcsId[] = __ELOG_H ;

namespace gnuworld
{

using std::string ;
using std::endl ;

ClientSocket::~ClientSocket()
{}

int ClientSocket::connect( const string& host,
	unsigned short int port )
{ 
if( host.empty() )
	{
	_host = hostname() ;
	}
else
	{
	_host = host ;
	}

portNum = port ;
setSocket() ;

if( isIPAddress( _host ) )
	{
	// host string is actually IP addr. string
	_ipAddr = _host ;
	}
else
	{
	_ipAddr = ipAddrOf( _host ) ;

	// Make sure that the lookup was successful
	// If it fails, _host is returned, and inet_addr()
	// will crash
	if( _ipAddr == _host )
		{
		return -1 ;
		}
	}

addr.sin_addr.s_addr = inet_addr( _ipAddr.c_str() ) ;

elog	<< "ClientSocket::connect> attempt to connect to: "
	<< _host
	<< " (" << _ipAddr << ") using port # "
	<< portNum
	<< endl ;

if( ::connect( fd, reinterpret_cast< sockaddr* >( &addr ),
	sizeof( struct sockaddr_in ) ) < 0 )
	{
	elog	<< "ClientSocket::connect> unable to connect to "
		<< _host
		<< ", port # "
		<< portNum
		<< endl ;

	::close( fd ) ;
	fd = -1 ;
	portNum = 0 ;

	memset( &addr, 0, sizeof( struct sockaddr_in ) ) ;
	}
 
return fd ;

} // connect

} // namespace gnuworld
