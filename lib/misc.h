
/* misc.h
 * This file contains a few miscellaneous methods.
 */

#ifndef __MISC_H
#define __MISC_H "$Id: misc.h,v 1.3 2000/07/06 20:47:06 dan_karrels Exp $"

#include	<string>
#include	<hash_map>

#include	<unistd.h>

#include	<cstring>
#include	<cctype>

using std::string ;
using std::hash ;

/**
 * Case insensitive comparison struct for use by STL structures/algorithms.
 */
struct noCaseCompare
{
inline bool operator()( const string& lhs, const string& rhs ) const
	{
	return (::strcasecmp( lhs.c_str(), rhs.c_str() ) < 0) ;
	}
} ;

struct eqstr
{
inline bool operator()( const string& s1, const string& s2 ) const
	{
	return (0 == ::strcasecmp( s1.c_str(), s2.c_str() )) ;
	}
} ;

struct eHash
{
inline size_t operator()( const string& s ) const
	{
	register size_t __h = 0 ;
	for ( register const char* ptr = s.c_str() ; *ptr ; ++ptr )
		{
		__h = (5 * __h) + tolower( *ptr ) ;
		}
	return __h ;
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
