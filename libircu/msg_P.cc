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
 * $Id: msg_P.cc,v 1.15 2005/03/25 03:07:29 dan_karrels Exp $
 */

#include	<string>
#include	<iostream>

#include	"gnuworld_config.h"
#include	"server.h"
#include	"Network.h"
#include	"iClient.h"
#include	"client.h"
#include	"ELog.h"
#include	"xparameters.h"
#include	"ServerCommandHandler.h"
#include	"StringTokenizer.h"

RCSTAG( "$Id: msg_P.cc,v 1.15 2005/03/25 03:07:29 dan_karrels Exp $" ) ;

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
for( Channel::userIterator userItr = theChan->userList_begin() ;
	userItr != theChan->userList_end() ; ++userItr )
	{
	unsigned int intYYXXX = userItr->second->getIntYYXXX() ;

	xClient* servicesClient = Network->findLocalClient( intYYXXX ) ;
	if( servicesClient != 0
		&& servicesClient->isOnChannel( theChan )
		&& !servicesClient->getMode( iClient::MODE_DEAF ) )
		{
		// xClient, invoke OnChannelMessage()
		servicesClient->OnChannelCTCP( srcClient,
			theChan,
			command,
			message ) ;
		continue ;
		}

	// Not an xClient, check if it's a fake client
	iClient* targetClient = Network->findFakeClient(
			userItr->second->getClient() ) ;
	if( 0 == targetClient )
		{
		// Nope
		continue ;
		}

	// Fake client
	// Get its owner, use a different variable name here
	// just for readability.
	xClient* ownerClient = Network->findFakeClientOwner( 
		targetClient ) ;
	if( 0 == ownerClient )
		{
		elog	<< "msg_P::channelMessage> Unable to "
			<< "find owner of client: "
			<< *targetClient
			<< ", in channel: "
			<< *theChan
			<< endl ;
		continue ;
		}

	ownerClient->OnFakeChannelCTCP( srcClient,
		targetClient,
		theChan,
		command,
		message ) ;
	} // for()
}

void channelMessage( iClient* srcClient,
	Channel* theChan,
	const string& message )
{
for( Channel::userIterator userItr = theChan->userList_begin() ;
	userItr != theChan->userList_end() ; ++userItr )
	{
	unsigned int intYYXXX = userItr->second->getIntYYXXX() ;

	xClient* servicesClient = Network->findLocalClient( intYYXXX ) ;
	if( servicesClient != 0
		&& servicesClient->isOnChannel( theChan )
		&& !servicesClient->getMode( iClient::MODE_DEAF ) )
		{
		// xClient, invoke OnChannelMessage()
		servicesClient->OnChannelMessage( srcClient,
			theChan,
			message ) ;
		continue ;
		}

	// Not an xClient, check if it's a fake client
	iClient* targetClient = Network->findFakeClient(
			userItr->second->getClient() ) ;
	if( 0 == targetClient )
		{
		// Nope
		continue ;
		}

	// Fake client
	// Get its owner, use a different variable name here
	// just for readability.
	xClient* ownerClient = Network->findFakeClientOwner( 
		targetClient ) ;
	if( 0 == ownerClient )
		{
		elog	<< "msg_P::channelMessage> Unable to "
			<< "find owner of client: "
			<< *targetClient
			<< ", in channel: "
			<< *theChan
			<< endl ;
		continue ;
		}

	ownerClient->OnFakeChannelMessage( srcClient,
		targetClient,
		theChan,
		message ) ;
	} // for()
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
iClient*	fakeTarget = 0 ;

if( (0 == theChan) && (strchr( Param[ 1 ], '@' ) != 0) )
	{
	// nick@host.name specified, secure message
	secure = true ;

	StringTokenizer st( Param[ 1 ], '@' ) ;
	targetClient = Network->findLocalNick( st[ 0 ] ) ;
	if( 0 == targetClient )
		{
		fakeTarget = Network->findFakeNick( st[ 0 ] ) ;
		if( 0 == fakeTarget )
			{
//			elog	<< "msg_P> Looking for control nick: "
//				<< st[ 0 ]
//				<< endl ;
			bool controlNick =
				theServer->findControlNick( st[ 0 ] ) ;
			if( controlNick )
				{
				theServer->ControlCommand( srcClient,
					Param[ 2 ] ) ;
				// All done
				return true ;
				}

			// !controlNick
			elog	<< "msg_P> Received message for "
				<< "unknown client: "
				<< Param[ 1 ]
				<< ", nick: "
				<< st[ 0 ]
				<< endl ;
			return true ;
			}
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
		// Not an xClient, is it a fake client?
		fakeTarget = Network->findFakeClient( Param[ 1 ] ) ;
		if( 0 == fakeTarget )
			{
			elog	<< "msg_P> Unable to find local client: "
				<< Param[ 1 ]
				<< endl ;
			return true ;
			}
		}
	}
else if( 0 == theChan )
	{
	// TODO
	elog	<< "msg_P> Unknown target: "
		<< Param[ 1 ]
		<< endl ;

	// May be a message to a juped client on a juped server,
	// ignore it.
	return true ;
	}
else
	{
	// theChan != 0
	// It's a channel message, this is not a problem for
	// the case in which there is an xClient in the channel.
	// However, it becomes a bit more complicated if there are
	// one or more fake clients (possibly in addition to the
	// xClient) in the channel.
	}

xClient* ownerClient = 0 ;
if( fakeTarget != 0 )
	{
	// The target is a fake client, let's find its owner
	ownerClient = Network->findFakeClientOwner( fakeTarget ) ;
	if( 0 == ownerClient )
		{
		elog	<< "msg_P> Fake client without owner: "
			<< *fakeTarget
			<< endl ;
		return true ;
		}
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
	// CTCP, remove the control chars from the command.
	// Tokenize by the CTCP delimiter, \1
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

		if( fakeTarget != 0 )
			{
			ownerClient->OnFakeCTCP( srcClient,
				fakeTarget,
				command,
				message,
				secure ) ;
			}
		else
			{
			targetClient->OnCTCP( srcClient,
				command,
				message,
				secure ) ;
			}
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

		channelMessage( srcClient,
			theChan,
			message ) ;
		}
	else
		{
//		elog	<< "msg_P> Private message from: "
//			<< *srcClient
//			<< ", message: "
//			<< message
//			<< endl ;

		if( fakeTarget != 0 )
			{
			ownerClient->OnFakePrivateMessage(
				srcClient,
				fakeTarget,
				message,
				secure ) ;
			}
		else
			{
			targetClient->OnPrivateMessage( srcClient,
				message,
				secure ) ;
			} // else()
		} // else( theChan != 0 )
	} // else( CTCP )

return true ;
} // msg_P

} // namespace gnuworld
