/**
 * msg_PART.cc
 */

#include	<string>
#include	<iostream>

#include	"server.h"
#include	"events.h"
#include	"iClient.h"
#include	"Channel.h"
#include	"ChannelUser.h"
#include	"Network.h"
#include	"ELog.h"
#include	"xparameters.h"
#include	"StringTokenizer.h"

const char msg_PART_cc_rcsId[] = "$Id: msg_PART.cc,v 1.7 2001/05/18 15:27:10 dan_karrels Exp $" ;
const char server_h_rcsId[] = __SERVER_H ;
const char events_h_rcsId[] = __EVENTS_H ;
const char iClient_h_rcsId[] = __ICLIENT_H ;
const char Channel_h_rcsId[] = __CHANNEL_H ;
const char ChannelUser_h_rcsId[] = __CHANNELUSER_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char xParameters_h_rcsId[] = __XPARAMETERS_H ;
const char StringTokenizer_h_rcsId[] = __STRINGTOKENIZER_H ;

namespace gnuworld
{

using std::string ;
using std::endl ;

// nick PART #channel,#channel2 <part msg>
int xServer::MSG_PART( xParameters& Param )
{
// Verify that there are at least 2 arguments:
// client_numeric #channel
if( Param.size() < 2 )
	{
	elog	<< "xServer::MSG_PART> Invalid number of arguments"
		<< endl ;
	return -1 ;
	}

// Find the client in question
iClient* theClient = Network->findNick( Param[ 0 ] ) ;

// Was the client found?
if( NULL == theClient )
	{
	// Nope, no matching client found

	// Log the error
	elog	<< "xServer::MSG_PART> ("
		<< Param[ 1 ]
		<< "): Unable to find client: "
		<< Param[ 0 ]
		<< endl ;

	// Return error
	return -1 ;
	}

// Tokenize the channel string
// Be sure to take into account the channel parting message
StringTokenizer _st( Param[ 1 ] ) ;
StringTokenizer st( _st[ 0 ], ',' ) ;

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
		elog	<< "xServer::MSG_PART> Unable to find channel: "
			<< st[ i ]
			<< endl ;

		// Continue on to the next channel
		continue ;
		}

	// Remove client<->channel associations

	// Remove and deallocate the ChannelUser instance from this
	// channel's ChannelUser structure.
	delete theChan->removeUser( theClient ) ;

	// Remove this channel from this client's channel structure.
	theClient->removeChannel( theChan ) ;

	// Post the event to the clients listening for events on this
	// channel, if any.
	PostChannelEvent( EVT_PART, theChan,
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

return 0 ;
}

} // namespace gnuworld
