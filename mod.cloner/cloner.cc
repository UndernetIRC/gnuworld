/**
 * cloner.cc
 * Load fake clones for testing or fun.
 *
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
 *		      Reed Loden <reed@reedloden.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 *
 * $Id: cloner.cc,v 1.25 2003/01/07 15:57:48 jeekay Exp $
 */

#include	<new>
#include	<list>
#include	<vector>
#include	<iostream>
#include	<sstream>
#include	<string>

#include	<ctime>
#include	<cstdlib>

#include	"client.h"
#include	"iClient.h"
#include	"cloner.h"
#include	"EConfig.h"
#include	"ip.h"
#include	"Network.h"
#include	"StringTokenizer.h"
#include	"misc.h"
#include	"ELog.h"

const char client_h_rcsId[] = __CLIENT_H ;
const char cloner_h_rcsId[] = __CLONER_H ;
const char cloner_cc_rcsId[] = "$Id: cloner.cc,v 1.25 2003/01/07 15:57:48 jeekay Exp $" ;
const char iClient_h_rcsId[] = __ICLIENT_H ;
const char EConfig_h_rcsId[] = __ECONFIG_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char ip_h_rcsId[] = __IP_H ;
const char misc_h_rcsId[] = __MISC_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char StringTokenizer_h_rcsId[] = __STRINGTOKENIZER_H ;

