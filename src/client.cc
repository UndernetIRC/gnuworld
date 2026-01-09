/**
 * client.cc
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
 *		Orlando Bassotto
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 * $Id: client.cc,v 1.89 2008/04/16 20:29:39 danielaustin Exp $
 */

#include	<new>
#include	<map>
#include	<string>
#include	<sstream>
#include	<vector>
#include	<iostream>

#include	<cstdio>
#include	<cctype>
#include	<cstdarg>
#include	<cstring>
#include	<cstdlib>

#include	"gnuworld_config.h"
#include	"misc.h"
#include	"iClient.h"
#include	"iServer.h"
#include	"Network.h"
#include	"ip.h"
#include	"NetworkTarget.h"
#include	"client.h"
#include	"EConfig.h"
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"events.h"

namespace gnuworld
{

using std::string ;
using std::stringstream ;
using std::endl ;
using std::make_pair ;

xClient::xClient()
{
me = 0 ;
MyUplink = NULL ;
}

xClient::xClient( const string& fileName )
 : configFileName( fileName )
{
MyUplink = 0 ;

EConfig conf( fileName ) ;
nickName = conf.Require( "nickname" )->second ;
userName = conf.Require( "username" )->second ;
hostName = conf.Require( "hostname" )->second ;
userDescription = conf.Require( "userdescription" )->second ;

Mode( conf.Require( "mode" )->second ) ;

/* Initialize logger */
logger = std::make_unique< Logger >( this ) ;
}

xClient::~xClient()
{}

void xClient::BurstChannels()
{}

bool xClient::BurstGlines()
{
return true ;
}

void xClient::OnAttach()
{
// We are connected to an xServer here.
Connected = true; 
}

void xClient::OnDetach( const string& Message )
{
if( !isConnected() )
	{
	return ;
	}

stringstream s ;
s	<< getCharYYXXX()
	<< " Q :"
	<< Message ;

MyUplink->Write( s ) ;

//xClient is no longer connected to the xServer.
Connected = false;
}

string xClient::getModes() const
{
string			Mode( "+" ) ;

if( mode & iClient::MODE_DEAF )		Mode += 'd' ;
if( mode & iClient::MODE_SERVICES )	Mode += 'k' ;
if( mode & iClient::MODE_OPER )		Mode += 'o' ;
if( mode & iClient::MODE_WALLOPS )	Mode += 'w' ;
if( mode & iClient::MODE_INVISIBLE )	Mode += 'i' ;
if( mode & iClient::MODE_TLS )		Mode += 'z' ;

return Mode ;
}

bool xClient::Mode( const string& Value )
{
//elog	<< "xClient::Mode> Value: "
//	<< Value
//	<< endl ;

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
		case '+': break ;
		case 'd': mode |= iClient::MODE_DEAF ; break;
		case 'k': mode |= iClient::MODE_SERVICES ; break;
		case 'o': mode |= iClient::MODE_OPER ; break;
		case 'w': mode |= iClient::MODE_WALLOPS ; break;
		case 'i': mode |= iClient::MODE_INVISIBLE ; break;
		case 'z': mode |= iClient::MODE_TLS ; break;

		default:
			elog	<< "xClient::Mode> Unknown mode: "
				<< *ptr
				<< endl ;
			break ;
		} // switch()
	} // close while

// Output to the network if we are connected
if( isConnected() && !Value.empty() )
	{
	stringstream s ;
	s	<< getCharYYXXX()
		<< " M "
		<< getCharYYXXX()
		<< " "
		<< Value ;

	return MyUplink->Write( s ) ;
	}

return false ;
}

bool xClient::QuoteAsServer( const string& Message )
{
if( isConnected() )
	{
	return MyUplink->Write( Message ) ;
	}
return false ;
}

bool xClient::Wallops( const string& Message )
{
return Write( getCharYYXXX() + " WA :" + Message ) ;
}

bool xClient::Wallops( const char* Format, ... )
{
if( isConnected() && Format && Format[ 0 ] != 0 )
	{
	char buffer[ 1024 ] ;
	memset( buffer, 0, 1024 ) ;
	va_list list;

	va_start( list, Format ) ;
	vsnprintf( buffer, 1024, Format, list ) ;
	va_end( list ) ;

	return MyUplink->Write( "%s WA :%s",
		getCharYYXXX().c_str(),
		buffer ) ;
	}
return false ;
}

bool xClient::WallopsAsServer( const string& buf )
{
if( !isConnected() )
	{
	return false ;
	}
return MyUplink->Wallops( buf ) ;
}

bool xClient::WallopsAsServer( const char* Format, ... )
{
if( isConnected() && Format && Format[ 0 ] != 0 )
	{
	char buffer[ 1024 ] = { 0 } ;
	va_list list;

	va_start( list, Format ) ;
	vsnprintf( buffer, 1024, Format, list ) ;
	va_end( list ) ;

	return MyUplink->Wallops( buffer ) ;
	}
return false ;
}

bool xClient::Mode( const string& chanName,
	const string& modes,
	const string& args,
	bool modeAsServer )
{
if( !isConnected() )
	{
	return false ;
	}

Channel* theChan = Network->findChannel( chanName ) ;
if( 0 == theChan )
	{
	return false ;
	}

return Mode( theChan, modes, args, modeAsServer ) ;
}

bool xClient::Mode( Channel* theChan,
	const string& modes,
	const string& args,
	bool modeAsServer )
{
assert( theChan != 0 ) ;

if( !isConnected() )
	{
	return false ;
	}

bool doJoinPart = false ;
if( !modeAsServer && !isOnChannel( theChan ) )
	{
	doJoinPart = true ;
	Join( theChan, string(), 0, true ) ;
	}

xClient* theClient = 0 ;
if( !modeAsServer )
	{
	// Set the mode as the client, so theClient needs to
	// be non-NULL
	theClient = this ;
	}

bool retVal = getUplink()->Mode( theClient, theChan, modes, args ) ;

if( doJoinPart )
	{
	Part( theChan ) ;
	}
return retVal ;
}

bool xClient::DoCTCP( iClient* Target,
	const string& CTCP,
	const string& Message )
{
if( !isConnected() )
	{
	return false ;
	}

string ctcpReply( "\001" ) ;
ctcpReply += CTCP ;

// Be careful not to include an extra space inside of the CTCP reply
// if Message is empty
if( !Message.empty() )
	{
	ctcpReply += string( " " ) + Message ;
	}
ctcpReply += "\001" ;

return MyUplink->Write( "%s O %s :%s\r\n",
	getCharYYXXX().c_str(),
	Target->getCharYYXXX().c_str(),
	ctcpReply.c_str() ) ;
}

bool xClient::DoFakeCTCP( const iClient* destClient,
	const iClient* srcClient,
	const string& CTCP,
	const string& Message )
{
assert( destClient != 0 ) ;
assert( srcClient != 0 ) ;

if( !isConnected() )
	{
	return false ;
	}

string ctcpReply( "\001" ) ;
ctcpReply += CTCP ;

// Be careful not to include an extra space inside of the CTCP reply
// if Message is empty
if( !Message.empty() )
	{
	ctcpReply += string( " " ) + Message ;
	}
ctcpReply += "\001" ;

return MyUplink->Write( "%s O %s :%s\r\n",
	srcClient->getCharYYXXX().c_str(),
	destClient->getCharYYXXX().c_str(),
	ctcpReply.c_str() ) ;
}

bool xClient::FakeMessage( const iClient* destClient,
	const iClient* srcClient,
	const string& Message )
{
assert( destClient != 0 ) ;
assert( srcClient != 0 ) ;

if( Message.empty() || !isConnected() )
	{
	return false ;
	}

return getUplink()->Write( "%s P %s :%s",
	srcClient->getCharYYXXX().c_str(),
	destClient->getCharYYXXX().c_str(),
	Message.c_str() ) ;
}

bool xClient::FakeNotice( const iClient* destClient,
	const iClient* srcClient,
	const string& Message )
{
assert( destClient != 0 ) ;
assert( srcClient != 0 ) ;

if( Message.empty() || !isConnected() )
	{
	return false ;
	}

return getUplink()->Write( "%s O %s :%s",
	srcClient->getCharYYXXX().c_str(),
	destClient->getCharYYXXX().c_str(),
	Message.c_str() ) ;
}

