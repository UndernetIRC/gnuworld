/* iServer.cc
 */

#include	<string>
#include	<ctime>

#include	"iServer.h"
#include	"Numeric.h"

const char iServer_h_rcsId[] = __ISERVER_H ;
const char iServer_cc_rcsId[] = "$Id: iServer.cc,v 1.2 2000/07/31 15:17:25 dan_karrels Exp $" ;

using std::string ;

namespace gnuworld
{

iServer::iServer( const int& _uplink,
	const string& _yxx,
	const string& _name,
	const time_t& _connectTime,
	const time_t& _startTime,
	const int& _version )
: uplinkIntYY( _uplink ),
	name( _name ),
	connectTime( _connectTime ),
	startTime( _startTime ),
	version( _version )
{

if( _yxx.size() == 5 )
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

clients = servers = 0 ;
IP = 0 ;
}

iServer::~iServer()
{}

} // namespace gnuworld
