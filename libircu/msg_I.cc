/**
 * msg_I.cc
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
 * $Id: msg_I.cc,v 1.1 2002/11/20 22:16:18 dan_karrels Exp $
 */

#include	<iostream>

#include	"server.h"
#include	"xparameters.h"
#include	"Channel.h"
#include	"iClient.h"
#include	"ELog.h"
#include	"client.h"
#include	"Network.h"
#include	"ServerCommandHandler.h"

const char server_h_rcsId[] = __SERVER_H ;
const char xparameters_h_rcsId[] = __XPARAMETERS_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char Channel_h_rcsId[] = __CHANNEL_H ;
const char iClient_h_rcsId[] = __ICLIENT_H ;
const char client_h_rcsId[] = __CLIENT_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char msg_I_cc_rcsId[] = "$Id: msg_I.cc,v 1.1 2002/11/20 22:16:18 dan_karrels Exp $" ;

namespace gnuworld
{

using std::endl ;

CREATE_HANDLER(msg_I)

// ABAHo I X :#lksdlkj
bool msg_I::Execute( const xParameters& Param )
{
if( Param.size() != 3 )
	{
	elog	<< "msg_I> Invalid number of arguments"
		<< endl ;
	return false ;
	}

iClient* srcClient = Network->findClient( Param[ 0 ] ) ;
if( NULL == srcClient )
	{
	elog	<< "msg_I> Unable to find source client: "
		<< Param[ 0 ]
		<< endl ;
	return false ;
	}

xClient* destClient = Network->findLocalNick( Param[ 1 ] ) ;
if( NULL == destClient )
	{
	elog	<< "msg_I> Unable to find destination client: "
		<< Param[ 1 ]
		<< endl ;
	return false ;
	}

Channel* theChan = Network->findChannel( Param[ 2 ] ) ;
if( NULL == theChan )
	{
	elog	<< "msg_I> Unable to find channel: "
		<< Param[ 2 ]
		<< endl ;
	return false ;
	}

return destClient->OnInvite( srcClient, theChan ) ;
}

} // namespace gnuworld