bool xClient::FakeMessage( const Channel* theChan,
	const iClient* srcClient,
	const string& Message )
{
assert( theChan != 0 ) ;
assert( srcClient != 0 ) ;

if( Message.empty() || !isConnected() )
	{
	return false ;
	}

return getUplink()->Write( "%s P %s :%s",
	srcClient->getCharYYXXX().c_str(),
	theChan->getName().c_str(),
	Message.c_str() ) ;
}

bool xClient::FakeNotice( const Channel* theChan,
	const iClient* srcClient,
	const string& Message )
{
assert( theChan != 0 ) ;
assert( srcClient != 0 ) ;

if( Message.empty() || !isConnected() )
	{
	return false ;
	}

return getUplink()->Write( "%s P %s :%s",
	srcClient->getCharYYXXX().c_str(),
	theChan->getName().c_str(),
	Message.c_str() ) ;
}

bool xClient::Message( const iClient* Target, const string& Message )
{
if( isConnected() )
	{
	return MyUplink->Write( "%s P %s :%s\r\n",
		getCharYYXXX().c_str(),
		Target->getCharYYXXX().c_str(),
		Message.c_str() ) ;
	}
return false ;
}

bool xClient::Message( const iClient* Target, const char* Message, ... )
{
if( isConnected() && Message && Message[ 0 ] !=0 )
	{
	char buffer[ 1024 ] ;
	memset( buffer, 0, 1024 ) ;
	va_list list;

	va_start( list, Message ) ;
	vsnprintf( buffer, 1024, Message, list ) ;
	va_end( list ) ;

	return MyUplink->Write( "%s P %s :%s\r\n",
		getCharYYXXX().c_str(),
		Target->getCharYYXXX().c_str(),
		buffer ) ;
	}
return false ;
}

bool xClient::Message( const string& Channel, const char* Message, ... )
{
if( isConnected() && Message && Message[ 0 ] != 0 )
	{
	char buffer[ 1024 ] = { 0 } ;
	va_list list ;

	va_start( list, Message ) ;
	vsnprintf( buffer, 1024, Message, list ) ;
	va_end( list ) ;

	return MyUplink->Write( "%s P #%s :%s\r\n",
		getCharYYXXX().c_str(),
		(Channel[ 0 ] == '#') ? (Channel.c_str() + 1) :
			Channel.c_str(),
		buffer ) ;
	}
return false ;
}

bool xClient::Message( const Channel* theChan, const string& Message )
{
assert( theChan != 0 ) ;

if( isConnected() )
	{
	return MyUplink->Write( "%s P %s :%s",
		getCharYYXXX().c_str(),
		theChan->getName().c_str(),
		Message.c_str() ) ;
	}
return false ;
}

bool xClient::Message( const string& chanName, const string& Message )
{
if( chanName.empty() || Message.empty() || !isConnected() )
	{
	return false ;
	}

return MyUplink->Write( "%s P %s :%s",
	getCharYYXXX().c_str(),
	chanName.c_str(),
	Message.c_str() ) ;
}

bool xClient::Notice( const iClient* Target, const string& Message )
{
//elog	<< "xClient::Notice( const iClient* )"
//	<< endl ;

if( !isConnected() )
	{
	return false ;
	}

return MyUplink->Write( "%s O %s :%s\r\n",
	getCharYYXXX().c_str(),
	Target->getCharYYXXX().c_str(),
	Message.c_str() ) ;
}

bool xClient::Notice( const iClient* Target, const char* Message, ... )
{
//elog	<< "xClient::Notice( const iClient* )"
//	<< endl ;

if( isConnected() && Message && Message[ 0 ] != 0 )
	{
	char buffer[ 1024 ] ;
	memset( buffer, 0, 1024 ) ;
	va_list list;

	va_start(list, Message);
	vsnprintf(buffer, 1024, Message, list);
	va_end(list);

	// O is the token for NOTICE, *shrug*
	return Notice( Target, string( buffer ) ) ;
	}
return false ;
}

bool xClient::Notice( const string& Channel, const char* Message, ... )
{
//elog	<< "xClient::Notice( const string& Channel )"
//	<< endl ;

if( isConnected() && Message && Message[ 0 ] != 0 )
	{
	char buffer[ 1024 ] ;
	memset( buffer, 0, 1024 ) ;
	va_list list;

	va_start(list, Message);
	vsnprintf(buffer, 1024, Message, list);
	va_end(list);

	return MyUplink->Write("%s O #%s :%s\r\n",
		getCharYYXXX().c_str(),
		('#' == Channel[ 0 ]) ? (Channel.c_str() + 1) :
			Channel.c_str(),
		buffer ) ;
	}
return false ;
}

bool xClient::Notice( const Channel* theChan, const string& Message )
{
assert( theChan != 0 ) ;

//elog	<< "xClient::Notice( const Channel* )> name: "
//	<< theChan->getName()
//	<< endl ;

if( Message.empty() || !isConnected() )
	{
	return false ;
	}

return MyUplink->Write( "%s O %s :%s\r\n",
	getCharYYXXX().c_str(),
	theChan->getName().c_str(),
	Message.c_str() ) ;
}

bool xClient::Notice( const Channel* theChan, const char* Message, ... )
{
assert( theChan != 0 ) ;

if( isConnected() && Message && Message[ 0 ] != 0 )
	{
	char buffer[ 1024 ] ;
	memset( buffer, 0, 1024 ) ;
	va_list list;

	va_start(list, Message);
	vsnprintf(buffer, 1024, Message, list);
	va_end(list);

	return Notice( theChan, string( buffer ) ) ;
	}
return false ;
}

bool xClient::NoticeChannelOps( const Channel* theChan, const char* Message, ... )
{
	assert( theChan != 0 ) ;

	if( isConnected() && Message && Message[ 0 ] != 0 )
	{
		char buffer[ 1024 ] ;
		memset( buffer, 0, 1024 ) ;
		va_list list;

		va_start(list, Message);
		vsnprintf(buffer, 1024, Message, list);
		va_end(list);

		if (!MyUplink->Write("%s WC %s :%s\r\n",
				getCharYYXXX().c_str(),
				theChan->getName().c_str(),
				buffer))
			return false;
	}
	return true ;
}

bool xClient::NoticeChannelOps( const string& chanName, const char* Message, ... )
{
	Channel* theChan = Network->findChannel(chanName) ;
	if (0 == theChan)
	{
		return false ;
	}
	if( isConnected() && Message && Message[ 0 ] != 0 )
	{
		char buffer[ 1024 ] ;
		memset( buffer, 0, 1024 ) ;
		va_list list;

		va_start(list, Message);
		vsnprintf(buffer, 1024, Message, list);
		va_end(list);

		if (!MyUplink->Write("%s WC %s :%s\r\n",
				getCharYYXXX().c_str(),
				theChan->getName().c_str(),
				buffer))
			return false;
	}
	return true ;
}

bool xClient::Message( const Channel* theChan, const char* Message, ... )
{
assert( theChan != 0 ) ;

if( isConnected() && Message && Message[ 0 ] != 0 )
	{
	char buffer[ 1024 ] ;
	memset( buffer, 0, 1024 ) ;
	va_list list;

	va_start(list, Message);
	vsnprintf(buffer, 1024, Message, list);
	va_end(list);

	return MyUplink->Write("%s P %s :%s\r\n",
		getCharYYXXX().c_str(),
		theChan->getName().c_str(),
		buffer ) ;
	}
return false ;
}

void xClient::OnCTCP( iClient*, const string&,
	const string&, bool )
{}

void xClient::OnFakeCTCP( iClient*, iClient*, const string&,
	const string&, bool )
{}

void xClient::OnChannelCTCP( iClient*, Channel*, const string&,
	const string& )
{}

void xClient::OnFakeChannelCTCP( iClient*, iClient*,
	Channel*, const string&,
	const string& )
{}

void xClient::OnEvent( const eventType&, void*, void*, void*, void* )
{}

void xClient::OnChannelEvent( const channelEventType&, Channel*,
	void*, void*, void*, void* )
{}

void xClient::OnNetworkKick( Channel*,
	iClient*, // srcClient, may be NULL
	iClient*, // destClient
	const string&, // kickMessage,
	bool ) // authoritative
{}

void xClient::OnChannelMode( Channel*, ChannelUser*,
	const xServer::modeVectorType& )
{}

