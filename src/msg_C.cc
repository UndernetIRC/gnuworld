/**
 * msg_C.cc
 */

#include	<new>
#include	<string>
#include	<pair.h>

#include	<cassert>

#include	"server.h"
#include	"Network.h"
#include	"events.h"

#include	"ELog.h"
#include	"Socket.h"
#include	"StringTokenizer.h"
#include	"xparameters.h"
#include	"iClient.h"
#include	"Channel.h"
#include	"ChannelUser.h"

const char msg_C_cc_rcsId[] = "$Id: msg_C.cc,v 1.2 2001/03/03 00:17:57 dan_karrels Exp $" ;

using std::string ;
using std::endl ;

namespace gnuworld
{

/**
 * Someone has just joined an empty channel (create)
 * UAA C #xfactor 957134023
 * zBP C #OaXaCa,#UruApan,#skatos 957207634
 */
int xServer::MSG_C( xParameters& Param )
{

// Verify that there exist sufficient arguments to successfully
// handle this command
// client_numeric #channel[,#channel2,...] timestamp
if( Param.size() < 3 )
	{
	// Insufficient arguments provided
	elog	<< "xServer::MSG_C> Invalid number of parameters"
		<< endl ;

	// Return error
	return -1 ;
	}

// Find the client in question.
iClient* theClient = Network->findClient( Param[ 0 ] ) ;

// Did we find the client?
if( NULL == theClient )
	{
	// Nope, log the error
	elog	<< "xServer::MSG_C> ("
		<< Param[ 1 ]
		<< ") Unable to find client: "
		<< Param[ 0 ]
		<< endl ;

	// Return error
	return -1 ;
	}

// Grab the creation time.
time_t creationTime =
	static_cast< time_t >( atoi( Param[ Param.size() - 1 ] ) ) ;

// Tokenize based on ','.  Multiple channels may be put into the
// same C(REATE) command.
StringTokenizer st( Param[ 1 ], ',' ) ;

for( StringTokenizer::const_iterator ptr = st.begin() ; ptr != st.end() ;
	++ptr )
	{

	// Is this a modeless channel?
	if( '+' == (*ptr)[ 0 ] )
		{
		// Modeless channel, ignore it
		continue ;
		}

	// Find the channel in question.
	Channel* theChan = Network->findChannel( *ptr ) ;

	// Did we find the channel?
	if( NULL == theChan )
		{
		// Channel doesn't exist..this transmutes to a create
		theChan = new (nothrow) Channel( *ptr, creationTime ) ;
		assert( theChan != 0 ) ;

		// Add this channel to the network channel table
		if( !Network->addChannel( theChan ) )
			{
			// Addition failed, log the error
			elog	<< "xServer::MSG_C> Failed to add channel: "
				<< *theChan
				<< endl ;

			// Prevent memory leaks by removing the unused
			// channel
			delete theChan ;

			// continue to next one *shrug*
			continue ;
			}
		}

	// Create a new ChannelUser to represent this iClient's
	// membership in this channel.
	ChannelUser* theUser =
		new (nothrow) ChannelUser( theClient ) ;
	assert( theUser != 0 ) ;

	// The user who creates a channel is automatically +o
	theUser->setMode( ChannelUser::MODE_O ) ;

	// Build associations

	// Add the ChannelUser to the Channel's information
	if( !theChan->addUser( theUser ) )
		{
		// Addition failed, log the error
		elog	<< "xServer::MSG_C> Unable to add user "
			<< theUser->getNickName()
			<< " to channel "
			<< theChan->getName()
			<< endl ;

		// Prevent a memory leak by deallocating the unused
		// ChannelUser structure
		delete theUser ;

		// Continue to next channel
		continue ;
		}

	// Add this channel to the client's channel structure.
	theClient->addChannel( theChan ) ;

	// Notify all listening xClients of this event
	PostChannelEvent( EVT_CREATE, theChan,
		static_cast< void* >( theClient ) ) ;

	} // for()

return 0 ;

}

} // namespace gnuworld
