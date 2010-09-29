/**
 * msg_O.cc
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
 * $Id: msg_O.cc,v 1.10 2005/03/25 03:07:29 dan_karrels Exp $
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

RCSTAG( "$Id: msg_O.cc,v 1.10 2005/03/25 03:07:29 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::string ;
using std::endl ;

CREATE_HANDLER(msg_O)

void channelNotice( iClient* srcClient,
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
		// xClient, invoke OnChannelNotice()
		servicesClient->OnChannelNotice( srcClient,
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
		elog	<< "msg_O::channelNotice> Unable to "
			<< "find owner of client: "
			<< *targetClient
			<< ", in channel: "
			<< *theChan
			<< endl ;
		continue ;
		}

	ownerClient->OnFakeChannelNotice( srcClient,
		targetClient,
		theChan,
		message ) ;
	} // for()
}

/**
 * A nick has sent a private message
 * QBg O PAA :help
 * QBg: Source nickname's numeric
 * O: NOTICE
 * PAA: Destination nickname's numeric
 * :help: Notice
 *
 * QAE O PAA :translate xaa
 * QAE O AAPAA :translate xaa
 * abcDE O #chanName :testing 12 3
 */
bool msg_O::Execute( const xParameters& Param )
{
if( Param.size() < 3 )
	{
	elog	<< "msg_O> Invalid number of arguments"
		<< endl ;
	return false ;
	}

Channel* theChan = 0 ;
if( '#' == Param[ 1 ][ 0 ] )
	{
	// Channel notice
	theChan = Network->findChannel( Param[ 1 ] ) ;
	if( 0 == theChan )
		{
		elog	<< "msg_O> Unable to locate channel: "
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
	elog	<< "msg_O> Unable to find source client: "
		<< Param[ 0 ]
		<< endl ;
	return false ;
	}

// abcDE O FGhij :hi, how are you?
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
			elog	<< "msg_O> Received notice for unknown "
				<< "client: "
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
//	elog	<< "msg_O> Notice for local client: "
//		<< Param[ 1 ]
//		<< endl ;

	// Normal notice to an xClient
	targetClient = Network->findLocalClient( Param[ 1 ] ) ;
	if( 0 == targetClient )
		{
		// Not an xClient, is it a fake client?
		fakeTarget = Network->findFakeClient( Param[ 1 ] ) ;
		if( 0 == fakeTarget )
			{
			elog	<< "msg_O> Unable to find local client: "
				<< Param[ 1 ]
				<< endl ;
			return true ;
			}
		}
	}
else if( 0 == theChan )
	{
	// TODO
	elog	<< "msg_O> Unknown target: "
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
		elog	<< "msg_O> Fake client without owner: "
			<< *fakeTarget
			<< endl ;
		return true ;
		}
	}

string message( Param[ 2 ] ) ;

if( theChan != 0 )
	{
//	elog	<< "msg_O> Channel message from: "
//		<< *srcClient
//		<< ", message: "
//		<< message
//		<< ", on channel: "
//		<< *theChan
//		<< endl ;

	channelNotice( srcClient,
		theChan,
		message ) ;
	}
else
	{
//	elog	<< "msg_O> Private message from: "
//		<< *srcClient
//		<< ", message: "
//		<< message
//		<< endl ;

	if( fakeTarget != 0 )
		{
		ownerClient->OnFakePrivateNotice(
			srcClient,
			fakeTarget,
			message,
			secure ) ;
		}
	else
		{
		targetClient->OnPrivateNotice( srcClient,
			message,
			secure ) ;
		} // else()
	} // else( theChan != 0 )

return true ;
} // msg_O

} // namespace gnuworld