void xClient::OnChannelModeL( Channel*,
	bool, ChannelUser*, const unsigned int& )
{}

void xClient::OnChannelModeK( Channel*,
	bool, ChannelUser*, const string& )
{}

void xClient::OnChannelModeA( Channel*,
	bool, ChannelUser*, const string& )
{}

void xClient::OnChannelModeU( Channel*,
	bool, ChannelUser*, const string& )
{}

void xClient::OnChannelModeO( Channel*, ChannelUser*,
	const xServer::opVectorType& )
{}

void xClient::OnChannelModeV( Channel*, ChannelUser*,
	const xServer::voiceVectorType& )
{}

void xClient::OnChannelModeB( Channel*, ChannelUser*,
	const xServer::banVectorType& )
{}

void xClient::OnPrivateMessage( iClient*, const string&, bool )
{}

void xClient::OnFakePrivateMessage( iClient*, iClient*,
	const string&, bool )
{}

void xClient::OnFakeChannelMessage( iClient*, iClient*,
	Channel*, const string& )
{}

void xClient::OnChannelMessage( iClient*, Channel*, const string& )
{}

void xClient::OnPrivateNotice( iClient*, const string&, bool )
{}

void xClient::OnFakePrivateNotice( iClient*, iClient*,
	const string&, bool )
{}

void xClient::OnChannelNotice( iClient*, Channel*, const string& )
{}

void xClient::OnFakeChannelNotice( iClient*, iClient*,
	Channel*, const string& )
{}

void xClient::OnServerMessage( iServer*, const string&, bool )
{}

void xClient::OnConnect()
{}

void xClient::OnDisconnect()
{}

void xClient::OnShutdown( const string& /* reason */ )
{}

void xClient::OnKill()
{}

void xClient::OnWhois( iClient*, iClient* )
{}

void xClient::OnInvite( iClient*, Channel* )
{}

bool xClient::Kill( iClient* theClient, const string& reason )
{
return Kill( theClient, reason, true ) ;
}

bool xClient::Kill( iClient* theClient, const string& reason, bool asServer )
{
assert( theClient != 0 ) ;

if( theClient->isModeK() || !isConnected() )
	{
	return false ;
	}

if( asServer )
	{
	Write( "%s D %s :%s (%s)",
		MyUplink->getCharYY().c_str(),
		theClient->getCharYYXXX().c_str(),
		MyUplink->getName().c_str(),
		reason.c_str() ) ;
	}
else
	{
	Write( "%s D %s :%s (%s)",
		getCharYYXXX().c_str(),
		theClient->getCharYYXXX().c_str(),
		getNickName().c_str(),
		reason.c_str() ) ;
	}

// Why was all this commented out? -- gk
// beats me -- dan

// Do NOT cast away constness
string localReason( reason ) ;

MyUplink->PostEvent( EVT_KILL,
	0,
	static_cast< void* >( theClient ),
	static_cast< void* >( &localReason ) ) ;

// Remove the user
delete Network->removeClient( theClient ) ;

return true ;
}

bool xClient::Op( Channel* theChan, iClient* theClient )
{
assert( theChan != NULL ) ;
assert( theClient != NULL) ;

if( !isConnected() )
	{
	return false ;
	}

ChannelUser* theUser = theChan->findUser( theClient ) ;
if( NULL == theUser )
	{
	elog	<< "xClient::Op> Unable to find ChannelUser: "
		<< *theClient
		<< endl ;
	return false ;
	}

if( theUser->getMode( ChannelUser::MODE_O ) )
	{
	// User is already opped
	return true ;
	}

bool OnChannel = isOnChannel( theChan ) ;
if( !OnChannel )
	{
	// Join, giving ourselves ops
	Join( theChan, string(), 0, true ) ;
	}
else
	{
	// Bot is already on the channel
	ChannelUser* meUser = theChan->findUser( me ) ;
	if( NULL == meUser )
		{
		elog	<< "xClient::Op> Unable to find myself in "
			<< "channel: "
			<< theChan->getName()
			<< endl ;
		return false ;
		}

	// Make sure we have ops
	if( !meUser->getMode( ChannelUser::MODE_O ) )
		{
		// The bot does NOT have ops
		return false ;
		}

	// The bot has ops
	}

// Op the user
Write( "%s M %s +o %s %ld",
	getCharYYXXX().c_str(),
	theChan->getName().c_str(),
	theClient->getCharYYXXX().c_str(),
	theChan->getCreationTime() ) ;

// Was the bot on the channel previously?
if( !OnChannel )
	{
	Part( theChan ) ;
	}

xServer::opVectorType opVector ;
opVector.push_back( xServer::opVectorType::value_type(
	true, theUser ) ) ;

MyUplink->OnChannelModeO( theChan, 0, opVector ) ;

return true ;
}

bool xClient::Op( Channel* theChan,
	const std::vector< iClient* >& clientVector )
{
assert( theChan != NULL ) ;

if( !isConnected() )
	{
	return false ;
	}

bool OnChannel = isOnChannel( theChan ) ;
if( !OnChannel )
	{
	// Join, giving ourselves ops
	Join( theChan, string(), 0, true ) ;
	}
else
	{
	// Bot is already on the channel
	ChannelUser* meUser = theChan->findUser( me ) ;
	if( NULL == meUser )
		{
		elog	<< "xClient::Op> Unable to find myself in "
			<< "channel: "
			<< theChan->getName()
			<< endl ;
		return false ;
		}

	// Make sure we have ops
	if( !meUser->getMode( ChannelUser::MODE_O ) )
		{
		// The bot does NOT have ops
		return false ;
		}

	// The bot has ops
	}

xServer::opVectorType opVector ;

for( std::vector< iClient* >::const_iterator ptr = clientVector.begin(),
	end = clientVector.end() ; ptr != end ; ++ptr )
	{
	if( NULL == *ptr )
		{
		elog	<< "xClient::Op(vector)> Found NULL "
			<< "iClient!"
			<< endl ;
		continue ;
		}

	ChannelUser* theUser = theChan->findUser( *ptr ) ;
	if( NULL == theUser )
		{
		elog	<< "xClient::Op(vector)> Unable to find "
			<< "client on channel: "
			<< theChan->getName()
			<< endl ;
		continue ;
		}

	if( !theUser->getMode( ChannelUser::MODE_O ) )
		{
		// User is not already opped
		opVector.push_back( xServer::opVectorType::value_type(
			true, theUser ) ) ;
		}
	}

string modeString ;
string args ;

for( xServer::opVectorType::const_iterator ptr = opVector.begin(),
	end = opVector.end() ; ptr != end ; ++ptr )
	{
	modeString += 'o' ;
	args += ptr->second->getCharYYXXX() + ' ' ;

	if( (MAX_CHAN_MODES == modeString.size()) ||
		((ptr + 1) == end) )
		{
		stringstream s ;
		s	<< getCharYYXXX() << " M "
			<< theChan->getName() << ' '
			<< "+" << modeString << ' ' << args
			<< " " << theChan->getCreationTime();

		Write( s ) ;

		modeString.erase( modeString.begin(), modeString.end() ) ;
		args.erase( args.begin(), args.end() ) ;

		} // if()

	} // for()

MyUplink->OnChannelModeO( theChan, 0, opVector ) ;

if( !OnChannel )
	{
	Part( theChan ) ;
	}
 
return true ;
}

