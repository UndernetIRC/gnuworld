/* FileSocket.cc
 */

#include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<unistd.h>

#include	<string>
#include	<iostream>

#include	<cstdlib>
#include	<cstring>
#include	<cerrno>

#include	"FileSocket.h"
#include	"ELog.h"

const char FileSocket_h_rcsId[] = __FILESOCKET_H ;
const char FileSocket_cc_rcsId[] = "$Id: FileSocket.cc,v 1.6 2001/07/29 22:44:06 dan_karrels Exp $" ;
const char ELog_h_rcsId[] = __ELOG_H ;

namespace gnuworld
{

using std::string ;
using std::endl ;

FileSocket::FileSocket( const string& fileName )
{
fd = ::open( fileName.c_str(), O_RDWR ) ;
if( fd < 0 )
	{
	elog	<< "FileSocket> Error opening input file "
		<< fileName << ": "
		<< strerror( errno )
		<< endl ;
	::exit( 0 ) ;
	}
}

FileSocket::~FileSocket()
{
close() ;
}

int FileSocket::close()
{
int retMe = ::close( fd ) ;
fd = -1 ;
return retMe ;
}

int FileSocket::connect( const string& uplink,
	unsigned short int port )
{
return fd ;
}

int FileSocket::send( const string& theString, const size_t numBytes )
{
//elog << "send( const string& ): " << theString ;
return numBytes ;
}

int FileSocket::recv( unsigned char* buf, size_t nb )
{
assert( buf != 0 ) ;

if( fd < 0 )
	{
	return fd ;
	}

int readReturn = ::read( fd, buf, nb ) ;
if( readReturn < 0 )
	{
	// EOF
	fd = -1 ;
	return readReturn ;
	}

// Need to append a \n here?
// Sure, let's give it a try.
buf[ readReturn++ ] = '\n' ;
buf[ readReturn ] = 0 ;

return readReturn ;
}

} // namespace gnuworld
