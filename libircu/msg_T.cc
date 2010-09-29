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
 * $Id: msg_T.cc,v 1.8 2005/06/20 11:26:33 kewlio Exp $
 */

#include	<string>
#include	<iostream>

#include	"gnuworld_config.h"
#include	"ip.h"
#include	"server.h"
#include	"xparameters.h"
#include	"Network.h"
#include	"ELog.h"
#include	"Channel.h"
#include	"ServerCommandHandler.h"

RCSTAG( "$Id: msg_T.cc,v 1.8 2005/06/20 11:26:33 kewlio Exp $" ) ;

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

// srcClient may be NULL if a server is setting the topic
iClient* srcClient = Network->findClient( Param[ 0 ] ) ;

#ifdef TOPIC_TRACK
if (Param.size() == 5)
{
	/* this is a .12 hub! */
	/* params = numeric, channel, channel creation ts, topic ts, topic */
	theChan->setTopic(Param[4]);
	theChan->setTopicTS(atoi(Param[3]));
} else {
	/* this is a .11 hub! (3 arguments) */
	/* params = numeric, channel, topic */
	theChan->setTopic(Param[2]);
	theChan->setTopicTS(::time(NULL));
}
if (srcClient == NULL)
{
	theChan->setTopicWhoSet("unknown");
} else {
	std::string client_ip;
	client_ip = xIP(srcClient->getIP()).GetNumericIP();
	theChan->setTopicWhoSet(srcClient->getNickUserHost() + " [" + client_ip + "]");
}
#endif // TOPIC_TRACK

std::string newTopic(Param[ 2 ] ) ;

// No need to pass the new topic, it has already been stored
// in the theChan
theServer->PostChannelEvent( EVT_TOPIC,
	theChan,
	static_cast< void* >( srcClient ),
	static_cast< void* >( &newTopic ) ) ;

return true ;
}

} // namespace gnuworld
