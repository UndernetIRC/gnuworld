/* misc.cc
 */

#include	<string>
#include	<cctype>
#include	<cstdio>
#include	<cstdlib>
#include	<cstdarg>

#include	"misc.h"

const char Misc_h_rcsId[] = __MISC_H ;
const char Misc_cc_rcsId[] = "$Id: misc.cc,v 1.3 2000/11/18 23:36:51 dan_karrels Exp $" ;

using std::string ;

/**
 * Create a string and copy into it (Key), but convert to all
 * lower case.
 */
string string_lower( const string& Key )
{
// This variable will be the string to be returned
// Make a copy of the original string to speed up
// allocation.
string retMe( Key ) ;

// Iterate through the new string, converting each character
// to lower case
for( string::size_type i = 0, end = retMe.size() ; i < end ; ++i )
	{
	retMe[ i ] = tolower( retMe[ i ] ) ;
	}

// Return by value the new string (this will create a copy of
// the new string)
return retMe ;
}

/**
 * Create a string and copy into it (Key), but convert to all
 * upper case.
 */
string string_upper( const string& Key )
{

// This variable will be the string to be returned
// Make a copy of the original string to speed up
// allocation.
string retMe( Key ) ;

// Iterate through the new string, converting each character
// to upper case
for( string::size_type i = 0, end = retMe.size() ; i < end ; i++ )
	{
	retMe[ i ] = toupper( retMe[ i ] ) ;
	}

// Return by value the new string (this will create a copy of
// the new string)
return retMe ;
}

/**
 * Convert the given string to all lowercase.  This method
 * mutates the original string, yet is much faster than the
 * equivalent method which receives the string by const
 * reference.
 */
void string_tolower( string& Key )
{
for( string::size_type i = 0, end = Key.size() ; i < end ; i++ )
	{
	Key[ i ] = tolower( Key[ i ] ) ;
	}
}

/**
 * Convert the given string to all uppercase.  This method
 * mutates the original string, yet is much faster than the
 * equivalent method which receives the string by const
 * reference.
 */
void string_toupper( string& Key )
{
for( string::size_type i = 0, end = Key.size() ; i < end ; i++ )
	{
	Key[ i ] = toupper( Key[ i ] ) ;
	}
}

/**
 * Return true if this string consists of all numerical
 * [0,9] characters.
 * Return false otherwise.
 */
bool IsNumeric( const string& s )
{
for( string::const_iterator ptr = s.begin(), endPtr = s.end() ;
	ptr != endPtr ; ++ptr )
	{
	if( !isdigit( *ptr ) )
		{
		return false ;
		}
	}
return true ;
}       