bool xClient::Voice( Channel* theChan,
	const std::vector< iClient* >& clientVector )
{
assert( theChan != NULL ) ;

if( !isConnected() )
	{
	return false ;
	}

bool OnChannel = isOnChannel( theChan ) ;
if( !OnChannel )
	{
	// Join, giving ourselves ops
	Join( theChan, string(), 0, true ) ;
	}
else
	{
	// Bot is already on the channel
	ChannelUser* meUser = theChan->findUser( me ) ;
	if( NULL == meUser )
		{
		elog	<< "xClient::Voice> Unable to find myself in "
			<< "channel: "
			<< theChan->getName()
			<< endl ;
		return false ;
		}

	// Make sure we have ops
	if( !meUser->getMode( ChannelUser::MODE_O ) )
		{
		// The bot does NOT have ops
		return false ;
		}

	// The bot has ops
	}

xServer::voiceVectorType voiceVector ;

for( std::vector< iClient* >::const_iterator ptr = clientVector.begin(),
	end = clientVector.end() ; ptr != end ; ++ptr )
	{
	if( NULL == *ptr )
		{
		elog	<< "xClient::Voice(vector)> Found NULL "
			<< "iClient for channel: "
			<< theChan->getName()
			<< endl ;
		continue ;
		}

	ChannelUser* theUser = theChan->findUser( *ptr ) ;
	if( NULL == theUser )
		{
		elog	<< "xClient::Voice(vector)> Unable to find "
			<< "client on channel: "
			<< theChan->getName()
			<< endl ;
		continue ;
		}

	if( !theUser->getMode( ChannelUser::MODE_V ) )
		{
		// User is not already voiced
		voiceVector.push_back(
			xServer::voiceVectorType::value_type(
			true, theUser ) ) ;
		}
	}

string modeString ;
string args ;

for( xServer::voiceVectorType::const_iterator ptr = voiceVector.begin(),
	end = voiceVector.end() ; ptr != end ; ++ptr )
	{
	modeString += 'v' ;
	args += ptr->second->getCharYYXXX() + ' ' ;

	if( (MAX_CHAN_MODES == modeString.size()) ||
		((ptr + 1) == end) )
		{
		stringstream s ;
		s	<< getCharYYXXX() << " M "
			<< theChan->getName() << ' '
			<< "+" << modeString << ' ' << args ;

		Write( s ) ;

		modeString.erase( modeString.begin(), modeString.end() ) ;
		args.erase( args.begin(), args.end() ) ;

		} // if()

	} // for()

MyUplink->OnChannelModeV( theChan, 0, voiceVector ) ;

if( !OnChannel )
	{
	Part( theChan ) ;
	}
 
return true ;
}

bool xClient::Voice( Channel* theChan, iClient* theClient )
{
assert( theChan != NULL ) ;
assert( theClient != NULL) ;

if( !isConnected() )
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

bool OnChannel = isOnChannel( theChan ) ;
if( !OnChannel )
	{
	// Join, giving ourselves ops
	Join( theChan, string(), 0, true ) ;
	}
else
	{
	// Bot is already on the channel
	ChannelUser* meUser = theChan->findUser( me ) ;
	if( NULL == meUser )
		{
		elog	<< "xClient::Voice> Unable to find myself in "
			<< "channel: "
			<< theChan->getName()
			<< endl ;
		return false ;
		}

	// Make sure we have ops
	if( !meUser->getMode( ChannelUser::MODE_O ) )
		{
		// The bot does NOT have ops
		return false ;
		}

	// The bot has ops
	}

Write( "%s M %s +v %s",
	getCharYYXXX().c_str(),
	theChan->getName().c_str(),
	theClient->getCharYYXXX().c_str() ) ;

if( !OnChannel )
	{
	Part( theChan ) ;
	}

xServer::voiceVectorType voiceVector ;
voiceVector.push_back( xServer::voiceVectorType::value_type(
	true, theUser ) ) ;

MyUplink->OnChannelModeV( theChan, 0, voiceVector ) ;

return true ;
} 
 
bool xClient::DeOp( Channel* theChan, iClient* theClient )
{
assert( theChan != NULL ) ;
assert( theClient != NULL ) ;

if( !isConnected() )
	{
	return false ;
	}

if( theClient->isModeK() )
	{
	return false ;
	}

ChannelUser* theUser = theChan->findUser( theClient ) ;
if( NULL == theUser )
	{
	elog	<< "xClient::DeOp> Unable to find ChannelUser: "
		<< *theClient
		<< endl ;
	return false ;
	}

if( !theUser->getMode( ChannelUser::MODE_O ) )
	{
	// User is not opped
	return true ;
	}

bool OnChannel = isOnChannel( theChan ) ;
if( !OnChannel )
	{
	// Join, giving ourselves ops
	Join( theChan, string(), 0, true ) ;
	}
else
	{
	// Bot is already on the channel
	ChannelUser* meUser = theChan->findUser( me ) ;
	if( NULL == meUser )
		{
		elog	<< "xClient::DeOp> Unable to find myself in "
			<< "channel: "
			<< theChan->getName()
			<< endl ;
		return false ;
		}

	// Make sure we have ops
	if( !meUser->getMode( ChannelUser::MODE_O ) )
		{
		// The bot does NOT have ops
		return false ;
		}

	// The bot has ops
	}

Write( "%s M %s -o %s %ld",
	getCharYYXXX().c_str(),
	theChan->getName().c_str(),
	theClient->getCharYYXXX().c_str(),
	theChan->getCreationTime() ) ;

if( !OnChannel )
	{
	Part( theChan ) ;
	}

xServer::opVectorType opVector ;
opVector.push_back( xServer::opVectorType::value_type(
	false, theUser ) ) ;

MyUplink->OnChannelModeO( theChan, 0, opVector ) ;

if( !OnChannel )
	{
	Part( theChan ) ;
	}

return true ;
}

bool xClient::DeOp( Channel* theChan,
	const std::vector< iClient* >& clientVector )
{
assert( theChan != NULL ) ;

if( !isConnected() )
	{
	return false ;
	}

bool OnChannel = isOnChannel( theChan ) ;
if( !OnChannel )
	{
	// Join, giving ourselves ops
	Join( theChan, string(), 0, true ) ;
	}
else
	{
	// Bot is already on the channel
	ChannelUser* meUser = theChan->findUser( me ) ;
	if( NULL == meUser )
		{
		elog	<< "xClient::DeOp> Unable to find myself in "
			<< "channel: "
			<< theChan->getName()
			<< endl ;
		return false ;
		}

	// Make sure we have ops
	if( !meUser->getMode( ChannelUser::MODE_O ) )
		{
		// The bot does NOT have ops
		return false ;
		}

	// The bot has ops
	}

xServer::opVectorType opVector ;

for( std::vector< iClient* >::const_iterator ptr = clientVector.begin(),
	end = clientVector.end() ; ptr != end ; ++ptr )
	{
	if( NULL == *ptr )
		{
		elog	<< "xClient::DeOp(vector)> Found NULL "
			<< "iClient!"
			<< endl ;
		continue ;
		}

	if( (*ptr)->isModeK() )
		{
		continue ;
		}

	ChannelUser* theUser = theChan->findUser( *ptr ) ;
	if( NULL == theUser )
		{
		elog	<< "xClient::DeOp(vector)> Unable to find "
			<< "client on channel: "
			<< theChan->getName()
			<< endl ;
		continue ;
		}

	if( theUser->getMode( ChannelUser::MODE_O ) )
		{
		// User is opped
		opVector.push_back( xServer::opVectorType::value_type(
			false, theUser ) ) ;
		}
	}

string modeString ;
string args ;

for( xServer::opVectorType::const_iterator ptr = opVector.begin(),
	end = opVector.end() ; ptr != end ; ++ptr )
	{
	modeString += 'o' ;
	args += ptr->second->getCharYYXXX() + ' ' ;

	if( (MAX_CHAN_MODES == modeString.size()) ||
		((ptr + 1) == end) )
		{
		stringstream s ;
		s	<< getCharYYXXX() << " M "
			<< theChan->getName() << ' '
			<< "-" << modeString << ' ' << args
			<< theChan->getCreationTime() ;

		Write( s ) ;

		modeString.erase( modeString.begin(), modeString.end() ) ;
		args.erase( args.begin(), args.end() ) ;

		} // if()

	} // for()

MyUplink->OnChannelModeO( theChan, 0, opVector ) ;

if( !OnChannel )
	{
	Part( theChan ) ;
	}
 
return true ;
}
 
