/* gnutest.h */

#ifndef __GNUTEST_H
#define __GNUTEST_H "$Id: gnutest.h,v 1.3 2001/06/14 22:14:13 dan_karrels Exp $"

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
	virtual int	OnChannelEvent( const channelEventType&,
		Channel*,
		void* data1 = 0, void* data2 = 0,
		void* data3 = 0, void* data4 = 0 ) ;

	/**
	 * This method is called when a network event occurs, and
	 * the client has registered for that event.
	 */
	virtual int	OnEvent( const eventType& theEvent,
		void* data1 = 0, void* data2 = 0,
		void* data3 = 0, void* data4 = 0 ) ;

	/**
	 * This method is called for the client to burst all channels
	 * once the server connects to the network.
	 */
	virtual int	BurstChannels() ;

	/**
	 * This method is called when a network client messages
	 * this client.
	 */
	virtual int	OnPrivateMessage( iClient*, const string&,
				bool secure = false ) ;

	/**
	 * This method is called when a timer expires.
	 */
	virtual int	OnTimer( xServer::timerID, void* ) ;

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
