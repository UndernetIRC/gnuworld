/* iServer.cc
 */

#include	<string>
#include	<ctime>

#include	"iServer.h"
#include	"Numeric.h"

const char iServer_h_rcsId[] = __ISERVER_H ;
const char iServer_cc_rcsId[] = "$Id: iServer.cc,v 1.3 2001/03/01 01:58:17 dan_karrels Exp $" ;

using std::string ;

namespace gnuworld
{

iServer::iServer( const unsigned int& _uplink,
	const string& _yxx,
	const string& _name,
	const time_t& _connectTime )
: uplinkIntYY( _uplink ),
	name( _name ),
	connectTime( _connectTime )
{

if( 5 == _yxx.size() )
	{
	// yyxxx, n2k
	intYY = base64toint( _yxx.c_str(), 2 ) ;
	inttobase64( charYY, intYY, 2 ) ;

	intXXX = base64toint( _yxx.c_str() + 2, 3 ) ;
	}
else
	{
	// yxx
	intYY = convert2n[ _yxx[ 0 ] ] ;
	charYY[ 0 ] = _yxx[ 0 ] ;
	charYY[ 1 ] = 0 ;
	charYY[ 2 ] = 0 ;

	intXXX = base64toint( _yxx.c_str() + 1, 2 ) ;
	}
}

iServer::~iServer()
{}

} // namespace gnuworld
