/* ClientSocket.cc
 */

#include	<string>

#include	"ClientSocket.h"
#include	"ELog.h"

const char ClientSocket_h_rcsId[] = __CLIENTSOCKET_H ;
const char ClientSocket_cc_rcsId[] = "$Id: ClientSocket.cc,v 1.1 2000/06/30 18:46:06 dan_karrels Exp $" ;

using std::string ;
using std::endl ;
using gnuworld::elog ;

ClientSocket::~ClientSocket()
{}

Socket::socketFd ClientSocket::connect( const string& host, int port )
{ 
if( host.empty() )
	{
	_host = hostname() ;
	}
else
	{
	_host = host ;
	}

if( port >= 0 )
	{
	_portNo = port ;
	}

if( _portNo <= 0 )
	{
	elog << "ClientSocket::connect> portNo = "
		<< _portNo << endl ;
	return -1 ;
	}
  
_sockinfo.fd = socket( AF_INET, SOCK_STREAM, 0 ) ;
if( _sockinfo.fd < 0 )
	{
	elog << "ClientSocket::connect> unable to create new stream "
		<< " socket" << endl ;
	return  _sockinfo.fd;
	}

setSocket( _sockinfo ) ; // allocates _sockinfo.addr

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

_sockinfo.addr->sin_addr.s_addr = inet_addr( _ipAddr.c_str() ) ;

elog	<< "ClientSocket::connect> attempt to connect to: " << _host
	<< " (" << _ipAddr << ") " << "using port # " << _portNo
	<< endl ;

if( ::connect( _sockinfo.fd, reinterpret_cast< sockaddr* >( _sockinfo.addr ),
	sizeof( sockaddr_in ) ) < 0 )
	{
	elog	<< "ClientSocket::connect> unable to connect to "
		<< _host << ", port # " << _portNo << endl ;
	delete _sockinfo.addr ;
	_sockinfo.addr = 0 ;
	_sockinfo.fd = -1 ;
	}
 
return _sockinfo.fd ;

} // connect
