/* client.cc
 */

#include	<string>
#include	<queue>
#include	<strstream>

#include	<cstdio>
#include	<cctype>
#include	<cstdarg>
#include	<cstring>
#include	<cstdlib>

#include	"config.h"
#include	"misc.h"
#include	"Numeric.h"
#include	"iClient.h"
#include	"iServer.h"
#include	"Network.h"
#include	"ip.h"

#include	"client.h"
#include	"EConfig.h"

#include	"ELog.h"
#include	"MessageNode.h"
#include	"events.h"

const char xClient_h_rcsId[] = __XCLIENT_H ;
const char xClient_cc_rcsId[] = "$Id: client.cc,v 1.2 2000/08/01 00:02:34 dan_karrels Exp $" ;

using std::string ;
using std::strstream ;

namespace gnuworld
{

xClient::xClient()
{
MyUplink = NULL ;
Connected = false ;

intYY = intXXX = 0 ;

memset( charYY, 0, sizeof( charYY ) ) ;
memset( charXXX, 0, sizeof( charXXX ) ) ;
}

xClient::xClient( const string& fileName )
{
EConfig conf( fileName ) ;
nickName = conf.Require( "nickname" )->second ;
userName = conf.Require( "username" )->second ;
hostName = conf.Require( "hostname" )->second ;
userDescription = conf.Require( "userdescription" )->second ;

Mode( conf.Require( "mode" )->second ) ;

MyUplink = 0 ;
Connected = false ;

intYY = intXXX = 0 ;

memset( charYY, 0, sizeof( charYY ) ) ;
memset( charXXX, 0, sizeof( charXXX ) ) ;

}

xClient::~xClient()
{
/*
while( !MessageQueue->empty() )
	{
	delete MessageQueue->front() ;
	MessageQueue->pop() ;
	}
delete MessageQueue ;
*/
}

// This method is called after being added
// to the network tables.
// It has been assigned int XX.
void xClient::ImplementServer( xServer* Server )
{
if( NULL == Server )
	{
	Connected = false ;
	elog << "ImplementServer> NULL Uplink\n" ;
	return ;
	}

MyUplink = Server ;

intYY = MyUplink->getIntYY() ;

inttobase64( charYY, intYY, 2 ) ;
inttobase64( charXXX, intXXX, 3 ) ;

}

int xClient::BurstChannels()
{
return 0 ;
}

int xClient::Connect( int ForceTime )
{
if( !Connected && MyUplink && MyUplink->isConnected() )
	{
	Connected = true ;
	return OnConnect() ;
	}
return -1 ;
}

int xClient::Exit( const string& Message )
{
if( !Connected )
	{
	return -1 ;
	}

char buf[ 256 ] = { 0 } ;

sprintf( buf, ":%s Q :%s\r\n",
	nickName.c_str(), Message.c_str() ) ;
if( QuoteAsServer( buf ) < 0 )
	{
	return -1 ;
	}
Connected = false ;
return OnQuit() ;
}

string xClient::getModes() const
{
string			Mode( "+" ) ;

if( mode & iClient::MODE_DEAF )		Mode += 'd' ;
if( mode & iClient::MODE_SERVICES )	Mode += 'k' ;
if( mode & iClient::MODE_OPER )		Mode += 'o' ;
if( mode & iClient::MODE_WALLOPS )	Mode += 'w' ;
if( mode & iClient::MODE_INVISIBLE )	Mode += 'i' ;

return Mode ;
}

int xClient::Mode( const string& Value )
{

// Set the bot's modes, and output to
// the network if we are connected

// Clear the internal modes
mode = 0 ;

// Iterate through the array and
// set modes appropriately
string::const_iterator ptr = Value.begin(),
	end = Value.end() ;

for( ; ptr != end ; ++ptr )
	{
	switch( *ptr )
		{
		case 'd': mode |= iClient::MODE_DEAF ; break;
		case 'k': mode |= iClient::MODE_SERVICES ; break;
		case 'o': mode |= iClient::MODE_OPER ; break;
		case 'w': mode |= iClient::MODE_WALLOPS ; break;
		case 'i': mode |= iClient::MODE_INVISIBLE ; break;
		}
	} // close while

// Output to the network if we are connected
if( MyUplink != NULL && MyUplink->isConnected() && !Value.empty() )
	{
	char buf[ 512 ] = { 0 } ;
	sprintf( buf, "%s MODE %s %s\r\n",
		getCharYYXXX().c_str(),
		getCharYYXXX().c_str(),
		Value.c_str() ) ;
	return QuoteAsServer( buf ) ;
	}

return( 1 ) ;
}

int xClient::QuoteAsServer( const string& Message )
{
if( MyUplink )
	{
	return MyUplink->Write( Message ) ;
	}
return -1 ;
}

int xClient::Wallops( const string& Message )
{
return QuoteAsServer( getCharYYXXX() + " WA :" + Message ) ;
}

int xClient::Wallops( const char* Format, ... )
{
if( Connected && MyUplink && Format && Format[ 0 ] != 0 )
	{
	char buffer[ 512 ] = { 0 } ;
	va_list list;

	va_start( list, Format ) ;
	vsprintf( buffer, Format, list ) ;
	va_end( list ) ;

	return MyUplink->Write( "%s WA :%s",
		getCharYYXXX().c_str(),
		buffer ) ;
	}
return -1 ;
}

int xClient::ModeAsServer( const string& Channel, const string& Mode )
{
if( Connected && MyUplink )
	{
	return MyUplink->Write( "%s M #%s %s\r\n",
		MyUplink->getCharYY(),
		(Channel[ 0 ] == '#' ? (Channel.c_str() + 1) :
			Channel.c_str()),
		Mode.c_str() ) ;
	}
return -1 ;
}

int xClient::ModeAsServer( const Channel* theChan, const string& Mode )
{
return ModeAsServer( theChan->getName(), Mode ) ;
}

int xClient::DoCTCP( iClient* Target,
	const string& CTCP,
	const string& Message )
{
if( !Connected && !MyUplink )
	{
	return -1 ;
	}

return MyUplink->Write( "%s O %s :\001%s %s\001\r\n",
	getCharYYXXX().c_str(),
	Target->getCharYYXXX().c_str(),
	CTCP.c_str(),
	Message.c_str() ) ;
}

int xClient::Message( const iClient* Target, const string& Message )
{
if( Connected && MyUplink )
	{
	return MyUplink->Write( "%s P %s :%s\r\n",
		getCharYYXXX().c_str(),
		Target->getCharYYXXX().c_str(),
		Message.c_str() ) ;
	}
return -1 ;
}

int xClient::Message( const iClient* Target, const char* Message, ... )
{
if( Connected && MyUplink && Message && Message[ 0 ] !=0 )
	{
	char buffer[ 512 ] = { 0 } ;
	va_list list;

	va_start( list, Message ) ;
	vsprintf( buffer, Message, list ) ;
	va_end( list ) ;

	return MyUplink->Write( "%s P %s :%s\r\n",
		getCharYYXXX().c_str(),
		Target->getCharYYXXX().c_str(),
		buffer ) ;
	}
	return -1;
}

int xClient::Message( const string& Channel, const char* Message, ... )
{
if( Connected && MyUplink && Message && Message[ 0 ] != 0 )
	{
	char buffer[ 512 ] = { 0 } ;
	va_list list ;

	va_start( list, Message ) ;
	vsprintf( buffer, Message, list ) ;
	va_end( list ) ;

	return MyUplink->Write( "%s P #%s :%s\r\n",
		getCharYYXXX().c_str(),
		(Channel[ 0 ] == '#') ? (Channel.c_str() + 1) :
			Channel.c_str(),
		buffer ) ;
	}
return -1 ;
}

int xClient::Notice( const iClient* Target, const string& Message )
{
if( Connected && MyUplink )
	{
	return MyUplink->Write( "%s O %s :%s\r\n",
		getCharYYXXX().c_str(),
		Target->getCharYYXXX().c_str(),
		Message.c_str() ) ;
	}
return -1 ;
}

int xClient::Notice( const iClient* Target, const char* Message, ... )
{
if( Connected && MyUplink && Message && Message[ 0 ] != 0 )
	{
	char buffer[ 512 ] = { 0 } ;
	va_list list;

	va_start(list, Message);
	vsprintf(buffer, Message, list);
	va_end(list);

	// O is the token for NOTICE, *shrug*
	return MyUplink->Write("%s O %s :%s\r\n",
		getCharYYXXX().c_str(),
		Target->getCharYYXXX().c_str(),
		buffer ) ;
	}
return -1 ;
}

int xClient::Notice( const string& Channel, const char* Message, ... )
{
if( Connected && MyUplink && Message && Message[ 0 ] != 0 )
	{
	char buffer[ 512 ] = { 0 } ;
	va_list list;

	va_start(list, Message);
	vsprintf(buffer, Message, list);
	va_end(list);

	return MyUplink->Write("%s O #%s :%s\r\n",
		getCharYYXXX().c_str(),
		('#' == Channel[ 0 ]) ? (Channel.c_str() + 1) :
			Channel.c_str(),
		buffer ) ;
	}
return -1 ;
}

int xClient::OnCTCP( iClient*, const string&,
	const string&, bool )
{
return 0;
}

int xClient::OnEvent( const eventType&, void*, void*, void*, void* )
{
return 0;
}

int xClient::OnChannelEvent( const channelEventType&, const string&,
	void*, void*, void*, void* )
{
return 0 ;
}

int xClient::OnPrivateMessage( iClient*, const string& )
{
return 0;
}

int xClient::OnConnect()
{
Connected = true ;
return 0 ;
}

int xClient::OnQuit()
{
Connected = false ;
return 0 ;
}

int xClient::OnKill()
{
Connected = false ;
return 0 ;
}

int xClient::Kill( iClient*, const string& )
{
return 0 ;
}

void xClient::ProcessMessageQueue()
{}

bool xClient::Op( Channel* theChan, iClient* theClient )
{
#ifndef NDEBUG
  assert( theChan != NULL && theClient != NULL ) ;
#endif

if( !Connected )
	{
	return false ;
	}

bool onChannel = isOnChannel( theChan ) ;
if( !onChannel )
	{
	Join( theChan ) ;
	}

Write( "%s M %s +o %s",
	getCharYYXXX().c_str(),
	theChan->getName().c_str(),
	theClient->getCharYYXXX().c_str() ) ;

if( !onChannel )
	{
	Part( theChan ) ;
	}

return true ;
}

bool xClient::Op( Channel* theChan, const vector< iClient* >& theClients )
{
#ifndef NDEBUG
  assert( theChan != NULL ) ;
#endif

if( !Connected )
	{
	return false ;
	}

return true ;
}

bool xClient::DeOp( Channel* theChan, iClient* theClient )
{
#ifndef NDEBUG
  assert( theChan != NULL && theClient != NULL ) ;
#endif

if( !Connected )
	{
	return false ;
	}

return true ;
}

bool xClient::DeOp( Channel* theChan, const vector< iClient* >& theClients )
{
#ifndef NDEBUG
  assert( theChan != NULL ) ;
#endif

if( !Connected )
	{
	return false ;
	}

return true ;
}

bool xClient::Ban( Channel* theChan, iClient* theClient )
{
#ifndef NDEBUG
  assert( theChan != NULL && theClient != NULL ) ;
#endif

if( !Connected )
	{
	return false ;
	}

return true ;
}

bool xClient::Ban( Channel* theChan, const vector< iClient* >& theClients )
{
#ifndef NDEBUG
  assert( theChan != NULL ) ;
#endif

if( !Connected )
	{
	return false ;
	}

return true ;
}

bool xClient::Kick( Channel* theChan, iClient* theClient, const string& reason )
{
#ifndef NDEBUG
  assert( theChan != NULL && theClient != NULL ) ;
#endif

if( !Connected )
	{
	return false ;
	}

if( NULL == theChan->findUser( theClient ) )
	{
	elog	<< "xClient::Kick> Can't find " << theClient->getNickName()
		<< " on channel " << theChan->getName() << endl ;
	return false ;
	}

strstream s ;
s	<< getCharYYXXX() << " K "
	<< theChan->getName() << ' '
	<< theClient->getCharYYXXX() << " :"
	<< reason << ends ;

Write( s ) ;
delete[] s.str() ;

// Notify the server
MyUplink->OnPartChannel( theClient, theChan ) ;

return true ;
}

bool xClient::Kick( Channel* theChan, const vector< iClient* >& theClients,
	const string& reason )
{
#ifndef NDEBUG
  assert( theChan != NULL ) ;
#endif

if( !Connected )
	{
	return false ;
	}

// We will assume that this client is on the channel pointed to by theChan

for( vector< iClient* >::const_iterator ptr = theClients.begin() ;
	ptr != theClients.end() ; ++ptr )
	{

	if( NULL == theChan->findUser( *ptr ) )
		{
		// The client is not on the channel
		continue ;
		}

	strstream s ;
	s	<< getCharYYXXX() << " K "
		<< theChan->getName() << ' '
		<< (*ptr)->getCharYYXXX() << " :"
		<< reason << ends ;

	Write( s ) ;
	delete[] s.str() ;

	// Notify the server that the user has parted the channel
	MyUplink->OnPartChannel( *ptr, theChan ) ;
	}

return true ;
}

bool xClient::Join( const string& chanName )
{
if( !Connected )
	{
	return false ;
	}

// Ask the server to join this bot into the given channel.
MyUplink->JoinChannel( this, chanName ) ;
return true ;
}

bool xClient::Join( Channel* theChan )
{
#ifndef NDEBUG
  assert( theChan != NULL ) ;
#endif

return Join( theChan->getName() ) ;
}

bool xClient::Part( const string& chanName )
{

if( !Connected )
	{
	return false ;
	}

// Ask the server to part us from the channel.
MyUplink->PartChannel( this, chanName ) ;

return true ;
}

bool xClient::Part( Channel* theChan )
{
#ifndef NDEBUG
  assert( theChan != NULL ) ;
#endif

return Part( theChan->getName() ) ;
}

bool xClient::Invite( iClient* theClient, const string& chanName )
{
#ifndef NDEBUG
  assert( theClient != NULL ) ;
#endif

Channel* theChan = Network->findChannel( chanName ) ;
if( NULL == theChan )
	{
	return false ;
	}

bool onChannel = isOnChannel( theChan ) ;
if( !onChannel )
	{
	Join( theChan ) ;
	}

Write( "%s I %s %s", getCharYYXXX().c_str(), theClient->getCharYYXXX().c_str(),
	theChan->getName().c_str() ) ;

if( !onChannel )
	{
	Part( theChan ) ;
	}

return true ;
}

bool xClient::Invite( iClient* theClient, Channel* theChan )
{

#ifndef NDEBUG
  assert( (theClient != 0) && (theChan != 0) ) ;
#endif

bool onChannel = isOnChannel( theChan ) ;
if( !onChannel )
	{
	Join( theChan ) ;
	}

Write( "%s I %s %s", getCharYYXXX().c_str(),
	theClient->getCharYYXXX().c_str(),
	theChan->getName().c_str() ) ;

if( !onChannel )
	{
	Part( theChan ) ;
	}

return true ; 
}

bool xClient::isOnChannel( const string& chanName ) const
{
return false ;
}

bool xClient::isOnChannel( const Channel* theChan ) const
{
#ifndef NDEBUG
  assert( theChan != NULL ) ;
#endif
return isOnChannel( theChan->getName() ) ;
}

int xClient::Write( const char* format, ... )
{
char buf[ 1024 ] = { 0 } ;
va_list _list ;

va_start( _list, format ) ;
vsprintf( buf, format, _list ) ;
va_end( _list ) ;

return Write( string( buf ) ) ;
}

} // namespace gnuworld
