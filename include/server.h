/* server.h
 * This is the header file for the server itself.
 * This class originally created by Orlando Bassotto.
 */

/* Command Map Description
 * -----------------------
 * Command messages read from the network are in the
 * form of characters and character strings.  Messages
 * are handled by passing the message and its arguments
 * to handler functions.
 * Command handler function names are all preceeded
 * with: MSG_
 * Pointers to offsets of these functions within the
 * global variable xServer* Server are stored in a
 * VectorTrie (see web page).
 */

#ifndef __SERVER_H
#define __SERVER_H "$Id: server.h,v 1.61 2002/05/23 19:41:32 dan_karrels Exp $"

#include	<string>
#include	<vector>
#include	<list>
#include	<sstream>
#include	<map>
#include	<queue>
#include	<algorithm>

#include	<ctime>

#include	"Numeric.h"
#include	"iServer.h"
#include	"iClient.h"
#include	"Socket.h"
#include	"ClientSocket.h"
#include	"Buffer.h"
#include	"events.h"
#include	"Gline.h"
#include	"xparameters.h"
#include	"misc.h"
#include	"moduleLoader.h"
#include	"ELog.h"
#include	"TimerHandler.h"
#include	"defs.h"

#ifdef GNU_EXTENSIONS
 #include       <ext/hash_map>
 using __gnu_cxx::hash_map ;
 using __gnu_cxx::hash ;
#else
 #include       <hash_map>
 using std::hash_map ;
 using std::hash ;
#endif