bool xClient::DeVoice( Channel* theChan, iClient* theClient )
{
assert( theChan != 0 ) ;
assert( theClient != 0 ) ;

if( !isConnected() )
	{
	return false ;
	}

ChannelUser* theUser = theChan->findUser( theClient ) ;
if( NULL == theUser )
	{
	elog	<< "xClient::DeVoice> Unable to find ChannelUser: "
		<< *theClient
		<< endl ;
	return false ;
	}

if( !theUser->getMode( ChannelUser::MODE_V ) )
	{
	// User is not voiced
	return true ;
	}

bool OnChannel = isOnChannel( theChan ) ;
if( !OnChannel )
	{
	// Join, giving ourselves ops
	Join( theChan, string(), 0, true ) ;
	}
else
	{
	// Bot is already on the channel
	ChannelUser* meUser = theChan->findUser( me ) ;
	if( NULL == meUser )
		{
		elog	<< "xClient::DeVoice> Unable to find myself in "
			<< "channel: "
			<< theChan->getName()
			<< endl ;
		return false ;
		}

	// Make sure we have ops
	if( !meUser->getMode( ChannelUser::MODE_O ) )
		{
		// The bot does NOT have ops
		return false ;
		}

	// The bot has ops
	}

Write( "%s M %s -v %s",
	getCharYYXXX().c_str(),
	theChan->getName().c_str(),
	theClient->getCharYYXXX().c_str() ) ;

xServer::voiceVectorType voiceVector ;
voiceVector.push_back( xServer::voiceVectorType::value_type(
	false, theUser ) ) ;

MyUplink->OnChannelModeV( theChan, 0, voiceVector ) ;

if( !OnChannel )
	{
	Part( theChan ) ;
	}

return true ;
}

bool xClient::DeVoice( Channel* theChan,
	const std::vector< iClient* >& clientVector )
{
assert( theChan != NULL ) ;

if( !isConnected() )
	{
	return false ;
	}

bool OnChannel = isOnChannel( theChan ) ;
if( !OnChannel )
	{
	// Join, giving ourselves ops
	Join( theChan, string(), 0, true ) ;
	}
else
	{
	// Bot is already on the channel
	ChannelUser* meUser = theChan->findUser( me ) ;
	if( NULL == meUser )
		{
		elog	<< "xClient::DeVoic> Unable to find myself in "
			<< "channel: "
			<< theChan->getName()
			<< endl ;
		return false ;
		}

	// Make sure we have ops
	if( !meUser->getMode( ChannelUser::MODE_O ) )
		{
		// The bot does NOT have ops
		return false ;
		}

	// The bot has ops
	}

xServer::voiceVectorType voiceVector ;

for( std::vector< iClient* >::const_iterator ptr = clientVector.begin(),
	end = clientVector.end() ; ptr != end ; ++ptr )
	{
	if( NULL == *ptr )
		{
		elog	<< "xClient::DeVoice(vector)> Found NULL "
			<< "iClient!"
			<< endl ;
		continue ;
		}

	ChannelUser* theUser = theChan->findUser( *ptr ) ;
	if( NULL == theUser )
		{
		elog	<< "xClient::DeVoice(vector)> Unable to find "
			<< "client on channel: "
			<< theChan->getName()
			<< endl ;
		continue ;
		}

	if( theUser->getMode( ChannelUser::MODE_V ) )
		{
		// User is voiced
		voiceVector.push_back(
			xServer::voiceVectorType::value_type(
			false, theUser ) ) ;
		}
	}

string modeString ;
string args ;

for( xServer::voiceVectorType::const_iterator ptr = voiceVector.begin(),
	end = voiceVector.end() ; ptr != end ; ++ptr )
	{
	modeString += 'v' ;
	args += ptr->second->getCharYYXXX() + ' ' ;

	if( (MAX_CHAN_MODES == modeString.size()) ||
		((ptr + 1) == end) )
		{
		stringstream s ;
		s	<< getCharYYXXX() << " M "
			<< theChan->getName() << ' '
			<< "-" << modeString << ' ' << args ;

		Write( s ) ;

		modeString.erase( modeString.begin(), modeString.end() ) ;
		args.erase( args.begin(), args.end() ) ;

		} // if()

	} // for()

MyUplink->OnChannelModeV( theChan, 0, voiceVector ) ;

if( !OnChannel )
	{
	Part( theChan ) ;
	}
 
return true ;
}
 
bool xClient::Ban( Channel* theChan, iClient* theClient )
{
assert( theChan != NULL ) ;
assert( theClient != NULL ) ;

if( !isConnected() )
	{
	return false ;
	}

if( theClient->isModeK() )
	{
	return false ;
	}

if( 0 == theChan->findUser( theClient ) )
	{
	// User is not on that channel
	return true ;
	}

bool OnChannel = isOnChannel( theChan ) ;
if( !OnChannel )
	{
	// Join, giving ourselves ops
	Join( theChan, string(), 0, true ) ;
	}
else
	{
	// Bot is already on the channel
	ChannelUser* meUser = theChan->findUser( me ) ;
	if( NULL == meUser )
		{
		elog	<< "xClient::Ban> Unable to find myself in "
			<< "channel: "
			<< theChan->getName()
			<< endl ;
		return false ;
		}

	// Make sure we have ops
	if( !meUser->getMode( ChannelUser::MODE_O ) )
		{
		// The bot does NOT have ops
		return false ;
		}

	// The bot has ops
	}

string banMask = Channel::createBan( theClient ) ;

Write( "%s M %s +b :%s",
	getCharYYXXX().c_str(),
	theChan->getName().c_str(),
	banMask.c_str() ) ;

// No users are kicked by just setting a ban.

xServer::banVectorType banVector ;
banVector.push_back( xServer::banVectorType::value_type(
	true, banMask ) ) ;

MyUplink->OnChannelModeB( theChan, 0, banVector ) ;

if( !OnChannel )
	{
	Part( theChan ) ;
	}

return true ;
}

bool xClient::UnBan( Channel* theChan, const string& banMask )
{
assert( theChan != 0 ) ;

if( !isConnected() )
	{
	return false ;
	}

if( !theChan->findBan( banMask ) )
	{
	return true ;
	}

// Ban exists, remove it
bool OnChannel = isOnChannel( theChan ) ;
if( !OnChannel )
	{
	// Join, giving ourselves ops
	Join( theChan, string(), 0, true ) ;
	}
else
	{
	// Bot is already on the channel
	ChannelUser* meUser = theChan->findUser( me ) ;
	if( NULL == meUser )
		{
		elog	<< "xClient::UnBan> Unable to find myself in "
			<< "channel: "
			<< theChan->getName()
			<< endl ;
		return false ;
		}

	// Make sure we have ops
	if( !meUser->getMode( ChannelUser::MODE_O ) )
		{
		// The bot does NOT have ops
		return false ;
		}

	// The bot has ops
	}

Write( "%s M %s -b %s",
	getCharYYXXX().c_str(),
	theChan->getName().c_str(),
	banMask.c_str() ) ;

xServer::banVectorType banVector ;
banVector.push_back( xServer::banVectorType::value_type(
	false, banMask ) ) ;

MyUplink->OnChannelModeB( theChan, 0, banVector ) ;

if( !OnChannel )
	{
	Part( theChan ) ;
	}

return true ;
}

bool xClient::UnBan( Channel* theChan,
	const xServer::banVectorType& banVector )
{
assert( theChan != nullptr ) ;

if( !isConnected() )
	{
	return false ;
	}

bool OnChannel = isOnChannel( theChan ) ;
if( !OnChannel )
	{
	// Join, giving ourselves ops
	Join( theChan, string(), 0, true ) ;
	}
else
	{
	// Bot is already on the channel
	ChannelUser* meUser = theChan->findUser( me ) ;
	if( nullptr == meUser )
		{
		elog	<< "xClient::UnBan> Unable to find myself in "
			<< "channel: "
			<< theChan->getName()
			<< endl ;
		return false ;
		}

	// Make sure we have ops
	if( !meUser->getMode( ChannelUser::MODE_O ) )
		{
		// The bot does NOT have ops
		return false ;
		}

	// The bot has ops
	}

string modeString { } ;
string args { } ;

for( xServer::banVectorType::const_iterator banPtr = banVector.begin(),
	end = banVector.end(); banPtr != end ; ++banPtr)
	{
	modeString += 'b' ;
	args += banPtr->second + ' ' ;

	if( ( MAX_CHAN_MODES == modeString.size() ) ||
		( ( banPtr + 1 ) == end ) )
		{
		stringstream s ;
		s	<< getCharYYXXX() << " M "
			<< theChan->getName() << ' '
			<< "-" << modeString << ' ' << args ;

		Write( s ) ;

		modeString.erase( modeString.begin(), modeString.end() ) ;
		args.erase( args.begin(), args.end() ) ;
		}
	} // for() banVector

MyUplink->OnChannelModeB( theChan, 0,
	const_cast< xServer::banVectorType& >( banVector ) ) ;

if( !OnChannel )
	{
	Part( theChan ) ;
	}

return true ;
}

