
/* misc.h
 * This file contains a few miscellaneous methods.
 */

#ifndef __MISC_H
#define __MISC_H "$Id: misc.h,v 1.6 2000/10/31 00:58:37 dan_karrels Exp $"

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

/**
 * A case insensitive binary predicate comparator for two
 * std::string's.
 */
struct eqstr
{
inline bool operator()( const string& s1, const string& s2 ) const
	{
	return (0 == ::strcasecmp( s1.c_str(), s2.c_str() )) ;
	}
} ;

/**
 * A hashing operator for the system hash tables.
 */
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

#endif /* __MISC__ */
