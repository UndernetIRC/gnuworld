/* StringTokenizer.cpp
 * Author: Daniel Karrels dan@karrels.com
 */

#include	<vector>
#include	<string>

#include	<cassert>

#include	"config.h"
#include	"StringTokenizer.h"

const char StringTokenizer_h_rcsId[] = __STRINGTOKENIZER_H ;
const char StringTokenizer_cc_rcsId[] = "$Id: StringTokenizer.cc,v 1.4 2001/03/31 01:26:10 dan_karrels Exp $" ;
const char config_h_rcsId[] = __CONFIG_H ;

namespace gnuworld
{

using std::string ;
using std::vector ;

/**
 * StringTokenizer()
 * Arguments --
 *  buf: the string which we will be tokenizing
 *  delimiter: The specified delimiter by which
 *   we will tokenize the string
 */
StringTokenizer::StringTokenizer( const string& buf, char _delimiter )
  : original( buf ),
    delimiter( _delimiter )
{
Tokenize( original ) ;
}

StringTokenizer::~StringTokenizer()
{ /* No heap space allocated */ }

/**
 * getToken()
 * Return an individual token to the calling
 * method.
 * This method will deliberately drop core in debug
 * mode if sub is out of range.
 */
const string& StringTokenizer::getToken( const size_type& sub ) const
{
// Dump core
assert( validSubscript( sub ) ) ;
return array[ sub ] ;
}

/**
 * Tokenize()
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

// addMe is the string which will be added to the vector
// Initialize it to buf.size() empty characters to speed
// up addition of characters onto the string
string addMe ;

// currentPtr is the current character pointer
string::const_iterator currentPtr = buf.begin() ;

// endPtr is the end of the string, currentPtr is valid on
// [buf.begin(), buf.end())
string::const_iterator endPtr = buf.end() ;

// Iterate through the entire string
for( ; currentPtr != endPtr ; ++currentPtr )
	{

	// Is this the delimiter for which we are searching?
	if( delimiter == *currentPtr )
		{
		// We have reached a delimiter

		// Is this an empty token?
		if( !addMe.empty() )
			{
			// Nope, go ahead and add it to the vector
			array.push_back( addMe ) ;

			// Clear the token string
			// Silly GNU std::string class doesn't implement
			// clear() method
			addMe.erase( addMe.begin(), addMe.end() ) ;
			}
		}

	// Otherwise, no delimiter was found...just add this
	// character to addMe
	else
		{
		addMe += *currentPtr ;
		}
	}

// currentPtr == endPtr
// Make sure to check for last token
if( !addMe.empty() )
	{
	array.push_back( addMe ) ;
	}
}

/**
 * assemble()
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

// retMe will be returned at the end of the method
string retMe ;

// continue while there are more tokens to concatenate
for( size_type i = start ; i < size() ; i++ )
	{
	// Add this token to the returning string
	retMe += array[ i ] ;

	// If there is another token, put a delimiter here
	if( (i + 1) < size() )
		{
		retMe += delimiter ;
		}

	} // close for

// Return the assembled string
return retMe ;
}

} // namespace gnuworld
