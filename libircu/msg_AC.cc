/**
 * msg_AC.cc
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
 * "$Id: msg_AC.cc,v 1.4 2004/01/11 11:13:01 dan_karrels Exp $"
 */

#include	<iostream>

#include	"ServerCommandHandler.h"
#include	"server.h"
#include	"xparameters.h"
#include	"Channel.h"
#include	"Network.h"
#include	"iClient.h"
#include	"ELog.h"
#include	"config.h"

RCSTAG( "$Id: msg_AC.cc,v 1.4 2004/01/11 11:13:01 dan_karrels Exp $" ) ;

namespace gnuworld
{

CREATE_HANDLER(msg_AC)

/**
 * ACCOUNT message handler.
 * SOURCE AC TARGET ACCOUNT
 * Eg:
 * AXAAA AC BQrTd Gte
 */
bool msg_AC::Execute( const xParameters& Param )
{
if( Param.size() != 3 )
	{
	elog	<< "msg_AC> Invalid number of parameters"
		<< std::endl ;
	return false ;
	}

if( 0 == Network->findClient( Param[ 0 ] ) )
	{
	elog	<< "msg_AC> Unable to find source client: "
		<< Param[ 0 ]
		<< std::endl ;
	return false ;
	}

// Find the target user
iClient* theClient = Network->findClient( Param[ 1 ] ) ;
if( !theClient )
	{
	elog	<< "msg_AC> Unable to find target client: "
		<< Param[ 1 ]
		<< std::endl ;
	return false;
	}

// Update user information
theClient->setAccount( Param[ 2 ] ) ;

// Post event to listening clients
theServer->PostEvent( EVT_ACCOUNT, static_cast< void* >( theClient ) ) ;

// Return success
return true;
}

} // namespace gnuworld
