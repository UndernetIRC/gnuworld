#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"Network.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"ip.h"

const char WHOISCommand_cc_rcsId[] = "$Id: WHOISCommand.cc,v 1.3 2001/02/23 20:19:43 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

// whois nickname
bool WHOISCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

iClient* Target = Network->findNick( st[ 1 ] ) ;
if( NULL == Target )
	{
	bot->Notice( theClient, "Unable to find nick: %s", st[ 1 ].c_str() ) ;
	return true ;
	}

iServer* targetServer = Network->findServer( Target->getIntYY() ) ;
if( NULL == targetServer )
	{
	elog	<< "WHOISCommand> Unable to find server: "
		<< Target->getIntYY() << endl ;
	return false ;
	}

bot->Notice( theClient, "%s is %s!%s@%s [%s]",
	st[ 1 ].c_str(),
	Target->getNickName().c_str(),
	Target->getUserName().c_str(),
	Target->getInsecureHost().c_str(),
	xIP( htonl( Target->getIP() ) ).GetNumericIP()
	) ;

bot->Notice( theClient, "Numeric: %s, UserModes: %s, Server Numeric: %s (%s)",
	Target->getCharYYXXX().c_str(),
	Target->getCharModes().c_str(),
	targetServer->getCharYY(),
	targetServer->getName().c_str()
	) ;

if( Target->isOper() )
	{
	bot->Notice( theClient, "%s is an IRCoperator",
		st[ 1 ].c_str() ) ;
	}

vector< string > channels ;
for( iClient::const_channelIterator ptr = Target->channels_begin() ;
	ptr != Target->channels_end() ; ++ptr )
	{
	channels.push_back( (*ptr)->getName() ) ;
	}

if( channels.empty() )
	{
	return true ;
	}

string chanNames ;
for( vector< string >::size_type i = 0 ; i < channels.size() ; i++ )
	{
	chanNames += channels[ i ] ;
	if( (i + 1) < channels.size() )
		{
		chanNames += ", " ;
		}
	}

bot->Notice( theClient, "On channels: %s",
	chanNames.c_str() ) ;

return true ;
}
}
