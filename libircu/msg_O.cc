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
 * $Id: msg_O.cc,v 1.5 2003/06/17 15:13:53 dan_karrels Exp $
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

RCSTAG( "$Id: msg_O.cc,v 1.5 2003/06/17 15:13:53 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::string ;
using std::endl ;

CREATE_HANDLER(msg_O)

void channelNotice( iClient* srcClient,
	Channel* theChan,
	const string& message )
{
for( xNetwork::localClientIterator lcItr = Network->localClient_begin() ;
	lcItr != Network->localClient_end() ; ++lcItr )
	{
	// Only deliver the channel ctcp (message) if this client
	// is on the channel, and is mode -d
	if( (*lcItr)->isOnChannel( theChan ) &&
		!(*lcItr)->getMode( iClient::MODE_DEAF ) )
		{
		(*lcItr)->OnChannelNotice( srcClient, theChan, message ) ;
		}
	}
}

/**
 * A nick has sent a private message
 * QBg O PAA :help
 * QBg: Source nickname's numeric
 * O: Notice
 * PAA: Destination nickname's numeric
 * :help: Message
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
	// Channel message
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
		<< Param[ 1 ]
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
		elog	<< "msg_O> Received message for unknown "
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
	// Normal message to an xClient
	targetClient = Network->findLocalClient( Param[ 1 ] ) ;
	if( 0 == targetClient )
		{
		elog	<< "msg_O> Unable to find local client: "
			<< Param[ 1 ]
			<< endl ;
		return true ;
		}
	}
else if( 0 == theChan )
	{
	// May be a message to a juped client on a juped server,
	// ignore it.
	return true ;
	}

string message( Param[ 2 ] ) ;

if( theChan != 0 )
	{
//	elog	<< "msg_O> Channel notice from: "
//		<< *srcClient
//		<< ", message: "
//		<< message
//		<< ", on channel: "
//		<< *theChan
//		<< endl ;

	channelNotice( srcClient, theChan, message ) ;
	return true ;
	}
else
	{
//	elog	<< "msg_O> Private notice from: "
//		<< *srcClient
//		<< ", message: "
//		<< message
//		<< endl ;

	return targetClient->OnPrivateNotice( srcClient,
		message,
		secure ) ;
	}

// This should not happen
return true ;

} // msg_O

} // namespace gnuworld
