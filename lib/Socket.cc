/* Socket.cc
 */

#include	<new>
#include	<strstream>

#include	<unistd.h>
#include	<fcntl.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<sys/ioctl.h>
#include	<sys/time.h>
#include	<netinet/tcp.h>
#include	<dirent.h>

#include	<cmath>
#include	<cstring>

#include	"Socket.h"
#include	"defs.h"
#include	"ELog.h"

#ifdef HAVE_ASM_IOCTLS_H
	#include	<asm/ioctls.h>
#endif

#ifdef HAVE_XTI_INET_H
	#include	<xti_inet.h>
#endif

#ifdef HAVE_SYS_FILIO_H
	#include	<sys/filio.h>
#endif

const char Socket_h_rcsId[] = __SOCKET_H ;
const char Socket_cc_rcsId[] = "$Id: Socket.cc,v 1.3 2000/11/05 23:09:39 dan_karrels Exp $" ;

using namespace std ;
using gnuworld::elog ;

float Socket::_timeOut = -1.0;

// moved thse into SocketInfo so that listen/accept funcs. can use them
int SocketInfo::close()
{
if( fd < 0 )
	{
	errno = EBADF ;
	return fd ;
	}

int _fd = fd;
fd = -1;
delete addr; addr = 0;
return ::close( _fd ) ;
}

int Socket::recvBufSize() const
{
int recvSize = 0 ;

socklen_t len = static_cast< socklen_t >( sizeof( recvSize ) ) ;

if( ::getsockopt( _sockinfo.fd, SOL_SOCKET, SO_RCVBUF,
	reinterpret_cast< char* >( &recvSize ), &len ) < 0 )
	{
	elog	<< "Socket::recvBufSize> Error in getsockopt(): "
		<< strerror( errno ) << endl ;
	return -1 ;
	}
return recvSize ;
}

int Socket::sendBufSize() const
{
int sendSize = 0 ;

socklen_t len = static_cast< socklen_t >( sizeof( sendSize ) ) ;

if( ::getsockopt( _sockinfo.fd, SOL_SOCKET, SO_SNDBUF,
	reinterpret_cast< char* >( &sendSize ), &len ) < 0 )
	{
	elog	<< "Socket::sendBufSize> Error in getsockopt(): "
		<< strerror( errno ) << endl ;
	return -1 ;
	}
return sendSize ;
}

int Socket::available() const
{
#ifdef FIONREAD
	if( _sockinfo.fd < 0 )
		{
		errno = EBADF ;
		return -1 ;
		}

	int bytes = 0 ;
	if( ::ioctl( _sockinfo.fd, FIONREAD, &bytes ) < 0 )
		{
		elog	<< "Socket::available> Error in call to ioctl(): "
			<< strerror( errno ) << endl ;
		return -1 ;
		}

	return bytes ;
#else
	errno = ENOSYS ;
	return -1 ;
#endif
}

string Socket::ipAddrOf( const string& host )
{
struct hostent *hostEntry = ::gethostbyname( host.c_str() ) ;
if( NULL == hostEntry )
	{
	elog	<< "Socket::ipAddrOf> Unable to find host name: "
		<< host << endl ;
	return host ;
	}

struct in_addr in ;
char **p = hostEntry->h_addr_list ;
memcpy( &in.s_addr, *p, sizeof( in.s_addr ) ) ;
char ipAddr[] = "000.000.000.000" ;
sprintf( ipAddr,"%s", inet_ntoa( in ) ) ;

return string( ipAddr ) ;
}
  
string Socket::description() const
{
strstream s;
s << "socket(file descr. = " << _sockinfo.fd<< " ): ";
if( _sockinfo.addr )
	{
	s	<< "_portNo= " << _sockinfo.addr->sin_port
		<<" ip= " << inet_ntoa( _sockinfo.addr->sin_addr )
		<< ends ;
	}
string retval( s.str() ) ;
delete[] s.str() ;

return retval ;
}

int Socket::resetSocket( SocketInfo& socket )
{
if( _sockinfo.fd >= 0 )
	{
	close() ;
	}

_sockinfo.fd = -1 ;
delete _sockinfo.addr ; _sockinfo.addr = 0 ;

return setSocket( socket ) ;
}

