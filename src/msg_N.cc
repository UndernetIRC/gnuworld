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
 * $Id: msg_N.cc,v 1.8 2002/05/27 17:18:13 dan_karrels Exp $
 */

#include	<new>
#include	<string>

#include	<cassert>

#include	"server.h"
#include	"iClient.h"
#include	"events.h"
#include	"ip.h"
#include	"Network.h"
#include	"ELog.h"
#include	"xparameters.h"

const char msg_N_cc_rcsId[] = "$Id: msg_N.cc,v 1.8 2002/05/27 17:18:13 dan_karrels Exp $" ;
const char iClient_h_rcsId[] = __ICLIENT_H ;
const char events_h_rcsId[] = __EVENTS_H ;
const char ip_h_rcsId[] = __IP_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char server_h_rcsId[] = __SERVER_H ;
const char xparameters_h_rcsId[] = __XPARAMETERS_H ;

namespace gnuworld
{

using std::string ;
using std::endl ;

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
int xServer::MSG_N( xParameters& params )
{

// AUAAB N Gte- 949527071
if( params.size() < 5 )
	{
	// User changing nick
	Network->rehashNick( params[ 0 ], params[ 1 ] ) ;
	return 0 ;
	}

// Else, it's the network giving us a new client.
iServer* nickUplink = Network->findServer( params[ 0 ] ) ;
if( NULL == nickUplink )
	{
	elog	<< "xServer::MSG_N> Unable to find server: "
		<< params[ 0 ] << endl ;
	return -1 ;
	}

// Default arguments, assuming
// no modes set.
const char* modes = "+" ;
const char* host = params[ 6 ] ;
const char* yyxxx = params[ 7 ] ;
const char* description = params [ 8 ] ;
const char* account = "";

// Are modes specified? (With a +r?)
// If so, token 7 is the authenticated account name,
// the rest shuffle up.
// TODO: Make this more futureproof.
if( params.size() > 10 )
	{
	modes = params[ 6 ] ;
	account = params[ 7 ];
	host = params[ 8 ] ;
	yyxxx = params[ 9 ] ;
	description = params[ 10 ];
	}
	else
	{
	if( params.size() > 9 )
		{
		// Just plain modes here without any parameters
		modes = params[ 6 ] ;
		host = params[ 7 ] ;
		yyxxx = params[ 8 ] ;
		description = params[ 9 ];
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
		description,
		atoi( params[ 3 ] ) // connection time
		) ;
assert( newClient != 0 ) ;

if( !Network->addClient( newClient ) )
	{
	elog	<< "xServer::MSG_B> Failed to add client: "
		<< *newClient
		<< ", user already exists? "
		<< (Network->findClient( newClient->getCharYYXXX() ) ?
		   "yes" : "no")
		<< endl ;
	delete newClient ;
	return -1 ;
	}

// TODO: Should this be posted?
PostEvent( EVT_NICK, static_cast< void* >( newClient ) ) ;

return 0 ;
}

} // namespace gnuworld
