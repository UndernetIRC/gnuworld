/**
 * msg_EA.cc
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
 * $Id: msg_EA.cc,v 1.5 2005/03/25 03:07:29 dan_karrels Exp $
 */

#include	<string>
#include	<iostream>

#include	<cstring>

#include	"gnuworld_config.h"
#include	"server.h"
#include	"Network.h"
#include	"events.h"
#include	"ELog.h"
#include	"iServer.h"
#include	"ServerCommandHandler.h"

RCSTAG( "$Id: msg_EA.cc,v 1.5 2005/03/25 03:07:29 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::string ;
using std::endl ;

CREATE_HANDLER(msg_EA)

// Q EA
// Q: Remote server numeric
// EA: End Of Burst Acknowledge
// Our uplink server has acknowledged our EB
bool msg_EA::Execute( const xParameters& Param )
{
if( !strcmp( Param[ 0 ], theServer->getUplinkCharYY().c_str() ) )
	{
	// My uplink! :)
	// Reset EOB just to be sure
	theServer->setBursting( false ) ; // ACKNOWLEDGE! :)
	}

//if( !theServer->isBursting() )
//	{
	iServer* burstServer = Network->findServer( Param[ 0 ] ) ;
	if( NULL == burstServer )
		{
		elog	<< "msg_EA> Unable to find server: "
			<< Param[ 0 ]
			<< endl ;
		return false ;
		}

	theServer->PostEvent( EVT_BURST_ACK,
		static_cast< void* >( burstServer ) );
//	}

return true ;
}

} // namespace gnuworld
