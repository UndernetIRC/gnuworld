/**
 * server.h
 * This is the header file for the server itself.
 * This class originally created by Orlando Bassotto.
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
 * $Id: server.h,v 1.71 2002/07/16 15:57:10 dan_karrels Exp $
 */

#ifndef __SERVER_H
#define __SERVER_H "$Id: server.h,v 1.71 2002/07/16 15:57:10 dan_karrels Exp $"

#include	<string>
#include	<vector>
#include	<list>
#include	<sstream>
#include	<map>
#include	<queue>
#include	<algorithm>

#include	<ctime>
#include	<cassert>

#include	"Numeric.h"
#include	"iServer.h"
#include	"iClient.h"
#include	"Buffer.h"
#include	"events.h"
#include	"Gline.h"
#include	"xparameters.h"
#include	"misc.h"
#include	"moduleLoader.h"
#include	"ELog.h"
#include	"TimerHandler.h"
#include	"ServerCommandHandler.h"
#include	"ConnectionManager.h"
#include	"ConnectionHandler.h"
#include	"Connection.h"

namespace gnuworld
{

using std::string ;
using std::list ;
using std::vector ;
using std::stringstream ;
using std::priority_queue ;
using std::map ;

/// Forward declaration of xClient
class xClient ;

/// Forward declaration of Channel
class Channel ;

/// The type used to store messages for delivery to clients of
/// class xServer
enum MessageType
	{
	SRV_SUCCESS, // all ok
	SRV_RELOAD, // reload request
	SRV_QUIT, // server shutdown
	SRV_DISCONNECT // server disconnected
	} ;

/**
 * This class is the server proper; it is responsible for the connection
 * to the IRC network, and for maintaining the services clients.
 */
class xServer : public ConnectionManager, ConnectionHandler
{

protected:

	/**
	 * The type used to store server numerics for juped
	 * servers.
	 */
	typedef vector< unsigned int > jupedServerListType ;

	/**
	 * The type of the structure to hold Gline's internally.
	 */
	typedef list< Gline* >	glineListType ;

public:

	/**
	 * The xServer constructor.  It takes the arguments
	 * from the command line.
	 */
	xServer( int, char** ) ;

	/**
	 * Destroy the server and its clients, disconnect
	 * from network.
	 */
	virtual ~xServer() ;

	/**
	 * The actual run method.
	 */
	void		run() ;

	/**
	 * This type is used for passing information to handler
	 * methods for channel op mode changes.
	 */
	typedef vector< pair< bool, ChannelUser* > > opVectorType ;

	/**
	 * This type is used for passing information to handler
	 * methods for channel voice mode changes.
	 */
	typedef opVectorType voiceVectorType ;

	/**
	 * This type is used for passing information to handler
	 * methods for channel ban changes.
	 */
	typedef vector< pair< bool, string > > banVectorType ;

	/**
	 * The iterator type used to iterate through the structure
	 * of juped servers.
	 */
	typedef jupedServerListType::iterator jupedServerIterator ;

	/**
	 * The const iterator type used to iterate through the
	 * structure of juped servers.
	 */
	typedef jupedServerListType::const_iterator
		const_jupedServerIterator ;

	/**
	 * Return a const iterator to the beginning of the juped
	 * servers structure.
	 */
	inline const_jupedServerIterator jupedServers_begin() const
		{ return jupedServers.begin() ; }

	/**
	 * Return a const iterator to the end of the juped
	 * servers structure.
	 */
	inline const_jupedServerIterator jupedServers_end() const
		{ return jupedServers.end() ; }

	/**
	 * The iterator type used to iterate through the
	 * structure of glines.
	 */
	typedef glineListType::iterator glineIterator ;

	/**
	 * The const iterator type used to iterate through the
	 * structure of glines.
	 */
	typedef glineListType::const_iterator const_glineIterator ;

	/**
	 * Return a const iterator to the beginning of the gline
	 * structure.
	 */
	inline const_glineIterator gline_begin() const
		{ return glineList.begin() ; }

	/**
	 * Return a const iterator to the end of the gline
	 * structure.
	 */
	inline const_glineIterator gline_end() const
		{ return glineList.end() ; }

	/**
	 * Return an iterator to the beginning of the gline structure.
	 */
	inline glineIterator	gline_begin()
		{ return glineList.begin() ; }

	/**
	 * Return an iterator to the end of the gline structure.
	 */
	inline glineIterator	gline_end()
		{ return glineList.end() ; }

