/* misc.cc
 */

/////////////////////////////////////////////////////////////////////////////
//
// This code was written by Orlando Bassotto.
//
// Miscellaneous functions
//
// History:
//	17 Dec 96 - Started
//	17 Dec 96 - Implemented 'strupr' for unix,
//		    a raw trimmer of strings is implemented. 
//	08 Feb 97 - Corrected the include for unistd.h.
//		    Removed my own 'stricmp' and 'strnicmp',
//		    now are defines.
//	09 Feb 97 - Removed unistd.h in the file and included
//		    in misc.h if the system is unixish.
//      16 Jun 97 - Installed autoconf support file.
//	17 Jun 97 - Added the functions long128_compare and MD5_sum
//		    for password checking.
//	24 Jul 97 - Adapted to be integrated in the COREset.
//

#include	<string>
#include	<cctype>
#include	<cstdio>
#include	<cstdlib>
#include	<cstdarg>

#include	"misc.h"
#include	"xmd5.h"

const char Misc_h_rcsId[] = __MISC_H ;
const char Misc_cc_rcsId[] = "$Id: misc.cc,v 1.1 2000/06/30 18:46:06 dan_karrels Exp $" ;

using std::string ;

string string_lower( const string& Key )
{
string retMe( Key ) ;
for( string::size_type i = 0 ; i < Key.size() ; i++ )
	{
	retMe[ i ] = tolower( Key[ i ] ) ;
	}
return retMe ;
}

string string_upper( const string& Key )
{
string retMe( Key ) ;
for( string::size_type i = 0 ; i < Key.size() ; i++ )
	{
	retMe[ i ] = toupper( Key[ i ] ) ;
	}
return retMe ;
}

void string_tolower( string& Key )
{
for( string::size_type i = 0 ; i < Key.size() ; i++ )
	{
	Key[ i ] = tolower( Key[ i ] ) ;
	}
}

const char* Spaces( int n )
{
static char buf[ 256 ] ;
memset( buf, ' ', 256 ) ;
if( n > 255 ) n = 255 ;
buf[ n ] = 0 ;
return buf ;
}

bool IsNumeric( const string& s )
{
for( string::size_type i = 0 ; i < s.size() ; i++ )
	{
	if( !isdigit( s[ i ] ) )
		{
		return false ;
		}
	}
return true ;
}       
