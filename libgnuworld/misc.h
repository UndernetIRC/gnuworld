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
 * $Id: misc.h,v 1.5 2003/07/22 21:36:48 dan_karrels Exp $
 */

#ifndef __MISC_H
#define __MISC_H "$Id: misc.h,v 1.5 2003/07/22 21:36:48 dan_karrels Exp $"

#include	<string>
#include	<iostream>

#include	<cctype>
#include	<cstring>

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
 * std::string's.
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
 * all numeric characters, return false otherwise.
 */
bool IsNumeric( const string& ) ;

} // namespace gnuworld

#endif /* __MISC__ */