int Socket::setSocket( SocketInfo& socket )
{
if( _sockinfo.fd < 0 && socket.fd > 0 )
	{
	_sockinfo = socket;
	_portNo = socket.addr->sin_port;
	}
else if( _sockinfo.fd < 0 )
	{
	elog << "Socket::setSocket> socket not initialized!" << endl ;
	return 0 ;
	}

if( _sockinfo.addr == 0 && _portNo > 0 )
	{
	try
		{
		_sockinfo.addr = new struct sockaddr_in;
		}
	catch( std::bad_alloc& )
		{
		elog << "Socket::setSocket> Memory allocation failure\n" ;
		return -1 ;
		}

	memset( static_cast< void* >( _sockinfo.addr ), 0,
		sizeof( sockaddr_in ) ) ;
	_sockinfo.addr->sin_family = AF_INET ;
	_sockinfo.addr->sin_port =
		htons( static_cast< u_short >( _portNo ) ) ;
	}

if( _sockinfo.addr != 0 && _portNo > 0 )
	{
	struct linger setLinger;
	setLinger.l_onoff = 0; // don't linger!     
	setLinger.l_linger = 0;
    
	if( ::setsockopt( _sockinfo.fd, SOL_SOCKET, SO_LINGER,
		reinterpret_cast< const char* >( &setLinger ),
		sizeof( setLinger ) ) < 0 )
		{
		elog << "Socket::setSocket> failed to set SO_LINGER" << endl ;
		elog << "Error: " << strerror( errno ) << endl ;
		}
	}

int optval = 1 ;

// detect closed connection
if( ::setsockopt( _sockinfo.fd, SOL_SOCKET, SO_KEEPALIVE,
	reinterpret_cast< const char* >( &optval ),
	sizeof( optval ) ) < 0 )
	{
	elog << "Socket::setSocket> failed to set SO_KEEPALIVE" << endl ;
	elog << "Error: " << strerror( errno ) << endl ;
	}

// immediately deliver msg
if( ::setsockopt( _sockinfo.fd, IPPROTO_TCP, TCP_NODELAY,
	reinterpret_cast< const char* >( &optval ),
	sizeof( optval ) ) < 0 )
	{
	elog << "Socket::setSocket> failed to set TCP_NODELAY" << endl ;
	elog << "Error: " << strerror( errno ) << endl ;
	}

// allow fast re-binds
if( ::setsockopt( _sockinfo.fd, SOL_SOCKET, SO_REUSEADDR,
	reinterpret_cast< const char* >( &optval ), sizeof( optval ) ) < 0 )
	{
	elog << "Socket::setSocket> failed to set SO_REUSEADDR" << endl ;
	elog << "Error: " << strerror( errno ) << endl ;
	}

// use max. buffer size
optval = 1024 * 1024 ;

if( ::setsockopt( _sockinfo.fd, SOL_SOCKET, SO_SNDBUF,
	reinterpret_cast< const char* >( &optval ), sizeof( optval ) ) < 0 )
	{
	elog << "Socket::setSocket> failed to set SO_SNDBUF" << endl ;
	elog << "Error: " << strerror( errno ) << endl ;
	}

if( ::setsockopt( _sockinfo.fd, SOL_SOCKET, SO_RCVBUF,
	reinterpret_cast< const char* >( &optval ), sizeof( optval ) ) < 0 )
	{
	elog << "Socket::setSocket> failed to set SO_RCVBUF" << endl ;
	elog << "Error: " << strerror( errno ) << endl ;
	}

// explicitly set to blocking
optval = ::fcntl( _sockinfo.fd, F_GETFL, 0 ) ;
if( optval < 0 )
	{
	elog << "Socket::setSocket> failed to get sock flags" << endl ;
	elog << "Error: " << strerror( errno ) << endl ;
	}

optval = ::fcntl( _sockinfo.fd, F_SETFL, optval & ~O_NONBLOCK ) ;
if( optval < 0 )
	{
	elog << "Socket::setSocket> failed to set O_NONBLOCK" << endl;
	elog << "Error: " << strerror( errno ) << endl ;
	}

return _portNo ;
}

// static
bool Socket::isIPAddress( const string& h )
{
unsigned short int count = 0 ;

for( string::size_type i = 0 ; (i < h.size()) && (count < 4) ; ++i )
	{
	// characters in an IP address may only be
	// digits or decimal points
	if( '.' == h[ i ] )
		{
		count++ ;
		}
	else if( !isdigit( h[ i ] ) )
		{
		return false ;
		}
	}

// there are exactly three decimal points in an IP address
return (3 == count) ;
}

