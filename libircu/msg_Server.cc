/**
 * msg_Server.cc
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
 * $Id: msg_Server.cc,v 1.7 2006/12/22 06:41:41 kewlio Exp $
 */

#include	<new>
#include	<iostream>

#include	<cstring>
#include	<cassert>

#include	"gnuworld_config.h"
#include	"server.h"
#include	"events.h"
#include	"Network.h"
#include	"iServer.h"
#include	"ELog.h"
#include	"xparameters.h"
#include	"ServerCommandHandler.h"

RCSTAG( "$Id: msg_Server.cc,v 1.7 2006/12/22 06:41:41 kewlio Exp $" ) ;

namespace gnuworld
{

using std::endl ;

CREATE_HANDLER(msg_Server)

/**
 * New server message
 * SERVER irc-r.mediabit.net 1 934191376 946934706 J10 BD] :MCS MEDIABIT Testnet
 * Routing Server, Padova, Italy
 * SERVER Austin-R.TX.US.KrushNet.Org 1 900000000 958147697 J10 1]] :Large Penis
 *  Support Group IRC Server
 *
 * The following command has been changed to token S
 * B SERVER irc.mediabit.net 2 0 945972199 P10 CD] 0 :[193.76.114.11]
 * [193.76.114.11] MCS MEDIABIT Testnet Server, Padov
 *
 * Declaration of our own server:
 * SERVER ripper.ufl.edu 1 933022556 948162945 J10 QD] :[128.227.184.152]
 * University of Florida
 *
 * Remember that the "SERVER" parameter is removed.
 */
bool msg_Server::Execute( const xParameters& Param )
{
theServer->setBurstEnd( 0 ) ;
theServer->setBurstStart( ::time( 0 ) ) ;

// Check the hopcount
// 1: It's our uplink
if( Param[ 1 ][ 0 ] == '1' )
	{

//	elog	<< "msg_Server> Got Uplink: "
//		<< Param[ 0 ]
//		<< endl ;

	// It's our uplink
	if( Param.size() < 6 )
		{
		elog	<< "msg_Server> Invalid number of parameters"
			<< endl ;
		return false ;
		}

	// Here's the deal:
	// We are just connecting to the network
	// We have just received the first server command,
	// telling us who our uplink server is.
	// We need to add our uplink to network tables.

	// Assume 5 character numerics
	unsigned int uplinkIntYY = base64toint( Param[ 5 ], 2 ) ;

	// Our uplink has its own numeric as its uplinkIntYY.
	iServer* tmpUplink = new (std::nothrow) iServer( 
		uplinkIntYY,
		Param[ 5 ], // yyxxx
		Param[ 0 ], // name
		atoi( Param[ 3 ] ) ) ; // connect time
	assert( tmpUplink != 0 ) ;

	// Check for P10 versus J10, J10 means the server is
	// bursting.
	if( 'J' == Param[ 4 ][ 0 ] )
		{
		tmpUplink->setBursting( true ) ;
		}

	// Set any appropriate server flags
	tmpUplink->setFlags( Param[ 6 ] ) ;

	theServer->setUplink( tmpUplink ) ;

	// Find this server (me)
	iServer* me = Network->findServer( theServer->getIntYY() ) ;
	if( NULL == me )
		{
		elog	<< "msg_Server> Unable to find myself "
			<< " ("
			<< theServer->getIntYY()
			<< ")"
			<< endl ;
		::exit( 0 ) ;
		}

	// Now that I know my uplink, I can set its numeric
	// in my own iServer info
	me->setUplinkIntYY( uplinkIntYY ) ;

	// We now have a pointer to our own uplink
	// Add it to the tables
	// We maintain a local pointer just for speed reasons
	Network->addServer( theServer->getUplink() ) ;

//	elog	<< "msg_Server> Added server: "
//		<< *(theServer->getUplink())
//		<< endl ;
	}

// Not posting message here because this method is only called once
// using tokenized commands - when the xServer connects
return true ;
}


} // namespace gnuworld
