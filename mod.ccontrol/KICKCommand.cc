#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"Network.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char KICKCommand_cc_rcsId[] = "$Id $";

namespace gnuworld
{

using std::string ;

// kick #channel nick reason
bool KICKCommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() < 4 )
	{
	Usage( theClient ) ;
	return true ;
	}

string chanName = st[ 1 ] ;
if( chanName[ 0 ] != '#' )
	{
	chanName.insert( chanName.begin(), '#' ) ;
	}

Channel* theChan = Network->findChannel( chanName ) ;
if( NULL == theChan )
	{
	bot->Notice( theClient, "Unable to find channel: %s",
		chanName.c_str() ) ;
	return true ;
	}

iClient* Target = Network->findNick( st[ 2 ] ) ;
if( NULL == Target )
	{
	bot->Notice( theClient, "Unable to find nick: %s",
		st[ 2 ].c_str() ) ;
	return true ;
	}

bot->Notice( theClient, "Kicking %s from channel %s because %s",
	Target->getNickName().c_str(),
	chanName.c_str(),
	st.assemble( 3 ).c_str() ) ;

bot->Kick( theChan, Target, st.assemble( 3 ) ) ;
return true ;
}
}
