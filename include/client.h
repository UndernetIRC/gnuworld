/* client.h
 * This file originally created by Orlando Bassotto.
 */

#ifndef __XCLIENT_H
#define __XCLIENT_H "$Id: client.h,v 1.5 2000/08/02 23:15:43 dan_karrels Exp $"

#include	<string>

#include	"server.h"
#include	"Numeric.h"
#include	"iClient.h"
#include	"MessageNode.h"
#include	"events.h"

using std::string ;

namespace gnuworld
{
 
/**
 * This is the public concrete base class that represents
 * a client that may connect to this server.  To build a new
 * services client, simple subclass this class, inheriting
 * much functionality and state, and overloading a few
 * appropriate functions.
 * This has proven to be extremely easy: I built a functioning
 * services client in 11 minutes, though it didn't do much :)
 */
class xClient
{

	/// Let xServer access our protected members.
	friend class xServer ;

	/// Let xNetwork access our protected members.
	friend class xNetwork ;

public:

	/**
	 * Declare the type for the xClient's internal
	 * mode representation.  Note that this typedef
	 * is deliberately public.
	 */
	typedef iClient::modeType modeType ;

	/**
	 * Construct a new xClient from a config file.
	 */
	xClient( const string& ) ;

	virtual ~xClient() ;

	/**
	 * Connect is called by an xServer object when the
	 * server has connected to the network.  The purpose
	 * of this method is to burst the client information
	 * to the network.
	 */
	virtual int Connect( int ForceTime = 0 ) ;

	/**
	 * This method must call xServer::BurstChannel() with appropriate
	 * channel modes for any channel it wishes to own.
	 */
	virtual int BurstChannels() ;

	/**
	 * Exit is called by xServer::DetachClient().
	 * Its purpose is to write any finishing data to the
	 * network and clean up.  Note that the client itself
	 * can call this method, and then reconnect later.
	 */
	virtual int Exit( const string& Message ) ;

	/**
	 * Kill will issue a KILL command to the network for
	 * the given iClient (network generic client).
	 */
	virtual int Kill( iClient*, const string& ) ;

	/**
	 * QuoteAsServer will send data to the network as the
	 * server itself.  Try to avoid using this method.
	 */
	virtual int QuoteAsServer( const string& Command );

	/**
	 * Write a string of data to the network.
	 */
	virtual int Write( const string& s )
		{ return QuoteAsServer( s ) ; }

	/**
	 * Write a string of data to the network.
	 */
	virtual int Write( strstream& s )
		{ return QuoteAsServer( s.str() ) ; }

	/**
	 * Write a variable length argument list to the network.
	 */
	virtual int Write( const char*, ... ) ;

	/**
	 * ModeAsServer will change modes in a channel as the server.
	 */
	virtual int ModeAsServer( const string& Channel, const string& Mode ) ;	

	/**
	 * Change modes as the server for the given channel.
	 */
	virtual int ModeAsServer( const Channel*, const string& ) ;

	/**
	 * Mode is used to set the bot's modes.  If connected to the
	 * network already, these new modes will be written to the
	 * network.
	 */
	virtual int Mode( const string& Mode ) ;

	// Handler methods

	/**
	 * OnConnect is called when the server connects to the
	 * network, during burst time.  The client's NICK
	 * information has already been sent to the network.
	 */
	virtual int OnConnect() ;

	/**
	 * OnQuit is called by Exit().
	 */
	virtual int OnQuit() ;

	/**
	 * OnKill() is called when the client has been KILL'd.
	 */
	virtual int OnKill() ;

	/**
	 * OnEvent is called when a network event occurs.
	 * To receive a particular event, the client must
	 * first register for that event with the xServer.
	 */
	virtual int OnEvent( const eventType& theEvent,
		void* Data1 = NULL, void* Data2 = NULL,
		void* Data3 = NULL, void* Data4 = NULL ) ;

	/**
	 * OnChannelEvent is called when a requested channel
	 * event occurs.
	 */
	virtual int OnChannelEvent( const channelEventType&,
		const string& chanName,
		void* Data1 = NULL, void* Data2 = NULL,
		void* Data3 = NULL, void* Data4 = NULL ) ;

	/**
	 * OnCTCP is called when a CTCP command is issued to
	 * the client.
	 */
	virtual int OnCTCP( iClient* Sender,
		const string& CTCP,
		const string& Message,
		bool Secure = false ) ;

	/**
	 * OnPrivateMessage is called when a PRIVMSG command
	 * is issued to the client.
	 */
	virtual int OnPrivateMessage( iClient* Sender,
		const string& Message ) ;

