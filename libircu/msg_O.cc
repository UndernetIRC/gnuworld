/**
 * msg_O.cc
 * Copyright (C) 2003 Daniel Karrels <dan@karrels.com>
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
 * $Id: msg_O.cc,v 1.1 2003/05/27 00:23:23 dan_karrels Exp $
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

const char msg_P_cc_rcsId[] = "$Id: msg_O.cc,v 1.1 2003/05/27 00:23:23 dan_karrels Exp $" ;
const char server_h_rcsId[] = __SERVER_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char iClient_h_rcsId[] = __ICLIENT_H ;
const char client_h_rcsId[] = __CLIENT_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char xParameters_h_rcsId[] = __XPARAMETERS_H ;

namespace gnuworld
{

using std::string ;
using std::endl ;

CREATE_HANDLER(msg_O)

/**
 * A nick has sent a private message
 * QBg O PAA :help
 * QBg: Source nickname's numeric
 * O: NOTICE
 * PAA: Destination nickname's numeric
 * :help: Message
 *
 * QAE O PAA :translate xaa
 * QAE O AAPAA :translate xaa
 */
bool msg_O::Execute( const xParameters& Param )
{
if( Param.size() < 3 )
	{
	elog	<< "msg_O> Invalid number of arguments"
		<< endl ;
	return false ;
	}

char* Sender	= Param[ 0 ] ;
char* Receiver	= Param[ 1 ] ;

// Is the PRIVMSG being sent to a channel?
if( '#' == *Receiver )
	{
	// It's a channel message, just ignore it
	return true ;
	}

char		*Server		= NULL,
		*Pos		= NULL,
		*Command	= NULL ;

bool		CTCP		= false ;
bool		secure		= false ;

xClient		*Client		= NULL ;

// Search for user@host in the receiver string
Pos = strchr( Receiver, '@' ) ;

// Was there a '@' in the Receiver string?
if( NULL != Pos )
	{
	// Yup, nickname specified
	Server = Receiver + (Pos - Receiver) + 1 ;
	Receiver[ Pos - Receiver ] = 0 ;
	Client = Network->findLocalNick( Receiver ) ;
	secure = true ;
	}
else if( Receiver[ 0 ] == theServer->getCharYY()[ 0 ]
	&& Receiver[ 1 ] == theServer->getCharYY()[ 1 ] )
	{
	// It's mine
	Client = Network->findLocalClient( Receiver ) ;
	}
else
	{
	elog	<< "msg_O> Received a message for unknown client: "
		<< Param
		<< endl ;
	return false ;
	}

char* Message = Param[ 2 ] ;

// :Sender PRIVMSG YXX :Message

if( NULL == Client )
	{
	elog	<< "msg_O> Local client not found: "
		<< Receiver
		<< endl ;
	return false ;
	}

iClient* Target = Network->findClient( Sender ) ;
if( NULL == Target )
	{
	elog	<< "msg_O> Unable to find Sender: "
		<< Sender
		<< endl ;
	return false ;
	}

return Client->OnNotice( Target, Message, secure ) ;
}

} // namespace gnuworld