namespace gnuworld
{

using std::vector ;
using std::endl ;
using std::stringstream ;
using std::ends ;
using std::string ;

/*
 *  Exported function used by moduleLoader to gain an
 *  instance of this module.
 */

extern "C"
{
  xClient* _gnuwinit(const string& args)
  { 
    return new cloner( args );
  }

} 
 
cloner::cloner( const string& configFileName )
 : xClient( configFileName )
{

EConfig conf( configFileName ) ;

cloneDescription = conf.Require( "clonedescription" )->second ;
cloneMode = conf.Require( "clonemode" )->second ;
fakeServerName = conf.Require( "fakeservername" )->second ;
fakeServerDescription = conf.Require( "fakeserverdescription" )->second ;

cloneBurstCount = atoi( conf.Require( "cloneburstcount" )->second.c_str() ) ;
if( cloneBurstCount < 1 )
	{
	elog	<< "cloner> cloneBurstCount must be at least 1"
		<< endl ;
	::exit( 0 ) ;
	}

EConfig::const_iterator ptr = conf.Find( "fakehost" ) ;
while( ptr != conf.end() && ptr->first == "fakehost" )
	{
	hostNames.push_back( ptr->second ) ;
	++ptr ;
	}

if( hostNames.empty() )
	{
	elog	<< "cloner> Must specify at least one hostname"
		<< endl ;
	::exit( 0 ) ;
	}

ptr = conf.Find( "fakeuser" ) ;
while( ptr != conf.end() && ptr->first == "fakeuser" )
	{
	userNames.push_back( ptr->second ) ;
	++ptr ;
	}

if( userNames.empty() )
	{
	elog	<< "cloner> Must specify at least one username"
		<< endl ;
	::exit( 0 ) ;
	}

makeCloneCount = 0 ;

minNickLength = atoi( conf.Require( "minnicklength" )->second.c_str() ) ;
maxNickLength = atoi( conf.Require( "maxnicklength" )->second.c_str() ) ;

if( minNickLength < 1 )
	{
	elog	<< "cloner> minNickLength must be at least 1"
		<< endl ;
	::exit( 0 );
	}
if( maxNickLength <= minNickLength )
	{
	elog	<< "cloner> minNickLength must be less than maxNickLength"
		<< endl ;
	::exit( 0 ) ;
	}
}

cloner::~cloner()
{}

int cloner::OnConnect()
{
fakeServer = new (std::nothrow) iServer(
	MyUplink->getIntYY(), // uplinkIntYY
	string(), // charYYXXX
	fakeServerName,
	::time( 0 ) ) ;
assert( fakeServer != 0 ) ;

MyUplink->AttachServer( fakeServer, fakeServerDescription ) ;

return xClient::OnConnect() ;
}

int cloner::OnPrivateMessage( iClient* theClient, const string& Message,
	bool)
{
//elog << "cloner::OnPrivateMessage> " << Message << endl ;

if( !theClient->isOper() )
	{
	return 0 ;
	}

StringTokenizer st( Message ) ;
if( st.empty() )
	{
	return 0 ;
	}

string command( string_upper( st[ 0 ] ) ) ;
string topic ;

if( st.size() > 1 )
	{
	topic = st[ 1 ] ;
	}

if( command == "SHOWCOMMANDS" )
	{
	if( st.size() < 1 )
		{
		Notice( theClient, "Usage: %s", command.c_str() ) ;
		return 0 ;
		}
	if( st.size() >= 1 )
		{
		Notice( theClient, "_-=[Cloner Commands]=-_" ) ;
		Notice( theClient, "LOADCLONES JOINALL PARTALL "
			"KILLALL/QUITALL SAYALL/MSGALL "
			"ACTALL/DOALL/DESCRIBEALL NOTICEALL" ) ;
		Notice( theClient, "_-=[End of Cloner Commands]=-_" ) ;
		}
	}
else if( command == "HELP" )
	{
	if( st.size() < 1 )
		{
		Notice( theClient, "Usage: %s <topic>",
			command.c_str() ) ;
		return 0 ;
		}

	if( topic == "SHOWCOMMANDS" )
		{
		Notice( theClient, "%s - Shows a list of all commands",
			topic.c_str() ) ;
		}
	else if( topic == "HELP" )
		{
		Notice( theClient, "%s <topic> - Gives help on a topic",
			topic.c_str() ) ;
		}
	else if( topic == "LOADCLONES" )
		{
		Notice( theClient, "%s <# of clones> - Queue creation "
			"  of clone(s)", topic.c_str() ) ;
		}
	else if( topic == "JOINALL" )
		{
		Notice( theClient, "%s <#channel> - Make all clones "
			"/join a #channel", topic.c_str() ) ;
		}
	else if( topic == "PARTALL" )
		{
		Notice( theClient, "%s <#channel> [reason] - Make "
			"all clones /part a #channel with an optional "
			"reason", topic.c_str() ) ;
		}
	else if( topic == "KILLALL" || topic == "QUITALL" )
		{
		Notice( theClient, "%s [reason] - Make all "
			"clones /quit with an optional reason",
			topic.c_str() ) ;
		}
	else if( topic == "SAYALL" || topic == "MSGALL" )
		{
		Notice( theClient, "%s <#channel/nickname> "
			"<message> - Make all clones /msg a #channel or "
			"nickname", topic.c_str() ) ;
		}
	else if( topic == "ACTALL" || topic == "DOALL" || topic ==
		"DESCRIBEALL" )
		{
		Notice( theClient, "%s <#channel/nickname> <action> - "
			"Make all clones /me a channel or nickname",
			topic.c_str() ) ;
		}
	else if( topic == "NOTICEALL" )
		{
		Notice( theClient, "%s <#channel/nickname> "
			"<notice> - Make all clones /notice a #channel "
			"or nickname", topic.c_str() ) ;
		}
	} // "HELP"
else if( command == "LOADCLONES" )
	{
	if( st.size() < 2 )
		{
		Notice( theClient, "Usage: %s <# of clones>",
			command.c_str() ) ;
		return 0 ;
		}

	int numClones = atoi( st[ 1 ].c_str() ) ;
	if( numClones < 1 )
		{
		Notice( theClient,
			"LOADCLONES: Invalid number of clones" ) ;
		return 0 ;
		}

	if( 0 == makeCloneCount )
		{
		MyUplink->RegisterTimer( ::time( 0 ) + 1, this, 0 ) ;
		}

	makeCloneCount += static_cast< size_t >( numClones ) ;
//	elog	<< "cloner::OnPrivateMessage> makeCloneCount: "
//		<< makeCloneCount
//		<< endl ;

	Notice( theClient, "Queuing %d Clones", numClones ) ;
	} // "LOADCLONES"
else if( command == "JOINALL" )
	{
	if( st.size() < 2 )
		{
		Notice( theClient, "Usage: %s <#channel>",
			command.c_str() ) ;
		return 0 ;
		}

	string chanName( st[ 1 ] ) ;
	if( chanName[ 0 ] != '#' )
		{
		chanName.insert( chanName.begin(), '#' ) ;
		}

	for( list< iClient* >::const_iterator ptr = clones.begin(),
		endPtr = clones.end() ; ptr != endPtr ; ++ptr )
		{
		stringstream s ;
		s	<< (*ptr)->getCharYYXXX()
			<< " J "
			<< chanName
			<< ends ;

		MyUplink->Write( s ) ;
		}
	} // JOINALL
else if( command == "PARTALL" )
	{
	if( st.size() < 2 )
		{
		Notice( theClient, "Usage: %s <#channel> [reason]",
			command.c_str() ) ;
		return 0 ;
		}

	if( st.size() == 2 )
		{
		string chanName( st[ 1 ] ) ;
		if( chanName[ 0 ] != '#' )
			{
			chanName.insert( chanName.begin(), '#' ) ;
			}

		for( list< iClient* >::const_iterator ptr = clones.begin(),
			endPtr = clones.end() ; ptr != endPtr ; ++ptr )
			{
			stringstream s ;
			s	<< (*ptr)->getCharYYXXX()
				<< " L "
				<< chanName
				<< ends ;

			MyUplink->Write( s ) ;
			}
		}
	if( st.size() >= 2 )
		{
		string chanName( st[ 1 ] ) ;
		if( chanName[ 0 ] != '#' )
			{
			chanName.insert( chanName.begin(), '#' ) ;
			}

		string partReason( st.assemble(2).c_str() ) ;

		for( list< iClient* >::const_iterator ptr = clones.begin(),
			endPtr = clones.end() ; ptr != endPtr ; ++ptr )
			{
			stringstream s ;
			s	<< (*ptr)->getCharYYXXX()
				<< " L "
				<< chanName
				<< " :"
				<< partReason
				<< ends ;

			MyUplink->Write( s ) ;
			}
		}
	} // PARTALL
else if( command == "KILLALL" || command == "QUITALL" )
	{
        if( st.size() < 1 )
                {
                Notice( theClient, "Usage: %s [reason]",
			command.c_str() ) ;
                return 0 ;
                }

	if( st.size() == 1 )
		{
		for( list< iClient* >::const_iterator ptr = clones.begin(),
			endPtr = clones.end() ; ptr != endPtr ; ++ptr )
			{
			stringstream s ;
			s	<< (*ptr)->getCharYYXXX()
				<< " Q"
				<< ends ;

			MyUplink->Write( s ) ;
			}
		}

	if( st.size() >= 2 )
		{
		string quitMsg( st.assemble(1).c_str() ) ;

		for( list< iClient* >::const_iterator ptr = clones.begin(),
			endPtr = clones.end() ; ptr != endPtr ; ++ptr )
			{
			stringstream s ;
			s	<< (*ptr)->getCharYYXXX()
				<< " Q :"
				<< quitMsg
				<< ends ;

			MyUplink->Write( s ) ;
			}
		}
	} // KILLALL/QUITALL
else if( command == "SAYALL" || command == "MSGALL" )
	{
	if( st.size() < 3 )
		{
		Notice( theClient, "Usage: %s <#channel/nickname> "
			"<message>", command.c_str() ) ;
		return 0 ;
		}

	string chanOrNickName( st[ 1 ] ) ;
	string privMsg( st.assemble(2).c_str() ) ;

        if( chanOrNickName[ 0 ] != '#' )
                { // Assume nickname
                iClient* Target = Network->findNick( st[ 1 ] ) ;
                if( NULL == Target )
                        {
                        Notice( theClient, "Unable to find nick: %s"
	                         , st[ 1 ].c_str() ) ;
	                return 0 ;
	                }
		chanOrNickName = Target->getCharYYXXX();
	        }

	for( list< iClient* >::const_iterator ptr = clones.begin(),
		endPtr = clones.end() ; ptr != endPtr ; ++ptr )
		{
		stringstream s ;
		s	<< (*ptr)->getCharYYXXX()
			<< " P "
			<< chanOrNickName
			<< " :"
			<< privMsg
			<< ends ;

		MyUplink->Write( s ) ;
		}
	} // SAYALL/MSGALL
else if( command == "ACTALL" || command == "DOALL" ||
	command == "DESCRIBEALL" )
	{
	if( st.size() < 3 )
		{
		Notice( theClient, "Usage: %s <#channel/nickname> "
			"<action>", command.c_str() ) ;
		return 0 ;
		}

	string chanOrNickName( st[ 1 ] ) ;
	string action( st.assemble(2).c_str() ) ;

        if( chanOrNickName[ 0 ] != '#' )
                { // Assume nickname
                iClient* Target = Network->findNick( st[ 1 ] ) ;
                if( NULL == Target )
                        {
                        Notice( theClient, "Unable to find nick: %s"
                                , st[ 1 ].c_str() ) ;
                        return 0 ;
                        }
		chanOrNickName = Target->getCharYYXXX();
	        }

	for( list< iClient* >::const_iterator ptr = clones.begin(),
		endPtr = clones.end() ; ptr != endPtr ; ++ptr )
		{
		stringstream s ;
		s	<< (*ptr)->getCharYYXXX()
			<< " P "
			<< chanOrNickName
			<< " :\001ACTION "
			<< action
			<< "\001"
			<< ends ;

		MyUplink->Write( s ) ;
		}
	} // ACTALL/DOALL/DESCRIBEALL
else if( command == "NOTICEALL" )
	{
	if( st.size() < 3 )
		{
		Notice( theClient, "Usage: %s <#channel/nickname> "
			"<notice>", command.c_str() ) ;
		return 0 ;
		}

	string chanOrNickName( st[ 1 ] ) ;
	string notice( st.assemble(2).c_str() ) ;

	if( chanOrNickName[ 0 ] != '#' ) 
		{ // Assume nickname
		iClient* Target = Network->findNick( st[ 1 ] ) ;
		if( NULL == Target )
		        {
		        Notice( theClient, "Unable to find nick: %s"
				, st[ 1 ].c_str() ) ;
		        return 0 ;
		        }
		chanOrNickName = Target->getCharYYXXX();
		}
	
	for( list< iClient* >::const_iterator ptr = clones.begin(),
		endPtr = clones.end() ; ptr != endPtr ; ++ptr )
		{
		stringstream s ;
		s	<< (*ptr)->getCharYYXXX()
			<< " O "
			<< chanOrNickName
			<< " :"
			<< notice
			<< ends ;

		MyUplink->Write( s ) ;
		}
	} // NOTICEALL
return 0 ;
}

int cloner::OnTimer( xServer::timerID, void* )
{
//elog	<< "cloner::OnTimer> makeCloneCount: "
//	<< makeCloneCount
//	<< endl ;

if( 0 == makeCloneCount )
	{
	return -1 ;
	}

size_t cloneCount = makeCloneCount ;
if( cloneCount > cloneBurstCount )
	{
	// Too many
	cloneCount = cloneBurstCount ;
	}

makeCloneCount -= cloneCount ;

//elog	<< "cloner::OnTimer> cloneCount: "
//	<< cloneCount
//	<< endl ;

for( size_t i = 0 ; i < cloneCount ; ++i )
	{
	addClone() ;
	}

if( makeCloneCount > 0 )
	{
	MyUplink->RegisterTimer( ::time( 0 ) + 1, this, 0 ) ;
	}

return 0 ;
}

void cloner::addClone()
{
char buf[ 4 ] = { 0 } ;

string yyxxx( fakeServer->getCharYY() ) ;

inttobase64( buf, Network->countClients( fakeServer ) + 1, 3 ) ;
buf[ 3 ] = 0 ;

yyxxx += buf ;

iClient* newClient = new iClient(
		fakeServer->getIntYY(),
		yyxxx,
		randomNick( minNickLength, maxNickLength ),
		randomUser(),
		randomNick( 6, 6 ),
		randomHost(),
		randomHost(),
		cloneMode,
		"",
		cloneDescription,
		::time( 0 ) ) ;
assert( newClient != 0 );

MyUplink->AttachClient( newClient ) ;
clones.push_back( newClient ) ;

}

string cloner::randomUser()
{
return userNames[ rand() % userNames.size() ] ;
}

string cloner::randomHost()
{
return hostNames[ rand() % hostNames.size() ] ;
}

string cloner::randomNick( int minLength, int maxLength )
{
string retMe ;

// Generate a random number between minLength and maxLength
// This will be the length of the nickname
int randomLength = minLength + (rand() % (maxLength - minLength + 1) ) ;

for( int i = 0 ; i < randomLength ; i++ )
        {
        retMe += randomChar() ;
        }

//elog << "randomNick: " << retMe << endl ;
return retMe ;
}

// ascii [65,122]
char cloner::randomChar()
{
char c = ('A' + (rand() % ('z' - 'A')) ) ;
//elog << "char: returning: " << c << endl ;
return c ;
         
//return( (65 + (rand() % 122) ) ;
//return (char) (1 + (int) (9.0 * rand() / (RAND_MAX + 1.0) ) ) ;
}

} // namespace gnuworld