	/// Deprecated.
	inline size_t		getTotalReceived() const
		{ return 0 ; }

	/// Deprecated.
	inline size_t		getTotalSent() const
		{ return 0 ; }

	virtual void OnDisconnect( Connection* ) ;

	virtual void OnConnect( Connection* ) ;

	virtual void OnRead( Connection*, const string& ) ;

	/**
	 * Attach a fake server to this services server.
	 */
	virtual bool AttachServer( iServer*, const string& = "JUPED Server" ) ;

	/**
	 * Squit a server from the network and remove it
	 * from the network tables.
	 */
	virtual bool SquitServer( const string& name, const string& reason ) ;

	/**
	 * Append a std::string to the output buffer.
	 * The second argument determines if data should be written
	 * during burst time.
	 */
	virtual bool Write( const string& ) ;

	/**
	 * Similar to the above signature of Write() except that data
	 * will be written to the normal output buffer even during
	 * burst time.
	 */
	virtual bool WriteDuringBurst( const string& ) ;

	/**
	 * Append a C variable argument list/character array to the output
	 * buffer.
	 * Since this method uses a variable argument list, this
	 * method cannot support a final default argument -- this method
	 * defaults to NOT writing during burst.
	 */
	virtual bool Write( const char*, ... ) ;

	/**
	 * This method is similar to the above Write(), except
	 * that the data will be written to the normal output
	 * buffer even during burst time.
	 */
	virtual bool WriteDuringBurst( const char*, ... ) ;

	/**
	 * Append a std::stringstream to the output buffer.
	 * The second argument determines if data should be written
	 * during burst time.
	 */
	virtual bool Write( const stringstream& ) ;

	/**
	 * This method is similar to the above Write(), except
	 * that the data will be written to the normal output
	 * buffer even during burst time.
	 */
	virtual bool WriteDuringBurst( const stringstream& ) ;

	/**
	 * Process is responsible for parsing lines of data.
	 */
	virtual void Process( char* String ) ;

	/**
	 * Add a network gline and update glines table.
	 */
	virtual bool setGline( const string& setBy,
		const string& userHost,
		const string& reason,
		const time_t& duration, 
		const xClient* setClient = NULL,  
		const string& server = "*") ;

	/**
	 * Remove a network gline and update internal gline table.
	 */
	virtual bool removeGline( const string& userHost,
		const  xClient* remClient = NULL) ;

	/**
	 * Erase a gline from the internal data structures.  This does
	 * NOT send a message to the network; for that functionality,
	 * use RemoveGline() instead.
	 */
	virtual void eraseGline( glineIterator removeMe )
		{ glineList.erase( removeMe ) ; }

	/**
	 * Add a gline to the internal data structures.  This does
	 * NOT send a message to the network; for that functionality,
	 * use SetGline() instead.
	 */
	virtual void addGline( Gline* newGline )
		{ assert( newGline != 0 ) ;
		  glineList.push_back( newGline ) ;
		}

	/**
	 * Find a gline by lexical searching, case insensitive.
	 */
	virtual const Gline* findGline( const string& userHost ) const ;

	/**
	 * Find one or more glines matching a given userHost string.
	 */
	virtual vector< const Gline* > matchGline( const string& userHost )
		const ;

	/**
	 * Send all glines to the network.
	 */
	virtual void	sendGlinesToNetwork() ;

	/* Client stuff */

	/**
	 * Attach a fake client to a fake (juped) server.
	 * The server must exist and must already be attached
	 * to this server.
	 */
	virtual bool AttachClient( iClient* Client ) ;

	/**
	 * Attempt to load a client given its client module name.
	 */
	virtual void	LoadClient( const string& moduleName,
				const string& configFileName ) ;

	/**
	 * Attempt to unload a client given its module name.
	 */
	virtual void	UnloadClient( const string& moduleName,
				const string& reason ) ;

	/**
	 * Attempt to unload a client given its pointer.
	 */
	virtual void	UnloadClient( xClient*,
				const string& reason ) ;

	/**
	 * Attach a client to the server.  This will add the client
	 * to the internal table, and call the client's ImplementServer()
	 * method.
	 * Clients must *not* call this method, use LoadClient()
	 * instead.
	 */
	virtual bool AttachClient( xClient* Client,
			bool doBurst = false ) ;