bool xClient::Ban( Channel* theChan,
	const std::vector< iClient* >& clientVector )
{
assert( theChan != nullptr ) ;

if( !isConnected() )
	{
	return false ;
	}

xServer::banVectorType banVector ;

for( std::vector< iClient* >::const_iterator ptr = clientVector.begin(),
	end = clientVector.end() ; ptr != end ; ++ptr )
	{
	if( nullptr == *ptr )
		{
		elog	<< "xClient::Ban(vector)> Found NULL "
			<< "iClient!"
			<< endl ;
		continue ;
		}

	if( (*ptr)->isModeK() )
		{
		continue ;
		}

	ChannelUser* theUser = theChan->findUser( *ptr ) ;
	if( nullptr == theUser )
		{
		elog	<< "xClient::Ban(vector)> Unable to find "
			<< "client on channel: "
			<< theChan->getName()
			<< endl ;
		continue ;
		}

	banVector.push_back( xServer::banVectorType::value_type(
		true, Channel::createBan( *ptr ) ) ) ;
	}

return Ban( theChan, banVector ) ;
}

bool xClient::Ban( Channel* theChan,
	const xServer::banVectorType& banVector )
{
assert( theChan != nullptr ) ;

if( !isConnected() )
	{
	return false ;
	}

bool OnChannel = isOnChannel( theChan ) ;
if( !OnChannel )
	{
	// Join, giving ourselves ops
	Join( theChan, string(), 0, true ) ;
	}
else
	{
	// Bot is already on the channel
	ChannelUser* meUser = theChan->findUser( me ) ;
	if( nullptr == meUser )
		{
		elog	<< "xClient::Ban> Unable to find myself in "
			<< "channel: "
			<< theChan->getName()
			<< endl ;
		return false ;
		}

	// Make sure we have ops
	if( !meUser->getMode( ChannelUser::MODE_O ) )
		{
		// The bot does NOT have ops
		return false ;
		}

	// The bot has ops
	}

string modeString { } ;
string args { } ;

for( xServer::banVectorType::const_iterator ptr = banVector.begin(),
	end = banVector.end() ; ptr != end ; ++ptr )
	{
	modeString += 'b' ;
	args += ptr->second + ' ' ;

	if( ( MAX_CHAN_MODES == modeString.size() ) ||
		( ( ptr + 1 ) == end ) )
		{
		stringstream s ;
		s	<< getCharYYXXX() << " M "
			<< theChan->getName() << ' '
			<< "+" << modeString << ' ' << args ;

		Write( s ) ;

		modeString.erase( modeString.begin(), modeString.end() ) ;
		args.erase( args.begin(), args.end() ) ;

		} // if()

	} // for()

MyUplink->OnChannelModeB( theChan, 0,
	const_cast< xServer::banVectorType& >( banVector ) ) ;

if( !OnChannel )
	{
	Part( theChan ) ;
	}
 
return true ;
}

bool xClient::BanKick( Channel* theChan, iClient* theClient,
	const string& reason )
{
assert( theChan != 0 ) ;
assert( theClient != 0 ) ;

if( !isConnected() )
	{
	return false ;
	}

if( theClient->isModeK() )
	{
	return false ;
	}

if( 0 == theChan->findUser( theClient ) )
	{
	// User is not on that channel
	return true ;
	}

bool OnChannel = isOnChannel( theChan ) ;
if( !OnChannel )
	{
	// Join, giving ourselves ops
	Join( theChan, string(), 0, true ) ;
	}
else
	{
	// Bot is already on the channel
	ChannelUser* meUser = theChan->findUser( me ) ;
	if( NULL == meUser )
		{
		elog	<< "xClient::BanKick> Unable to find myself in "
			<< "channel: "
			<< theChan->getName()
			<< endl ;
		return false ;
		}

	// Make sure we have ops
	if( !meUser->getMode( ChannelUser::MODE_O ) )
		{
		// The bot does NOT have ops
		return false ;
		}

	// The bot has ops
	}

string banMask = Channel::createBan( theClient ) ;

Write( "%s M %s +b :%s",
	getCharYYXXX().c_str(),
	theChan->getName().c_str(),
	banMask.c_str() ) ;

Write( "%s K %s %s :%s",
	getCharYYXXX().c_str(),
	theChan->getName().c_str(),
	theClient->getCharYYXXX().c_str(),
	reason.c_str() ) ;

if( !OnChannel )
	{
	Part( theChan ) ;
	}

// Update the channel's ban list
theChan->setBan( banMask ) ;

return true ;
}

bool xClient::Topic( Channel* theChan, const std::string& newTopic )
{
assert( theChan != 0 ) ;
// Empty newTopic is ok

if( !isConnected() )
	{
	return false ;
	}

bool OnChannel = isOnChannel( theChan ) ;
if( !OnChannel )
	{
	// Join, giving ourselves ops
	Join( theChan, string(), 0, true ) ;
	}

// Now joined the channel
// If the channel is mode +t, and the bot -o, op the bot before
// trying to set the topic.
ChannelUser* theUser = theChan->findUser( getInstance() ) ;

// By definition (of the above Join()), this client is on the
// channel, but check to be sure.
assert( theUser != 0 ) ;

if( theChan->getMode( Channel::MODE_T ) && !theUser->isModeO() )
	{
	// Mode +t, and bot is mode -o
	// Op the bot
	theUser->setMode( ChannelUser::MODE_O ) ;

	stringstream s ;
	s	<< getUplink()->getCharYY()
		<< " M "
		<< theChan->getName()
		<< " +o "
		<< getCharYYXXX() ;
	Write( s ) ;
	}

// Bot is on channel, and has privileges to change the topic
stringstream s ;
s	<< getCharYYXXX()
	<< " T "
	<< theChan->getName()
	<< " :"
	<< newTopic ;
Write( s ) ;

#ifdef TOPIC_TRACK
  theChan->setTopic( newTopic ) ;
#endif

if( !OnChannel )
	{
	Part( theChan ) ;
	}

return true ;
}

bool xClient::Kick( Channel* theChan, iClient* theClient,
	const string& reason,
	bool modeAsServer)
{
assert( theChan != NULL ) ;
assert( theClient != NULL ) ;

if( theClient->isModeK() )
	{
	return false ;
	}

if( NULL == theChan->findUser( theClient ) )
	{
	elog	<< "xClient::Kick> Can't find "
		<< theClient->getNickName()
		<< " on channel "
		<< theChan->getName()
		<< endl ;
	return false ;
	}

bool OnChannel = isOnChannel( theChan ) ;
if( !OnChannel && !modeAsServer)
	{
	// Join, giving ourselves ops
	Join( theChan, string(), 0, true ) ;
	}
else if (!modeAsServer)
	{
	// Bot already on channel
	// Make sure we have ops
	ChannelUser* meUser = theChan->findUser( getInstance() ) ;
	assert( meUser != 0 ) ;

	if( !meUser->getMode( ChannelUser::MODE_O ) )
		{
		// The bot does NOT have ops
		return false ;
		}

	// The bot has ops
	}

string SendAs = modeAsServer ? getCharYY() : getCharYYXXX();

stringstream s ;
s	<< SendAs << " K "
	<< theChan->getName() << ' '
	<< theClient->getCharYYXXX() << " :"
	<< reason ;

Write( s ) ;

// On a network with more than 2 servers, the chances are greater
// that the two clients will be on different servers...thus,
// initialize localKick to false here.
bool localKick = false ;

// Check to see if the source and destination user are
// on the same server.
if (theClient->getIntYY() == this->getIntYY())
{
	// Local kick
	localKick = true ;
}
/*
 * Remove/cleanup now the client, an eventual later part message from a server will be ignored anyway
 * In this way any own (fake)clients will be cleaned up too
 */
ChannelUser* destChanUser = theChan->findUser( theClient ) ;
if (destChanUser != NULL)
{
	theChan->removeUser( theClient ) ;

	// Deallocate the ChannelUser
	delete destChanUser ; destChanUser = 0 ;

	// Remove the channel information from the client's internal channel structure
	if (!theClient->removeChannel(theChan))
	{
		elog	<< "xClient::Kick> Unable to remove channel "
			<< theChan->getName()
			<< " from the iClient "
			<< *theClient
			<< endl ;
	}
	MyUplink->PostChannelKick(theChan, getInstance(), theClient, reason, localKick) ;

} else elog << "xClient::Kick> destChanUser == NULL !!" << endl;

if( !OnChannel && !modeAsServer)
	{
	Part( theChan ) ;
	}
// Any users or services clients left in the channel?
if (theChan->empty())
{
	// Nope, remove the channel
	delete Network->removeChannel( theChan->getName() ) ;
	// TODO: Post event
}
return true ;
}

