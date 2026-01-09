/*
 * msg_CM. cc
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
 * $Id: msg_CM.cc,v 1.11 2008/04/16 20:29:37 danielaustin Exp $
 */

#include	<map>
#include	<string>
#include	<iostream>

#include	"gnuworld_config.h"
#include	"server.h"
#include	"Channel.h"
#include	"ChannelUser.h"
#include	"Network.h"
#include	"iClient.h"
#include	"xparameters.h"
#include	"ELog.h"
#include	"ServerCommandHandler.h"

namespace gnuworld
{
using std::pair ;
using std::make_pair ;
using std::endl ;

CREATE_HANDLER(msg_CM)

/**
 * CLEARMODE message handler.
 * ZZAAA CM #channel obv
 * The above message would remove all ops, bans, and voice modes
 *  from channel #channel.
 */
bool msg_CM::Execute( const xParameters& Param )
{
if( Param.size() < 3 )
	{
	elog	<< "msg_CM> Invalid number of parameters"
		<< endl ;
	return false ;
	}

Channel* tmpChan = Network->findChannel( Param[ 1 ] ) ;
if( !tmpChan )
	{
	// Log Error.
	elog	<< "msg_CM> Unable to locate channel: "
		<< Param[ 1 ]
		<< endl ;
	return false ;
	}

/*
 * First, determine what we are going to clear.
 */
std::string Modes = Param[ 2 ] ;

// These three variables will be set to true if we are to clear either
// the ops, voice, or bans, respectively
bool clearOps = false ;
bool clearVoice = false ;
bool clearBans = false ;

// Go ahead and post the server mode event
iServer* serverSource = 0 ;

if( NULL != strchr( Param[ 0 ], '.' ) )
{
	// Server, by name
	serverSource = Network->findServerName( Param[ 0 ] ) ;
} else if( strlen( Param[ 0 ] ) < 3 )
{
	// 1 or 2 char numeric, server
	serverSource = Network->findServer( Param[ 0 ] ) ;
}

if (serverSource != 0)
	theServer->PostChannelEvent( EVT_SERVERMODE, tmpChan,
		static_cast< void* >( serverSource ) );

xServer::modeVectorType modeVector ;

for( std::string::size_type i = 0 ; i < Modes.size() ; i++ )
	{
	switch( Modes[ i ] )
		{
		case 'o':
			clearOps = true ;
//			elog	<< tmpChan->getName()
//				<< "msg_CM> Doing CLEAR_CHANOPS"
//				<< endl;
			break ;
		case 'v':
			clearVoice = true ;
//			elog	<< tmpChan->getName()
//				<< "msg_CM> Doing CLEAR_VOICED"
//				<< endl;
			break ;
		case 's':
			modeVector.push_back( make_pair(
				false, Channel::MODE_S ) ) ;
//			elog	<< tmpChan->getName()
//				<< "msg_CM> Doing CLEAR_SECRET"
//				<< endl;
			break ;
		case 'r':
			modeVector.push_back( make_pair(
				false, Channel::MODE_R ) ) ;
//			elog	<< tmpChan->getName()
//				<< "msg_CM> Doing CLEAR_REGISTER"
//				<< endl;
			break ;
		// Do not remove mode R (Channel::MODE_REG)
		case 'D':
			modeVector.push_back( make_pair(
				false, Channel::MODE_D ) ) ;
//			elog	<< tmpChan->getName()
//				<< "msg_CM> Doing CLEAR_MODED"
//				<< endl;
			break ;
		case 'c':
			modeVector.push_back(make_pair(
				false, Channel::MODE_C));
			//			elog	<< tmpChan->getName()
			//				<< "msg_CM> Doing CLEAR_MODE_C"
			//				<< endl;
			break;
		case 'C':
			modeVector.push_back(make_pair(
				false, Channel::MODE_CTCP));
			//			elog	<< tmpChan->getName()
			//				<< "msg_CM> Doing CLEAR_MODE_CTCP"
			//				<< endl;
			break;
		case 'm':
			modeVector.push_back( make_pair(
				false, Channel::MODE_M ) ) ;
//			elog	<< tmpChan->getName()
//				<< "msg_CM> Doing CLEAR_MODERATED"
//				<< endl;
			break ;
		case 't':
			modeVector.push_back( make_pair(
				false, Channel::MODE_T ) ) ;
//			elog	<< tmpChan->getName()
//				<< "msg_CM> Doing CLEAR_TOPICLIMIT"
//				<< endl;
			break ;
		case 'i':
			modeVector.push_back( make_pair(
				false, Channel::MODE_I ) ) ;
//			elog	<< tmpChan->getName()
//				<< "msg_CM> Doing CLEAR_INVITEONLY"
//				<< endl;
			break ;
		case 'n':
			modeVector.push_back( make_pair(
				false, Channel::MODE_N ) ) ;
//			elog	<< tmpChan->getName()
//				<< "msg_CM> Doing CLEAR_NOPRIVMSGS"
//				<< endl;
			break ;
		case 'p':
			modeVector.push_back( make_pair(
				false, Channel::MODE_P ) ) ;
//			elog	<< tmpChan->getName()
//				<< "msg_CM> Doing CLEAR_NOPRIVMSGS"
//				<< endl;
			break ;
		case 'u':
			modeVector.push_back( make_pair(
				false, Channel::MODE_PART ) ) ;
//			elog	<< tmpChan->getName()
//				<< "msg_CM> Doing CLEAR_PART"
//				<< endl;
			break ;
		case 'M':
			modeVector.push_back( make_pair(
				false, Channel::MODE_MNOREG ) ) ;
//			elog	<< tmpChan->getName()
//				<< "msg_CM> Doing CLEAR_MNOREG"
//				<< endl;
			break ;
		case 'Z':
			modeVector.push_back( make_pair(
				false, Channel::MODE_Z ) ) ;
//			elog	<< tmpChan->getName()
//				<< "msg_CM> Doing CLEAR_Z"
//				<< endl;
			break ;
		case 'k':
			theServer->OnChannelModeK( tmpChan, false, 0, 
				std::string() ) ;
			tmpChan->setKey( std::string() );
//			elog	<< tmpChan->getName()
//				<< "msg_CM> Doing CLEAR_KEY"
//				<< endl;
			break ;
		case 'b':
			clearBans = true ;
			tmpChan->removeAllBans();
//			elog	<< tmpChan->getName()
//				<< "msg_CM> Doing CLEAR_BANS"
//				<< endl;
			break ;
		case 'l':
			theServer->OnChannelModeL( tmpChan, false, 0, 0 ) ;
//			elog	<< tmpChan->getName()
//				<< "msg_CM> Doing CLEAR_LIMIT"
//				<< endl;
			break ;
		default:
			// Unknown mode
			elog	<< "msg_CM> Unknown mode: "
				<< Modes[ i ]
				<< endl ;
			break ;
		} // switch
	} // for

if( !modeVector.empty() )
	{
	theServer->OnChannelMode( tmpChan, 0, modeVector ) ;
	}

if( clearOps || clearVoice )
	{
	/*
	 * Lets loop over everyone in the channel and either deop
	 * or devoice them.
	 */
	xServer::opVectorType opVector ;
	xServer::voiceVectorType voiceVector ;

	for( Channel::const_userIterator ptr = tmpChan->userList_begin();
		ptr != tmpChan->userList_end() ; ++ptr )
		{
		if( clearOps && ptr->second->isModeO() )
			{
			ptr->second->removeModeO();
			opVector.push_back( pair< bool, ChannelUser* >
				( false, ptr->second ) ) ;
			}
		if( clearVoice && ptr->second->isModeV() )
			{
			ptr->second->removeModeV();
			voiceVector.push_back( pair< bool, ChannelUser* >
				( false, ptr->second ) ) ;
			}
		}

	if( !voiceVector.empty() )
		{
		theServer->OnChannelModeV( tmpChan, 0, voiceVector ) ;
		}
	if( !opVector.empty() )
		{
		theServer->OnChannelModeO( tmpChan, 0, opVector ) ;
		}
	} // if( clearOps || clearVoice )

if( clearBans )
	{
	xServer::banVectorType banVector ;

	for( Channel::banIterator ptr = tmpChan->banList_begin(),
		endPtr = tmpChan->banList_end() ;
		ptr != endPtr ; ++ptr )
		{
		banVector.push_back( pair< bool, std::string >
			( false, *ptr ) ) ;
		}

	theServer->OnChannelModeB( tmpChan, 0, banVector ) ;
	} // if( clearBans )

return true ;
}

} // namespace gnuworld