	/**
	 * Attach a client to the server.  This will add the client
	 * to the internal table, and call the client's ImplementServer()
	 * method.
	 * Locate the client by its module name.
	 * Clients must *not* call this method, use LoadClient()
	 * instead.
	 */
	virtual bool AttachClient( const string& moduleName,
			const string& configFileName,
			bool doBurst = false ) ;

	/**
	 * Detach a client from the server.  This will call the
	 * client's Exit() method and remove the client from the
	 * internal tables.
	 * Clients must *not* call this method, use UnloadClient()
	 * instead.
	 */
	virtual bool DetachClient( const string& moduleName,
			const string& reason ) ;

	/**
	 * Detach a client from the server.  This will call the
	 * client's Exit() method and remove the client from the
	 * internal tables.
	 * Clients must *not* call this method, use UnloadClient()
	 * instead.
	 */
	virtual bool DetachClient( xClient* Client,
			const string& reason ) ;

	/**
	 * Output the information for a channel, and make the given
	 * xClient operator in that channel.
	 * This works at all times, bursting or not.
	 */
	virtual bool JoinChannel( xClient*, const string& chanName,
		const string& chanModes = "+tn",
		const time_t& joinTime = 0,
		bool getOps = true ) ;

	/**
	 * Notify the network that one of the services clients has
	 * parted a channel.
	 */
	virtual void PartChannel( xClient* theClient, const string& chanName,
			const string& reason = string() ) ;

	/**
	 * Notify the network that one of the services clients has
	 * parted a channel.
	 */
	virtual void PartChannel( xClient* theClient, Channel* theChan,
			const string& reason = string() ) ;

	/**
 	 * Handle the parting of a services client from a channel.  This
	 * method updates internal tables.
 	 */
	virtual void OnPartChannel( xClient* theClient,
			const string& chanName ) ;

	/**
 	 * Handle the parting of a services client from a channel.  This
	 * method updates internal tables.
 	 */
	virtual void OnPartChannel( xClient* theClient, Channel* theChan ) ;

	/**
	 * Handle the parting of a network client from a channel.  This method
	 * updates internal tables.
	 */
	virtual void OnPartChannel( iClient* theClient,
			const string& chanName ) ;

	/**
	 * Handle the parting of a network client from a channel.  This method
	 * updates internal tables.
	 */
	virtual void OnPartChannel( iClient* theClient, Channel* theChan ) ;

	/**
	 * Output the information about an xClient to the network.
	 * (localClient) is true when the xClient is to appear to
	 * reside on this xServer, false when it is to reside on
	 * a juped/fake server.
	 */
	virtual void	BurstClient( xClient*, bool localClient = true ) ;

	/**
	 * Send a wallops to the network as the server.
	 */
	virtual int	Wallops( const string& ) ;

	/**
	 * Set modes as the server, update internal tables, and notify
	 * all clients of the mode change(s).
	 * The first argument is the xClient requesting the mode
	 * change(s), and cannot be NULL.
	 */
	virtual int	Mode( xClient*, Channel*,
				const string& modes,
				const string& args ) ;

	/* Event registration stuff */

	/**
	 * RegisterEvent is called by xClient's wishing
	 * to receive a particular event.
	 * When a particular event occurs, the server will call
	 * OnEvent for each xClient registered to receive that
	 * event.
	 */
	virtual bool RegisterEvent( const eventType&, xClient* ) ;

	/**
	 * Halt delivery of the given event to the given xClient.
	 */
	virtual bool UnRegisterEvent( const eventType&, xClient* ) ;

	/**
	 * The channel event distribution system is rather
	 * expensive...such is the nature of IRC.
	 * Each channel name is converted to lower case.
	 * When a channel event occurs, the server will call
	 * each OnChannelEvent() with the event type
	 * and channel name for each xClient registered for
	 * that <event,channel> pair.
	 */
	virtual bool RegisterChannelEvent( const string&,
		xClient* ) ;

	/**
	 * Halt delivery of any channel event for the particular channel
	 * to the particular xClient.
	 */
	virtual bool UnRegisterChannelEvent( const string&,
		xClient* ) ;

	/**
	 * The type used to represent client timer events.
	 */
	typedef TimerHandler::timerID timerID ;

	/**
	 * Register for a timer event.  The first argument is the
	 * absolute time at which the timed event is to occur.
	 * The second argument is a pointer to an argument to be
	 * passed to the timer handler.
	 * Returns 0 on failure, a valid timerID otherwise.
	 */
	virtual timerID RegisterTimer( const time_t& absoluteTime,
		TimerHandler* theHandler,
		void* data = 0 ) ;