	/* Utility methods */

	/**
	 * Op a user on a channel, join/part the channel if necessary.
	 */
	virtual bool	Op( Channel*, iClient* ) ;

	/**
	 * Op a user on a channel, join/part the channel if necessary.
	 */
	virtual bool	Op( Channel*, const vector< iClient* >& ) ;

	/**
	 * Deop a user on a channel, join/part the channel if necessary.
	 */
	virtual bool	DeOp( Channel*, iClient* ) ;

	/**
	 * Deop a user on a channel, join/part the channel if necessary.
	 */
	virtual bool	DeOp( Channel*, const vector< iClient* >& ) ;

	/**
	 * Set a ban on a channel, join/part the channel if necessary.
	 */
	virtual bool	Ban( Channel*, iClient* ) ;

	/**
	 * Set a ban on a channel, join/part the channel if necessary.
	 */
	virtual bool	Ban( Channel*, const vector< iClient* >& ) ;

	/**
	 * Ban kick a client from a channel for the given reason.
	 */
	virtual bool	BanKick( Channel*, iClient*, const string& ) ;

	/**
	 * This method creates a ban mask for the given user on the
	 * given channel.
	 * Should this be a const method?
	 */
	virtual string	makeBan( Channel*, iClient* ) ;

	/**
	 * Kick a user from a channel, join/part if necessary.
	 */
	virtual bool	Kick( Channel*, iClient*, const string& ) ;

	/**
	 * Kick several users from a channel, join/part if necessary.
	 */
	virtual bool	Kick( Channel*, const vector< iClient* >&,
				const string& ) ;

	/**
	 * Join will cause the client to join a channel.
	 */
	virtual bool	Join( const string& ) ;

	/**
	 * Join the given channel.
	 */
	virtual bool	Join( Channel* ) ;

	/**
	 * Part will cause the client to part a channel.
	 */
	virtual bool	Part( const string& ) ;

	/**
	 * Part the given channel.
	 */
	virtual bool	Part( Channel* ) ;

	/**
	 * Invite a user to a channel.  Join the channel if necessary
	 * (and then part).
	 */
	virtual bool	Invite( iClient*, const string& ) ;

	/**
	 * Invite a user to a channel.  Join the channel if necessary
	 * (and then part).
	 */
	virtual bool	Invite( iClient*, Channel* ) ;

	/**
	 * Return true if the bot is on the given channel.
	 */
	virtual bool	isOnChannel( const string& chanName ) const ;

	/**
	 * Return true if the bot is on the given channel.
	 */
	virtual bool	isOnChannel( const Channel* theChan ) const ;

	/**
	 * DoCTCP will issue a CTCP (reply) to the given iClient.
	 */
	virtual int DoCTCP( iClient* Target,
		const string& CTCP,
		const string& Message ) ;

	/**
	 * Message will PRIVMSG a string of data to the given iClient.
	 */
	virtual int Message( const iClient* Target,
		const char* Message, ... ) ;

	/**
	 * Message will PRIVMSG a string of data to the given iClient.
	 */
	virtual int Message( const iClient* Target, const string& Message ) ;

	/**
	 * This format of Message will write a string of data
	 * to a channel.
	 */
	virtual int Message( const string& Channel, const char* Message, ... ) ;

	/**
	 * Notice will send a NOTICE command to the given iClient.
	 */
	virtual int Notice( const iClient* Target,
		const char* Message, ... ) ;

	/**
	 * Notice will send a NOTICE command to the given iClient.
	 */
	virtual int Notice( const iClient* Target, const string& ) ;

	/**
	 * This Notice() signature will send a channel NOTICE.
	 */
	virtual int Notice( const string& Channel, const char* Message, ... ) ;

	/**
	 * Have this bot send a global wallops message.
	 */
	virtual int Wallops( const string& ) ;

	/**
	 * Have this bot send a global wallops message.
	 */
	virtual int Wallops( const char* Format, ... ) ;

	/**
	 * Return true if this xClient is attached to a server.
	 */
	inline bool isConnected() const
		{ return Connected ; }

	/**
	 * Return this bot's nick name.
	 */
	inline const string& getNickName() const
		{ return nickName ; }

	/**
	 * Retrieve this bot's user name.
	 */
	inline const string& getUserName() const
		{ return userName ; }

	/**
	 * Retrieve this bot's host name.
	 */
	inline const string& getHostName() const
		{ return hostName ; }

	/**
	 * Retrieve this bot's description.
	 */
	inline const string& getDescription() const
		{ return userDescription ; }

