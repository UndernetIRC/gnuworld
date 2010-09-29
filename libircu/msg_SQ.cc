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
 * $Id: msg_SQ.cc,v 1.8 2007/04/27 19:30:43 mrbean_ Exp $
 */

#include	<iostream>
#include	<string>

#include	<cstring>

#include	"gnuworld_config.h"
#include	"server.h"
#include	"events.h"
#include	"Network.h"
#include	"iServer.h"
#include	"ELog.h"
#include	"xparameters.h"
#include	"ServerCommandHandler.h"

RCSTAG( "$Id: msg_SQ.cc,v 1.8 2007/04/27 19:30:43 mrbean_ Exp $" ) ;

namespace gnuworld
{

using std::string ;
using std::endl ;

CREATE_HANDLER(msg_SQ)

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
bool msg_SQ::Execute( const xParameters& Param )
{

if( Param.size() < 2 )
	{
	elog	<< "msg_SQ> Invalid number of parameters"
		<< endl ;
	return false ;
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
	elog	<< "msg_SQ> Unable to find server: "
		<< Param[ 1 ]
		<< endl ;
	return false ;
	}

if( squitServer->getIntYY() == theServer->getUplinkIntYY() )
	{
	elog	<< "msg_SQ> Ive been delinked!!"
		<< endl ;

	// It's my uplink, we have been squit...those bastards!
	theServer->Shutdown() ;
	return true ;
	}
else
	{
//	elog	<< "msg_SQ> " << squitServer->getName()
//		<< " has been squit"
//		<< endl ;

	string source( Param[ 0 ] ) ;
	string reason( Param[ 3 ] ) ;
	squitServer->setBursting(false); //If the server was in bursting state, its not anymore :)
	theServer->PostEvent( EVT_NETBREAK,
		static_cast< void* >( squitServer ),
		static_cast< void* >( &source ),
		static_cast< void* >( &reason ) ) ;

	// Otherwise, it's just some server.
	// xNetwork::OnSplit() will deallocate all servers
	// which are split
	Network->OnSplit( squitServer->getIntYY() ) ;
	}

return true ;
}

} // namespace gnuworld
