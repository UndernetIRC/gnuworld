#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

namespace gnuworld
{

using std::string ;


bool SCANGLINECommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

vector< const Gline* > glines = server->matchGline( st[ 1 ] ) ;

bot->Notice( theClient, "Found %d matches", glines.size() ) ;
if( glines.empty() )
	{
	return true ;
	}

bot->Notice( theClient, "Current time: %d", ::time( 0 ) ) ;

for( vector< const Gline* >::const_iterator ptr = glines.begin() ;
	ptr != glines.end() ; ++ptr )
	{
	bot->Notice( theClient, "%s expires at %d, set by %s because %s",
		(*ptr)->getUserHost().c_str(),
		(*ptr)->getExpiration(),
		(*ptr)->getSetBy().c_str(),
		(*ptr)->getReason().c_str() ) ;
	}

return true ;
}
}