	/**
	 * Remove a timed event from the timer system.
	 * If data is non-NULL, the timer argument passed to
	 * RegisterTimer() will be returned through data.
	 * Return true if successful, false otherwise.
	 */
	virtual bool	UnRegisterTimer( const timerID&, void*& data ) ;

	/**
	 * Post a system event to the rest of the system.  Note
	 * that this method is public, so xClients may post
	 * events.
	 * The last argument is the an exclude xClient -- the
	 * event will NOT be sent to that client (in the case that
	 * an xClient calls PostEvent(), it may not want to receive
	 * that event back).
	 */
	virtual void PostEvent( const eventType&,
		void* = 0, void* = 0, void* = 0, void* = 0 ,
		const xClient* ourClient = 0) ;

	/**
	 * Post a channel event to the rest of the system.  Note
	 * that this method is public, so xClients may post
	 * channel events.
	 */
	virtual void PostChannelEvent( const channelEventType&,
		Channel* theChan,
		void* = 0, void* = 0, void* = 0, void* = 0 ) ;

	/**
	 * This method is called when a kick occurs on a channel
	 * for which this client is registered to receive events.
	 * The srcClient may be NULL, as the ircu protocol still
	 * allows servers to issue KICK commands.
	 * The authoritative variable is true if the kick
	 * transaction is complete, false otherwise.  If it is false,
	 * then the destClient is still on the channel pending
	 * a PART from its server, and it is in the ZOMBIE state.
	 */
	virtual void PostChannelKick( Channel* theChan,
			iClient* srcClient, // may be NULL
			iClient* destClient,
			const string& kickMessage,
			bool authoritative ) ;

	/**
	 * This variable represents "all channels."  Clients may
	 * register for events of this channel, and each will receive
	 * this event for every channel in existence on the network.
	 */
	const static string	CHANNEL_ALL ;

	/**
	 * Post a signal to the server and all clients.
	 * Returns true if the signal was handled, false
	 * otherwise.
	 */
	virtual bool	PostSignal( int ) ;

	/*
	 * Server message system
	 * The message system is used for communicating with
	 * main() any server critical messages, such as a
	 * disconnect on error.
	 */

	/**
	 * Post a server message for the world to see.
	 */
	virtual void PostMessage( MessageType Msg )
		{ Message = Msg ; }

	/**
	 * Return true if a message has been posted.
	 */
	virtual bool MessageReady() const
		{ return (Message != SRV_SUCCESS) ; }

	/**
	 * Retrieve a message.  Returns -1 if no message ready.
	 */
	virtual MessageType GetMessage() const
		{ return Message ; }

	/**
	 * Once a message has been received, call this method to
	 * make sure that the internal state is updated.
	 */
	virtual void ResetMessage()
		{ Message = SRV_SUCCESS ; }

	/* Utility methods */

	/**
	 * Return true if no BURST state exists, false otherwise.
	 */
	virtual bool IsEndOfBurst() const
		{ return !bursting ; }

	/**
	 * Return true if this server is currently bursting, false
	 * otherwise.
	 */
	inline bool isBursting() const
		{ return bursting ; }

	/**
	 * Set the bursting value to the given argument, with default
	 * argument set to true.
	 * This method should NOT be called by anything other than the
	 * server command handlers.
	 */
	inline void setBursting( bool newVal = true )
		{ bursting = newVal ; }

	/**
	 * Return true if the server has a valid connection to
	 * its uplink, false otherwise.
	 */
	virtual bool isConnected() const
		{ return (serverConnection != 0) ; }

	/* Numeric utility methods */

	/**
	 * Return an unsigned int representation of this server's
	 * numeric server numeric.
	 */
	inline const unsigned int& getIntYY() const
		{ return intYY ; }

	/**
	 * Return an unsigned int representation of this server's
	 * maximum number of possible clients.
 	 */
	inline const unsigned int& getIntXXX() const
		{ return intXXX ; }

	/**
	 * Return a character array representation of this server's
	 * server numeric, base64.
	 */
	inline const char* getCharYY() const
		{ return charYY ; }

	/**
	 * Return a character array representation of this server's
	 * maximum number of possible clients, base 64.
	 */
	inline const char* getCharXXX() const
		{ return charXXX ; }

	/**
	 * Return an unsigned int representation of this server's uplink's
	 * server numeric.
	 */
	inline const unsigned int& getUplinkIntYY() const
		{ return Uplink->getIntYY() ; }