// static
int Socket::writableList( const vector< socketFd >& socFds, 
	vector< socketFd >& writeList, float timeOut )
{

// don't block
struct timeval		*usetimeout = 0,
			timeout,
			poll = { 0, 0 } ;

if( timeOut >= 0.0 )
	{
	_timeOut = timeOut ;
	timeout.tv_sec = static_cast< long >( ::floor( _timeOut ) ) ;
	timeout.tv_usec = static_cast< long >( ::floor( 1000000 *
		(_timeOut - timeout.tv_sec ) ) ) ;
	usetimeout = &timeout ;
	}
else
	{
	usetimeout = &poll ;
	}

fd_set writefds;
FD_ZERO( &writefds ) ;

for( vector< socketFd >::size_type i = 0 ; i < socFds.size() ; ++i )
	{
	FD_SET( socFds[ i ], &writefds ) ;
	}

int		fdcnt,
		cnt = 10 ;
do
	{
	struct timeval to = *usetimeout ;
	errno = 0 ;
	fdcnt = ::select( FD_SETSIZE, 0, &writefds, 0, &to ) ;
	} while( errno == EINTR && --cnt >= 0 ) ;

if( fdcnt < 0 )
	{
	elog	<< "Socket::writeableList> error occured on select, fdcnt= "
		<< fdcnt << endl ;
	elog	<< "Error: " << strerror( errno ) << endl ;
	return fdcnt ;
	}

if( 0 == fdcnt )
	{
	// No descriptors ready, no big deal.
	return fdcnt ;
	}

for( vector< socketFd >::size_type i = 0 ; i < socFds.size() ; ++i )
	{
	if( FD_ISSET( socFds[ i ], &writefds ) )
		{
		writeList.push_back( socFds[ i ] ) ;
		}
	}
  
return fdcnt ;
}

int Socket::pendingInputList( const vector< socketFd >& socFds, 
	vector< socketFd >& pendingList, float timeOut )
{

struct timeval		*usetimeout = 0,
			timeout,
			poll = { 0, 0 } ; // don't block

if( timeOut >= 0.0 )
	{
	_timeOut = timeOut ;
	timeout.tv_sec = static_cast< long >( floor( _timeOut ) ) ;
	timeout.tv_usec = static_cast< long >( floor( 1000000 *
		( _timeOut - timeout.tv_sec ) ) ) ;
	usetimeout = &timeout ;
	}
else
	{
	usetimeout = &poll ;
	}

fd_set readfds ;
FD_ZERO( &readfds ) ;
 
for( vector< socketFd >::size_type i = 0 ; i < socFds.size() ; ++i )
	{
	FD_SET( socFds[ i ], &readfds ) ;
	}

int		fdcnt,
		cnt= 10 ;

do
	{
	struct timeval to = *usetimeout ;
	errno = 0 ;
	fdcnt = ::select( FD_SETSIZE, &readfds, 0, 0, &to ) ;
	} while( errno == EINTR && --cnt >= 0 ) ;

if( fdcnt < 0 )
	{
	elog	<< "Socket::pendingInputList> error occured on select, fdcnt= "
		<< fdcnt << endl ;
	return fdcnt ;
	}

if( fdcnt == 0 )
	{
	return fdcnt ;
	}

for( vector< socketFd >::size_type i = 0 ; i < socFds.size() ; ++i )
	{
	if( FD_ISSET( socFds[ i ], &readfds ) )
		{
		pendingList.push_back( socFds[ i ] ) ;
		}
	}
  
return fdcnt ;
}

int Socket::readable() const
{

if( _sockinfo.fd < 0 )
	{
	errno = EBADF ;
	return -1 ;
	}

fd_set readfds;
FD_ZERO( &readfds ) ;
FD_SET( _sockinfo.fd, &readfds ) ;
 
struct timeval		*usetimeout = 0,
			timeout,
			poll = { 0, 0 } ; // don't block

if( _timeOut >= 0.0 )
	{
	timeout.tv_sec = static_cast< long >( ::floor( _timeOut ) ) ;
	timeout.tv_usec = static_cast< long >( ::floor( 1000000 *
		( _timeOut - timeout.tv_sec ) ) ) ;
	usetimeout = &timeout ;
	}
else
	{
	usetimeout = &poll ;
	}

int		fdcnt,
		cnt= 10 ;

do
	{
	// since timeval may be modified by call
	struct timeval to = *usetimeout;
	errno = 0 ;
	fdcnt = ::select( 1+_sockinfo.fd, &readfds, 0, 0, &to ) ;
	} while( errno == EINTR && --cnt >= 0 ) ;

if( fdcnt < 0 )
	{
 	//elog << "Socket::readable> error occured on select" << endl;
	return fdcnt;
 	}
if( fdcnt == 0 )
	{
	//elog << "Socket::readable> timed-out, no input pending" << endl;
	return fdcnt;
	}

return FD_ISSET( _sockinfo.fd, &readfds ) ;
}

