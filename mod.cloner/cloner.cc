/* cloner.cc
 * Author: Daniel Karrels dan@karrels.com
 */

#include	<new>
#include	<list>
#include	<vector>
#include	<iostream>
#include	<strstream>
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

namespace gnuworld
{

using std::vector ;
using std::endl ;
using std::strstream ;
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
fakeServerName = conf.Require( "fakeservername" )->second ;

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
if( maxNickLength > 9 )
	{
	elog	<< "cloner> maxNickLength cannot exceed 9"
		<< endl ;
	::exit( 0 ) ;
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

MyUplink->AttachServer( fakeServer ) ;

return xClient::OnConnect() ;
}

int cloner::OnPrivateMessage( iClient* theClient, const string& Message,
	bool)
{
//elog << "cloner::OnPrivateMessage> " << Message << endl ;

/*
if( !theClient->isOper() )
	{
	return 0 ;
	}
*/

StringTokenizer st( Message ) ;
if( st.empty() )
	{
	return 0 ;
	}

string command( string_upper( st[ 0 ] ) ) ;

if( command == "LOADCLONES" )
	{
	if( st.size() < 2 )
		{
		Notice( theClient, "Usage: LOADCLONES <num clones>" ) ;
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

	Notice( theClient, "Queuing %d clones", numClones ) ;

	}
else if( command == "JOINALL" )
	{
	if( st.size() < 2 )
		{
		Notice( theClient, "Usage: JOINALL <channel>" ) ;
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
		strstream s ;
		s	<< (*ptr)->getCharYYXXX()
			<< " J "
			<< chanName
			<< ends ;

		MyUplink->Write( s ) ;
		delete[] s.str() ;
		}
	} // JOINALL
else if( command == "PARTALL" )
	{
	if( st.size() < 2 )
		{
		Notice( theClient, "Usage: PARTALL <channel>" ) ;
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
		strstream s ;
		s	<< (*ptr)->getCharYYXXX()
			<< " L "
			<< chanName
			<< ends ;

		MyUplink->Write( s ) ;
		delete[] s.str() ;
		}
	} // PARTALL

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
		randomMode(),
		"",
		"I'm a clone.",
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

string cloner::randomMode()
{
return string( "+d" ) ;
}

string cloner::randomNick( int minLength, int maxLength )
{
         
string retMe ;

// Generate a random number between 1 and 9
// Will be the length of the nickname
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

}
// namespace gnuworld
