/* FileSocket.cc
 */

#include	<string>
#include	<fstream>
#include	<iostream>
#include	<vector>

#include	<cstdlib>

#include	"FileSocket.h"
#include	"ELog.h"

const char FileSocket_h_rcsId[] = __FILESOCKET_H ;
const char FileSocket_cc_rcsId[] = "$Id: FileSocket.cc,v 1.2 2000/07/09 18:08:11 dan_karrels Exp $" ;

using std::string ;
using std::ifstream ;
using std::vector ;
using gnuworld::elog ;

FileSocket::FileSocket( const string& fileName )
{
inFile.open( fileName.c_str() ) ;
if( !inFile.is_open() )
	{
	elog	<< "FileSocket> Unable to open socket input file: "
		<< fileName << endl ;
	exit( 0 ) ;
	}
string line ;
while( (theFile.size() <= maxLines) && getline( inFile, line ) )
	{
	theFile.push( line ) ;
	}
}

FileSocket::~FileSocket()
{
inFile.close() ;
}

int FileSocket::close()
{
return 0 ;
}

Socket::socketFd FileSocket::connect( const string& uplink,
	int port )
{
return theFile.size() ;
}

int FileSocket::available() const
{
return theFile.size() ;
}

int FileSocket::readable() const
{
if( theFile.empty() )
	{
	return -1 ;
	}
return theFile.front().size() ;
}

int FileSocket::writable() const
{
// Always writable
return 100 ;
}

int FileSocket::send( const string& theString )
{
//elog << "send( const string& ): " << theString ;
return theString.size() ;
}

int FileSocket::recv( unsigned char* buf, int nb )
{
if( theFile.empty() )
	{
	return -1 ;
	}

string line ;

if( !inFile.eof() )
	{
	getline( inFile, line ) ;
	theFile.push( line ) ;
	}

// Get a line from the input buffer
line = theFile.front() ;

// Remove the line from the input buffer
theFile.pop() ;

// Append a new line terminator
line += '\n' ;

// Copy this line into the return buffer.
// This could possibly overflow the buffer, but oh well.
strcpy( reinterpret_cast< char* >( buf ), line.c_str() ) ;

//elog	<< "FileSocket::recv> Returning: " << line ;

return line.size() ;

}
