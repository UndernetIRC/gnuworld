/**
 * gnutest.h
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
 * $Id: gnutest.h,v 1.5 2003/06/28 16:26:46 dan_karrels Exp $
 */

#ifndef __GNUTEST_H
#define __GNUTEST_H "$Id: gnutest.h,v 1.5 2003/06/28 16:26:46 dan_karrels Exp $"

#include	<string>
#include	<vector>

#include	"client.h"
#include	"iClient.h"
#include	"Channel.h"

namespace gnuworld
{

using std::string ;
using std::vector ;

/**
 * The purpose of this class is to test new features added to
 * the gnuworld server core.
 */
class gnutest : public xClient
{

public:
	/**
	 * Constructor receives name of config file.
	 */
	gnutest( const string& ) ;

	/**
	 * Destructor closes all streams and deallocates any memory
	 * created by this instance.
	 */
	virtual ~gnutest() ;

	/**
	 * This method is called when a channel event occurs on one
	 * of the channels for which this client has requested to
	 * be notified of events.
	 */
	virtual void	OnChannelEvent( const channelEventType&,
		Channel*,
		void* data1 = 0, void* data2 = 0,
		void* data3 = 0, void* data4 = 0 ) ;

	/**
	 * This method is called when a network event occurs, and
	 * the client has registered for that event.
	 */
	virtual void	OnEvent( const eventType& theEvent,
		void* data1 = 0, void* data2 = 0,
		void* data3 = 0, void* data4 = 0 ) ;

	/**
	 * This method is called for the client to burst all channels
	 * once the server connects to the network.
	 */
	virtual bool	BurstChannels() ;

	/**
	 * This method is called when a network client messages
	 * this client.
	 */
	virtual void	OnPrivateMessage( iClient*, const string&,
				bool secure = false ) ;

	/**
	 * This method is called when a timer expires.
	 */
	virtual void	OnTimer( xServer::timerID, void* ) ;

	/**
	 * This method returns true if the client is on the given
	 * channel, by name; false otherwise.
	 */
	virtual bool	isOnChannel( const string& chanName ) const ;

	/**
	 * This method returned true if the client is on the given
	 * channel, by pointer; false otherwise.
	 */
	virtual bool	isOnChannel( const Channel* theChan ) const ;

protected:

	/**
	 * This method is used to add a channel to the internal
	 * structure of channels which the client is on.
	 */
	virtual bool	addChan( Channel* ) ;

	/**
	 * This method is used to add a channel to the internal
	 * structure of channels which the client is on.
	 */
	virtual bool	addChan( const string& ) ;

	/**
	 * This method is used to remove a channel from the internal
	 * structure of channels which the client is on.
	 */
	virtual bool	removeChan( Channel* ) ;

	/**
	 * This method is used to remove a channel from the internal
	 * structure of channels which the client is on.
	 */
	virtual bool	removeChan( const string& ) ;

	/**
	 * This is the name of the operator only channel on which this
	 * client sits.
	 */
	string		operChan ;

	/**
	 * I have no idea what this is.
	 */
	string		timerChan ;

	/**
	 * The data structure used to store the names of channels
	 * which this channel is on.
	 */
	vector< string >	channels ;

} ;

} // namespace gnuworld

#endif // __GNUTEST_H
