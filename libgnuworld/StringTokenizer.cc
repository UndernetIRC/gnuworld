/**
 * StringTokenizer.cpp
 * Author: Daniel Karrels dan@karrels.com
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
 * $Id: StringTokenizer.cc,v 1.7 2005/02/20 15:49:21 dan_karrels Exp $
 */

#include	<new>
#include	<vector>
#include	<string>

#include	<cassert>
#include	"StringTokenizer.h"

const char rcsId[] = "$Id: StringTokenizer.cc,v 1.7 2005/02/20 15:49:21 dan_karrels Exp $" ;

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
 : delimiter( _delimiter )
{
Tokenize( buf ) ;
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
// This method is only called once for the life of this object,
// so no need for memory of any kind here.
char*	addMe = new (std::nothrow) char[ buf.size() + 1 ] ;
assert( addMe != 0 ) ;

addMe[ 0 ] = 0 ;

// (addMePtr) is used to walk down the addMe buffer, adding new
// characters to the end.
// (addMePtr) always points to the next location in the addMe ptr
// in which a character may be placed.
char*	addMePtr = addMe ;

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
		// Null terminate the token
		*addMePtr = 0 ;

		// Is this an empty token?
		if( addMe[ 0 ] != 0 )
			{
			// Nope, go ahead and add it to the vector
			array.push_back( addMe ) ;

			addMePtr = addMe ;
			*addMePtr = 0 ;
			}
		}

	// Otherwise, no delimiter was found...just add this
	// character to addMe
	else
		{
		*addMePtr = *currentPtr ;
		++addMePtr ;
		}
	}

// Null terminate, if necessary
*addMePtr = 0 ;

// currentPtr == endPtr
// Make sure to check for last token
if( addMe[ 0 ] != 0 )
	{
	array.push_back( addMe ) ;
	}

// This point is always reached, excluding a crash
delete[] addMe ;
}

/**
 * assemble()
 * Assemble into a string all tokens starting from index
 * (start).
 */
string StringTokenizer::assemble( const size_type& start, int end ) const
{

// check if the beginning index is valid
if( !validSubscript( start ) )
	{
	return string() ;
	}

if(end < 0) 
	{
        end = size();
	} 
else if(!validSubscript(end-1)) 
	{
        return string();
	}	

// retMe will be returned at the end of the method
string retMe ;
retMe.reserve( totalChars() ) ;

// continue while there are more tokens to concatenate
for( size_type i = start ; i < end ; i++ )
	{
	// Add this token to the returning string
	retMe += array[ i ] ;

	// If there is another token, put a delimiter here
	if( (i + 1) < end )
		{
		retMe += delimiter ;
		}

	} // close for

// Return the assembled string
return retMe ;
}

StringTokenizer::size_type StringTokenizer::totalChars() const
{
size_type numChars = 0 ;

for( const_iterator currentPtr = begin() ; currentPtr != end() ;
	++currentPtr )
	{
	numChars += (*currentPtr).size() ;
	}

// Account for delimiters
numChars += size() - 1 ;

return numChars ;
}

} // namespace gnuworld
