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
 * $Id: msg_N.cc,v 1.11 2005/11/30 19:37:34 kewlio Exp $
 */

#include	<new>
#include	<string>
#include	<iostream>

#include	<cassert>

#include	"gnuworld_config.h"
#include	"server.h"
#include	"iClient.h"
#include	"events.h"
#include	"ip.h"
#include	"Network.h"
#include	"ELog.h"
#include	"xparameters.h"
#include	"ServerCommandHandler.h"
#include	"StringTokenizer.h"

namespace gnuworld
{

using std::string ;
using std::endl ;

CREATE_HANDLER(msg_N)

/**
 * A new user has joined the network, we are receiving a burst,
 * or a user has changed his/her nickname.
 *
 * Possible cases:
 * 1) Simple nick change
 *    AUAAB N Gte- 949527071
 * 2) Client without modes
 *    AU N Gte2 3 949526996 Gte 212.49.240.147 DUMfCT AUAAB :I am the
 *    one that was.
 * 3) Client with modes
 *    B0 N hektik 2 948677656 hektik p62-max7.ham.ihug.co.nz +i DLbcbC
 *    B0AAA :DiMeBoX ProduXiiions
 *
 * 1) <nickname>
 * 2) <hops>
 * 3) <TS>
 * 4) <userid>
 * 5) <host>
 * 6) [<+modes>]
 * 7+) [<mode parameters>]
 * -3 <base64 IP>
 * -2 <numeric>
 * -1 <fullname
 *
 * AF N Client1 1 947957573 User userhost.net +oiwg DAqAoB AFAAA
 * :Generic Client.
 *
 * AF - numeric of the server the user is on
 * N - NICK token
 * Client1 - nick
 * 1 - hopcount
 * 947957573 - timestamp
 * User - username
 * userhost.net - domain
 * +oiwg - modes
 * DAqAoB - base64 IP
 * AFAAA - numnick
 * :Generic Client - description
 */
bool msg_N::Execute( const xParameters& params )
{
if( params.size() < 3 )
	{
	// Error
	elog	<< "msg_N> Invalid format: "
		<< params
		<< endl ;
	return false ;
	}

iServer* nickUplink = 0;
if (3 == params.size())
	{
	char serverYY[3];
	strncpy(serverYY, params[0], 2);
	serverYY[2] = '\0';
	nickUplink = Network->findServer(serverYY);
	}
else
	nickUplink = Network->findServer(params[0]);

if (!nickUplink->isBursting())
	{
	// Set the server's lag time
	time_t nick_ts = atoi(params[2]);
	time_t lag = 0;
	if (::time(0) > nick_ts)
		lag = ::time(0) - nick_ts;
	else
		lag = 0;
	nickUplink->setLag(lag);
	}

// AUAAB N Gte- 949527071
if( 3 == params.size() )
	{
	// User changing nick
//	elog	<< "msg_N> Rehashing nickname: "
//		<< params
//		<< endl ;

	Network->rehashNick( params[ 0 ], params[ 1 ], atoi( params[ 2 ] ) ) ;
	return true ;
	}

// Else, it's the network giving us a new client.
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

/*
 * 1) <nickname>
 * 2) <hops>
 * 3) <TS>
 * 4) <userid>
 * 5) <host>
 * 6) [<+modes>]
 * 7+) [<mode parameters>]
 * -3 <base64 IP>
 * -2 <numeric>
 * -1 <fullname
 */

string account ;
unsigned int account_id = 0 ;
unsigned short int account_flags = 0;
string sethost ;
string fakehost ;
string tlsFingerprint ;

xParameters::size_type currentArgIndex = 6 ;

// precondition: currentArgIndex points at the next params[] index
// to check
if( '+' == params[ currentArgIndex ][ 0 ] )
	{
	// Got modes
	currentArgIndex = 7 ;

	// If any of the modes 'r', 'h', 'f' are present, then
	// another param will follow.
	// The mode order will always be rhf if all 3 are present.
	modes = params[ 6 ] ;

	for( const char* modePtr = params[ 6 ] ; *modePtr ; ++modePtr )
		{
		switch( *modePtr )
			{
			case 'r':
				account = params[ currentArgIndex++ ] ;
				break ;
			case 'h':
				sethost = params[ currentArgIndex++ ] ;
				break ;
			case 'f':
				fakehost = params[ currentArgIndex++ ] ;
				break ;
#ifndef NO_FINGERPRINT_BURST
			case 'z':
				tlsFingerprint = params[ currentArgIndex++ ] ;
				break ;
#endif
			default: break ;
			} // switch( *modePtr )
		} // for()
	} // if( '+' )
// postcondition: currentArgIndex points at the next params[] index
// to check

if( !account.empty() )
	{
	StringTokenizer st( account, ':' ) ;
	account = st[ 0 ] ;
	if( 2 <= st.size() )
		{
		// id present
		std::stringstream ss ;
		ss	<< st[ 1 ] ;
		if( !(ss >> account_id) )
			{
			elog	<< "msg_N> Invalid account id: "
				<< st[ 1 ]
				<< endl ;
			// non-fatal error
			}
		} // if( 2 <= st.size() )
	if( 3 == st.size() )
		{
		// flags present
		std::stringstream ss ;
		ss	<< st[ 2 ] ;
		if( !(ss >> account_flags) )
			{
			elog	<< "msg_N> Invalid account flags: "
				<< st[ 2 ]
				<< endl ;
			// non-fatal error
			}
		} // if( 3 == st.size() )
	} // if( !account.empty() )

/* Set the fingerprint to empty if it is empty. */
if( tlsFingerprint == "_" )
	tlsFingerprint.clear() ;

/*
 * -3 <base64 IP>
 * -2 <numeric>
 * -1 <fullname
 */
const char* host = params[ currentArgIndex++ ] ;
const char* yyxxx = params[ currentArgIndex++ ] ;
const char* description = params [ currentArgIndex ] ;

iClient* newClient = new (std::nothrow) iClient(
		nickUplink->getIntYY(),
		yyxxx,		// numeric
		params[ 1 ],	// nickname
		params[ 4 ],	// username
		host,		// base64 encoded ip
		params[ 5 ],	// insecureHost
		params[ 5 ],	// realInsecureHost
		modes,		// modes (default: +)
		account,	// account
		account_id,	// account id
		account_flags,  // account flags
		tlsFingerprint, // TLS fingerprint
		sethost,	// asuka sethost
		fakehost,	// srvx fakehost
		description,	// real name / infoline
		atoi( params[ 3 ] ) // nick timestamp
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
	delete newClient ; newClient = 0 ;
	return false ;
	}

//elog	<< "msg_N> Added user: "
//	<< *newClient
//	<< endl ;

theServer->PostEvent( EVT_NICK, static_cast< void* >( newClient ) ) ;

return true ;
}

} // namespace gnuworld
