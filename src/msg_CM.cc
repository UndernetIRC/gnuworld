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
 * $Id: msg_CM.cc,v 1.6 2002/07/05 01:10:05 dan_karrels Exp $
 */

#include	<iostream>

#include	"server.h"
#include	"Channel.h"
#include	"ChannelUser.h"
#include	"Network.h"
#include	"iClient.h"
#include	"xparameters.h"
#include	"ELog.h"
#include	"ServerCommandHandler.h"

const char msg_CM_cc_rcsId[] = "$Id: msg_CM.cc,v 1.6 2002/07/05 01:10:05 dan_karrels Exp $" ;

namespace gnuworld
{

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
string Modes = Param[ 2 ] ;

// These two variables will be set to true if we are to clear either
// the ops, voice, or bans, respectively
bool clearOps = false ;
bool clearVoice = false ;
bool clearBans = false ;

for( string::size_type i = 0 ; i < Modes.size() ; i++ )
	{
	switch( Modes[ i ] )
		{
		case 'o':
		case 'O':
			clearOps = true ;
			elog	<< tmpChan->getName()
				<< "msg_CM> Doing CLEAR_CHANOPS"
				<< endl;
			break ;
		case 'v':
		case 'V':
			clearVoice = true ;
			elog	<< tmpChan->getName()
				<< "msg_CM> Doing CLEAR_VOICED"
				<< endl;
			break ;
		case 's':
		case 'S':
			theServer->OnChannelModeS( tmpChan, false, 0 ) ;
			elog	<< tmpChan->getName()
				<< "msg_CM> Doing CLEAR_SECRET"
				<< endl;
			break ;
		case 'm':
		case 'M':
			theServer->OnChannelModeM( tmpChan, false, 0 ) ;
			elog	<< tmpChan->getName()
				<< "msg_CM> Doing CLEAR_MODERATED"
				<< endl;
			break ;
		case 't':
		case 'T':
			theServer->OnChannelModeT( tmpChan, false, 0 ) ;
			elog	<< tmpChan->getName()
				<< "msg_CM> Doing CLEAR_TOPICLIMIT"
				<< endl;
			break ;
		case 'i':
		case 'I':
			theServer->OnChannelModeI( tmpChan, false, 0 ) ;
			elog	<< tmpChan->getName()
				<< "msg_CM> Doing CLEAR_INVITEONLY"
				<< endl;
			break ;
		case 'n':
		case 'N':
			theServer->OnChannelModeN( tmpChan, false, 0 ) ;
			elog	<< tmpChan->getName()
				<< "msg_CM> Doing CLEAR_NOPRIVMSGS"
				<< endl;
			break ;
		case 'k':
		case 'K':
			theServer->OnChannelModeK( tmpChan, false, 0, 
				string() ) ;
			tmpChan->setKey( string() );
			elog	<< tmpChan->getName()
				<< "msg_CM> Doing CLEAR_KEY"
				<< endl;
			break ;
		case 'b':
		case 'B':
			clearBans = true ;
			tmpChan->removeAllBans();
			elog	<< tmpChan->getName()
				<< "msg_CM> Doing CLEAR_BANS"
				<< endl;
			break ;
		case 'l':
		case 'L':
			theServer->OnChannelModeL( tmpChan, false, 0, 0 ) ;
			elog	<< tmpChan->getName()
				<< "msg_CM> Doing CLEAR_LIMIT"
				<< endl;
			break ;
		default:
			// Unknown mode
			elog	<< "msg_CM> Unknown mode: "
				<< Modes[ i ]
				<< endl ;
			break ;
		} // switch
	} // for

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
		if( clearOps )
			{
			ptr->second->removeModeO();
			opVector.push_back( pair< bool, ChannelUser* >
				( false, ptr->second ) ) ;
			}
		if( clearVoice )
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
		banVector.push_back( pair< bool, string >
			( false, *ptr ) ) ;
		}

	theServer->OnChannelModeB( tmpChan, 0, banVector ) ;
	} // if( clearBans )

return true ;
}

} // namespace gnuworld
