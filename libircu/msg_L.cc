/**
 * msg_L.cc
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
 * $Id: msg_L.cc,v 1.5 2005/03/25 03:07:29 dan_karrels Exp $
 */

#include	<new>
#include	<string>

#include	"gnuworld_config.h"
#include	"server.h"
#include	"iClient.h"
#include	"Channel.h"
#include	"ChannelUser.h"
#include	"Network.h"
#include	"ELog.h"
#include	"StringTokenizer.h"
#include	"events.h"
#include	"xparameters.h"
#include	"ServerCommandHandler.h"

RCSTAG( "$Id: msg_L.cc,v 1.5 2005/03/25 03:07:29 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::string ;
using std::endl ;

CREATE_HANDLER(msg_L)

/**
 * Someone has just left a channel.
 * AABBB L #channel
 */
bool msg_L::Execute( const xParameters& Param )
{
// Verify that there are at least 2 arguments:
// client_numeric #channel
if( Param.size() < 2 )
	{
	elog	<< "msg_L> Invalid number of arguments"
		<< endl ;
	return false ;
	}

// Find the client in question
iClient* theClient = Network->findClient( Param[ 0 ] ) ;

// Was the client found?
if( NULL == theClient )
	{
	// Nope, no matching client found

	// Log the error
	elog	<< "msg_L> ("
		<< Param[ 1 ]
		<< "): Unable to find client: "
		<< Param[ 0 ]
		<< endl ;

	// Return error
	return false ;
	}

// Tokenize the channel string
// Be sure to take into account the channel parting message
// This first line will first tokenize the string containing the
// names of the channel(s) being parted and the part message (if any)
// by ' ' (space).  The first token of this StringTokenizer (the channel
// name(s)) will then be tokenized by ':' to separate out the channel
// name(s) from the part message (if any).

// This first partString StringTokenizer will tokenize out the string
// into two tokens (by space): chan1[,chan2,...] :part message
StringTokenizer partString( Param[ 1 ] ) ;

// This tokenizer will separate out the invidividual channel names
StringTokenizer st( partString[ 0 ], ',' ) ;

// Iterate through all channels that this user is parting
for( StringTokenizer::size_type i = 0 ; i < st.size() ; ++i )
	{

	// Is this a modeless channel?
	if( '+' == st[ i ][ 0 ] )
		{
		// Ignore modeless channels
		continue ;
		}

	// Get the channel that was just parted.
	Channel* theChan = Network->findChannel( st[ i ] ) ;

	// Was the channel found?
	if( NULL == theChan )
		{
		// Channel not found, log the error
		elog	<< "msg_L> Unable to find channel: "
			<< st[ i ]
			<< endl ;

		// Continue on to the next channel
		continue ;
		}

	// Remove client<->channel associations

	// Remove and deallocate the ChannelUser instance from this
	// channel's ChannelUser structure.
	ChannelUser* theChanUser = theChan->removeUser( theClient ) ;
	if( NULL == theChanUser )
		{
		// This can happen if the user is a zombie
		// Since atm GNUWorld ignores zombies, just ignore
		// this message.
		continue ;

//		elog	<< "msg_L> Unable to remove "
//			<< *theClient
//			<< " from channel: "
//			<< *theChan
//			<< endl ;
		}
	delete theChanUser ; theChanUser = 0 ;

	// Remove this channel from this client's channel structure.
	if( !theClient->removeChannel( theChan ) )
		{
		elog	<< "msg_L> Unable to remove iClient "
			<< *theClient
			<< " from channel "
			<< *theChan
			<< endl ;
		}

	// Post the event to the clients listening for events on this
	// channel, if any.
	// TODO: Update message posting
	theServer->PostChannelEvent( EVT_PART, theChan,
		static_cast< void* >( theClient ) ) ;

	// Is the channel now empty, and no services clients are
	// on the channel?
	if( theChan->empty() )
		{
		// No users in the channel, remove it.
		delete Network->removeChannel( theChan->getName() ) ;

		// TODO: Post event
		}
	} // for

return true ;
}

} // namespace gnuworld
