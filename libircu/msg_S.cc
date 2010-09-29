/**
 * msg_S.cc
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
 * $Id: msg_S.cc,v 1.7 2007/05/12 13:20:00 mrbean_ Exp $
 */

#include	<new>
#include	<string>
#include	<iostream>

#include	<cassert>

#include	"gnuworld_config.h"
#include	"server.h"
#include	"events.h"
#include	"Network.h"
#include	"iServer.h"
#include	"ELog.h"
#include	"xparameters.h"
#include	"ServerCommandHandler.h"

RCSTAG( "$Id: msg_S.cc,v 1.7 2007/05/12 13:20:00 mrbean_ Exp $" ) ;

namespace gnuworld
{

using std::string ;
using std::endl ;

CREATE_HANDLER(msg_S)

/**
 * New server joined the network.
 * Q S irc.dynmc.net 2 0 948159347 P10 BD] +6hs :[209.0.37.10]
 * [209.0.37.10] Dynamic Networking Solutions
 *
 * B S EUWorld1.test.net 3 0 947284938 P10 OD] +6hs :[128.227.184.152]
 * EUWorld Undernet Service
 * B: Uplink Server numeric
 * S: SERVER message
 * EUWorld1.test.net: server name
 * 3: hopcount
 * 0: creation time
 * 947284938: link time
 * P10: Protocol
 * B: Server numeric
 * D]: Last used nick number for clients
 * +6/+h/+s: server/services flags
 * EUWorld Undernet Server: description
 * As always, the second token, the command, is not
 * included in the xParameters passed here.
 */
bool msg_S::Execute( const xParameters& params )
{
// We need at least 9 tokens
if( params.size() < 9 )
	{
	elog	<< "msg_S> Not enough parameters"
		<< endl ;
	return false ;
	}

int uplinkIntYY = base64toint( params[ 0 ] ) ;
iServer* uplinkServer = Network->findServer( uplinkIntYY ) ;

if( NULL == uplinkServer )
	{
	elog	<< "msg_S> Unable to find uplink server"
		<< endl ;
	return false ;
	}

const string serverName( params[ 1 ] ) ;
// Don't care about hop count
// Don't care about start time
time_t connectTime = static_cast< time_t >( atoi( params[ 4 ] ) ) ;
// Don't care about version

int serverIntYY = base64toint( params[ 6 ], 2 ) ;

// Does the new server's numeric already exist?
if( NULL != Network->findServer( serverIntYY ) )
	{
	elog	<< "msg_S> Server numeric collision, numeric: "
		<< params[ 6 ]
		<< ", old name: "
		<< Network->findServer( serverIntYY )->getName()
		<< ", new name: "
		<< serverName
		<< endl ;
	delete Network->removeServer( serverIntYY ) ;
	}

// Dun really care about the server description
iServer* newServer = new (std::nothrow) iServer( uplinkIntYY,
		params[ 6 ], // yxx
		serverName,
		connectTime ) ;
assert( newServer != 0 ) ;

// params[ 5 ] is either "P10", or "J10".  The J10 means
// that the server is bursting
if( 'J' == params[ 5 ][ 0 ] )
	{
	newServer->setBursting( true ) ;
	}

// Set any appropriate server flags
newServer->setFlags( params[ 7 ] ) ;

Network->addServer( newServer ) ;
//elog	<< "msg_S> Added server: "
//	<< *newServer
//	<< endl ;

theServer->PostEvent( EVT_NETJOIN,
	static_cast< void* >( newServer ),
	static_cast< void* >( uplinkServer ) ) ;

return true ;
}

} // namespace gnuworld
