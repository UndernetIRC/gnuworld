/**
 * misc.cc
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
 * $Id: misc.cc,v 1.4 2007/03/16 12:07:51 mrbean_ Exp $
 */

#include	<string>
#include	<algorithm>

#include	<cctype>
#include	<cstdio>
#include	<cstdlib>
#include	<cstdarg>
#include	<cstring>

#include	"misc.h"

const char rcsId[] = "$Id: misc.cc,v 1.4 2007/03/16 12:07:51 mrbean_ Exp $" ;

namespace gnuworld
{

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
 * Return false otherwise. */
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

int strcasecmp( const string& s1, const string& s2 )
{
return ::strcasecmp( s1.c_str(), s2.c_str() ) ;
}

/**
 * Returns the time which is given as #<d/h/m/s> as seconds */

time_t extractTime( string Length )
{
unsigned int Units = 1;         // Default for seconds

if (!strcasecmp(Length.substr(Length.length()-1).c_str(),"d"))
	{
	Units = (24*3600);
	Length.resize(Length.length()-1);
	}
else if(!strcasecmp(Length.substr(Length.length()-1).c_str(),"h"))
        {
        Units = 3600;
        Length.resize(Length.length()-1);
        }
else if(!strcasecmp(Length.substr(Length.length()-1).c_str(),"m"))
        {
        Units = 60;
        Length.resize(Length.length()-1);
        }
else if(!strcasecmp(Length.substr(Length.length()-1).c_str(),"s"))
        {
        Units = 1;
        Length.resize(Length.length()-1);
        }

return atoi(Length.c_str()) * Units;
}
} // namespace gnuworld