bool xClient::Kick( Channel* theChan,
	const std::vector< iClient* >& theClients,
	const string& reason,
	bool modeAsServer)
{
assert( theChan != NULL ) ;

if( !isConnected() )
	{
	return false ;
	}

if( theClients.empty() )
	{
	return true ;
	}

bool OnChannel = isOnChannel( theChan ) ;
if( !OnChannel && !modeAsServer)
	{
	// Join, giving ourselves ops
	Join( theChan, string(), 0, true ) ;
	}
else if (!modeAsServer)
	{
	// Bot is already on the channel
	ChannelUser* meUser = theChan->findUser( me ) ;
	assert( meUser != 0 ) ;

	// Make sure we have ops
	if( !meUser->getMode( ChannelUser::MODE_O ) )
		{
		// The bot does NOT have ops
		return false ;
		}

	// The bot has ops
	}

// We will assume that this client is on the channel pointed to by theChan

for( std::vector< iClient* >::const_iterator ptr = theClients.begin() ;
	ptr != theClients.end() ; ++ptr )
	{
	if( 0 == *ptr )
		{
		elog	<< "xClient::Kick(vector)> NULL target client"
			<< endl ;
		continue ;
		}

	if( (*ptr)->isModeK() )
		{
		continue ;
		}

	if( NULL == theChan->findUser( *ptr ) )
		{
		// The client is not on the channel
		continue ;
		}

	string SendAs = modeAsServer ? getCharYY() : getCharYYXXX();

	stringstream s ;
	s	<< SendAs << " K "
		<< theChan->getName() << ' '
		<< (*ptr)->getCharYYXXX() << " :"
		<< reason ;

	Write( s ) ;

	iClient* theClient = *ptr;

	// On a network with more than 2 servers, the chances are greater
	// that the two clients will be on different servers...thus,
	// initialize localKick to false here.
	bool localKick = false ;

	// Check to see if the source and destination user are
	// on the same server.
	if (theClient->getIntYY() == this->getIntYY())
	{
		// Local kick
		localKick = true ;
	}
	/*
	 * Remove/cleanup now the client, an eventual later part message from a server will be ignored anyway
	 * In this way any own (fake)clients will be cleaned up too
	 */
	ChannelUser* destChanUser = theChan->findUser( theClient ) ;
	if (destChanUser != NULL)
	{
		theChan->removeUser( theClient ) ;

		// Deallocate the ChannelUser
		delete destChanUser ; destChanUser = 0 ;

		// Remove the channel information from the client's internal channel structure
		if (!theClient->removeChannel(theChan))
		{
			elog	<< "xClient::Kick[]> Unable to remove channel "
				<< theChan->getName()
				<< " from the iClient "
				<< *theClient
				<< endl ;
		}
		MyUplink->PostChannelKick(theChan, getInstance(), theClient, reason, localKick) ;

	} else elog << "xClient::Kick[]> destChanUser == NULL !!" << endl;
	}

if( !OnChannel && !modeAsServer)
	{
	Part( theChan ) ;
	}
// Any users or services clients left in the channel?
if (theChan->empty())
{
	// Nope, remove the channel
	delete Network->removeChannel( theChan->getName() ) ;
	// TODO: Post event
}
return true ;
}

bool xClient::Kick( Channel* theChan,
	const string& IP,
	const string& reason,
	bool modeAsServer)
{
assert( theChan != NULL ) ;

if( !isConnected() )
	{
	return false ;
	}

if ( IP.empty() )
	{
	return true ;
	}

bool OnChannel = isOnChannel( theChan ) ;
if( !OnChannel && !modeAsServer)
	{
	// Join, giving ourselves ops
	Join( theChan, string(), 0, true ) ;
	}
else if (!modeAsServer)
	{
	// Bot is already on the channel
	ChannelUser* meUser = theChan->findUser( me ) ;
	assert( meUser != 0 ) ;

	// Make sure we have ops
	if( !meUser->getMode( ChannelUser::MODE_O ) )
		{
		// The bot does NOT have ops
		return false ;
		}

	// The bot has ops
	}
	std::vector <iClient*> toBoot;
	for(Channel::userIterator chanUsers = theChan->userList_begin(); chanUsers != theChan->userList_end(); ++chanUsers)
	{
		ChannelUser* tmpUser = chanUsers->second;
		string currIP = xIP(tmpUser->getClient()->getIP()).GetNumericIP();
		string currIP64 = xIP(tmpUser->getClient()->getIP()).GetNumericIP(true);
		/* Idented and unidented clients need to be handled separately
		 * In case of floodpro kick, IP can be in format of ident@ip !
		 */
		if (IP.find('@') != string::npos)
		{
			currIP = tmpUser->getUserName() + "@" + currIP;
			currIP64 = tmpUser->getUserName() + "@" + currIP64;
		}
		if ((!IP.compare(currIP)) || (!IP.compare(currIP64)))
		{
			/* Don't kick +k things */
			if (!tmpUser->getClient()->getMode(iClient::MODE_SERVICES))
			{
				toBoot.push_back(tmpUser->getClient());
			}
		}
	}
	return Kick(theChan, toBoot, reason, modeAsServer);
}

bool xClient::Join( const string& chanName,
	const string& chanModes,
	const time_t& joinTime,
	bool getOps )
{
Channel* theChan = Network->findChannel(chanName);
return theChan ? Join(theChan,chanModes,joinTime,getOps) :
    MyUplink->JoinChannel(this,chanName,chanModes,joinTime,getOps);
}

bool xClient::Join( Channel* theChan,
	const string& chanModes,
	const time_t& joinTime,
	bool getOps )
{
if ( !isConnected() )
	{
	return false;
	}
assert( theChan != NULL ) ;
if( ( joinTime > 0 ) && ( joinTime < theChan->getCreationTime() ) )
	{
	//The join is older than the creation time of the channel, need to remove all the modes of the channel
	theChan->removeAllModes();
	//Now set the channel creation ts to the join ts
	theChan->setCreationTime(joinTime);
	}

return MyUplink->JoinChannel( this, theChan->getName(), chanModes, joinTime, getOps ) ;
}

bool xClient::Part( const string& chanName, const string& reason )
{
if( !isConnected() )
	{
	return false ;
	}

// Ask the server to part us from the channel.
MyUplink->PartChannel( this, chanName, reason ) ;

return true ;
}

bool xClient::Part( Channel* theChan )
{
assert( theChan != NULL ) ;

return Part( theChan->getName() ) ;
}

bool xClient::Invite( iClient* theClient, const string& chanName )
{
// No need for this assert as we dont use theClient and its tested in
// Invite(iClient*,Channel*)
//assert( theClient != NULL ) ;

Channel* theChan = Network->findChannel( chanName ) ;
if( 0 == theChan )
	{
	return false ;
	}

return Invite(theClient, theChan);
}

bool xClient::Invite( iClient* theClient, Channel* theChan )
{
assert( theClient != 0 ) ;
assert( theChan != 0 ) ;

if( !isConnected() )
	{
	return false ;
	}

/* bool OnChannel = isOnChannel( theChan ) ;
if( !OnChannel )
	{
	Join( theChan ) ;
	} */

Write( "%s I %s %s",
	getCharYYXXX().c_str(),
	theClient->getNickName().c_str(),
	theChan->getName().c_str() ) ;

/* if( !OnChannel )
	{
	Part( theChan ) ;
	} */

return true ; 
}

bool xClient::isOnChannel( const string& chanName ) const
{
Channel* theChannel = Network->findChannel(chanName);
if( 0 == theChannel )
	{
	return false ;
	}
return isOnChannel(theChannel) ;
}

bool xClient::isOnChannel( const Channel* theChan ) const
{
assert( theChan != NULL ) ;

ChannelUser* meUser = theChan->findUser( me ) ;

return (meUser != NULL) ;
}

