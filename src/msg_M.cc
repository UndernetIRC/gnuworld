/**
 * msg_M.cc
 */

#include	<new>
#include	<string>
#include	<vector>
#include	<iostream>

#include	"misc.h"
#include	"events.h"

#include	"server.h"
#include	"iClient.h"
#include	"Channel.h"
#include	"ChannelUser.h"
#include	"Network.h"
#include	"ELog.h"
#include	"StringTokenizer.h"

const char msg_M_cc_rcsId[] = "$Id: msg_M.cc,v 1.10 2001/06/24 13:49:14 dan_karrels Exp $" ;
const char misc_h_rcsId[] = __MISC_H ;
const char events_h_rcsId[] = __EVENTS_H ;
const char server_h_rcsId[] = __SERVER_H ;
const char iClient_h_rcsId[] = __ICLIENT_H ;
const char Channel_h_rcsId[] = __CHANNEL_H ;
const char ChannelUser_h_rcsId[] = __CHANNELUSER_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char StringTokenizer_h_rcsId[] = __STRINGTOKENIZER_H ;

namespace gnuworld
{

using std::string ;
using std::vector ;
using std::endl ;
using std::ends ;

// Mode change
// OAD M ripper_ :+owg
//
// i M #3dx +o eAA
// J[K M DEMET_33 :+i
int xServer::MSG_M( xParameters& Param )
{

if( Param.size() < 3 )
	{
	elog	<< "xServer::MSG_M> Invalid number of arguments"
		<< endl ;
	return -1 ;
	}

// This source stuff really isn't used here, but it's here for
// debugging and validation.
iServer* serverSource = 0 ;
iClient* clientSource = 0 ;

// Note that the order of this if/else if/else is important
if( NULL != strchr( Param[ 0 ], '.' ) )
	{
	// Server, by name
	serverSource = Network->findServerName( Param[ 0 ] ) ;
	}
else if( strlen( Param[ 0 ] ) >= 3 )
	{
	// Client numeric
	clientSource = Network->findClient( Param[ 0 ] ) ;
	}
else
	{
	// 1 or 2 char numeric, server
	serverSource = Network->findServer( Param[ 0 ] ) ;
	}

if( (NULL == clientSource) && (NULL == serverSource) )
	{
	elog	<< "xServer::MSG_M> Unable to find source: "
		<< Param[ 0 ]
		<< endl ;
	// return -1
	}

// Is it a user mode change?
if( '#' != Param[ 1 ][ 0 ] )
	{
	// Yup, process the user's mode change(s)
	onUserModeChange( Param ) ;

	// Return
	return 0 ;
	}

// Find the channel in question
Channel* theChan = Network->findChannel( Param[ 1 ] ) ;
if( NULL == theChan )
	{
	elog	<< "xServer::MSG_M> Unable to find channel: "
		<< Param[ 1 ]
		<< endl ;
	return -1 ;
	}

// Find the ChannelUser of the source client
// It is possible that the ChannelUser will be NULL, in the
// case that a server is setting the mode(s)
ChannelUser* theUser = 0 ;
if( clientSource != 0 )
	{
	theUser = theChan->findUser( clientSource ) ;
	if( NULL == theUser )
		{
		elog	<< "xServer::MSG_M> ("
			<< theChan->getName()
			<< ") Unable to find channel user: "
			<< clientSource->getCharYYXXX()
			<< endl ;
		return -1 ;
		}
	}

bool polarity = true ;
xParameters::size_type argPos = 3 ;

opVectorType opVector ;
voiceVectorType voiceVector ;
banVectorType banVector ;

for( const char* modePtr = Param[ 2 ] ; *modePtr ; ++modePtr )
	{
	switch( *modePtr )
		{
		case '+':
			polarity = true ;
			break ;
		case '-':
			polarity = false ;
			break ;
		case 't':
			onChannelModeT( theChan,
				polarity, theUser ) ;
			break ;
		case 'n':
			onChannelModeN( theChan,
				polarity, theUser ) ;
			break ;
		case 's':
			onChannelModeS( theChan,
				polarity, theUser ) ;
			break ;
		case 'p':
			onChannelModeP( theChan,
				polarity, theUser ) ;
			break ;
		case 'm':
			onChannelModeM( theChan,
				polarity, theUser ) ;
			break ;
		case 'i':
			onChannelModeI( theChan,
				polarity, theUser ) ;
			break ;

		// Channel mode l only has an argument if
		// it is being added, but not removed
		case 'l':
			onChannelModeL( theChan,
				polarity, theUser,
				polarity ? atoi( Param[ argPos++ ] )
					: 0 ) ;
			break ;

		// Channel mode k always has an argument
		case 'k':
			onChannelModeK( theChan,
				polarity, theUser,
				Param[ argPos++ ] ) ;
			break ;
		case 'o':
			{
			iClient* targetClient = Network->findClient(
				Param[ argPos++ ] ) ;
			if( NULL == targetClient )
				{
				elog	<< "xServer::MSG_M> Unable to "
					<< "find op target client: "
					<< Param[ argPos - 1 ]
					<< endl ;
				break ;
				}
			ChannelUser* targetUser = theChan->findUser(
				targetClient ) ;
			if( NULL == targetUser )
				{
				elog	<< "xServer::MSG_M> Unable to "
					<< "find op target user: "
					<< Param[ argPos - 1 ]
					<< endl ;
				break ;
				}
			opVector.push_back(
				pair< bool, ChannelUser* >(
				polarity, targetUser ) ) ;

			// If the op mode is +o, remove the ZOMBIE
			// state from this user.
			if( polarity && targetUser->isZombie() )
				{
				targetUser->removeZombie() ;
				elog	<< "xServer::msg_M> Removing "
					<< "zombie"
					<< endl ;
				}
			break ;
			}
		case 'v':
			{
			iClient* targetClient = Network->findClient(
				Param[ argPos++ ] ) ;
			if( NULL == targetClient )
				{
				elog	<< "xServer::MSG_M> Unable to "
					<< "find voice target client: "
					<< Param[ argPos - 1 ]
					<< endl ;
				break ;
				}
			ChannelUser* targetUser = theChan->findUser(
				targetClient ) ;
			if( NULL == targetUser )
				{
				elog	<< "xServer::MSG_M> Unable to "
					<< "find voice target user: "
					<< Param[ argPos - 1 ]
					<< endl ;
				break ;
				}
			voiceVector.push_back(
				pair< bool, ChannelUser* >(
				polarity, targetUser ) ) ;
			break ;
			}
		case 'b':
			{
			const char* targetBan = Param[ argPos++ ] ;
			banVector.push_back(
				pair< bool, string >(
				polarity, string( targetBan ) ) ) ;
			break ;
			}

		} // switch()
	} // for()

if( !opVector.empty() )
	{
	onChannelModeO( theChan, theUser, opVector ) ;
	}
if( !voiceVector.empty() )
	{
	onChannelModeV( theChan, theUser, voiceVector ) ;
	}
if( !banVector.empty() )
	{
	onChannelModeB( theChan, theUser, banVector ) ;
	}

return 0 ;
}

void xServer::onUserModeChange( xParameters& Param )
{

// Since users aren't allowed to change modes for anyone other than
// themselves, there is no need to lookup the second user argument
// For some reason, when a user changes his/her/its modes, it still
// specifies the second argument to be nickname instaed of numeric.
iClient* theClient = Network->findNick( Param[ 1 ] ) ;
if( NULL == theClient )
	{
	elog	<< "xServer::MSG_M> Unable to find target client: "
		<< Param[ 1 ]
		<< endl ;
	return ;
	}

// Local channels are not propogated across the network.

// It's important that the mode '+' be default
bool plus = true ;

for( const char* modePtr = Param[ 2 ] ; *modePtr ; ++modePtr )
	{
	switch( *modePtr )
		{
		case '+':
			plus = true ;
			break;
		case '-':
			plus = false ;
			break;
		case 'i':
			if( plus )	theClient->setModeI() ;
			else		theClient->removeModeI() ;
			break ;
		case 'k':
			if( plus )	theClient->setModeK() ;
			else		theClient->removeModeK() ;
			break ;
		case 'd':
			if( plus )	theClient->setModeD() ;
			else		theClient->removeModeD() ;
			break ;
		case 'w':
			if( plus )	theClient->setModeW() ;
			else		theClient->removeModeW() ;
			break ;
		case 'o':
		case 'O':
			if( plus )
				{
				theClient->setModeO() ;
				PostEvent( EVT_OPER,
					static_cast< void* >( theClient ) ) ;
				}
			else
				{
//				elog	<< "xServer::onUserModeChange> "
//					<< "Caught -o for user: "
//					<< *theClient
//					<< endl ;
				theClient->removeModeO() ;
				}
			break ;
		default:
			break ;
		} // close switch
	} // close for

}

} // namespace gnuworld
