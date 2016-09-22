/**
 * misc.h
 * Author: Daniel Karrels (dan@karrels.com
 * Purpose: This file contains a few miscellaneous methods.
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
 * $Id: misc.h,v 1.9 2007/12/27 20:45:15 kewlio Exp $
 */

#ifndef __MISC_H
#define __MISC_H "$Id: misc.h,v 1.9 2007/12/27 20:45:15 kewlio Exp $"

#include	<string>
#include	<iostream>

#include	<cctype>
#include	<cstring>
#include	<cstdlib>

#include	"match.h"

namespace gnuworld
{

using std::string ;
/**
 * Return 0 if the two strings are equivalent, according to
 * case insensitive searches.
 * Otherwise, it returns the comparison between
 * s1 and s2.
 */
int strcasecmp( const string&, const string& ) ;

/**
 * Case insensitive comparison struct for use by STL structures/algorithms.
 */
struct noCaseCompare
{
inline bool operator()( const string& lhs, const string& rhs ) const
	{
	return (strcasecmp( lhs, rhs ) < 0) ;
	}
} ;

/**
 * A case insensitive binary predicate comparator for two
 * string's.
 */
struct eqstr
{
inline bool operator()( const string& s1, const string& s2 ) const
	{
	return (0 == strcasecmp( s1, s2 )) ;
	}
} ;

/**
 * A hashing operator for the system hash tables.
 * This is not used for now, since hash_map has been removed
 * from gnuworld completely.
 */
struct eHash
{
inline size_t operator()( const string& s ) const
	{
	if( s.empty() )
		{
		return 0 ;
		}

	register size_t __h = 0 ;
	for ( register const char* ptr = s.c_str() ; *ptr ; ++ptr )
		{
		__h = (5 * __h) + tolower( *ptr ) ;
		}
	return __h ;
	}
} ;

/**
 * A functor suitable for using in STL style containers which provides
 * wildcard matching routine.
 */
struct Match
{
inline bool operator()( const string& lhs, const string& rhs ) const
	{
	return (match( lhs, rhs ) < 0) ;
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
 * Convert all characters of a given C++ string to
 * upper case.
 */
void string_toupper( string& ) ;

/**
 * Examine a given C++ string and return true if it contains
 * a time specification, return false otherwise.
 */
bool IsTimeSpec( const string& ) ;

/**
 * Examine a given C++ string and return true if it contains
 * all numeric characters, return false otherwise.
 */
bool IsNumeric( const string& ) ;

/**
 * Returns the time which is given as #<d/h/m/s> as seconds
 */
time_t extractTime( string Length, unsigned int defaultUnits ) ;

int atoi( const string& ) ;

/* itoa:  convert n to characters in s */
string itoa(int n);

/**
 * Extract the parts of a *valid* nick!user@hostip address
 */
string extractNick( const string& ) ;

string extractUser( const string& ) ;

string extractNickUser( const string& ) ;

string extractHostIP( const string& ) ;

// Check for valid hostmask.
bool validUserMask(const string& );

bool validCIDRLength(const string& );

string fixAddress( const string& );

//Check if we have !at least! a *@hostip format
bool isUserHost( const string& );

/* Truncate a > /64 IPv6 address to a /64 cidr address
 * or creates a between /32 - /64 valid cidr address
 */
unsigned char fixToCIDR64(string& );

//Same as above, just returns the fixed address. Easier to use many times
string fixToCIDR64(const string& );

//Constructs a 'generally valid' banmask for a [nick!]user@hostip address
string createBanMask(const string& );

/* Create a 24/64 cidr address (optionally wildcarded)
 * for an IPv4 or IPv6 address, or for a hostname
 * If the parameter is a user@host it will be returned correspondingly
 */
string createClass(const string&, bool wildcard = false);

/* Formats a timestamp into a "X Days, XX:XX:XX" from 'Now'. */
const string prettyDuration( int ) ;

/* General assemble parameters into one result string */
const string TokenStringsParams(const char*, ...);

} // namespace gnuworld

#endif /* __MISC__ */