int Socket::writable() const
{ 
if( _sockinfo.fd < 0 )
	{
	errno = EBADF ;
	return -1 ;
	}

fd_set writefds ;
FD_ZERO( &writefds) ;
FD_SET( _sockinfo.fd, &writefds ) ;

struct timeval	poll = { 0, 0 } ; // don't block
int		fdcnt,
		cnt = 10;
do
	{
	struct timeval to = poll ;
	errno = 0 ;
	fdcnt = ::select( 1 + _sockinfo.fd, 0, &writefds, 0, &to ) ;
	} while( errno == EINTR && --cnt >= 0 ) ;

if( fdcnt < 0 )
	{
	//elog << "Socket::writable> error occured on select" << endl;
	return fdcnt;
	}

if( fdcnt == 0 )
	{
	//elog << "Socket::writable> timed-out, not writable" << endl;
	return fdcnt;
	}

return FD_ISSET( _sockinfo.fd, &writefds ) ;
}

int Socket::pendingIO() const
{
if( _sockinfo.fd < 0 )
	{
	errno = EBADF ;
	return -1 ;
	}

fd_set readfds;
fd_set writefds;
fd_set excptfds;
FD_ZERO( &readfds );
FD_ZERO( &writefds );
FD_ZERO( &excptfds );
FD_SET( _sockinfo.fd, &readfds );
FD_SET( _sockinfo.fd, &writefds );
FD_SET( _sockinfo.fd, &excptfds );

struct timeval	poll = { 0, 0 }; // don't block
int		fdcnt,
		cnt = 10 ;

do
	{
	struct timeval to = poll;
	errno = 0;
	fdcnt = ::select( 1 + _sockinfo.fd, &readfds, &writefds,
		&excptfds, &to ) ;
	} while( (errno == EINTR) && (--cnt >= 0) ) ;

if( fdcnt < 0 )
	{
	//elog << "Socket::pendingIO> error occured on select" << endl;
	return fdcnt;
	}

if( 0 == fdcnt )
	{
	//elog << "Socket::pendingIO> timed-out, no IO pending" << endl;
	return fdcnt;
	}

return FD_ISSET(_sockinfo.fd, &readfds) +
	2 * (FD_ISSET(_sockinfo.fd, &writefds)) +
	4 * (FD_ISSET(_sockinfo.fd, &excptfds)) ;
}

int Socket::send( const unsigned char* buf, int nb )
{
if( _sockinfo.fd < 0 )
	{
	errno = EBADF ;
	return -1 ;
	}

if( nb <= 0 )
	{
	errno = EINVAL ;
	elog << "Socket::send> sorry, no bytes to send; nb= " << nb << endl ;
	return -1 ;
	}

int result = 0 ;
short int cnt = 10 ;
do
	{
	errno = 0 ;
	result = ::send( _sockinfo.fd,
		reinterpret_cast< const char* >( buf ), nb, 0 ) ;
	} while( (--cnt >= 0) && (EINTR == errno) ) ;

return result ;

}

int Socket::send( const char* s )
{
return send( reinterpret_cast< const unsigned char* >( s ), strlen( s ) ) ;
}

// support call by reference
int Socket::send( const string& val )
{
if( _sockinfo.fd < 0 )
	{
	errno = EBADF ;
	return -1 ;
	}

int result = 0 ;
short int cnt = 10 ;
do
	{
	errno = 0 ;
	result = ::send( _sockinfo.fd, val.c_str(), val.size(), 0 ) ;
	} while( (--cnt >= 0) && (EINTR == errno) ) ;

return result ;
}

int Socket::recv( string& readBuf, size_t numBytes )
{
unsigned char* buf = reinterpret_cast< unsigned char* >(
	const_cast< char* >( readBuf.data() ) ) ;

return recv( buf, numBytes ) ;
}

int Socket::recv( unsigned char* buf, size_t nb )
{
if( _sockinfo.fd < 0 )
	{
	errno = EBADF ;
	return -1 ;
	}

if( readable() < 0 )
	{
	elog << "Socket::recv> sorry, no connection for recv, abort." << endl;
	return -1;
	}

int cnt = 10;
int nbresult = 0;

do
	{
	errno = 0 ;
	nbresult = ::recv( _sockinfo.fd, reinterpret_cast< char* >( buf ), nb, 0 ) ;
 	} while( --cnt > 0 && (nbresult < 0) && (errno == EINTR) && (readable() >= 0) ) ;

if( cnt == 0 )
	{
	elog << "Socket::recv> iterations: 10, errno: " << errno << endl ;
	}

return nbresult ;

}

// static
string Socket::hostname()
{
char info[ MAXNAMLEN + 1 ] ;
memset( info,0,sizeof( info ) ) ;
::gethostname( info, sizeof( info ) ) ;

return string( info ) ;
}
