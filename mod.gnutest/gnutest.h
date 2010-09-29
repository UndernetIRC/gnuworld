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
 * $Id: gnutest.h,v 1.14 2005/01/17 23:09:54 dan_karrels Exp $
 */

#ifndef __GNUTEST_H
#define __GNUTEST_H "$Id: gnutest.h,v 1.14 2005/01/17 23:09:54 dan_karrels Exp $"

#include	<string>
#include	<vector>

#include	"client.h"
#include	"iClient.h"
#include	"Channel.h"
#include	"StringTokenizer.h"

namespace gnuworld
{

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
	gnutest( const std::string& ) ;

	/**
	 * Destructor closes all streams and deallocates any memory
	 * created by this instance.
	 */
	virtual ~gnutest() ;

	/**
	 * This method is invoked when this module is first loaded.
	 * This is a good place to setup timers, connect to DB, etc.
	 * At this point, the server may not yet be connected to the
	 * network, so please do not issue join/nick requests.
	 */
	virtual void OnAttach() ;

	/**
	 * This method is called when this module is being unloaded from
	 * the server.  This is a good place to cleanup, including
	 * deallocating timers, closing connections, closing log files,
	 * and deallocating private data stored in iClients.
	 */
	virtual void OnDetach( const std::string& =
			std::string( "Server Shutdown" ) ) ;

	/**
	 * This method is called when the server connects to the network.
	 * Note that if this module is attached while already connected
	 * to a network, this method is still invoked.
	 */
	virtual void OnConnect() ;

	/**
	 * This method is invoked when the server disconnects from
	 * its uplink.
	 */
	virtual void OnDisconnect() ;

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
	virtual void	BurstChannels() ;

	/**
	 * This method is called when a network client messages
	 * this client.
	 */
	virtual void	OnPrivateMessage( iClient*, const std::string&,
				bool secure = false ) ;

	/**
	 * This method is called when a channel message occurs
	 * in a channel in which an xClient resides, and the
	 * xClient is user mode -d.
	 */
	virtual void OnChannelMessage( iClient* Sender,
		Channel* theChan,
		const std::string& Message ) ;


	/**
	 * This method is invoked when a fake client belonging to this
	 * xClient receives a channel message.
	 */
	virtual void	OnFakeChannelMessage( iClient* srcClient,
				iClient* destClient,
				Channel* theChan,
				const std::string& message ) ;

	/**
	 * This method is invoked when a fake client belonging to this
	 * xClient receives a channel notice.
	 */
	virtual void	OnFakeChannelNotice( iClient* srcClient,
				iClient* destClient,
				Channel* theChan,
				const std::string& message ) ;

	/**
	 * This method is called when a network message arrives for
	 * one of the fake clients owned by this xClient.
	 */
	virtual void	OnFakePrivateMessage( iClient* srcClient,
				iClient* destClient,
				const std::string& message,
				bool secure = false ) ;

	/**
	 * This method is called when a network notice arrives for
	 * one of the fake clients owned by this xClient.
	 */
	virtual void	OnFakePrivateNotice( iClient* srcClient,
				iClient* destClient,
				const std::string& message,
				bool secure = false ) ;

	/**
	 * Invoked when a fake client of this xClient receives a
	 * channel CTCP.
	 */
	virtual void	OnFakeChannelCTCP( iClient* srcClient,
				iClient* fakeClient,
				Channel* theChan,
				const std::string& command,
				const std::string& message ) ;

	/**
	 * Invoked when a fake client of this xClient receives a
	 * channel CTCP.
	 */
	virtual void	OnFakeCTCP( iClient* srcClient,
				iClient* fakeClient,
				const std::string& command,
				const std::string& message,
				bool secure = false ) ;

	/**
	 * This method is called when a timer expires.
	 */
	virtual void	OnTimer( const xServer::timerID&, void* ) ;

protected:

	/**
	 * Spawn a fake client.
	 */
	virtual void	spawnClient( iClient* requestingClient,
				const StringTokenizer& st ) ;

	/**
	 * Spawn a fake server.
	 */
	virtual void	spawnServer( iClient* requestingClient,
				const StringTokenizer& st ) ;

	/**
	 * Remove a fake client.
	 */
	virtual void	removeClient( iClient* requestingClient,
				const StringTokenizer& st ) ;

	/**
	 * Remove a fake server.
	 */
	virtual void	removeServer( iClient* requestingClient,
				const StringTokenizer& st ) ;

	/**
	 * Request that a spawned (fake) client join a channel.
	 */
	virtual void	spawnJoin( iClient* requestingClient,
				const StringTokenizer& st ) ;

	/**
	 * Request that a spawned (fake) client part a channel.
	 */
	virtual void	spawnPart( iClient* requestingClient,
				const StringTokenizer& st ) ;

	/**
	 * Report information about a channel.
	 */
	virtual void	chanInfo( const Channel* theChan ) ;

	/**
	 * This is the name of the operator only channel on which this
	 * client sits.
	 */
	std::string	operChan ;

	/**
	 * I have no idea what this is.
	 */
	std::string	timerChan ;

	/**
	 * This type is used for the helpTable, which will store basic
	 * information about what commands gnutest supports.
	 */
	typedef std::map< std::string, std::string > helpTableType ;

	/**
	 * This is used to store basic information about what commands 
	 * gnutest supports.
	 */
	helpTableType	helpTable ;

} ;

} // namespace gnuworld

#endif // __GNUTEST_H
