
#include	<string>
#include	<iostream>

#include	<cstdlib> // atoi()

#include	"testbot.h"
#include	"ELog.h"
#include	"ConnectionManager.h"
#include	"ConnectionHandler.h"
#include	"EConfig.h"
#include	"StringTokenizer.h"

using namespace gnuworld ;

using std::string ;
using std::cout ;
using std::endl ;

gnuworld::ELog gnuworld::elog ;

void usage( const string& progName )
{
cout	<< "Usage: "
	<< progName
	<< " <conf file>"
	<< endl ;
}

int main( int argc, const char** argv )
{
if( argc != 2 )
	{
	usage( argv[ 0 ] ) ;
	return 0 ;
	}

testBot bot( argv[ 1 ] ) ;
bot.Run() ;

return 0 ;
}

testBot::testBot( const string& fileName )
 : keepRunning( true ),
   uplinkPort( 0 ),
   myConnect( 0 )
{
EConfig conf( fileName ) ;
setUplinkName( conf.Require( "uplinkname" )->second ) ;
setUplinkPort( static_cast< unsigned short int >(
	atoi( conf.Require( "uplinkport" )->second.c_str() ) ) ) ;
setUserName( conf.Require( "username" )->second ) ;
setNickName( conf.Require( "nickName" )->second ) ;
setRealName( conf.Require( "realname" )->second ) ;

for( EConfig::const_iterator chanItr = conf.Find( "channel" ) ;
	(chanItr != conf.end()) &&
	(chanItr->first == "channel") ; ++chanItr )
	{
	channels.push_back( chanItr->second ) ;
	}

elog.setStream( &cout ) ;
}

testBot::~testBot()
{}

void testBot::Run()
{
myConnect = cm.Connect( this, uplinkName, uplinkPort ) ;
if( !myConnect )
	{
	cout	<< "Run> Failed to connect"
		<< endl ;
	return ;
	}

login() ;

while( keepRunning )
	{
	cm.Poll( 100 ) ; // poll on 100ms intervals
	}

cout	<< "Run> Out of main loop, shutting down"
	<< endl ;
}

void testBot::OnDisconnect( Connection* )
{
keepRunning = false ;
}

void testBot::OnConnectFail( Connection* )
{
keepRunning = false ;
}

void testBot::OnTimeout( Connection* )
{
keepRunning = false ;
}

void testBot::OnConnect( Connection* )
{
cout	<< "Connected"
	<< endl ;
}

void testBot::OnRead( Connection*, const string& line )
{
size_t len = line.size() - 1 ;
while( ('\r' == line[ len ]) ||
	('\n' == line[ len ]) )
	{
	--len ;
	}

string	processMe = line.substr( 0, len + 1 ) ;
cout	<< "[IN]: "
	<< processMe
	<< endl ;

processInput( processMe ) ;
}

void testBot::login()
{
// NICK <nick>
// USER ident host server :realname
string writeMe( "NICK " ) ;
writeMe += getNickName() ;
writeMe += "\r\n" ;
myConnect->Write( writeMe ) ;

writeMe = "USER " ;
writeMe += getUserName() ;
writeMe += " dummy.host.name dummy.server.name :" ;
writeMe += getRealName() ;
writeMe += "\r\n" ;

myConnect->Write( writeMe ) ;
}

void testBot::processInput( const string& line )
{
// PING :1834576261
StringTokenizer st( line ) ;
if( st.empty() )
	{
	cout	<< "processInput> Empty tokenizer"
		<< endl ;
	return ;
	}

if( st[ 0 ] == "PING" )
	{
	string writeMe( "PONG " ) ;
	writeMe += st[ 1 ] + "\r\n" ;

	myConnect->Write( writeMe ) ;
	return ;
	}

// :Princeton.NJ.US.Undernet.org 376 _ripper_ :End of /MOTD command.
if( st.size() < 2 )
	{
	return ;
	}

if( st[ 1 ] == "376" )
	{
	joinChannels() ;

	string writeMe( "JOIN #c++\r\n" ) ;
	myConnect->Write( writeMe ) ;
	return ;
	}

}

void testBot::joinChannels()
{
for( list< string >::const_iterator chanItr = channels.begin() ;
	chanItr != channels.end() ; ++chanItr )
	{
	string writeMe( "JOIN " ) ;
	writeMe += *chanItr + "\r\n" ;
	myConnect->Write( writeMe ) ;
	}
}

