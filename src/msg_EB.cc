/**
 * msg_EB.cc
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
 * $Id: msg_EB.cc,v 1.6 2002/05/27 17:18:13 dan_karrels Exp $
 */

#include	<sys/types.h>
#include	<sys/time.h>

#include	<iostream>

#include	"server.h"
#include	"iServer.h"
#include	"events.h"
#include	"Network.h"
#include	"ELog.h"
#include	"xparameters.h"

const char msg_EB_cc_rcsId[] = "$Id: msg_EB.cc,v 1.6 2002/05/27 17:18:13 dan_karrels Exp $" ;
const char server_h_rcsId[] = __SERVER_H ;
const char iServer_h_rcsId[] = __ISERVER_H ;
const char events_h_rcsId[] = __EVENTS_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char xParameters_h_rcsId[] = __XPARAMETERS_H ;

namespace gnuworld
{

using std::endl ;

// Q EB
// Q: Remote server numeric
// EB: End Of Burst
int xServer::MSG_EB( xParameters& params )
{

if( !strcmp( params[ 0 ], Uplink->getCharYY() ) )
	{
	// It's my uplink
	burstEnd = ::time( 0 ) ;

	// Our uplink is done bursting
	// This is done here instead of down below the if/else
	// structure because some of the methods called here
	// may depend or use the Uplink's isBursting() method
	Uplink->stopBursting() ;

	// Signal that all Write()'s should write to the
	// normal output buffer
	useBurstBuffer = false ;

	// Burst our clients
	BurstClients() ;

	// Burst our channels
	BurstChannels() ;

	// We are no longer bursting
	bursting = false ;

	// For some silly reason, EB must come before EA
	// *shrug*

	// Called PostEvent() here to notify all attached clients
	// that we are no longer bursting.  This will ensure
	// that all end of burst items are written to the
	// burstOutputBuffer before the burst if officially
	// completed (as seen by the network)
	PostEvent( EVT_BURST_CMPLT, static_cast< void* >( Uplink ) ) ;

	// Send our EB
	Write( "%s EB\n", charYY ) ;

	// Acknowledge their end of burst
	Write( "%s EA\n", charYY ) ;

	// Is the burstOutputBuffer empty?
	if( !burstOutputBuffer.empty() )
		{
		// It has data, concatenate this data
		// onto the normal outputBuffer
		outputBuffer += burstOutputBuffer ;
		burstOutputBuffer.clear() ;

		elog	<< "xServer::MSG_EB> Adding "
			<< burstOutputBuffer.size()
			<< " bytes from burstOutputBuffer to "
			<< "outputBuffer"
			<< endl ;

		}

	elog	<< "*** Completed net burst"
		<< endl ;
	}
else
	{
	/* Its another server that has just completed its net.burst. */

	iServer* theServer = Network->findServer( params[ 0 ] ) ;
	if( NULL == theServer )
		{
		elog	<< "xServer::MSG_EB> Unable to find server: "
			<< params[ 0 ]
			<< endl ;
		return -1 ;
	}

	theServer->stopBursting() ;

	PostEvent( EVT_BURST_CMPLT, static_cast< void* >( theServer ) ) ;
	}

return 0 ;
}

} // namespace gnuworld
