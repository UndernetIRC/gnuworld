
/* misc.h
 * This file contains a few miscellaneous methods.
 */

#ifndef __MISC_H
#define __MISC_H "$Id: misc.h,v 1.2 2000/07/06 19:13:07 dan_karrels Exp $"

#include	<cstring>
#include	<unistd.h>

#include	<string>
#include	<cstring>

using std::string ;

/**
 * Case insensitive comparison struct for use by STL structures/algorithms.
 */
struct noCaseCompare
{
inline bool operator()( const string& lhs, const string& rhs ) const
	{
	return (strcasecmp( lhs.c_str(), rhs.c_str() ) < 0) ;
	}
} ;

struct eqstr
{
inline bool operator()( const char* s1, const char* s2 ) const
	{
	return (0 == ::strcasecmp( s1, s2 )) ;
	}
} ;

/**
 * Return a copy of a given C++ string, whose characters
 * are all lower case.
 */
string string_lower( const string& ) ;

/**
 * Return a copy of a given C++ string, whose
 * characters are all upper case.
 */
string string_upper( const string& ) ;

/**
 * Convert all characters of a given C++ string to
 * lower case.
 */
void string_tolower( string& ) ;

/**
 * Examine a given C++ string and return true if it contains
 * all numeric characters, return false otherwise.
 */
bool IsNumeric( const string& ) ;

/**
 * Return a pointer to a const array of space characters.
 */
const char* Spaces( int ) ;

/// A type used for MD5.	
typedef int long128[4];

#endif /* __MISC__ */
