/* StringTokenizer.cpp
 * Author: Daniel Karrels dan@karrels.com
 */

#include	<vector>
#include	<string>
#include	<stdexcept>
#include	<iostream>

#include	<cassert>

#include	"config.h"
#include	"StringTokenizer.h"

const char StringTokenizer_h_rcsId[] = __STRINGTOKENIZER_H ;
const char StringTokenizer_cc_rcsId[] = "$Id: StringTokenizer.cc,v 1.1 2000/06/30 18:46:06 dan_karrels Exp $" ;

using std::string ;
using std::vector ;

/* StringTokenizer()
 * Arguments --
 *  buf: the string which we will be tokenizing
 *  delimiter: The specified delimiter by which
 *   we will tokenize the string
 */
StringTokenizer::StringTokenizer( const string& buf, char _delimiter )
	: original( buf ), delimiter( _delimiter )
{
Tokenize( original ) ;
}

StringTokenizer::~StringTokenizer()
{ /* No heap space allocated */ }

/* getToken()
 * Return an individual token to the calling
 * method.
 * Throws an exception if the requested index
 * is out of range
 */
const string& StringTokenizer::getToken( const size_type& sub ) const
{
#ifdef DEBUG
	// Dump core
	assert( validSubscript( sub ) ) ;
#endif
return array[ sub ] ;
}

/* Tokenize()
 * Break the string into elements delimited
 * by the given delimiter.
 * Fill the array with individual tokens.
 * Rewritten on 5/09/00, much faster now, fewer
 *  function calls, and more linear algorithm
 *  complexity (though still not quite linear).
 */
void StringTokenizer::Tokenize( const string& buf )
{

// Make sure that there is something
// worth tokenizing
if( buf.empty() )
	{
	return ;
	}

string addMe ;
string::const_iterator currentPtr = buf.begin() ;
string::const_iterator endPtr = buf.end() ;

for( ; currentPtr != endPtr ; ++currentPtr )
	{
	if( delimiter == *currentPtr )
		{
		if( !addMe.empty() )
			{
			array.push_back( addMe ) ;

			// Silly GNU std::string class doesn't implement
			// clear() method
			addMe.erase( addMe.begin(), addMe.end() ) ;
			}
		}
	else
		{
		addMe += *currentPtr ;
		}
	}

// currentPtr == endPtr
if( !addMe.empty() )
	{
	array.push_back( addMe ) ;
	}

}

/* assemble()
 * Assemble into a string all tokens starting from index
 * start
 */
string StringTokenizer::assemble( const size_type& start ) const
{

// check if the beginning index is valid
if( !validSubscript( start ) )
	{
	return string() ;
	}

string retMe ;

// continue while there are more tokens to concatenate
for( size_type i = start ; i < size() ; i++ )
	{
	retMe += array[ i ] ;

	// If there is another token, put a delimiter here
	if( (i + 1) < size() )
		{
		retMe += delimiter ;
		}

	} // close for

return retMe ;

}
