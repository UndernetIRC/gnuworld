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
 * $Id: misc.h,v 1.3 2003/06/30 14:49:59 dan_karrels Exp $
 */

#ifndef __MISC_H
#define __MISC_H "$Id: misc.h,v 1.3 2003/06/30 14:49:59 dan_karrels Exp $"

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

/**
 * Count the number of occurences of the given character in the
 * given string.
 */
size_t countChar( const string&, const char& ) ;

/**
 * This specialization of the char_traits class is used for
 * basic_string< unsigned char >.
 */
struct My_unsigned_char_traits
{
   typedef unsigned char char_type;
   typedef int         int_type;
   typedef std::streamoff   off_type;
   typedef std::streampos   pos_type;
   typedef mbstate_t   state_type;

   static void assign(char_type& c1, const char_type& c2);
//	{  __c1 = __c2; }
   static bool eq(const char_type& c1, const char_type& c2);
//	{ return __c1 == __c2; }
   static bool lt(const char_type& c1, const char_type& c2);
//	{ return __c1 < __c2; }

   static int compare(const char_type* s1, const char_type* s2, size_t n);
//	{ return memcmp(__s1, __s2, __n); }
   static size_t length(const char_type* s);
//	{ return strlen(__s); }
   static const char_type* find(const char_type* s, size_t n, const
		char_type& a);
   static char_type* move(char_type* s1, const char_type* s2, size_t n)
	{ return static_cast<char_type*>(memmove(s1, s2, n)); }
   static char_type* copy(char_type* s1, const char_type* s2, size_t n)
	{ return static_cast<char_type*>(memcpy(s1, s2, n)); }
   static char_type* assign(char_type* s, size_t n, char_type a);

   static int_type not_eof(const int_type& c);
   static char_type to_char_type(const int_type& c);
   static int_type to_int_type(const char_type& c);
   static bool eq_int_type(const int_type& c1, const int_type& c2);
   static int_type eof();
};

typedef std::basic_string< unsigned char, My_unsigned_char_traits >
	binary_string ;

} // namespace gnuworld

#endif /* __MISC__ */
