/**
 * msg_L.cc
 */

#include	<new>
#include	<string>

#include	"server.h"
#include	"iClient.h"
#include	"Channel.h"
#include	"ChannelUser.h"
#include	"Network.h"
#include	"ELog.h"
#include	"StringTokenizer.h"
#include	"events.h"
#include	"xparameters.h"

const char server_h_rcsId[] = __SERVER_H ;
const char xparameters_h_rcsId[] = __XPARAMETERS_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char events_h_rcsId[] = __EVENTS_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char Channel_h_rcsId[] = __CHANNEL_H ;
const char ChannelUser_h_rcsId[] = __CHANNELUSER_H ;
const char iClient_h_rcsId[] = __ICLIENT_H ;
const char StringTokenizer_h_rcsId[] = __STRINGTOKENIZER_H ;
const char msg_L_cc_rcsId[] = "$Id: msg_L.cc,v 1.6 2002/04/28 16:11:23 dan_karrels Exp $" ;

namespace gnuworld
{

using std::string ;
using std::endl ;

/**
 * Someone has just left a channel.
 * AABBB L #channel
 */
int xServer::MSG_L( xParameters& Param )
{

// Verify that there are at least 2 arguments:
// client_numeric #channel
if( Param.size() < 2 )
	{
	elog	<< "xServer::MSG_L> Invalid number of arguments"
		<< endl ;
	return -1 ;
	}

// Find the client in question
iClient* theClient = Network->findClient( Param[ 0 ] ) ;

// Was the client found?
if( NULL == theClient )
	{
	// Nope, no matching client found

	// Log the error
	elog	<< "xServer::MSG_L> ("
		<< Param[ 1 ]
		<< "): Unable to find client: "
		<< Param[ 0 ]
		<< endl ;

	// Return error
	return -1 ;
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
		elog	<< "xServer::MSG_L> Unable to find channel: "
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
		elog	<< "xServer::MSG_L> Unable to remove "
			<< *theClient
			<< " from channel: "
			<< *theChan
			<< endl ;
		}
	delete theChanUser ; theChanUser = 0 ;

	// Remove this channel from this client's channel structure.
	if( !theClient->removeChannel( theChan ) )
		{
		elog	<< "xServer::MSG_L> Unable to remove iClient "
			<< *theClient
			<< " from channel "
			<< *theChan
			<< endl ;
		}

	// Post the event to the clients listening for events on this
	// channel, if any.
	// TODO: Update message posting
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
