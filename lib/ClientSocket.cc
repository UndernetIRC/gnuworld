/* ClientSocket.cc
 */

#include	<string>

#include	<unistd.h>

#include	"ClientSocket.h"
#include	"ELog.h"

const char ClientSocket_h_rcsId[] = __CLIENTSOCKET_H ;
const char ClientSocket_cc_rcsId[] = "$Id: ClientSocket.cc,v 1.2 2000/12/15 00:13:44 dan_karrels Exp $" ;

using std::string ;
using std::endl ;
using gnuworld::elog ;

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

elog	<< "ClientSocket::connect> attempt to connect to: " << _host
	<< " (" << _ipAddr << ") " << "using port # " << portNum
	<< endl ;

if( ::connect( fd, reinterpret_cast< sockaddr* >( &addr ),
	sizeof( struct sockaddr_in ) ) < 0 )
	{
	elog	<< "ClientSocket::connect> unable to connect to "
		<< _host << ", port # " << portNum << endl ;

	::close( fd ) ;
	fd = -1 ;
	portNum = 0 ;

	memset( &addr, 0, sizeof( struct sockaddr_in ) ) ;
	}
 
return fd ;

} // connect