	/**
	 * Return a string representation of this server's uplink's
	 * server numeric.
	 */
	inline const string getUplinkCharYY() const
		{ return Uplink->getCharYY() ; }

	/**
	 * Return a std::string representation of this server's full
	 * numeric, base64.
	 */
	inline const string getCharYYXXX() const
		{ return( string( charYY ) + charXXX ) ; }

	/* General server utility methods */

	/**
	 * Return this server's name, as the network sees it.
	 */
	inline const string& getName() const
		{ return ServerName ; }

	/**
	 * Return a description of this server.
	 */
	inline const string& getDescription() const
		{ return ServerDescription ; }

	/**
	 * Return this server's uplink password.
	 */
	inline const string& getPassword() const
		{ return Password ; }

	/**
	 * Return the time at which this server was instantiated.
	 */
	inline const time_t& getStartTime() const
		{ return StartTime ; }

	/**
	 * Return the time this server last connected to its uplink.
	 */
	inline const time_t& getConnectionTime() const
		{ return ConnectionTime ; }

	/**
	 * Return a pointer to this server's uplink.
	 */
	inline iServer*		getUplink() const
		{ return Uplink ; }

	/**
	 * Set this server's uplink.
	 * This method should ONLY be called by the server command
	 * handlers.
	 */
	inline void		setUplink( iServer* newUplink )
		{ Uplink = newUplink ; }

	/**
	 * Enable or disable the burstBuffer.
	 * This method should ONLY be called by the server command
	 * handlers.
	 */
	inline void setUseHoldBuffer( bool newVal )
		{ useHoldBuffer = newVal ; }

	/**
	 * Set the time of the most recent end of burst.
	 * This method should ONLY be called by the server command
	 * handlers.
	 */
	inline void setBurstEnd( const time_t newVal )
		{ burstEnd = newVal ; }

	/**
	 * Set the time of the most recent start of burst.
	 * This method should ONLY be called by the server command
	 * handlers.
	 */
	inline void setBurstStart( const time_t newVal )
		{ burstStart = newVal ; }

	/**
	 * Transfer the burstHoldBuffer data to the outputBuffer.
	 * This method should ONLY be called by the server command
	 * handlers.
	 */
	virtual void WriteBurstBuffer() ;

	/**
	 * Shutdown the server.
	 */
	virtual void Shutdown() ;

	/**
	 * Output server statistics to the console (clog).
	 */
	virtual void dumpStats() ;

	/**
	 * Execute any waiting client timers.
	 * Return the number of timers executed.
	 */
	virtual unsigned int CheckTimers() ;

	/**
	 * The main loop which runs the server.  This contains
	 * all of the server essential logic.
	 */
	void		mainLoop() ;

	/**
	 * This method is called when a channel mode 't' change is
	 * Keep in mind that the source ChannelUser may be NULL
	 * if the mode is being set/unset by a server.
	 * detected.
	 */
	virtual void	OnChannelModeT( Channel*, bool, ChannelUser* ) ;

	/**
	 * This method is called when a channel mode 'n' change is
	 * detected.
	 * Keep in mind that the source ChannelUser may be NULL
	 * if the mode is being set/unset by a server.
	 */
	virtual void	OnChannelModeN( Channel*, bool, ChannelUser* ) ;

	/**
	 * This method is called when a channel mode 's' change is
	 * detected.
	 * Keep in mind that the source ChannelUser may be NULL
	 * if the mode is being set/unset by a server.
	 */
	virtual void	OnChannelModeS( Channel*, bool, ChannelUser* ) ;

	/**
	 * This method is called when a channel mode 'p' change is
	 * detected.
	 * Keep in mind that the source ChannelUser may be NULL
	 * if the mode is being set/unset by a server.
	 */
	virtual void	OnChannelModeP( Channel*, bool, ChannelUser* ) ;

	/**
	 * This method is called when a channel mode 'm' change is
	 * detected.
	 * Keep in mind that the source ChannelUser may be NULL
	 * if the mode is being set/unset by a server.
	 */
	virtual void	OnChannelModeM( Channel*, bool, ChannelUser* ) ;

	/**
	 * This method is called when a channel mode 'i' change is
	 * detected.
	 * Keep in mind that the source ChannelUser may be NULL
	 * if the mode is being set/unset by a server.
	 */
	virtual void	OnChannelModeI( Channel*, bool, ChannelUser* ) ;

