/**
 * msg_J.cc
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
 * $Id: msg_J.cc,v 1.13 2002/05/27 17:18:13 dan_karrels Exp $
 */

#include	<new>
#include	<string>
#include	<iostream>

#include	<cassert>

#include	"server.h"
#include	"iClient.h"
#include	"Channel.h"
#include	"ChannelUser.h"
#include	"events.h"
#include	"Network.h"
#include	"ELog.h"
#include	"StringTokenizer.h"

const char server_h_rcsId[] = __SERVER_H ;
const char xparameters_h_rcsId[] = __XPARAMETERS_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char events_h_rcsId[] = __EVENTS_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char Channel_h_rcsId[] = __CHANNEL_H ;
const char iClient_h_rcsId[] = __ICLIENT_H ;
const char ChannelUser_h_rcsId[] = __CHANNELUSER_H ;
const char msg_J_cc_rcsId[] = "$Id: msg_J.cc,v 1.13 2002/05/27 17:18:13 dan_karrels Exp $" ;

namespace gnuworld
{

using std::string ;
using std::endl ;

/**
 * Someone has just joined a non-empty channel.
 *
 * 0AT J #coder-com
 * OAT J #coder-com,#blah
 */
int xServer::MSG_J( xParameters& Param )
{

// Verify that sufficient arguments have been provided
// client_numeric #channel[,#channel2,...]
if( Param.size() < 2 )
	{
	// Insufficient arguments provided, log the error
	elog	<< "xServer::MSG_J> Invalid number of arguments"
		<< endl ;

	// Return error
	return -1 ;
	}

// Find the client in question.
iClient* Target = Network->findClient( Param[ 0 ] ) ;

// Did we find the client?
if( NULL == Target )
	{
	// Nope, log the error
	elog	<< "xServer::MSG_J> ("
		<< Param[ 1 ] << ") Unable to find user: "
		<< Param[ 0 ]
		<< endl ;

	// Return error
	return -1 ;
	}

// Tokenize by ',', as the client may join more than one
// channel at once.
StringTokenizer st( Param[ 1 ], ',' ) ;

for( StringTokenizer::size_type i = 0 ; i < st.size() ; i++ )
	{

	// Is it a modeless channel?
	if( '+' == st[ i ][ 0 ] )
		{
		// Don't care about modeless channels
		continue ;
		}

	// Is the user parting all channels?
	if( '0' == st[ i ][ 0 ] )
		{
		// Yup, call userPartAllChannels which will update
		// the user's information and notify listening
		// services clients of the parts
		userPartAllChannels( Target ) ;

		// continue to next channel
		continue ;
		}

	// Attempt to allocate a ChannelUser structure for this
	// user<->channel association
	ChannelUser* theUser =
		new (std::nothrow) ChannelUser( Target ) ;
	assert( theUser != 0 ) ;

	// This variable represents which event actually occurs
	channelEventType whichEvent = EVT_JOIN ;

	// On a JOIN command, the channel should already exist.
	Channel* theChan = Network->findChannel( st[ i ] ) ;

	// Does the channel already exist?
	if( NULL == theChan )
		{
		// Nope, this transmutes to a CREATE
		// Create a new Channel to represent this
		// network channel
		theChan = new (std::nothrow)
			Channel( st[ i ], ::time( 0 ) ) ;
		assert( theChan != 0 ) ;

		// Add the channel to the network tables
		if( !Network->addChannel( theChan ) )
			{
			// Addition to network tables failed
			// Log the error
			elog	<< "xServer::MSG_J> Unable to add channel: "
				<< theChan->getName()
				<< endl ;

			// Prevent memory leaks by deallocating the
			// Channel and ChannelUser objects
			delete theChan ; theChan = 0 ;
			delete theUser ; theUser = 0 ;

			// Continue to next channel
			continue ;
			}

		// Since this is equivalent to a CREATE, set the user
		// as operator.
		theUser->setModeO() ;

		// Update the event type
		whichEvent = EVT_CREATE ;

		} // if( NULL == theChan )
	else if( theChan->findUser( Target ) != 0 )
		{
		// The user is already in the channel...check for
		// zombie state
		ChannelUser* oldUser = theChan->findUser( Target ) ;
		if( oldUser->getMode( ChannelUser::ZOMBIE ) )
			{
			// The user was in the zombie state
			// Remove the zombie state, and continue
			// to the next channel
			oldUser->removeMode( ChannelUser::ZOMBIE ) ;

			// TODO: Should this post an event?
			elog	<< "xServer::msg_J> Removed zombie: "
				<< *oldUser
				<< " on channel "
				<< theChan->getName()
				<< endl ;
			}
		else
			{
			// User was found in channel, no reason apparent
			elog	<< "xServer::MSG_J> Unexpectadly found "
				<< "user "
				<< *Target
				<< " in channel "
				<< theChan->getName()
				<< endl ;
			}

		// In either case, there is no need to add the newly
		// created ChannelUser to the channel, because it
		// is already there.
		delete theUser ; theUser = 0 ;

		// Continue, nothing more to do here
		continue ;
		}

	// Add a new ChannelUser representing this client to this
	// channel's user structure.
	if( !theChan->addUser( theUser ) )
		{
		// Addition of this ChannelUser to the Channel failed
		// Log the error
		elog	<< "xServer::MSG_J> Unable to add user "
			<< theUser->getNickName()
			<< " to channel: "
			<< theChan->getName()
			<< endl ;

		// Prevent memory leaks by deallocating the unused
		// ChannelUser object
		delete theUser ; theUser = 0 ;

		if( EVT_CREATE == whichEvent )
			{
			// The channel did not exist before
			// this message, so go ahead and
			// remove it
			Network->removeChannel( theChan->getName() ) ;

			// Do some cleanup
			delete theChan ; theChan = 0 ;
			}

		// Continue to next channel
		continue ;
		}

	// Add this channel to this client's channel structure.
	if( !Target->addChannel( theChan ) )
		{
		elog	<< "xServer::MSG_J> Unable to add channel "
			<< *theChan
			<< " to iClient "
			<< *Target
			<< endl ;

		// Remove the ChannelUser from this channel, and
		// deallocate the ChannelUser to prevent memory
		// leaks
		theChan->removeUser( theUser ) ;
		delete theUser ; theUser = 0 ;

		// Did we just create the channel?
		if( EVT_CREATE == whichEvent )
			{
			// Yup, remove the channel from the network
			// data structures
			Network->removeChannel( theChan->getName() ) ;

			// Prevent memory leaks by deallocating
			// the channel
			delete theChan ; theChan = 0 ;
			}

		// Continue on with the next channel
		continue ;
		}

	// Post the event to the clients listening for events on this
	// channel, if any.
	PostChannelEvent( whichEvent, theChan,
		static_cast< void* >( Target ),
		static_cast< void* >( theUser ) ) ;

	// TODO: Update event posting so that CREATE is also
	// passed the client who created the channel

	} // for()

return 0 ;

}

void xServer::userPartAllChannels( iClient* theClient )
{
// Artifact, user is parting all channels
for( iClient::channelIterator ptr = theClient->channels_begin(),
	endPtr = theClient->channels_end() ; ptr != endPtr ; ++ptr )
	{

	// Remove this ChannelUser from the Channel's internal
	// structure.
	// Deallocate the ChannelUser
	ChannelUser* theChanUser =  (*ptr)->removeUser( theClient ) ;
	if( NULL == theChanUser )
		{
		elog	<< "xServer::userPartAllChannels> Unable to "
			<< "remove iClient "
			<< *theClient
			<< " from channel "
			<< *(*ptr)
			<< endl ;
		}
	delete theChanUser ; theChanUser = 0 ;

	// BUG: This iClient has inconsistent state because
	// no channels have been removed from its internal
	// structure until the end of this method.

	// Post this event to all listeners
	PostChannelEvent( EVT_PART,
		*ptr,
		static_cast< void* >( theClient ) ) ; // iClient*

	// Is the channel empty of all network and services
	// clients?
	if( (*ptr)->empty() )
		{
		// TODO: Post event

		// Yup, remove the channel from the network channel
		// table
		delete Network->removeChannel( (*ptr)->getName() ) ;
		}
	}

// Just to be sure
theClient->clearChannels() ;

} // userPartAllChannels()

} // namespace gnuworld
