/**
 * msg_P.cc
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
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
 * $Id: msg_P.cc,v 1.7 2003/08/09 23:15:33 dan_karrels Exp $
 */

#include	<string>
#include	<iostream>

#include	"server.h"
#include	"Network.h"
#include	"iClient.h"
#include	"client.h"
#include	"ELog.h"
#include	"xparameters.h"
#include	"ServerCommandHandler.h"
#include	"StringTokenizer.h"
#include	"config.h"

RCSTAG( "$Id: msg_P.cc,v 1.7 2003/08/09 23:15:33 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::string ;
using std::endl ;

CREATE_HANDLER(msg_P)

void channelCTCP( iClient* srcClient,
	Channel* theChan,
	const string& command,
	const string& message )
{
for( xNetwork::localClientIterator lcItr = Network->localClient_begin() ;
	lcItr != Network->localClient_end() ; ++lcItr )
	{
	// Only deliver the channel ctcp (message) if this client
	// is on the channel, and is mode -d
	if( lcItr->second->isOnChannel( theChan ) &&
		!lcItr->second->getMode( iClient::MODE_DEAF ) )
		{
		lcItr->second->OnChannelCTCP( srcClient,
			theChan,
			command,
			message ) ;
		}
	}
}

void channelMessage( iClient* srcClient,
	Channel* theChan,
	const string& message )
{
for( xNetwork::localClientIterator lcItr = Network->localClient_begin() ;
	lcItr != Network->localClient_end() ; ++lcItr )
	{
	// Only deliver the channel ctcp (message) if this client
	// is on the channel, and is mode -d
	if( lcItr->second->isOnChannel( theChan ) &&
		!lcItr->second->getMode( iClient::MODE_DEAF ) )
		{
		lcItr->second->OnChannelMessage(
			srcClient, theChan, message ) ;
		}
	}
}

/**
 * A nick has sent a private message
 * QBg P PAA :help
 * QBg: Source nickname's numeric
 * P: PRIVMSG
 * PAA: Destination nickname's numeric
 * :help: Message
 *
 * QAE P PAA :translate xaa
 * QAE P AAPAA :translate xaa
 * abcDE P #chanName :testing 12 3
 */
bool msg_P::Execute( const xParameters& Param )
{
if( Param.size() < 3 )
	{
	elog	<< "msg_P> Invalid number of arguments"
		<< endl ;
	return false ;
	}

Channel* theChan = 0 ;
if( '#' == Param[ 1 ][ 0 ] )
	{
	// Channel message
	theChan = Network->findChannel( Param[ 1 ] ) ;
	if( 0 == theChan )
		{
		elog	<< "msg_P> Unable to locate channel: "
			<< Param[ 1 ]
			<< endl ;
		return  false ;
		}
	}

if( '+' == Param[ 1 ][ 0 ] )
	{
	// Chances of receiving a local channel are slim to
	// none anyway
	// *shrug*
	return true ;
	}

iClient* srcClient = Network->findClient( Param[ 0 ] ) ;
if( 0 == srcClient )
	{
	elog	<< "msg_P> Unable to find source client: "
		<< Param[ 0 ]
		<< endl ;
	return false ;
	}

// abcDE P FGhij :hi, how are you?
bool		secure = false ;
xClient*	targetClient = 0 ;

if( (0 == theChan) && (strchr( Param[ 1 ], '@' ) != 0) )
	{
	// nick@host.name specified, secure message
	secure = true ;

	StringTokenizer st( Param[ 1 ], '@' ) ;
	targetClient = Network->findLocalNick( st[ 0 ] ) ;
	if( 0 == targetClient )
		{
		elog	<< "msg_P> Received message for unknown "
			<< "client: "
			<< Param[ 1 ]
			<< ", nick: "
			<< st[ 0 ]
			<< endl ;
		return true ;
		}
	// Found the target xClient
	}
else if( (0 == theChan) &&
	(Param[ 1 ][ 0 ] == theServer->getCharYY()[ 0 ]) &&
	(Param[ 1 ][ 1 ] == theServer->getCharYY()[ 1 ]) )
	{
//	elog	<< "msg_P> Message for local client: "
//		<< Param[ 1 ]
//		<< endl ;

	// Normal message to an xClient
	targetClient = Network->findLocalClient( Param[ 1 ] ) ;
	if( 0 == targetClient )
		{
		elog	<< "msg_P> Unable to find local client: "
			<< Param[ 1 ]
			<< endl ;
		return true ;
		}
	}
else if( 0 == theChan )
	{
	elog	<< "msg_P> Unknown target: "
		<< Param[ 1 ]
		<< endl ;

	// May be a message to a juped client on a juped server,
	// ignore it.
	return true ;
	}

bool CTCP = (Param[ 2 ][ 0 ] == 1) ? true : false ;

// Prepare a message string to pass to the client which is
// void of any CTCP control chars
// CTCP messages begin and end with '\1'
// In the case of CTCP, it is of the form:
// abcDE P EFghi :\1ctcp_command\1 message
// abcDE P EFghi :\1PING 123456789\1
// Note that Param[ 2 ] is \1PING 123456789\1 message

string message( Param[ 2 ] ) ;
string command ;

if( CTCP )
	{
	// CTCP, remove the control chars from the command
	// Tokenizer by the CTCP delimiter, \1
	StringTokenizer st( Param[ 2 ], '\1' ) ;

	// Make sure there is at least one token
	if( st.empty() )
		{
		elog	<< "msg_P> Found empty tokenizer for CTCP, from: "
			<< Param[ 2 ]
			<< endl ;
		return false ;
		}

	// The CTCP command is now everything that was surrounded
	// by the two \1's, or the first token of st, st[ 0 ]
	command = st[ 0 ] ;

	// If there was an optional message after the CTCP command,
	// st.size() will be greater than 1.
	if( st.size() > 1 )
		{
		message = st.assemble( 2 ) ;
		}
	else
		{
		// No message, clear it
		message = "" ;
		}
	}

if( CTCP )
	{
	if( theChan != 0 )
		{
//		elog	<< "msg_P> Found channel ctcp, command from: "
//			<< *srcClient
//			<< ", command: "
//			<< command
//			<< ", message: "
//			<< message
//			<< ", on channel: "
//			<< *theChan
//			<< endl ;

		channelCTCP( srcClient, theChan, command, message ) ;
		}
	else
		{
//		elog	<< "msg_P> Found privmsg CTCP, command from: "
//			<< *srcClient
//			<< ", command: "
//			<< command
//			<< ", message: "
//			<< message
//			<< endl ;

		targetClient->OnCTCP( srcClient,
			command,
			message,
			secure ) ;
		}
	}
else
	{
	if( theChan != 0 )
		{
//		elog	<< "msg_P> Channel message from: "
//			<< *srcClient
//			<< ", message: "
//			<< message
//			<< ", on channel: "
//			<< *theChan
//			<< endl ;

		channelMessage( srcClient, theChan, message ) ;
		}
	else
		{
//		elog	<< "msg_P> Private message from: "
//			<< *srcClient
//			<< ", message: "
//			<< message
//			<< endl ;

		targetClient->OnPrivateMessage( srcClient,
			message,
			secure ) ;
		}
	}

return true ;
} // msg_P

} // namespace gnuworld