namespace gnuworld
{

using std::string ;
using std::list ;
using std::vector ;
using std::stringstream ;
using std::priority_queue ;
using std::map ;

/**
 * This macro constructs a method prototype for a command
 * handler with the given name MSG_handlerFunc.
 */
#define DECLARE_MSG( handlerFunc ) \
 virtual int MSG_##handlerFunc( xParameters& ) ;

/**
 * This method registers a command handler with the xServer's
 * command table.  It must first be prototyped in the xServer
 * class declaration.
 */
#define REGISTER_MSG( key, handlerFunc ) \
  if( !commandMap->insert( commandMapType::value_type( key, \
	&xServer::MSG_##handlerFunc ) ).second ) \
	{\
	elog << "Unable to register function: "\
		<< key << std::endl ;\
	exit( 0 ) ; \
	}

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
class xServer
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
	 * Connect to a network uplink of the given address
	 * (IP or hostname) and on the given port.
	 */
	virtual int Connect( const string& Address, int Port ) ;

	/**
	 * Connect to the default uplink and port.
	 */
	virtual int Connect()
		{ return Connect( UplinkName, Port ) ; }

	/**
	 * Call this method when a read/write error occurs on the
	 * uplink socket.  This will close and deallocate the socket,
	 * as well as clear the input/output buffers and post a
	 * message for GetMessage() later.
	 */
	virtual void OnDisConnect() ;

	/**
	 * Attach a fake server to this services server.
	 */
	virtual bool AttachServer( iServer*, const string& = "JUPED Server" ) ;

	/**
	 * Squit a server from the network and remove it
	 * from the network tables.
	 */
	virtual bool SquitServer( const string& name, const string& reason ) ;

	/* I/O stuff */

	/**
	 * Perform the physical read of the socket.
	 */
	virtual void DoRead() ;

	/**
	 * Perform the physical write to the socket.
	 */
	virtual void DoWrite() ;

	/**
	 * Append a std::string to the output buffer.
	 * The second argument determines if data should be written
	 * during burst time.
	 */
	virtual size_t Write( const string& ) ;

	/**
	 * Similar to the above signature of Write() except that data
	 * will be written to the normal output buffer even during
	 * burst time.
	 */
	virtual size_t WriteDuringBurst( const string& ) ;

	/**
	 * Append a C variable argument list/character array to the output
	 * buffer.
	 * Since this method uses a variable argument list, this
	 * method cannot support a final default argument -- this method
	 * defaults to NOT writing during burst.
	 */
	virtual size_t Write( const char*, ... ) ;

	/**
	 * This method is similar to the above Write(), except
	 * that the data will be written to the normal output
	 * buffer even during burst time.
	 */
	virtual size_t WriteDuringBurst( const char*, ... ) ;

	/**
	 * Append a std::stringstream to the output buffer.
	 * The second argument determines if data should be written
	 * during burst time.
	 */
	virtual size_t Write( const stringstream& ) ;

	/**
	 * This method is similar to the above Write(), except
	 * that the data will be written to the normal output
	 * buffer even during burst time.
	 */
	virtual size_t WriteDuringBurst( const stringstream& ) ;

	/**
	 * Write any bufferred data to the network.
	 * Returns false on write error.
	 */
	virtual inline bool flushBuffer() ;

	/**
	 * Read a '\n' delimited line from the input buffer.
	 * Return true is none exist.  (size) is the length of
	 * the C string buffer (buf).
	 */
	virtual inline bool GetString( char* buf ) ;

	/**
	 * Return true if a read attempt from the network
	 * would NOT block.
	 * This method is not const beause it may modify
	 * _connected and Socket if an error occurs.
	 */
	virtual inline bool ReadyForRead() ;

	/**
	 * Return true if data exists to be written to the
	 * network, and a write would NOT block.
	 */
	virtual inline bool ReadyForWrite() const ;

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
	virtual MessageType GetMessage()
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
	 * Return true if the server has a valid connection to
	 * its uplink, false otherwise.
	 */
	virtual bool isConnected() const
		{ return _connected ; }

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
	 * Returns the total number of bytes recieved from the uplink
	 */
	inline const unsigned long getTotalReceived() const
		{ return theSock->getTotalReceived(); }

	/**
	 * Returns the total number of bytes sent to the uplink
	 */
	inline const unsigned long getTotalSent() const
		{ return theSock->getTotalSent(); }
	
	/**
	 * This is a simple mutator of the server's socket pointer.
	 * This is used ONLY for implementing the simulation mode.
	 */
	inline void setSocket( ClientSocket* newSock )
		{ theSock = newSock ; }

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
	 * Deletes a juped server from the juped server list.
	 * This does not alter the server itself.
	 */
	virtual bool	RemoveJupe( const iServer* );

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
	 * Return an iterator to the beginning of the gline structure.
	 */
	inline glineIterator	gline_begin()
		{ return glineList.begin() ; }

	/**
	 * Return an iterator to the end of the gline structure.
	 */
	inline glineIterator	gline_end()
		{ return glineList.end() ; }

	/**
	 * Burst out information about all xClients on this server.
	 */
	virtual void 	BurstClients() ;

	/**
	 * Output channel information for each client on this server.
	 */
	virtual void	BurstChannels() ;

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
	 * Parse a burst line for channel bans.
	 */
	virtual void	parseBurstBans( Channel*, const char* ) ;

	/**
	 * Parse a burst line for channel users.
	 */
	virtual void	parseBurstUsers( Channel*, const char* ) ;

	/**
	 * Convenience method that will part a given network
	 * client from all channels, and notify each listening
	 * xClient of the parts.
	 */
	virtual void	userPartAllChannels( iClient* ) ;

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
	virtual bool	loadModules( const string& ) ;

	/**
	 * Signal handler for the server itself.
	 * Returns true if the signal was handled.
	 */
	virtual bool	OnSignal( int ) ;

	/**
	 * This method is called when a user mode change is detected.
	 */
	virtual void	onUserModeChange( xParameters& ) ;


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

	/* Network message handlers */

	/// AD(MIN)
	DECLARE_MSG(AD);

	/// B(URST) message handler.
	DECLARE_MSG(B);

	/// C(REATE) message handler.
	DECLARE_MSG(C);

	/// CM(CLEARMODE) message handler.
	DECLARE_MSG(CM);

	/// D(KILL) message handler.
	DECLARE_MSG(D);

	/// DE(SYNCH) ?
	DECLARE_MSG(DS);

	/// EA (End of burst Acknowledge) message handler.
	DECLARE_MSG(EA);

	/// EB (End of BURST) message handler.
	DECLARE_MSG(EB);

	/// ERROR message handler, deprecated.
	DECLARE_MSG(Error);

	/// G(PING) message handler.
	DECLARE_MSG(G);

	/// GL(INE) message handler
	DECLARE_MSG(GL);

	/// I(NVITE)
	DECLARE_MSG(I);

	/// J(OIN) message handler.
	DECLARE_MSG(J);

	/// K(ICK) message handler.
	DECLARE_MSG(K);

	// JU(PE) message handler.
	DECLARE_MSG(JU);

	/// L(EAVE) message handler.
	DECLARE_MSG(L);

	/// P(RIVMSG) message handler.
	DECLARE_MSG(P);

	/// PART message handler, non-tokenized, bogus
	DECLARE_MSG(PART);

	// PRIVMSG message handler, bogus.
	DECLARE_MSG(PRIVMSG);

	/// M(ODE) message handler.
	DECLARE_MSG(M);

	/// N(ICK) message handler.
	DECLARE_MSG(N);

	/// Q(UIT) message handler.
	DECLARE_MSG(Q);

	/// PASS message handler.
	DECLARE_MSG(PASS);

	// STATS message handler.
	DECLARE_MSG(R);

	/// RPING message handler, deprecated.
	DECLARE_MSG(RemPing);

	/// S(ERVER) message handler.
	DECLARE_MSG(S);

	/// SERVER message handler, deprecated.
	DECLARE_MSG(Server);

	/// SQ(UIT) message handler.
	DECLARE_MSG(SQ);

	/// T(OPIC) message handler.
	DECLARE_MSG(T);

	/// U(SILENCE)
//	DECLARE_MSG(U);

	/// WA(LLOPS) message handler.
	DECLARE_MSG(WA);

	/// W(HOIS) message handler.
	DECLARE_MSG(W);

	/// Account message handler.
	DECLARE_MSG(AC);

	/// NOOP message.
	/// Use this handler for any messages that we don't need to handle.
	/// Included for completeness.
	DECLARE_MSG(NOOP);

	/// 351 message
	/// when our client recieve back a version reply from a server
	DECLARE_MSG(M351);

	// Non-tokenized command handlers
	// Replication of code *sigh*

	/**
	 * Bounds checker for events.
	 */
	inline bool validEvent( const eventType& theEvent ) const
		{ return (theEvent >= 0 && theEvent < EVT_NOOP) ; }

	/**
	 * This is the command map type.  Pointers to
	 * the bound offset of the command handler methods
	 * are stored in this structure.
	 */
	typedef hash_map< string, int (xServer::*)( xParameters& ),
		eHash, eqstr > commandMapType ;

	/**
	 * A pointer to the server command handler.
	 */
	commandMapType		*commandMap ;

	/**
	 * This points to the input/output stream to be used for
	 * server->server communication.  This may point to an
	 * instance of FileSocket if the server is running in
	 * simulation mode.
	 */
	ClientSocket		*theSock ;

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
	typedef hash_map< string, list< xClient* >*, eHash, eqstr >
		channelEventMapType ;

	/**
	 * This structure provides a nice iterator interface, and
	 * runs in O(logn) time.  This should probably be moved
	 * to some form of a hashtable, though it will have to
	 * be specially built to meet the server's needs.
	 * Any volunteers? :)
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
	 * of Write() is to write to the burstOutputBuffer.
	 */
	bool			useBurstBuffer ;

	/**
	 * This variable is true when the socket connection is valid.
	 */
	bool			_connected ;

	/**
	 * This variable remains true while the server should continue
	 * running.  It may be set false by user input, or caught
	 * signals.
	 */
	volatile bool		keepRunning ;

	/**
	 * This is the current message error number, or -1 if no
	 * error exists.
	 */
	MessageType		Message ;

	/**
	 * This is the port number to which we connect on our uplink.
	 */
	int			Port ;

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
	 * This is the buffer into which network commands are read
	 * and from which they are later processed.
	 */
	Buffer			inputBuffer ;

	/**
	 * This is the output buffer from which data is written to
	 * the network.
	 */
	Buffer			outputBuffer ;

	/**
	 * This buffer will hold data to be written during burst time.
	 */
	Buffer			burstOutputBuffer ;

	/**
	 * This is the size of the TCP input window.
	 */
	size_t			inputReadSize ;

	/**
	 * This is the size of the TCP output window.
	 */
	size_t			outputWriteSize ;

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
	typedef vector< moduleLoader< xClient* >* >	moduleListType;

	/**
	 * Structure used to store runtime client modules.
	 */
	moduleListType		moduleList;

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
	 */
	char*		inputCharBuffer ;

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
	void		loadCommandHandlers() ;

	/**
	 * This method maps all relevant signals to sigHandler().
	 */
	bool		setupSignals() ;
} ;

} // namespace gnuworld

#endif // __SERVER_H
