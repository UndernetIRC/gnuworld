/**
 * msg_RemPing.cc
 */

#include	<sys/time.h>
#include	<unistd.h>

#include	<strstream>
#include	<string>

#include	"server.h"
#include	"xparameters.h"

const char msg_RemPing_cc_rcsId[] = "$Id: msg_RemPing.cc,v 1.2 2001/05/18 15:27:10 dan_karrels Exp $" ;
const char server_h_rcsId[] = __SERVER_H ;
const char xParameters_h_rcsId[] = __XPARAMETERS_H ;

namespace gnuworld
{

using std::string ;
using std::strstream ;
using std::ends ;

const string militime( const char* sec, const char* msec )
{
struct timeval tv;
strstream s ;
::gettimeofday( &tv, NULL ) ;
                                 
if( sec && msec )
	{
        s	<< (int)((tv.tv_sec - atoi(sec)) * 1000 +
			(tv.tv_usec - atoi(msec)) / 1000)
		<< ends ;
	}
else
	{
	s	<< static_cast< int>( tv.tv_sec )
		<< ' '
		<< static_cast< int >( tv.tv_usec )
		<< ends ;
	}
string retMe( s.str() ) ;
delete[] s.str() ;
return retMe ;
}

// Remote Ping message
int xServer::MSG_RemPing( xParameters& Param )
{
if( Param[ 1 ][ 0 ] == charYY[ 0 ]
	&& Param[ 1 ][ 1 ] == charYY[ 1 ] && !bursting )
	{
	// It's me
	Write( ":%s RPONG %s %s %s :%s\n",
		ServerName.c_str(),
		Param[ 2 ],
		ServerName.c_str(),
		militime( Param[ 3 ], Param[ 4 ] ).c_str(),
		Param[ 5 ] ) ;
	}
return 0 ;
}

} // namespace gnuworld
