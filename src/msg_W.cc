/**
 * msg_W.cc
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
 * $Id: msg_W.cc,v 1.5 2002/05/27 17:18:13 dan_karrels Exp $
 */

#include	<iostream>

#include	"server.h"
#include	"xparameters.h"
#include	"ELog.h"
#include	"Network.h"
#include	"iClient.h"
#include	"client.h"

const char msg_W_cc_rcsId[] = "$Id: msg_W.cc,v 1.5 2002/05/27 17:18:13 dan_karrels Exp $" ;
const char server_h_rcsId[] = __SERVER_H ;
const char xParameters_h_rcsId[] = __XPARAMETERS_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char iClient_h_rcsId[] = __ICLIENT_H ;
const char client_h_rcsId[] = __CLIENT_H ;

namespace gnuworld
{

using std::endl ;

// ABAG7 W Az :Gte-
int xServer::MSG_W( xParameters& Param )
{
if( Param.size() != 3 )
	{
	elog	<< "xServer::MSG_W> Invalid number of parameters"
		<< endl ;
	return -1 ;
	}

iClient* sourceClient = Network->findClient( Param[ 0 ] ) ;
if( NULL == sourceClient )
	{
	elog	<< "xServer::MSG_W> Unable to find source client: "
		<< Param[ 0 ]
		<< endl ;
	return -1 ;
	}

iClient* targetClient = Network->findNick( Param[ 2 ] ) ;
if( NULL == targetClient )
	{
	elog	<< "xServer::MSG_W> Unable to find target client: "
		<< Param[ 2 ]
		<< endl ;
	return -1 ;
	}

// WHOIS must be delivered to all xclients
xNetwork::localClientIterator ptr = Network->localClient_begin() ;
for( ; ptr != Network->localClient_end() ; ++ptr )
	{
	(*ptr)->OnWhois( sourceClient, targetClient ) ;
	}

return 0 ;
}

} // namespace gnuworld
