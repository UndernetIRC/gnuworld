/* Socket.cc
 */

#include	<new>
#include	<strstream>
#include	<iostream>

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
#include	<cassert>

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
const char Socket_cc_rcsId[] = "$Id: Socket.cc,v 1.10 2001/01/07 22:59:32 dan_karrels Exp $" ;

using gnuworld::elog ;
using std::endl ;
using std::strstream ;
using std::ends ;

Socket::Socket()
{
memset( &addr, 0, sizeof( struct sockaddr_in ) ) ;
fd = -1 ;
portNum = 0 ;
}

Socket::Socket( const Socket& rhs )
 : fd( rhs.fd ),
   portNum( rhs.portNum )
{
memcpy( &addr, &rhs.addr, sizeof( struct sockaddr_in ) ) ;
}

int Socket::close()
{
memset( &addr, 0, sizeof( struct sockaddr_in ) ) ;
portNum = 0 ;

int retVal = ::close( fd ) ;
fd = -1 ;
return retVal ;
}

int Socket::recvBufSize() const
{
int recvSize = 0 ;

socklen_t len = static_cast< socklen_t >( sizeof( recvSize ) ) ;

if( ::getsockopt( fd, SOL_SOCKET, SO_RCVBUF,
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

if( ::getsockopt( fd, SOL_SOCKET, SO_SNDBUF,
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
	if( fd < 0 )
		{
		errno = EBADF ;
		return -1 ;
		}

	int bytes = 0 ;
	if( ::ioctl( fd, FIONREAD, &bytes ) < 0 )
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
	return string() ;
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
std::strstream s;
s	<< "socket(file descr. = " << fd
	<< " ): " << "portNum = "
	<< addr.sin_port
	<< " ip= " << inet_ntoa( addr.sin_addr )
	<< std::ends ;
string retval( s.str() ) ;
delete[] s.str() ;

return retval ;
}

bool Socket::setSocket()
{
if( fd != -1 )
	{
	// Socket is already open, ignore
	elog	<< "Socket::setSocket> Attempt to reinitialize "
		<< "socket" << endl ;
	return false ;
	}

fd = ::socket( AF_INET, SOCK_STREAM, 0 ) ;
if( fd < 0 )
	{
	elog	<< "Socket::setSocket> Unable to open new socket"
		<< endl ;
	return false ;
	}

// zero out the address structure
memset( static_cast< void* >( &addr ), 0,
	sizeof( sockaddr_in ) ) ;

// setup with basic options
addr.sin_family = AF_INET ;
addr.sin_port = htons( static_cast< u_short >( portNum ) ) ;

// disabled linger
struct linger setLinger;
setLinger.l_onoff = 0; // don't linger!     
setLinger.l_linger = 0;
    
if( ::setsockopt( fd, SOL_SOCKET, SO_LINGER,
	reinterpret_cast< const char* >( &setLinger ),
	sizeof( setLinger ) ) < 0 )
	{
	elog	<< "Socket::setSocket> failed to set SO_LINGER"
		<< endl ;
	elog	<< "Error: " << strerror( errno ) << endl ;
	}

int optval = 1 ;

// detect closed connection
if( ::setsockopt( fd, SOL_SOCKET, SO_KEEPALIVE,
	reinterpret_cast< const char* >( &optval ),
	sizeof( optval ) ) < 0 )
	{
	elog	<< "Socket::setSocket> failed to set SO_KEEPALIVE"
		<< endl ;
	elog	<< "Error: " << strerror( errno ) << endl ;
	}

// allow fast re-binds
if( ::setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
	reinterpret_cast< const char* >( &optval ), sizeof( optval ) ) < 0 )
	{
	elog	<< "Socket::setSocket> failed to set SO_REUSEADDR"
		<< endl ;
	elog	<< "Error: " << strerror( errno ) << endl ;
	}

// use max. buffer size
optval = 1024 * 1024 ;

if( ::setsockopt( fd, SOL_SOCKET, SO_SNDBUF,
	reinterpret_cast< const char* >( &optval ), sizeof( optval ) ) < 0 )
	{
	elog	<< "Socket::setSocket> failed to set SO_SNDBUF"
		<< endl ;
	elog	<< "Error: " << strerror( errno ) << endl ;
	}

if( ::setsockopt( fd, SOL_SOCKET, SO_RCVBUF,
	reinterpret_cast< const char* >( &optval ), sizeof( optval ) ) < 0 )
	{
	elog	<< "Socket::setSocket> failed to set SO_RCVBUF"
		<< endl ;
	elog	<< "Error: " << strerror( errno ) << endl ;
	}

// explicitly set to blocking
optval = ::fcntl( fd, F_GETFL, 0 ) ;
if( optval < 0 )
	{
	elog	<< "Socket::setSocket> failed to get sock flags"
		<< endl ;
	elog	<< "Error: " << strerror( errno ) << endl ;
	}

optval = ::fcntl( fd, F_SETFL, optval & ~O_NONBLOCK ) ;
if( optval < 0 )
	{
	elog	<< "Socket::setSocket> failed to set O_NONBLOCK"
		<< endl;
	elog	<< "Error: " << strerror( errno ) << endl ;
	}

return true ;
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

int Socket::readable() const
{

if( fd < 0 )
	{
	errno = EBADF ;
	return -1 ;
	}

fd_set readfds;
FD_ZERO( &readfds ) ;
FD_SET( fd, &readfds ) ;

struct timeval		poll = { 0, 0 },
			*usetimeout = &poll ;

int			fdcnt = -1,
			cnt = 10 ;

do
	{
	// since timeval may be modified by call
	struct timeval to = *usetimeout;
	errno = 0 ;
	fdcnt = ::select( 1 + fd, &readfds, 0, 0, &to ) ;
	} while( (errno == EINTR) && (--cnt >= 0) ) ;

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

return FD_ISSET( fd, &readfds ) ;
}

int Socket::writable() const
{ 
if( fd < 0 )
	{
	errno = EBADF ;
	return -1 ;
	}

fd_set writefds ;
FD_ZERO( &writefds) ;
FD_SET( fd, &writefds ) ;

struct timeval	poll = { 0, 0 } ; // don't block
int		fdcnt,
		cnt = 10;
do
	{
	struct timeval to = poll ;
	errno = 0 ;
	fdcnt = ::select( 1 + fd, 0, &writefds, 0, &to ) ;
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

return FD_ISSET( fd, &writefds ) ;
}

int Socket::pendingIO() const
{
if( fd < 0 )
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
FD_SET( fd, &readfds );
FD_SET( fd, &writefds );
FD_SET( fd, &excptfds );

struct timeval	poll = { 0, 0 }; // don't block
int		fdcnt,
		cnt = 10 ;

do
	{
	struct timeval to = poll;
	errno = 0;
	fdcnt = ::select( 1 + fd, &readfds, &writefds,
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

return FD_ISSET( fd, &readfds ) +
	2 * ( FD_ISSET( fd, &writefds ) ) +
	4 * ( FD_ISSET( fd, &excptfds ) ) ;
}

int Socket::send( const unsigned char* buf, size_t nb )
{
#ifndef NDEBUG
  assert( buf != 0 ) ;
#endif

if( fd < 0 )
	{
	errno = EBADF ;
	return -1 ;
	}

int result = 0 ;
short int cnt = 10 ;
do
	{
	errno = 0 ;
	result = ::send( fd,
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
if( fd < 0 )
	{
	errno = EBADF ;
	return -1 ;
	}

int result = 0 ;
short int cnt = 10 ;
do
	{
	errno = 0 ;
	result = ::send( fd, val.c_str(), val.size(), 0 ) ;
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
#ifndef NDEBUG
  assert( buf != 0 ) ;
#endif 

if( fd < 0 )
	{
	errno = EBADF ;
	return -1 ;
	}

short int cnt = 10;
int nbresult = 0;

do
	{
	errno = 0 ;
	nbresult = ::recv( fd, reinterpret_cast< char* >( buf ),
		nb, 0 ) ;
 	} while( (--cnt > 0) && (errno == EINTR) ) ;

return nbresult ;
}

// static
string Socket::hostname()
{
char info[ MAXNAMLEN + 1 ] = { 0 } ;
::gethostname( info, sizeof( info ) ) ;

return string( info ) ;
}
