/* cloner.cc
 * Author: Daniel Karrels dan@karrels.com
 */

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

using std::vector ;
using std::endl ;
using std::strstream ;
using std::ends ;
using std::string ;

namespace gnuworld
{

cloner::cloner( const string& configFileName )
{
EConfig conf( configFileName ) ;

nickName = conf.Find( "nickname" )->second ;
userName = conf.Find( "username" )->second ;
hostName = conf.Find( "hostname" )->second ;
userDescription = conf.Find( "userdescription" )->second ;
cloneDescription = conf.Find( "clonedescription" )->second ;

Mode( conf.Find( "mode" )->second ) ;

EConfig::const_iterator ptr = conf.Find( "fakehost" ) ;
while( ptr != conf.end() && ptr->first == "fakehost" )
	{
	hostNames.push_back( ptr->second ) ;
	++ptr ;
	}

if( hostNames.empty() )
	{
	elog	<< "cloner> Must specify at least one hostname\n" ;
	exit( 0 ) ;
	}

ptr = conf.Find( "fakeuser" ) ;
while( ptr != conf.end() && ptr->first == "fakeuser" )
	{
	userNames.push_back( ptr->second ) ;
	++ptr ;
	}

if( userNames.empty() )
	{
	elog	<< "cloner> Must specify at least one username\n" ;
	exit( 0 ) ;
	}

fakeServer = new iServer(
	0, // uplinkIntYY
	"", // charYYXXX
	conf.Find( "fakeservername" )->second,
	time( 0 ),
	time( 0 ),
	10 ) ;

}

cloner::~cloner()
{}

int cloner::OnConnect()
{

xClient::OnConnect() ;

if( NULL == Network->findServer( fakeServer->getIntYY() ) )
	{
	MyUplink->AttachServer( fakeServer ) ;
	}

for( vector< iClient* >::size_type i = 0 ; i < clones.size() ; ++i )
	{
	strstream s ;
	s	<< fakeServer->getCharYY() << " N "
		<< clones[ i ]->getNickName() << " 0 "
		<< clones[ i ]->getConnectTime() << " "
		<< clones[ i ]->getUserName() << " "
		<< clones[ i ]->getInsecureHost() << " "
		<< clones[ i ]->getModes() << " "
		<< xIP( 0, 0, 0, 0 ).GetBase64IP() << " "
		<< clones[ i ]->getCharYYXXX() << " :"
		<< cloneDescription << ends ;
	if( QuoteAsServer( s.str() ) < 0 )
		{
		delete[] s.str() ;
		return -1 ;
		}
	delete[] s.str() ;
	} // close for

return 0 ;

}

int cloner::OnPrivateMessage( iClient* theClient, const string& Message )
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

if( command == "LOADCLONES" )
	{
	if( st.size() < 2 )
		{
		Notice( theClient, "Usage: LOADCLONES <num clones>" ) ;
		return 0 ;
		}

	int numClones = atoi( st[ 1 ].c_str() ) ;

	for( int i = 0 ; i < numClones ; i++ )
		{
		addClone() ;
		}

	Notice( theClient, "Added %d clones", numClones ) ;

	}

return 0 ;
}

void cloner::addClone()
{

char buf[ 4 ] ;

string yyxxx( fakeServer->getCharYY() ) ;

inttobase64( buf, fakeServer->getClients() + 1, 3 ) ;
buf[ 3 ] = 0 ;

yyxxx += buf ;

MyUplink->AttachClient( new iClient(
	fakeServer->getIntYY(),
	yyxxx,
	randomNick( 5 ),
	randomUser(),
	randomNick( 6, 6 ),
	randomHost(),
	randomMode(), 
	"I'm a clone.",
	time( 0 ) ) ) ;

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
