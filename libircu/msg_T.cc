/**
 * msg_T.cc
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
 * $Id: msg_T.cc,v 1.5 2005/01/12 03:50:28 dan_karrels Exp $
 */

#include	<iostream>

#include	"server.h"
#include	"xparameters.h"
#include	"Network.h"
#include	"ELog.h"
#include	"gnuworld_config.h"
#include	"Channel.h"
#include	"ServerCommandHandler.h"

RCSTAG( "$Id: msg_T.cc,v 1.5 2005/01/12 03:50:28 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::endl ;

CREATE_HANDLER(msg_T)

// Channel topics currently are not tracked.
// kAI T #omniplex :-=[ Washington.DC.US.Krushnet.Org / Luxembourg.
// LU.EU.KrushNet.Org Admin Channel ]=-
bool msg_T::Execute( const xParameters& Param )
{
if( Param.size() < 3 )
	{
	elog	<< "msg_T> Invalid number of arguments"
		<< endl ;
	return false ;
	}

Channel* theChan = Network->findChannel( Param[ 1 ] ) ;
if( 0 == theChan )
	{
	elog	<< "msg_T> Unable to locate channel: "
		<< Param[ 1 ]
		<< endl;
	return false ;
	}

#ifdef TOPIC_TRACK

theChan->setTopic( Param[ 2 ] ) ;

#endif // TOPIC_TRACK

// srcClient may be NULL if a server is setting the topic
iClient* srcClient = Network->findClient( Param[ 0 ] ) ;

string newTopic( Param[ 2 ] ) ;

// No need to pass the new topic, it has already been stored
// in the theChan
theServer->PostChannelEvent( EVT_TOPIC,
	theChan,
	static_cast< void* >( srcClient ),
	static_cast< void* >( &newTopic ) ) ;

return true ;
}

} // namespace gnuworld