	/**
	 * This method is called when a channel mode 'l' change is
	 * detected.
	 * Keep in mind that the source ChannelUser may be NULL
	 * if the mode is being set/unset by a server.
	 */
	virtual void	OnChannelModeL( Channel*, bool,
				ChannelUser*, unsigned int ) ;

	/**
	 * This method is called when a channel mode 'k' change is
	 * detected.
	 * Keep in mind that the source ChannelUser may be NULL
	 * if the mode is being set/unset by a server.
	 */
	virtual void	OnChannelModeK( Channel*, bool,
				ChannelUser*, const string& ) ;

	/**
	 * This method is called when one or more mode 'o' changes
	 * are set/unset on a particular channel.
	 * Keep in mind that the source ChannelUser may be NULL
	 * if the mode is being set/unset by a server.
	 */
	virtual void	OnChannelModeO( Channel*, ChannelUser*,
		const opVectorType& ) ;

	/**
	 * This method is called when one or more mode 'v' changes
	 * are set/unset on a particular channel.
	 * Keep in mind that the source ChannelUser may be NULL
	 * if the mode is being set/unset by a server.
	 */
	virtual void	OnChannelModeV( Channel*, ChannelUser*,
		const voiceVectorType& ) ;

	/**
	 * This method is called when one or more mode 'b' changes
	 * are set/unset on a particular channel.
	 * Keep in mind that the source ChannelUser may be NULL
	 * if the mode is being set/unset by a server.
	 */
	virtual void	OnChannelModeB( Channel*, ChannelUser*,
		banVectorType& ) ;

	/**
	 * Check the list of glines for any that are about to
	 * expire.
	 */
	virtual void updateGlines() ;

	/**
	 * Check if a server is juped
	 */
	virtual bool isJuped( const iServer* ) const ;

	/**
	 * Burst out information about all xClients on this server.
	 */
	virtual void 	BurstClients() ;

	/**
	 * Output channel information for each client on this server.
	 */
	virtual void	BurstChannels() ;

	/**
	 * Deletes a juped server from the juped server list.
	 * This does not alter the server itself.
	 */
	virtual bool	RemoveJupe( const iServer* );

protected:

	/**
	 * Allow only subclasses to call the default
	 * constructor.
	 */
	xServer() {}

	/**
	 * Disable copy constructor, this method is declared but
	 * NOT defined.
	 */
	xServer( const xServer& ) ;

	/**
	 * Disable assignment, this method is declared but NOT
	 * defined.
	 */
	xServer operator=( const xServer& ) ;

	/**
	 * Remove glines which match the given userHost, post event.
	 */
	virtual void	removeMatchingGlines( const string& ) ;

	/**
	 * This method is responsible for updating the systems internal
	 * data structures, and deallocating the given xClient when it
	 * is being removed from the server.
	 */
	virtual void	removeClient( xClient* ) ;

	/**
	 * Remove all modes from a channel, used when bursting an
	 * older timestamp into a channel.
	 */
	virtual void	removeAllChanModes( Channel* ) ;

	/**
	 * This support method is called by Mode() to parse the modes
	 * and arguments.
	 */
	virtual int	parseModeRequest( const Channel*, const string&,
				const string& ) const ;

	/**
	 * Returns true if the given mask uses the nick!user@host syntax.
	 */
	virtual bool	banSyntax( const string& ) const ;

	/**
	 * Read the config file.  Return true if success, false
	 * otherwise.
	 */
	virtual bool	readConfigFile( const string& ) ;

	/**
	 * Parses a config file and attempts to load all modules
	 * specified therein.  If any part of the process fails,
	 * false is returned.  Otherwise, true is returned.
	 */
	virtual bool	loadClients( const string& ) ;

	/**
	 * Signal handler for the server itself.
	 * Returns true if the signal was handled.
	 */
	virtual bool	OnSignal( int ) ;

	/**
	 * This variable is false when no signal has occured, true
	 * otherwise.  This variable is checked each iteration of
	 * the main server loop.
	 */
	static bool	caughtSignal ;

	/**
	 * This variable holds the signal identifier for the most
	 * recently issued software signal, or 0 if no signal
	 * is currently pending.
	 */
	static int	whichSig ;

