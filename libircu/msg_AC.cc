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
 * "$Id: msg_AC.cc,v 1.9 2005/03/25 03:07:29 dan_karrels Exp $"
 */

#include	<string>
#include	<iostream>

#include	"gnuworld_config.h"
#include	"ServerCommandHandler.h"
#include	"server.h"
#include	"xparameters.h"
#include	"Channel.h"
#include	"Network.h"
#include	"iClient.h"
#include	"ELog.h"

RCSTAG( "$Id: msg_AC.cc,v 1.9 2005/03/25 03:07:29 dan_karrels Exp $" ) ;

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

// Find the target user
iClient* theClient = Network->findClient( Param[ 1 ] ) ;
if( !theClient )
	{
	elog	<< "msg_AC> Unable to find target client: "
		<< Param[ 1 ]
		<< std::endl ;
	return false;
	}

std::string account( Param[2] );
time_t account_ts = 0;

/* If we have an account, does it have a timestamp? */
if( ! account.empty() ) {
	std::string::size_type pos = account.find(':');
	if( ! ( pos == std::string::npos ) ) {
		/* We have a timestamp */
		if ( pos == ( account.length() - 1 ) ) {
			/* Bizarre - colon but no following TS */
			elog	<< "msg_N> Invalid account format: "
				<< account
				<< std::endl;
		} else {
			std::string account_ts_s = account;
			account_ts_s.erase(0, pos + 1);
			account.erase(pos);
			
			account_ts = atoi(account_ts_s.c_str());
		}
	}
}


// Update user information
theClient->setAccount( account ) ;

if( account_ts != 0 ) theClient->setAccountTS( account_ts );

// Post event to listening clients
theServer->PostEvent( EVT_ACCOUNT, static_cast< void* >( theClient ) ) ;

// Return success
return true;
}

} // namespace gnuworld
