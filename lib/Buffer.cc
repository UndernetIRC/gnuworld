/* Buffer.cpp
 * Author: Daniel Karrels dan@karrels.com
 * History: Mon Jan 25 23:29:47 EST 1999 -- beginning
 */

#include	<string>

#include	"Buffer.h"

const char Buffer_h_rcsId[] = __BUFFER_H ;
const char Buffer_cc_rcsId[] = "$Id: Buffer.cc,v 1.6 2001/08/18 15:03:20 dan_karrels Exp $" ;

namespace gnuworld
{

using std::string ;

Buffer::Buffer( const string& aString, char delimiter )
	: buf( aString ), delim( delimiter )
{}

Buffer::~Buffer()
{ /* No heap space allocated */ }

bool Buffer::ReadLine( string& retMe )
{

/* remove any leading new line characters */
while( !empty() && ('\n' == buf[ 0 ]) )
	{
	/* just remove the first char */
	buf.erase( buf.begin() ) ;
	}

/* Check to see if there is anything left */
if( empty() )
	{
	return false ;
	}

/* find() returns the index of the character, or 
 * npos if it was not found
 */
size_type pos = buf.find( delim ) ;

if( string::npos == pos )
	{
	// Unable to find the delimiter
	return false ;
	}

// else I need to add this substring into retMe
// and remove it from buf
retMe = buf.substr( 0, pos + 1 ) ;
buf.erase( 0, pos + 1 ) ;

return true ;

}

// Delete numBytes bytes from the beginning
// of the Buffer
void Buffer::Delete( const size_type& numBytes )
{
if( numBytes >= size() )
	{
	// Clear the entire Buffer
	// GNU's string implementation doesn't include clear() for some
	// reason :(
	buf.erase( buf.begin(), buf.end() ) ;
	return ;
	}

// Else just erase the number of bytes given
buf.erase( 0, numBytes ) ;
}

} // namespace gnuworld
