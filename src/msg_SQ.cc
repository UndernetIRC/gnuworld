/**
 * msg_SQ.cc
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
 * $Id: msg_SQ.cc,v 1.9 2002/05/27 17:18:13 dan_karrels Exp $
 */

#include	<iostream>
#include	<string>

#include	<cstring>

#include	"server.h"
#include	"events.h"
#include	"Network.h"
#include	"iServer.h"
#include	"ELog.h"
#include	"xparameters.h"

const char msg_SQ_cc_rcsId[] = "$Id: msg_SQ.cc,v 1.9 2002/05/27 17:18:13 dan_karrels Exp $" ;
const char server_h_rcsId[] = __SERVER_H ;
const char events_h_rcsId[] = __EVENTS_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char iServer_h_rcsId[] = __ISERVER_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char xParameters_h_rcsId[] = __XPARAMETERS_H ;

namespace gnuworld
{

using std::string ;
using std::endl ;

/**
 * SQUIT message handler.
 * :ripper_ SQ ripper.ufl.edu 0 :squitting the services server
 * :ripper_ SQ maniac.krushnet.org 954445164 :remote server squit
 * 0 SQ Auckland.NZ.KrushNet.Org 957468458 :Ping timeout
 * kAm SQ Luxembourg.LU.EU.KrushNet.Org 957423889 :byyyyyye
 *
 * The following example is gnuworld being squitted:
 * OAO SQ Asheville-R.NC.US.KrushNet.Org 0 :I bid you adoo
 * asheville-r.* was gnuworld's uplink
 * 0 SQ Asheville-R.NC.US.KrushNet.Org 0 :Ping timeout
 * Az SQ Seattle-R.WA.US.KrushNet.Org 0 :Ping timeout
 */
int xServer::MSG_SQ( xParameters& Param )
{

if( Param.size() < 2 )
	{
	elog	<< "xServer::MSG_SQ> Invalid number of parameters"
		<< endl ;
	return -1 ;
	}

iServer* squitServer = 0 ;
if( strchr( Param[ 1 ], '.' ) != NULL )
	{
	// Full server name specified
	squitServer = Network->findServerName( Param[ 1 ] ) ;
	}
else
	{
	// Numeric
	squitServer = Network->findServer( Param[ 1 ] ) ;
	}

if( NULL == squitServer )
	{
	elog	<< "xServer::MSG_SQ> Unable to find server: "
		<< Param[ 1 ]
		<< endl ;
	return -1 ;
	}

if( squitServer->getIntYY() == Uplink->getIntYY() )
	{
	elog	<< "xServer::MSG_SQ> Ive been delinked!!"
		<< endl ;

	// It's my uplink, we have been squit...those bastards!
	OnDisConnect() ;
	}
else
	{

//	elog	<< "xServer::MSG_SQ> " << squitServer->getName()
//		<< " has been squit\n" ;

	string source( Param[ 0 ] ) ;
	string reason( Param[ 3 ] ) ;

	PostEvent( EVT_NETBREAK,
		static_cast< void* >( squitServer ),
		static_cast< void* >( &source ),
		static_cast< void* >( &reason ) ) ;

	// Otherwise, it's just some server.
	// xNetwork::OnSplit() will deallocate all servers
	// which are split
	Network->OnSplit( squitServer->getIntYY() ) ;

	// Remove this server from the juped list (if its there)
	RemoveJupe(squitServer);
	}

return 0 ;
}

} // namespace gnuworld