	/**
	 * Retrieve the unsigned integer representation of this
	 * bot's server.
	 */
	inline const unsigned int& getIntYY() const
		{ return intYY ; }

	/**
	 * Retrieve the unsigned integer representation of this
	 * bot's client numeric.
	 */
	inline const unsigned int& getIntXXX() const
		{ return intXXX ; }

	/**
	 * Retrieve a pointer to this bot's character representation
	 * of its server's numeric.
	 */
	inline const char* getCharYY() const
		{ return charYY ; }

	/**
	 * Retrieve a pointer to this bot's character representation
	 * of its client numeric.
	 */
	inline const char* getCharXXX() const
		{ return charXXX ; }

	/**
	 * Retrieve this bot's std::string network numeric.
	 */
	inline const string getCharYYXXX() const
		{ return( string( charYY ) + charXXX ) ; }

	/**
	 * Retrieve this bot's uplink's numeric, integer format.
	 */
	inline const unsigned int& getUplinkIntYY() const
		{ return MyUplink->getIntYY() ; }

	/**
	 * Retrieve this bot's uplink's highest client count
	 * numeric, integer format.
	 */
	inline const unsigned int& getUplinkIntXXX() const
		{ return MyUplink->getIntXXX() ; }

	/**
	 * Retrieve this bot's uplink's numeric, character
	 * array format.
	 */
	inline const char* getUplinkCharYY() const
		{ return MyUplink->getCharYY() ; }

	/**
	 * Retrieve this bot's uplink's client count numeric,
	 * character array format.
	 */
	inline const char* getUplinkCharXXX() const
		{ return MyUplink->getCharXXX() ; }

	/**
	 * Retrieve this bot's uplink's network numeric,
	 * std::string format.
	 */
	inline const string getUplinkCharYYXXX() const
		{ return MyUplink->getCharYYXXX() ; }

	/**
	 * Retrieve this bot's uplink's server name.
	 */
	inline const string& getUplinkName() const
		{ return MyUplink->getName() ; }

	/**
	 * Retrieve this bot's uplink's description.
	 */
	inline const string& getUplinkDescription() const
		{ return MyUplink->getDescription() ; }

	/**
	 * Accessor method for the bot's user modes.
	 */
	virtual string getModes() const ;

	/**
	 * Obtain a pointer to the single xServer instance, which
	 * is the uplink of every services bot.
	 * Use of this method is discouraged.
	 */
	inline xServer* getUplink() const
		{ return MyUplink ; }

protected:

	/**
	 * Allow sub classes to call default constructor
	 * This method is defined in the source file.
	 */
	xClient() ;

	/**
	 * Disallow copying.
	 * This method is declared, but NOT defined.
	 */
	xClient( const xClient& ) ;

	/**
	 * Disallow default assignment.
	 * This method is declared, but NOT defined.
	 */
	xClient operator=( const xClient& ) ;
	
	/**
	 * setIntXXX is called by the network table
	 * structure when the client is added to the table.
	 * This is because the (Network) structure is
	 * responsible for allocating and deallocating
	 * numerics.
	 */
	inline void setIntXXX( const unsigned int& newXX )
		{ intXXX = newXX ; }

	/**
	 * ImplementServer is called by the xServer once
	 * the client has been attached to the server and
	 * has been assigned a YY numeric.  This method
	 * must then acquire the rest of its numeric information
	 * from the xServer.
	 */
	virtual void ImplementServer( xServer* Server ) ;

	/**
	 * MyUplink is a pointer to the xServer to which this
	 * client is attached.
	 */
	xServer		*MyUplink ;

	/**
	 * This bot's nick name.
	 */
	string		nickName ;

	/**
	 * This bot's user name.
	 */
	string		userName ;

	/**
	 * This bot's host name.
	 */
	string		hostName ;

	/**
	 * This bot's description.
	 */
	string		userDescription ;

	/**
	 * Connected is true when we are connected to an xServer.
	 * It says nothing of whether the xServer is connected
	 * to a network.
	 */
	bool		Connected ;

	/**
	 * This is the user mode of this client.
	 */
	modeType	mode ;

	/**
	 * Integer representation of this bot's uplink's numeric.
	 */
	unsigned int	intYY ;

	/**
	 * Integer representation of this bot's client numeric.
	 */
	unsigned int	intXXX ;

	/**
	 * Character array representation of this client's uplink's
	 * numeric.
	 */
	char		charYY[ 3 ] ;

	/**
	 * Character array representation of this client's numeric.
	 */
	char		charXXX[ 4 ] ;

} ;

} // namespace gnuworld

#endif // __CLIENT__
