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
 * $Id: msg_P.cc,v 1.2 2003/06/06 20:03:31 dan_karrels Exp $
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

const char msg_P_cc_rcsId[] = "$Id: msg_P.cc,v 1.2 2003/06/06 20:03:31 dan_karrels Exp $" ;
const char server_h_rcsId[] = __SERVER_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char iClient_h_rcsId[] = __ICLIENT_H ;
const char client_h_rcsId[] = __CLIENT_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char xParameters_h_rcsId[] = __XPARAMETERS_H ;
const char StringTokenizer_h_rcsId[] = __STRINGTOKENIZER_H ;

namespace gnuworld
{

using std::string ;
using std::endl ;

CREATE_HANDLER(msg_P)

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
string message( Param.assemble( 2 ) ) ;
string command( Param[ 2 ] ) ;

if( CTCP )
	{
	// CTCP, remove the control chars from the command
	command.erase( command.begin() ) ;
	command.erase( command.size() - 1 ) ;

	// It's possible to have a CTCP command without a message
	if( Param.size() >= 4 )
		{
		message = Param.assemble( 3 ) ;
		}
	}

if( CTCP )
	{
	if( theChan != 0 )
		{
		elog	<< "msg_P> Found channel ctcp, command from: "
			<< *srcClient
			<< ", command: "
			<< command
			<< ", message: "
			<< message
			<< ", on channel: "
			<< *theChan
			<< endl ;

		return targetClient->OnChannelCTCP( srcClient,
			theChan,
			command,
			message ) ;
		}
	else
		{
		elog	<< "msg_P> Found privmsg CTCP, command from: "
			<< *srcClient
			<< ", command: "
			<< command
			<< ", message: "
			<< message
			<< ", on channel: "
			<< *theChan
			<< endl ;

		return targetClient->OnCTCP( srcClient,
			command,
			message,
			secure ) ;
		}
	}
else
	{
	if( theChan != 0 )
		{
		elog	<< "msg_P> Channel message from: "
			<< *srcClient
			<< ", message: "
			<< message
			<< ", on channel: "
			<< *theChan
			<< endl ;

		return targetClient->OnChannelMessage( srcClient,
			theChan,
			message ) ;
		}
	else
		{
		elog	<< "msg_P> Private message from: "
			<< *srcClient
			<< ", message: "
			<< message
			<< endl ;

		return targetClient->OnPrivateMessage( srcClient,
			message,
			secure ) ;
		}
	}

// This should not happen
return true ;

} // msg_P

} // namespace gnuworld
