/*
 * WHOISCommand.cc
 *
 * Gives varius data about a user
 *
*/

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"Network.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"ip.h"

const char WHOISCommand_cc_rcsId[] = "$Id: WHOISCommand.cc,v 1.9 2002/01/05 15:34:54 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

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
ccUser* tmpUser = bot->IsAuth(theClient);
if(tmpUser)
        bot->MsgChanLog("(%s) - %s : WHOIS %s\n",tmpUser->getUserName().c_str()
                        ,theClient->getNickUserHost().c_str(),st.assemble(1).c_str());
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
	xIP(Target->getIP() ).GetNumericIP()
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

if(Target->getMode(iClient::MODE_SERVICES))
	{
	return true;
	}
	
vector< string > channels ;
string curChannel ;
string::size_type curPlace;
string tChannel;
char curChar[10];
for( iClient::const_channelIterator ptr = Target->channels_begin() ;
	ptr != Target->channels_end() ; ++ptr )
	{
	curChannel = "";
	tChannel = (*ptr)->getName();
	for(curPlace = 0; curPlace < tChannel.size();++curPlace)
		{
		if(((tChannel[curPlace] > 1) && (tChannel[curPlace] < 4))
			|| (tChannel[curPlace] == 15) 
			|| ((tChannel[curPlace] > 27) && (tChannel[curPlace] < 33))
			|| (tChannel[curPlace] == 22)
			|| (tChannel[curPlace] == 160)
			|| ((tChannel[curPlace] > 252) 	&& (tChannel[curPlace] <= 254)))
			{
			sprintf(curChar,"%d",tChannel[curPlace]);
			curChannel += string("^") + curChar;
			}
		else
			{
			curChannel += tChannel[curPlace];
			}
		}
	channels.push_back( curChannel) ;
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
}
