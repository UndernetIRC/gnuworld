/*
 * FileSocket.cc
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
 * $Id: FileSocket.cc,v 1.7 2002/05/27 17:18:12 dan_karrels Exp $
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
const char FileSocket_cc_rcsId[] = "$Id: FileSocket.cc,v 1.7 2002/05/27 17:18:12 dan_karrels Exp $" ;
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
