/* misc.cc
 */

#include	<string>
#include	<cctype>
#include	<cstdio>
#include	<cstdlib>
#include	<cstdarg>

#include	"misc.h"

const char Misc_h_rcsId[] = __MISC_H ;
const char Misc_cc_rcsId[] = "$Id: misc.cc,v 1.2 2000/10/31 00:58:37 dan_karrels Exp $" ;

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
