/**
 * msg_N.cc
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
 * $Id: msg_N.cc,v 1.6 2004/06/14 22:17:56 jeekay Exp $
 */

#include	<new>
#include	<string>
#include	<iostream>

#include	<cassert>

#include	"server.h"
#include	"iClient.h"
#include	"events.h"
#include	"ip.h"
#include	"Network.h"
#include	"ELog.h"
#include	"xparameters.h"
#include	"ServerCommandHandler.h"
#include	"config.h"

RCSTAG( "$Id: msg_N.cc,v 1.6 2004/06/14 22:17:56 jeekay Exp $" ) ;

namespace gnuworld
{

using std::string ;
using std::endl ;

CREATE_HANDLER(msg_N)

/**
 * A new user has joined the network, or a user has changed
 * its nickname.
 * O N EUworld1 2 000201527 gnuworld1 undernet.org AAAAAA OAA :P10 Undernet
 * EUworld Service
 *
 * O: another server numeric
 * 2: hopcount
 * 000201527: timestamp
 * gnuworld1: username
 * undernet.org: domain
 * AAAAAA: base64 IP
 * OAA: numnick
 * :P10 Undernet: description
 *
 * B N hektik 2 948677656 hektik p62-max7.ham.ihug.co.nz +i DLbcbC BAA
 * :DiMeBoX ProduXiiions
 *
 * AU N Gte2 3 949526996 Gte 212.49.240.147 DUMfCT AUAAB :I am the one
 *  that was.
 */
bool msg_N::Execute( const xParameters& params )
{
// AUAAB N Gte- 949527071
if( params.size() < 5 )
	{
	// User changing nick
//	elog	<< "N> Rehashing nickname: "
//		<< params
//		<< endl ;

	Network->rehashNick( params[ 0 ], params[ 1 ] ) ;
	return true ;
	}

// Else, it's the network giving us a new client.
iServer* nickUplink = Network->findServer( params[ 0 ] ) ;
if( NULL == nickUplink )
	{
	elog	<< "msg_N> Unable to find server: "
		<< params[ 0 ]
		<< endl ;
	return false ;
	}

// Default arguments, assuming
// no modes set.
const char* modes = "+" ;
const char* host = params[ 6 ] ;
const char* yyxxx = params[ 7 ] ;
const char* description = params [ 8 ] ;
string account = "";
time_t account_ts = 0;

// Are modes specified? (With a +r?)
// If so, token 7 is the authenticated account name,
// the rest shuffle up.
if( 11 == params.size() )
	{
	// User logged in with AC account
	// server nick '1' someothernumber username hostname
	// modes account base64host numeric <description>
	modes = params[ 6 ] ;
	account = params[ 7 ];
	host = params[ 8 ] ;
	yyxxx = params[ 9 ] ;
	description = params[ 10 ];
	}
else if( 10 == params.size() )
	{
	// User not logged in
	// server nick '1' someothernumber username hostname
	// modes base64host numeric <description>

	// Just plain modes here without any parameters
	modes = params[ 6 ] ;
	host = params[ 7 ] ;
	yyxxx = params[ 8 ] ;
	description = params[ 9 ];
	}
else if( 12 == params.size() )
	{
	// asuka sethost, user logged in
	// server nick '1' someothernumber username hostname
	// modes account sethost base64host numeric <description>
	modes = params[ 6 ] ;
	account = params[ 7 ] ;
	host = params[ 9 ] ;
	yyxxx = params[ 10 ] ;
	description = params[ 11 ] ;

//	elog	<< "msg_N> 12 params: "
//		<< params
//		<< endl ;
	}
else if( 9 == params.size() )
	{
	// server nick '1' someothernumber username hostname
	// base64host numeric <description>
	host = params[ 6 ] ;
	yyxxx = params[ 7 ] ;
	description = params[ 8 ] ;
	}
else
	{
	elog	<< "msg_N> Unknown token formation: "
		<< params
		<< endl ;
	return false ;
	}

/* If we have an account, does it have a timestamp? */
if( ! account.empty() ) {
	string::size_type pos = account.find(':');
	if( ! ( pos == string::npos ) ) {
		/* We have a timestamp */
		if ( pos == ( account.length() - 1 ) ) {
			/* Bizarre - colon but no following TS */
			elog	<< "msg_N> Invalid account format: "
				<< account
				<< endl;
		} else {
			string account_ts_s = account;
			account_ts_s.erase(0, pos + 1);
			account.erase(pos);
			
			account_ts = atoi(account_ts_s.c_str());
		}
	}
}

iClient* newClient = new (std::nothrow) iClient(
		nickUplink->getIntYY(),
		yyxxx,
		params[ 1 ], // nickname
		params[ 4 ], // username
		host, // base 64 host
		params[ 5 ], // insecurehost
		params[ 5 ], // realInsecurehost
		modes,
		account,
		account_ts,
		description,
		atoi( params[ 3 ] ) // connection time
		) ;
assert( newClient != 0 ) ;

if( !Network->addClient( newClient ) )
	{
	elog	<< "msg_N> Failed to add client: "
		<< *newClient
		<< ", user already exists? "
		<< (Network->findClient( newClient->getCharYYXXX() ) ?
		   "yes" : "no")
		<< endl ;
	delete newClient ;
	return false ;
	}

//elog	<< "msg_N> Added user: "
//	<< *newClient
//	<< endl ;

theServer->PostEvent( EVT_NICK, static_cast< void* >( newClient ) ) ;

return true ;
}

} // namespace gnuworld