	/**
	 * The structure type to hold information about client timed
	 * events.
	 */
	struct timerInfo
	{
	timerInfo( const timerID& _ID,
		const time_t& _absTime,
		TimerHandler* _theHandler,
		void* _data = 0 )
	: ID( _ID ),
	  absTime( _absTime ),
	  theHandler( _theHandler ),
	  data( _data )
	{}

		/// The unique identifier of this timer
		timerID		ID ;

		/// The absolute time at which the timer expires
		time_t		absTime ;

		/// The handler for this timed event
		TimerHandler*	theHandler ;

		/// The argument to pass to the handler
		void*		data ;
	} ;

	/**
	 * This structure is used as a comparator functor for
	 * the timerQueue.
	 */
	struct timerGreater
	{
	inline bool operator()( const pair< time_t, timerInfo* >& lhs,
		const pair< time_t, timerInfo* >& rhs ) const
		{
		return lhs.first > rhs.first ;
		}
	} ;

	/**
	 * Register the timers that the server uses.
	 */
	virtual void		registerServerTimers() ;

	/**
	 * Return a unique timerID.
	 */
	virtual timerID		getUniqueTimerID() ;

	/**
	 * Bounds checker for events.
	 */
	inline bool validEvent( const eventType& theEvent ) const
		{ return (theEvent >= 0 && theEvent < EVT_NOOP) ; }

	Connection		*serverConnection ;

	/**
	 * The name of the server, as the network sees it.
	 */
	string			ServerName ;

	/**
	 * This server's description line.
	 */
	string			ServerDescription ;

	/**
	 * The password for our uplink server.
	 */
	string			Password ;

	/**
	 * The hostname/IP of our uplink
	 */
	string			UplinkName ;

	/**
	 * This vector holds the server numerics of any servers
	 * that we are juping.  The actual iServer instances
	 * are maintained by the xNetwork instance, Network.
	 */
	jupedServerListType	jupedServers ;

	/**
	 * The type used to store the system event map.
	 */
	typedef vector< list< xClient* > > eventListType ;

	/**
	 * This is the vector of lists of xClient pointers.
	 * When clients register to receive an event, that xClient's
	 * pointer is added to eventListType[ theEvent ].
	 * When an event of that type occurs, each xClient in the
	 * list is called (OnEvent()).
	 */
	eventListType		eventList ;

	/**
	 * Type used to store the channel event map.
	 */
	typedef map< string, list< xClient* >* > channelEventMapType ;

	/**
	 * The structure used to maintain xClient registrations for
	 * channel events.
	 */
	channelEventMapType	channelEventMap ;

	/**
	 * This structure holds the current network glines.
	 */
	glineListType		glineList ;

	/**
	 * This is the time this xServer was instantiated.
	 */
	time_t 			StartTime ;

	/**
	 * This is the time that we last connected to our uplink.
	 */
	time_t			ConnectionTime ;

	/**
	 * Time at which we began the last burst.
	 */
	time_t			burstStart ;

	/**
	 * Time at which we ended the last burst.
	 */
	time_t			burstEnd ;

	/**
	 * The timer interval for the glineUpdateTimer timer.
	 */
	time_t			glineUpdateInterval ;

	/**
	 * The timer interval for the PINGTimer timer.
	 */
	time_t			pingUpdateInterval ;

	/**
	 * This is the version of the xServer, pretty useless.
	 */
	long			Version ;

	/**
	 * This variable is true when this server is bursting.
	 */
	bool			bursting ;

	/**
	 * This variable will be true when the default behavior
	 * of Write() is to write to the burstHoldBuffer.
	 */
	bool			useHoldBuffer ;

	/**
	 * This variable remains true while the server should continue
	 * running.  It may be set false by user input, or caught
	 * signals.
	 */
	bool			keepRunning ;

	/**
	 * This is the current message error number, or -1 if no
	 * error exists.
	 */
	MessageType		Message ;

	/**
	 * This is the port number to which we connect on our uplink.
	 */
	unsigned short int	Port ;

	/**
	 * This is the unsigned integer representation of our server numeric.
	 */
	unsigned int		intYY ;

	/**
	 * This is the unsigned integer representation of the max number
	 * of clients we can accept.
	 */
	unsigned int		intXXX ;

	/**
 	 * This is the base 64 character array representation of this
	 * server's numeric.
	 */
	char			charYY[ 3 ] ;

	/**
	 * This is the base 64 character array representation of this
	 * server's maximum number of allowable clients.
	 */
	char			charXXX[ 4 ] ;

	/**
	 * This is a pointer into the network table to our uplink
	 * server.  It is kept here for convenience.
	 */
	iServer* 		Uplink ;