bool xClient::Write( const char* format, ... )
{
assert( format != 0 ) ;

if( !isConnected() )
	{
	return false ;
	}

char buf[ 4096 ] ;
memset( buf, 0, 4096 ) ;
va_list _list ;

va_start( _list, format ) ;
vsnprintf( buf, 4096, format, _list ) ;
va_end( _list ) ;

return Write( string( buf ) ) ;
}

void xClient::OnJoin( Channel* theChan )
{
assert( theChan != 0 ) ;

addChan( theChan ) ;
}

void xClient::OnJoin( const string& chanName )
{
	//elog << "xClient::OnJoin " << chanName << endl;
	Channel* theChan = Network->findChannel( chanName ) ;
	if( NULL == theChan )
		{
		elog	<< "xClient::OnJoin> Failed to find channel: "
			<< chanName
			<< endl ;
		return ;
		}
	OnJoin( theChan ) ;
}

void xClient::OnPart( Channel* theChan )
{
assert( theChan != 0 ) ;

removeChan( theChan ) ;
}

void xClient::OnPart( const string& chanName )
{
Channel* theChan = Network->findChannel( chanName ) ;
if( NULL == theChan )
	{
	elog	<< "xClient::OnPart> Failed to find channel: "
		<< chanName
		<< endl ;
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

void xClient::OnTimer( const xServer::timerID& , void* )
{}

void xClient::OnTimerDestroy( xServer::timerID, void* )
{}

void xClient::OnSignal( int )
{}

// This method courtesy of OUTSider
bool xClient::ClearMode( Channel* theChan, const string& modes,
	bool modeAsServer )
{
assert( theChan != 0 ) ;

if( !Connected || modes.empty() )
	{
	return false ;
	}

xServer::opVectorType opVector ;
xServer::voiceVectorType voiceVector ;
xServer::banVectorType banVector ;
xServer::modeVectorType modeVector ;

string chanKey = "";

for( string::size_type modePos = 0 ; modePos < modes.size() ; ++modePos )
	{
	switch( modes[ modePos ] ) 
		{
		case 'b':  // Ban ?
			{
			Channel::const_banIterator ptr =
				theChan->banList_begin();
			while (ptr != theChan->banList_end())
				{
				banVector.push_back( make_pair(
					false, *ptr ) ) ;
		                theChan->removeBan(*ptr);
		                ptr = theChan->banList_begin();
				}
			}
			break;
		case 'o':  //Chanops?
			{
			Channel::const_userIterator ptr =
				theChan->userList_begin();
			for( ; ptr != theChan->userList_end() ; ++ptr )
				{
				if( ptr->second->getMode(ChannelUser::MODE_O))
					{
					opVector.push_back( make_pair(
						false, ptr->second ) ) ;
					ptr->second->removeMode(
						ChannelUser::MODE_O);
					}
				}
			}
			break;
		case 'v':  //Chanvoice?
			{
			Channel::const_userIterator ptr =
				theChan->userList_begin();
			for( ; ptr != theChan->userList_end() ; ++ptr )
				{
				if( ptr->second->getMode(ChannelUser::MODE_V))
					{
					voiceVector.push_back( make_pair(
						false, ptr->second ) ) ;
					ptr->second->removeMode(
						ChannelUser::MODE_V);
					}
				}
			}
			break;
		case 'k':  //Key?
			if(theChan->getMode(Channel::MODE_K))
				{
				chanKey = " ";
				chanKey += theChan->getKey();
				theChan->removeMode(Channel::MODE_K);
				theChan->setKey( "" );
				MyUplink->OnChannelModeK( theChan,
					false, 0, string() ) ;
				}
			break;
		case 'i':  //Invite?
			theChan->removeMode(Channel::MODE_I);
			modeVector.push_back( make_pair( false,
				Channel::MODE_I ) ) ;
			break;
		case 'l': //Limit?
			if(theChan->getMode(Channel::MODE_L))
				{
				theChan->removeMode(Channel::MODE_L);
				theChan->setLimit( 0 );
				MyUplink->OnChannelModeL( theChan,
					false, 0, 0 ) ;
				}
			break;
		case 'p':  //Private?
			theChan->removeMode(Channel::MODE_P);
			modeVector.push_back( make_pair( false,
				Channel::MODE_P ) ) ;
			break;
		case 's':  //Secret?
			theChan->removeMode(Channel::MODE_S);
			modeVector.push_back( make_pair( false,
				Channel::MODE_S ) ) ;
			break;
		case 'm':  //Moderated?
			theChan->removeMode(Channel::MODE_M);
			modeVector.push_back( make_pair( false,
				Channel::MODE_M ) ) ;
			break;
		case 'n':  //No External Messages?
			theChan->removeMode(Channel::MODE_N);
			modeVector.push_back( make_pair( false,
				Channel::MODE_N ) ) ;
			break;
		case 't':  //Topic?
			theChan->removeMode(Channel::MODE_T);
			modeVector.push_back( make_pair( false,
				Channel::MODE_T ) ) ;
			break;
		case 'r':  //Registered Only?
			theChan->removeMode(Channel::MODE_R);
			modeVector.push_back( make_pair( false,
				Channel::MODE_R ) ) ;
			break;
		// Do not remove 'R' mode (Channel::MODE_REG)
		case 'D':  // new .12 mode for busy channels
			theChan->removeMode(Channel::MODE_D);
			modeVector.push_back( make_pair( false,
				Channel::MODE_D ) ) ;
			break;
		case 'c':  // new u2.10.12.15 mode to prevent chan colours
			theChan->removeMode(Channel::MODE_C);
			modeVector.push_back(make_pair(false,
				Channel::MODE_C));
			break;
		case 'C':  // new u2.10.12.15 mode to prevent chan CTCPs (except ACTION)
			theChan->removeMode(Channel::MODE_CTCP);
			modeVector.push_back(make_pair(false,
				Channel::MODE_CTCP));
			break;
		case 'u':  // mode to prevent part messages
			theChan->removeMode(Channel::MODE_PART);
			modeVector.push_back(make_pair(false,
				Channel::MODE_PART));
			break;
		case 'M':  // mode to moderate for non-authed users
			theChan->removeMode(Channel::MODE_MNOREG);
			modeVector.push_back(make_pair(false,
				Channel::MODE_MNOREG));
			break;
		case 'Z':  // TLS only?
			theChan->removeMode(Channel::MODE_Z);
			modeVector.push_back(make_pair(false,
				Channel::MODE_Z));
			break;
		case 'A':  // Apass for oplevels
			if (theChan->getMode(Channel::MODE_A))
				{
				theChan->removeMode(Channel::MODE_A);
				theChan->setApass( "" );
				MyUplink->OnChannelModeA( theChan,
					false, 0, string() ) ;
				}
			break;
		case 'U':  // Upass for oplevels
			if (theChan->getMode(Channel::MODE_U))
				{
				theChan->removeMode(Channel::MODE_U);
				theChan->setUpass( "" );
				MyUplink->OnChannelModeU( theChan,
					false, 0, string() ) ;
				}
			break;
		default:
			break;
		}
	}

if( !modeVector.empty() )
	{
	MyUplink->OnChannelMode( theChan, 0, modeVector ) ;
	}
if( !opVector.empty() )
	{
	MyUplink->OnChannelModeO( theChan, 0, opVector ) ;
	}
if( !voiceVector.empty() )
	{
	MyUplink->OnChannelModeV( theChan, 0, voiceVector ) ;
	}
if( !banVector.empty() )
	{
	MyUplink->OnChannelModeB( theChan, 0, banVector ) ;
	}

	if (modeAsServer)
	{
		return Write( "%s CM %s :%s\r\n",
			MyUplink->getCharYY().c_str(),
			theChan->getName().c_str(),
			modes.c_str()) ;
	} else {
		/* perform the mode as the bot instead */
		if (me->isOper())
		{
			/* bot is an oper, can use CM */
			return Write( "%s CM %s :%s\r\n",
				getCharYYXXX().c_str(),
				theChan->getName().c_str(),
				modes.c_str()) ;
		} else {
			/* bot is not an oper, use M */
			return Write( "%s M %s -%s%s\r\n",
				getCharYYXXX().c_str(),
				theChan->getName().c_str(),
				modes.c_str(),
				chanKey.c_str()) ;
		}
	}
}

} // namespace gnuworld
