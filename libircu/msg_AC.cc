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

namespace gnuworld
{

CREATE_HANDLER(msg_AC)

/**
 * ACCOUNT message handler.
 * SOURCE AC TARGET ACCOUNT ACCOUNT_ID ACCOUNT_FLAGS
 * Eg:
 * AXAAA AC BQrTd Hidden 1694970265 1024
 * Note: ACCOUNT_ID and ACCOUNT_FLAGS are optional
 */
bool msg_AC::Execute( const xParameters& Param )
{
if( Param.size() < 3 )
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
unsigned int account_id = 0;
iClient::flagType account_flags = 0;

/* If we have an account, does it have an id? */
if( ! account.empty() ) {
	std::string::size_type pos = account.find(':');
	if( ! ( pos == std::string::npos ) ) {
		/* We have an account id */
		if ( pos == ( account.length() - 1 ) ) {
			/* Bizarre - colon but no following account id */
			elog	<< "msg_N> Invalid account format: "
				<< account
				<< std::endl;
		} else {
			std::string account_id_s = account;
			account_id_s.erase(0, pos + 1);
			account.erase(pos);

			account_id = atoi(account_id_s.c_str());
		}
	}
	if (Param.size() > 3) {
		std::string account_id_s = Param[3];
		if (!account_id_s.empty()) {
			account_id = atoi(account_id_s);
		}
	}
	if (Param.size() > 4) {
		std::string account_flags_s = Param[4];
		if (!account_flags_s.empty()) {
			account_flags = atoi(account_flags_s);
		}
	}
}

// Is this a change of flags or a new login?
bool alreadyAuthed = false ;
if( theClient->isModeR() )
	alreadyAuthed = true ;

// Update user information
theClient->setAccount( account ) ;
theClient->setAccountID( account_id ) ;
theClient->setAccountFlags( account_flags ) ;

// Post event to listening clients
theServer->PostEvent( alreadyAuthed ? EVT_ACCOUNT_FLAGS : EVT_ACCOUNT, static_cast< void* >( theClient ) ) ;

// Return success
return true;
}

} // namespace gnuworld
