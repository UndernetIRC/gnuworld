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
 * $Id: msg_EA.cc,v 1.4 2002/05/27 17:18:13 dan_karrels Exp $
 */

#include	<stack>

#include	<cstring>

#include	"server.h"
#include	"Network.h"
#include	"events.h"
#include	"ELog.h"
#include	"iServer.h"

const char server_h_rcsId[] = __SERVER_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char events_h_rcsId[] = __EVENTS_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char iServer_h_rcsId[] = __ISERVER_H ;
const char msg_EA_cc_rcsId[] = "$Id: msg_EA.cc,v 1.4 2002/05/27 17:18:13 dan_karrels Exp $" ;

using std::string ;
using std::endl ;
using std::stack ;

namespace gnuworld
{

// Q EA
// Q: Remote server numeric
// EA: End Of Burst Acknowledge
// Our uplink server has acknowledged our EB
int xServer::MSG_EA( xParameters& Param )
{
if( !strcmp( Param[ 0 ], Uplink->getCharYY() ) )
	{
	// My uplink! :)
	// Reset EOB just to be sure
	bursting = false ; // ACKNOWLEDGE! :)
	}

if( !bursting )
	{
	iServer* theServer = Network->findServer( Param[ 0 ] ) ;
	if( NULL == theServer )
		{
		elog	<< "xServer::MSG_EA> Unable to find server: "
			<< Param[ 0 ] << endl ;
		return -1 ;
		}

	PostEvent( EVT_BURST_ACK, static_cast< void* >( theServer ) );
	}
return( 0 ) ;

}

} // namespace gnuworld
