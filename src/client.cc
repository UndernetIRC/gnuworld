/* client.cc
 */

#include	<string>
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
#include	"StringTokenizer.h"

#include	"ELog.h"
#include	"MessageNode.h"
#include	"events.h"

const char xClient_h_rcsId[] = __XCLIENT_H ;
const char xClient_cc_rcsId[] = "$Id: client.cc,v 1.20 2001/01/06 15:31:53 dan_karrels Exp $" ;

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
{}

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
#ifndef NDEBUG
  assert( theChan != 0 ) ;
#endif

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

int xClient::Message( const Channel* theChan, const string& Message )
{
#ifndef NDEBUG
  assert( theChan != 0 ) ;
#endif

if( Connected && MyUplink )
	{
	return MyUplink->Write( "%s P %s :%s",
		getCharYYXXX().c_str(),
		theChan->getName().c_str(),
		Message.c_str() ) ;
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

int xClient::OnChannelEvent( const channelEventType&, Channel*,
	void*, void*, void*, void* )
{
return 0 ;
}

void xClient::OnChannelModeM( Channel*,
	bool, ChannelUser* )
{
}

void xClient::OnChannelModeI( Channel*,
	bool, ChannelUser* )
{
}

void xClient::OnChannelModeP( Channel*,
	bool, ChannelUser* )
{
}

void xClient::OnChannelModeS( Channel*,
	bool, ChannelUser* )
{
}

void xClient::OnChannelModeN( Channel*,
	bool, ChannelUser* )
{
}

void xClient::OnChannelModeT( Channel*,
	bool, ChannelUser* )
{
}

void xClient::OnChannelModeL( Channel*,
	bool, ChannelUser*, const unsigned int& )
{
}

void xClient::OnChannelModeK( Channel*,
	bool, ChannelUser*, const string& )
{
}

void xClient::OnChannelModeO( Channel*, ChannelUser*,
	const xServer::opVectorType& )
{
}

void xClient::OnChannelModeV( Channel*, ChannelUser*,
	const xServer::voiceVectorType& )
{
}

void xClient::OnChannelModeB( Channel*, ChannelUser*,
	const xServer::banVectorType& )
{
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

int xClient::Kill( iClient* theClient, const string& reason )
{
#ifndef NDEBUG
  assert( theClient != 0 ) ;
#endif

Write( "%s D %s :%s",
	MyUplink->getCharYY(),
	theClient->getCharYYXXX().c_str(),
	reason.c_str() ) ;

/*
// Do NOT cast away constness
string localReason( reason ) ;

MyUplink->PostEvent( EVT_KILL,
	0,
	static_cast< void* >( theClient ),
	static_cast< void* >( &localReason ) ) ;

// Remove the user
delete Network->removeClient( theClient ) ;
*/

return 0 ;
}

bool xClient::Op( Channel* theChan, iClient* theClient )
{
#ifndef NDEBUG
  assert( (theChan != NULL) && (theClient != NULL) ) ;
#endif

if( !Connected )
	{
	return false ;
	}

ChannelUser* theUser = theChan->findUser( theClient ) ;
if( NULL == theUser )
	{
	elog	<< "xClient::Op> Unable to find ChannelUser: "
		<< *theClient << endl ;
	return false ;
	}

if( theUser->getMode( ChannelUser::MODE_O ) )
	{
	// User is already opped
	return true ;
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

theUser->setMode( ChannelUser::MODE_O ) ;

// TODO: Post message

return true ;
}

bool xClient::Op( Channel* theChan, const vector< iClient* >& theClients )
{
#ifndef NDEBUG
  assert( theChan != NULL ) ;
#endif
	unsigned short runCounter = 0;
	unsigned short totalCounter = 0;

if( !Connected )
	{
	return false ;
	}
 
string opList;    // Stack of numerics we're opping.
string paramList; // To build up our "+ooo"'s.

for( vector< iClient* >::const_iterator ptr = theClients.begin(); ptr != theClients.end() ; ++ptr )
	{ 
		ChannelUser* theUser = theChan->findUser( *ptr );
		if(theUser) // If on Channel.
		{
			if( !theUser->getMode( ChannelUser::MODE_O ) ) // If not alread opped..
			{
				opList += " " + (*ptr)->getCharYYXXX();
				paramList += "o";
				theUser->setMode( ChannelUser::MODE_O ) ;
				runCounter++; 
			}
			totalCounter++;

			if ((runCounter == 6) || (totalCounter == theClients.size()))
			{
				strstream s ;
				s	<< getCharYYXXX() << " M "
					<< theChan->getName() << " +" << paramList
					<< opList << ends ;
				Write( s ) ;
				delete[] s.str() ;
				opList = "";
				paramList = "";
				runCounter = 0;
			} 
		} 
	}
 
return true ;
}

bool xClient::Voice( Channel* theChan, const vector< iClient* >& theClients )
{
#ifndef NDEBUG
  assert( theChan != NULL ) ;
#endif
	unsigned short runCounter = 0;
	unsigned short totalCounter = 0;

if( !Connected )
	{
	return false ;
	}
 
string voiceList; // Stack of numerics we're Voicing.
string paramList; // To build up our "+vvv"'s.

for( vector< iClient* >::const_iterator ptr = theClients.begin(); ptr != theClients.end() ; ++ptr )
	{ 
		ChannelUser* theUser = theChan->findUser( *ptr );
		if(theUser) // If on Channel.
		{
			if( !theUser->getMode( ChannelUser::MODE_V ) ) // If not alread voiced..
			{
				voiceList += " " + (*ptr)->getCharYYXXX();
				paramList += "v";
				theUser->setMode( ChannelUser::MODE_V ) ;
				runCounter++; 
			}
			totalCounter++;

			if ((runCounter == 6) || (totalCounter == theClients.size()))
			{
				strstream s ;
				s	<< getCharYYXXX() << " M "
					<< theChan->getName() << " +" << paramList
					<< voiceList << ends ;
				Write( s ) ;
				delete[] s.str() ;
				voiceList = "";
				paramList = "";
				runCounter = 0;
			} 
		} 
	}
 
return true ;
}

bool xClient::Voice( Channel* theChan, iClient* theClient )
{
#ifndef NDEBUG
  assert( (theChan != NULL) && (theClient != NULL) ) ;
#endif

if( !Connected )
	{
	return false ;
	}

ChannelUser* theUser = theChan->findUser( theClient ) ;
if( NULL == theUser )
	{
	elog	<< "xClient::Voice> Unable to find ChannelUser: "
		<< *theClient << endl ;
	return false ;
	}

if( theUser->getMode( ChannelUser::MODE_V ) )
	{
	// User is already voiced
	return true ;
	}

bool onChannel = isOnChannel( theChan ) ;
if( !onChannel )
	{
	Join( theChan ) ;
	}

Write( "%s M %s +v %s",
	getCharYYXXX().c_str(),
	theChan->getName().c_str(),
	theClient->getCharYYXXX().c_str() ) ;

if( !onChannel )
	{
	Part( theChan ) ;
	}

theUser->setMode( ChannelUser::MODE_V ) ;

// TODO: Post message

return true ;
} 
 
bool xClient::DeOp( Channel* theChan, iClient* theClient )
{
#ifndef NDEBUG
  assert( (theChan != NULL) && (theClient != NULL) ) ;
#endif

if( !Connected )
	{
	return false ;
	}

ChannelUser* theUser = theChan->findUser( theClient ) ;
if( NULL == theUser )
	{
	elog	<< "xClient::Op> Unable to find ChannelUser: "
		<< *theClient << endl ;
	return false ;
	}

if( !theUser->getMode( ChannelUser::MODE_O ) )
	{
	// User is not opped
	return true ;
	}

bool onChannel = isOnChannel( theChan ) ;
if( !onChannel )
	{
	Join( theChan ) ;
	}

Write( "%s M %s -o %s",
	getCharYYXXX().c_str(),
	theChan->getName().c_str(),
	theClient->getCharYYXXX().c_str() ) ;

if( !onChannel )
	{
	Part( theChan ) ;
	}

// Update the user's channel state
theUser->removeMode( ChannelUser::MODE_O ) ;

// TODO: Post message

return true ;
}

bool xClient::DeOp( Channel* theChan, const vector< iClient* >& theClients )
{
#ifndef NDEBUG
  assert( theChan != NULL ) ;
#endif
	unsigned short runCounter = 0;
	unsigned short totalCounter = 0;

if( !Connected )
	{
	return false ;
	}
 
string deopList;  // Stack of numerics we're de-opping.
string paramList; // To build up our "-ooo"'s.

for( vector< iClient* >::const_iterator ptr = theClients.begin(); ptr != theClients.end() ; ++ptr )
	{ 
		ChannelUser* theUser = theChan->findUser( *ptr );
		if(theUser) // If on Channel.
		{
			if( theUser->getMode( ChannelUser::MODE_O ) ) // If opped..
			{
				deopList += " " + (*ptr)->getCharYYXXX();
				paramList += "o";
				theUser->removeMode( ChannelUser::MODE_O ) ;
				runCounter++; 
			}
			totalCounter++;

			if ((runCounter == 6) || (totalCounter == theClients.size()))
			{
				strstream s ;
				s	<< getCharYYXXX() << " M "
					<< theChan->getName() << " -" << paramList
					<< deopList << ends ;
				Write( s ) ;
				delete[] s.str() ;
				deopList = "";
				paramList = "";
				runCounter = 0;
			} 
		} 
	}
 
return true ;
}
 
bool xClient::DeVoice( Channel* theChan, iClient* theClient )
{
#ifndef NDEBUG
  assert( (theChan != NULL) && (theClient != NULL) ) ;
#endif

if( !Connected )
	{
	return false ;
	}

ChannelUser* theUser = theChan->findUser( theClient ) ;
if( NULL == theUser )
	{
	elog	<< "xClient::Voice> Unable to find ChannelUser: "
		<< *theClient << endl ;
	return false ;
	}

if( !theUser->getMode( ChannelUser::MODE_V ) )
	{
	// User is not voiced
	return true ;
	}

bool onChannel = isOnChannel( theChan ) ;
if( !onChannel )
	{
	Join( theChan ) ;
	}

Write( "%s M %s -v %s",
	getCharYYXXX().c_str(),
	theChan->getName().c_str(),
	theClient->getCharYYXXX().c_str() ) ;

if( !onChannel )
	{
	Part( theChan ) ;
	}

// Update the user's channel state
theUser->removeMode( ChannelUser::MODE_V ) ;

// TODO: Post message

return true ;
}

bool xClient::DeVoice( Channel* theChan, const vector< iClient* >& theClients )
{
#ifndef NDEBUG
  assert( theChan != NULL ) ;
#endif
	unsigned short runCounter = 0;
	unsigned short totalCounter = 0;

if( !Connected )
	{
	return false ;
	}
 
string devoiceList;  // Stack of numerics we're de-voicing.
string paramList;    // To build up our "-vvv"'s.

for( vector< iClient* >::const_iterator ptr = theClients.begin(); ptr != theClients.end() ; ++ptr )
	{ 
		ChannelUser* theUser = theChan->findUser( *ptr );
		if(theUser) // If on Channel.
		{
			if( theUser->getMode( ChannelUser::MODE_V ) ) // If voiced..
			{
				devoiceList += " " + (*ptr)->getCharYYXXX();
				paramList += "v";
				theUser->removeMode( ChannelUser::MODE_V ) ;
				runCounter++; 
			}
			totalCounter++;

			if ((runCounter == 6) || (totalCounter == theClients.size()))
			{
				strstream s ;
				s	<< getCharYYXXX() << " M "
					<< theChan->getName() << " -" << paramList
					<< devoiceList << ends ;
				Write( s ) ;
				delete[] s.str() ;
				devoiceList = "";
				paramList = "";
				runCounter = 0;
			} 
		} 
	}
 
return true ;
}
 
bool xClient::Ban( Channel* theChan, iClient* theClient )
{
#ifndef NDEBUG
  assert( (theChan != NULL) && (theClient != NULL) ) ;
#endif

if( !Connected )
	{
	return false ;
	}

if( 0 == theChan->findUser( theClient ) )
	{
	// User is not on that channel
	return true ;
	}

bool onChannel = isOnChannel( theChan ) ;
if( !onChannel )
	{
	Join( theChan ) ;
	}

string banMask = makeBan( theChan, theClient ) ;

Write( "%s M %s +b :%s",
	getCharYYXXX().c_str(),
	theChan->getName().c_str(),
	banMask.c_str() ) ;

// Update the channel's ban list
theChan->setBan( banMask ) ;

// No users are kicked by just setting a ban.

if( !onChannel )
	{
	Part( theChan ) ;
	}

return true ;
}

bool xClient::UnBan( Channel* theChan, const string& banMask )
{

#ifndef NDEBUG
  assert( theChan != 0 ) ;
#endif

if( !Connected )
	{
	return false ;
	}

if( !theChan->findBan( banMask ) )
	{
	return true ;
	}

// Ban exists, remove it
bool onChannel = isOnChannel( theChan ) ;
if( !onChannel )
	{
	Join( theChan ) ;
	}

Write( "%s M %s -b %s",
	getCharYYXXX().c_str(),
	theChan->getName().c_str(),
	banMask.c_str() ) ;

if( !onChannel )
	{
	Part( theChan ) ;
	}

theChan->removeBan( banMask ) ;

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

bool xClient::BanKick( Channel* theChan, iClient* theClient, const string& reason )
{
#ifndef NDEBUG
  assert( (theChan != NULL) && (theClient != NULL) ) ;
#endif

if( !Connected )
	{
	return false ;
	}

if( 0 == theChan->findUser( theClient ) )
	{
	// User is not on that channel
	return true ;
	}

bool onChannel = isOnChannel( theChan ) ;
if( !onChannel )
	{
	Join( theChan ) ;
	}

string banMask = makeBan( theChan, theClient ) ;

Write( "%s M %s +b :%s",
	getCharYYXXX().c_str(),
	theChan->getName().c_str(),
	banMask.c_str() ) ;

Write( "%s K %s %s :%s",
	getCharYYXXX().c_str(),
	theChan->getName().c_str(),
	theClient->getCharYYXXX().c_str(),
	reason.c_str() ) ;

if( !onChannel )
	{
	Part( theChan ) ;
	}

// Update the channel's ban list
theChan->setBan( banMask ) ;

// Let the server know about the departure of the client
// This will handle posting the message
MyUplink->OnPartChannel( theClient, theChan ) ;

return true ;
}

string xClient::makeBan( Channel* theChan, iClient* theClient )
{
#ifndef NDEBUG
  assert( (theChan != 0) && (theClient != 0) ) ;
#endif

// Setup the ban mask.

// Ban all nicks, and non-identd'd hosts
string banMask = "*!*" ;

// Add the username
if( !theClient->getUserName().empty() )
	{
	banMask += (theClient->getUserName().c_str() + 1) ;
	}

// Add the @ symbol between user@host
banMask += "@" ;

// Parse the user's hostname
StringTokenizer st( theClient->getInsecureHost(), '.' ) ;
if( st.size() < 2 )
	{
	elog	<< "xClient::makeBan> Invalid hostname: "
		<< st.toString() << endl ;
	return (banMask + "*") ;
	}

if( Socket::isIPAddress( theClient->getInsecureHost() ) )
	{
	banMask += st[ 0 ] + '.' + st[ 1 ] + ".*" ;
	}
else
	{
	// Hostname
	// Remove everything in the host except the last two tokens
	banMask += "*" ;
	banMask += st.assemble( st.size() - 2 ) ;
	}

return banMask ;
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

bool onChannel = isOnChannel( theChan ) ;
if( !onChannel )
	{
	Join( theChan ) ;
	}

strstream s ;
s	<< getCharYYXXX() << " K "
	<< theChan->getName() << ' '
	<< theClient->getCharYYXXX() << " :"
	<< reason << ends ;

Write( s ) ;
delete[] s.str() ;

if( !onChannel )
	{
	Part( theChan ) ;
	}

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

if( theClients.empty() )
	{
	return true ;
	}

bool onChannel = isOnChannel( theChan ) ;
if( !onChannel )
	{
	Join( theChan ) ;
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

if( !onChannel )
	{
	Part( theChan ) ;
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

Write( "%s I %s %s",
	getCharYYXXX().c_str(),
	theClient->getNickName().c_str(),
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

Write( "%s I %s %s",
	getCharYYXXX().c_str(),
	theClient->getNickName().c_str(),
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

void xClient::OnJoin( Channel* theChan )
{
#ifndef NDEBUG
  assert( theChan != 0 ) ;
#endif

addChan( theChan ) ;
}

void xClient::OnJoin( const string& chanName )
{
Channel* theChan = Network->findChannel( chanName ) ;
if( NULL == theChan )
	{
	elog	<< "xClient::OnJoin> Failed to find channel: "
		<< chanName << endl ;
	return ;
	}
OnJoin( theChan ) ;
}

void xClient::OnPart( Channel* theChan )
{
#ifndef NDEBUG
  assert( theChan != 0 ) ;
#endif

removeChan( theChan ) ;
}

void xClient::OnPart( const string& chanName )
{
Channel* theChan = Network->findChannel( chanName ) ;
if( NULL == theChan )
	{
	elog	<< "xClient::OnPart> Failed to find channel: "
		<< chanName << endl ;
	return ;
	}
OnPart( theChan ) ;
}

bool xClient::addChan( Channel* )
{
return true ;
}

bool xClient::removeChan( Channel* )
{
return true ;
}

int xClient::OnTimer( xServer::timerID ID, void* data )
{
return 0 ;
}

int xClient::OnSignal( int whichSig )
{
return 0 ;
}

} // namespace gnuworld