	/**
	 * This buffer will hold data to be written during burst time.
	 */
	Buffer			burstHoldBuffer ;

	/**
	 * The name of the file which contains the command handler
	 * mapping from network message to handler.
	 */
	string			commandMapFileName ;

	/**
	 * The path prefix to the command handler, of the form
	 * "/path/to/command/map/file"
	 */
	string			commandHandlerPrefix ;

	/**
	 * Burst() is called when the network connection is
	 * established.  Its purpose is to call each xClient's
	 * Connect() method so that each client may burst itself
	 * and its channels.
	 * NOTE: The scope of this method will be altered in
	 * the future to reduce the requirements of xClient
	 * when bursting its information.
	 */
	void 			Burst() ;

	/**
	 * Type used to store runtime client modules.
	 */
	typedef vector< moduleLoader< xClient* >* >	clientModuleListType;

	/**
	 * Structure used to store runtime client modules.
	 */
	clientModuleListType		clientModuleList;

	/**
	 * The type of the modules used to load ServerCommandHandlers
	 * from dynamically loadable libraries.
	 * This is stored here in order to properly close them when
	 * needed, including reloading command handlers.
	 */
	typedef moduleLoader< ServerCommandHandler*, xServer* >
			commandModuleType ;

	/**
	 * A vector of modules representing ServerCommandHandlers.
	 */
	typedef vector< commandModuleType* > commandModuleListType ;

	/**
	 * The structure of moduleLoader's, each representing a
	 * ServerCommandHandler.
	 */
	commandModuleListType		commandModuleList ;

	/**
	 * The type used to store ServerCommandHandlers, each
	 * associated with a particular server message (key).
	 */
	typedef map< string, ServerCommandHandler*, noCaseCompare >
			commandMapType ;

	/**
	 * The structure used to store ServerCommandHandlers, each
	 * associated with a particular server message (key).
	 */
	commandMapType			commandMap ;

	/**
	 * Attempt to locate a commandModuleType by its key.  This is
	 * used to reload a module, and to ensure that a module is
	 * not accidentally loaded more than once.
	 */
	commandModuleType*	lookupCommandModule( const string& ) const ;

	/**
	 * The type used to store timed events.
	 */
	typedef priority_queue< pair< time_t, timerInfo* >,
		vector< pair< time_t, timerInfo* > >,
		timerGreater >
		timerQueueType ;

	/**
	 * The structure used to store timed events.
	 */
	timerQueueType  timerQueue ;

	/**
	 * The type used to store timer ID's currently in use.
	 */
	typedef map< timerID, bool > uniqueTimerMapType ;

	/**
	 * The structure used to store timer ID's current in use.
	 */
	uniqueTimerMapType		uniqueTimerMap ;

	/**
	 * The last unique timerID to be used.
	 */
	timerID		lastTimerID ;

#ifdef LOG_SOCKET
	/**
	 * The output file to which to write raw data read from
	 * the network.
	 */
	ofstream	socketFile ;
#endif

#ifdef EDEBUG
	/**
	 * The name of the file for which elog to write all
	 * debugging information.
	 */
	string		elogFileName ;
#endif

	/**
	 * The name of the server config file.
	 */
	string		configFileName ;

	/**
	 * The char array to be used to read in network data.
	 * This is allocated only once in the server for
	 * performance reasons.
	 * It is of fixed size since this buffer isn't used for
	 * actual reading from the network connection, only for
	 * handling a single network message a time (max 512 bytes).
	 */
	char		inputCharBuffer[ 1024 ] ;

	/**
	 * True if all elog data should be output to clog.
	 */
	bool		verbose ;

	/**
	 * The signal handler method for the system.
	 */
	static void	sigHandler( int ) ;

	/**
	 * This method initializes the entire server.
	 */
	void		initializeSystem() ;

	/**
	 * This method initializes all server variables.
	 */
	void		initializeVariables() ;

	/**
	 * This method loads all command handlers.
	 */
	bool		loadCommandHandlers() ;

	/**
	 * Load an individual command handler from a file (fileName),
	 * and associate that handler with the network message
	 * (commandKey).
	 */
	bool		loadCommandHandler( const string& fileName,
				const string& commandKey ) ;

	/**
	 * This method maps all relevant signals to sigHandler().
	 */
	bool		setupSignals() ;
} ;

} // namespace gnuworld

#endif // __SERVER_H
