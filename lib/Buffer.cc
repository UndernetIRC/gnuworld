/* Buffer.cpp
 * Author: Daniel Karrels dan@karrels.com
 * History: Mon Jan 25 23:29:47 EST 1999 -- beginning
 */

#include	<string>

#include	"Buffer.h"

const char Buffer_h_rcsId[] = __BUFFER_H ;
const char Buffer_cc_rcsId[] = "$Id: Buffer.cc,v 1.4 2001/01/06 19:46:35 dan_karrels Exp $" ;

using std::basic_string ;

template< class charType >
Buffer< charType >::Buffer( const basic_string< charType >& aString,
	charType delimiter )
	: buf( aString ), delim( delimiter )
{}

template< class charType >
Buffer< charType >::~Buffer()
{ /* No heap space allocated */ }

template< class charType >
bool Buffer< charType >::ReadLine( basic_string< charType >& retMe )
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
basic_string< charType >::size_type pos = buf.find( delim ) ;

if( basic_string< charType >::npos == pos )
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
template< class charType >
void Buffer< charType >::Delete( const size_type& numBytes )
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
