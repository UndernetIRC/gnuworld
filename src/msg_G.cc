/**
 * msg_G.cc
 */

#include	<string>

#include	"server.h"
#include	"xparameters.h"
#include	"ELog.h"

const char msg_G_cc_rcsId[] = "$Id:" ;

using std::string ;
using std::endl ;

namespace gnuworld
{

// Q G :ripper.ufl.edu
// Q: Remote server numeric
// G: PING
// :ripper.ufl.edu: Ping argument
// Reply with:
// <Our Numeric> Z <Their Numeric> :<arguments>
// Strings will be passed to this method in format:
// Q ripper.ufl.edu
int xServer::MSG_G( xParameters& params )
{
if( params.size() < 2 )
	{
	elog	<< "xServer::MSG_G> Invalid number of parameters"
		<< endl ;
	return -1 ;
	}

string s( charYY ) ;
s += " Z " ;
s += params[ 0 ] ;

if( params.size() >= static_cast< xParameters::size_type >( 1 ) )
	{
	s += " :" ;
	s += params[ 1 ] ;
	}

return Write( s ) ;
}

} // namespace gnuworld
