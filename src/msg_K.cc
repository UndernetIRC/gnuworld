/**
 * msg_K.cc
 */

#include	<new>
#include	<string>

#include	<cassert>

#include	"server.h"
#include	"iClient.h"
#include	"Channel.h"
#include	"ChannelUser.h"
#include	"events.h"
#include	"Network.h"

#include	"ELog.h"
#include	"StringTokenizer.h"

const char msg_K_cc_rcsId[] = "$Id: msg_K.cc,v 1.7 2002/04/10 07:10:10 isomer Exp $" ;
const char server_h_rcsId[] = __SERVER_H ;
const char iClient_h_rcsId[] = __ICLIENT_H ;
const char Channel_h_rcsId[] = __CHANNEL_H ;
const char ChannelUser_h_rcsId[] = __CHANNELUSER_H ;
const char events_h_rcsId[] = __EVENTS_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char StringTokenizer_h_rcsId[] = __STRINGTOKENIZER_H ;

namespace gnuworld
{

using std::string ;
using std::endl ;

// AIAAA K #coder-com 0C] :This is now an IRCoperator only channel
// Note that when a user is kicked from a channel, the user is not
// actually parted.  A separate MSG_L message will be issued after
// the MSG_K().
//
// About the zombie state
// ----------------------
// P10 CHECK POINT:
// If the KICK message is issued for a user on the same
// server of the sender, the KICK is authoritative, and
// we may proceed to remove the user from the channel.
// In all other cases, the server will issue a PART
// message for the user that will make us acknowledge
// the KICK has happened. During this phase the user
// is placed in zombie state. However, the server has
// the ability to send a MODE or a JOIN while the user
// is in zombie state. If the MODE issued for the user
// is a +o, or JOIN is issued, the user gets reinstated
// in the channel and the zombie state is cleared out.
// The server can accept the MODE and the JOIN silently
// or bounce them back to the sender, thus invalidating
// the command sent.
//
int xServer::MSG_K( xParameters& Param )
{

// Verify that there are at least three arguments provided
// client_source_numeric #channel client_target_numeric
if( Param.size() < 3 )
	{
	// Invalid number of arguments
	elog	<< "xServer::MSG_K> Invalid number of arguments"
		<< endl ;

	// Return error
	return -1 ;
	}

// Is this a modeless channel?
// Can there even be kicks on modeless channels?
if( '+' == Param[ 1 ][ 0 ] )
	{
	// Don't care about modeless channels
	return 0 ;
	}

// Find the source client
// The source can still be a server, so the srcClient here
// may be NULL
iClient* srcClient = Network->findClient( Param[ 0 ] ) ;

// Find the target client
iClient* destClient = Network->findClient( Param[ 2 ] ) ;

// Did we find the target client?
if( NULL == destClient )
	{
	// Nope, log the error
	elog	<< "xServer::MSG_K> ("
		<< Param[ 1 ]
		<< ") Unable to find client: "
		<< Param[ 2 ]
		<< endl ;

	// Return error
	return -1 ;
	}

// Find the channel in question.
Channel* theChan = Network->findChannel( Param[ 1 ] ) ;

// Did we find the channel?
if( NULL == theChan )
	{
	// Nope, log the error
	elog	<< "xServer::MSG_K> Unable to find channel: "
		<< Param[ 1 ]
		<< endl ;

	// Return error
	return -1 ;
	}

ChannelUser* destChanUser = theChan->findUser( destClient ) ;
if( NULL == destChanUser )
	{
	elog	<< "xServer::msg_K> Unable to find ChannelUser "
		<< "for channel "
		<< theChan->getName()
		<< ", iClient: "
		<< *destClient
		<< endl ;
	// Return error
	return -1;
	}

// On a network with more than 2 servers, the chances are greater
// that the two clients will be on different servers...thus,
// initialize localKick to false here.
bool localKick = false ;

// Check to see if the source and destination user are
// on the same server.
if( srcClient != 0 )
	{
	if( srcClient->getIntYY() == destClient->getIntYY() )
		{
		// Local kick
		localKick = true ;
		}
	}

if( localKick )
	{
	// Only remove the ChannelUser from the channel if this
	// is a local kick...otherwise, the kick is unauthoritative
	theChan->removeUser( destClient ) ;

	// Deallocate the ChannelUser
	delete destChanUser ;

	// Remove the channel information from the client's internal
	// channel structure
	if( !destClient->removeChannel( theChan ) )
		{
		elog	<< "xServer::msg_K> Unable to remove channel "
			<< theChan->getName()
			<< " from the iClient "
			<< *destClient
			<< endl ;
		}
	}
else
	{
	// The kick is unauthoritative, the destination client
	// is now in the zombie state
	destChanUser->setZombie() ;

	elog	<< "xServer::msg_K> Adding zombie for user "
		<< *destChanUser
		<< " on channel "
		<< theChan->getName()
		<< endl ;
	}

// Post the channel kick event
PostChannelKick( theChan,
	srcClient,
	destClient,
	(Param.size() >= 4) ? Param[ 3 ] : string(),
	localKick ) ;

// Any users or services clients left in the channel?
if( theChan->empty() )
	{
	// Nope, remove the channel
	delete Network->removeChannel( theChan->getName() ) ;

	// TODO: Post event
	}

return 0 ;

}

} // namespace gnuworld
