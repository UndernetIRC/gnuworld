/**
 * client.h
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
 *                    Orlando Bassotto
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
 * $Id: client.h,v 1.44 2003/08/03 18:57:44 dan_karrels Exp $
 */

#ifndef __CLIENT_H
#define __CLIENT_H "$Id: client.h,v 1.44 2003/08/03 18:57:44 dan_karrels Exp $"

#include	<sstream>
#include	<string>

#include	"server.h"
#include	"Numeric.h"
#include	"iClient.h"
#include	"events.h"
#include	"TimerHandler.h"

namespace gnuworld
{

using std::stringstream ; 
using std::string ;

/**
 * This is the public concrete base class that represents
 * a client that may connect to this server.  To build a new
 * services client, simple subclass this class, inheriting
 * much functionality and state, and overloading a few
 * appropriate functions.
 * This has proven to be extremely easy: I built a functioning
 * services client in 11 minutes, though it didn't do much :)
 */
class xClient : public TimerHandler
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

	/**
	 * The primary purpose of this method is do call the
	 * deallocation methods of any xClient's which are storing
	 * data in the customDataMap.
	 * Client responsibilities:
	 *  Removing all timers registered with the xServer
	 *  Removing all custom data members held in the iClient's
	 */
	virtual ~xClient() ;

	/**
	 * Connect is called by an xServer object when the
	 * server has connected to the network.  The purpose
	 * of this method is to burst the client information
	 * to the network.
	 */
	virtual bool Connect( int ForceTime = 0 ) ;

	/**
	 * This method must call xServer::BurstChannel() with appropriate
	 * channel modes for any channel it wishes to own.
	 */
	virtual bool BurstChannels() ;

	/**
	 * BurstGlines is called before eob, for the client to burst
	 * all of its glines
	 */
	virtual bool BurstGlines() ;

	/**
	 * Exit is called by xServer::DetachClient().
	 * Its purpose is to write any finishing data to the
	 * network and clean up.  Note that the client itself
	 * can call this method, and then reconnect later.
	 */
	virtual bool Exit( const string& Message ) ;

	/**
	 * Kill will issue a KILL command to the network for
	 * the given iClient (network generic client).
	 */
	virtual bool Kill( iClient*, const string& ) ;

	/**
	 * QuoteAsServer will send data to the network as the
	 * server itself.  Try to avoid using this method.
	 */
	virtual bool QuoteAsServer( const string& Command );

	/**
	 * Write a string of data to the network.
	 */
	virtual bool Write( const string& s )
		{ return QuoteAsServer( s ) ; }

	/**
	 * Write a string of data to the network.
	 */
	virtual bool Write( const stringstream& s )
		{ return QuoteAsServer( s.str() ) ; }

	/**
	 * Write a variable length argument list to the network.
	 */
	virtual bool Write( const char*, ... ) ;

	/**
	 * ModeAsServer will change modes in a channel as the server.
	 */
	virtual bool ModeAsServer( const string& Channel,
			const string& Mode ) ;	

	/**
	 * Change modes as the server for the given channel.
	 */
	virtual bool ModeAsServer( const Channel*, const string& ) ;

	/**
	 * Mode is used to set the bot's modes.  If connected to the
	 * network already, these new modes will be written to the
	 * network.
	 */
	virtual bool Mode( const string& Mode ) ;

	// Handler methods

	/**
	 * OnConnect is called when the server connects to the
	 * network, during burst time.  The client's NICK
	 * information has already been sent to the network.
	 */
	virtual void OnConnect() ;

	/**
	 * OnQuit is called by Exit().
	 * This method will be invoked when the server is unloading
	 * the client for whatever reason.
	 */
	virtual void OnQuit() ;

	/**
	 * OnKill() is called when the client has been KILL'd.
	 */
	virtual void OnKill() ;

	/**
	 * This method is called when a network client performs
	 * a whois on this xClient.
	 */
	virtual void OnWhois( iClient* sourceClient,
			iClient* targetClient ) ;

	/**
	 * This method is called when a network client invites
	 * a services client to a channel.
	 */
	virtual void OnInvite( iClient* sourceClient,
			Channel* theChan );

	/**
	 * OnEvent is called when a network event occurs.
	 * To receive a particular event, the client must
	 * first register for that event with the xServer.
	 */
	virtual void OnEvent( const eventType& theEvent,
		void* Data1 = NULL, void* Data2 = NULL,
		void* Data3 = NULL, void* Data4 = NULL ) ;

	/**
	 * OnChannelEvent is called when a requested channel
	 * event occurs.
	 */
	virtual void OnChannelEvent( const channelEventType&,
		Channel*,
		void* Data1 = NULL, void* Data2 = NULL,
		void* Data3 = NULL, void* Data4 = NULL ) ;

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
	virtual void OnNetworkKick( Channel* theChan,
			iClient* srcClient, // may be NULL
			iClient* destClient,
			const string& kickMessage,
			bool authoritative ) ;

	/**
	 * This method is invoked when a user sets or removes
	 * channel mode m (moderate).  Keep in mind that the
	 * source ChannelUser may be NULL if a server is
	 * setting the mode.
	 */
	virtual void OnChannelModeM( Channel*, bool polarity,
			ChannelUser* ) ;

	/**
	 * This method is invoked when a user sets or removes
	 * channel mode i (invite).  Keep in mind that the
	 * source ChannelUser may be NULL if a server is
	 * setting the mode.
	 */
	virtual void OnChannelModeI( Channel*, bool polarity,
			ChannelUser* ) ;

	/**
	 * This method is invoked when a user sets or removes
	 * channel mode r (authenticate).  Keep in mind that the
	 * source ChannelUser may be NULL if a server is
	 * setting the mode.
	 */
	virtual void OnChannelModeR( Channel*, bool polarity,
			ChannelUser* ) ;

	/**
	 * This method is invoked when a user sets or removes
	 * channel mode p (private).  Keep in mind that the
	 * source ChannelUser may be NULL if a server is
	 * setting the mode.
	 */
	virtual void OnChannelModeP( Channel*, bool polarity,
			ChannelUser* ) ;

	/**
	 * This method is invoked when a user sets or removes
	 * channel mode s (secret).  Keep in mind that the
	 * source ChannelUser may be NULL if a server is
	 * setting the mode.
	 */
	virtual void OnChannelModeS( Channel*, bool polarity,
			ChannelUser* ) ;

	/**
	 * This method is invoked when a user sets or removes
	 * channel mode n (nomessage).  Keep in mind that the
	 * source ChannelUser may be NULL if a server is
	 * setting the mode.
	 */
	virtual void OnChannelModeN( Channel*, bool polarity,
			ChannelUser* ) ;

	/**
	 * This method is invoked when a user sets or removes
	 * channel mode t (topic).  Keep in mind that the
	 * source ChannelUser may be NULL if a server is
	 * setting the mode.
	 */
	virtual void OnChannelModeT( Channel*, bool polarity,
			ChannelUser* ) ;

	/**
	 * This method is invoked when a user sets or removes
	 * channel mode l (limit).  Keep in mind that the
	 * source ChannelUser may be NULL if a server is
	 * setting the mode.
	 * If the mode is being removed, the limit argument
	 * will be 0.
	 */
	virtual void OnChannelModeL( Channel*, bool polarity,
			ChannelUser*, const unsigned int& ) ;

	/**
	 * This method is invoked when a user sets or removes
	 * channel mode k (key).  Keep in mind that the
	 * source ChannelUser may be NULL if a server is
	 * setting the mode.
	 * If the mode is being removed, the key argument will
	 * be empty.
	 */
	virtual void OnChannelModeK( Channel*, bool polarity,
			ChannelUser*, const string& ) ;

	/**
	 * This method is invoked when a user sets or removes
	 * one or more channel mode (o).  Keep in mind that the
	 * source ChannelUser may be NULL if a server is
	 * setting the mode.
	 */
	virtual void OnChannelModeO( Channel*, ChannelUser*,
			const xServer::opVectorType& ) ;

	/**
	 * This method is invoked when a user sets or removes
	 * one or more channel mode (v).  Keep in mind that the
	 * source ChannelUser may be NULL if a server is
	 * setting the mode.
	 */
	virtual void OnChannelModeV( Channel*, ChannelUser*,
			const xServer::voiceVectorType& ) ;

	/**
	 * This method is invoked when a user sets or removes
	 * one or more channel mode (b).  Keep in mind that the
	 * source ChannelUser may be NULL if a server is
	 * setting the mode.
	 */
	virtual void OnChannelModeB( Channel*, ChannelUser*,
			const xServer::banVectorType& ) ;

	/**
	 * This method is called for each signal that occurs
	 * in the system.  There is no registration needed to
	 * receive signals, just overload this method.  Be
	 * sure to end the method with a call to the base
	 * class OnSignal (or the closest base class).
	 */
	virtual void OnSignal( int ) ;

	/**
	 * OnCTCP is called when a CTCP command is issued to
	 * the client.
	 */
	virtual void OnCTCP( iClient* Sender,
		const string& CTCP,
		const string& Message,
		bool Secure = false ) ;

	/**
	 * This method is called when a channel CTCP occurs
	 * in a channel in which an xClient resides, and the
	 * xClient is user mode -d.
	 */
	virtual void OnChannelCTCP( iClient* Sender,
		Channel* theChan,
		const string& CTCPCommand,
		const string& Message ) ;

	/**
	 * OnPrivateMessage is called when a PRIVMSG command
	 * is issued to the client.
	 */
	virtual void OnPrivateMessage( iClient* Sender,
		const string& Message,
		bool secure = false ) ;

	/**
	 * This method is called when a channel message occurs
	 * in a channel in which an xClient resides, and the
	 * xClient is user mode -d.
	 */
	virtual void OnChannelMessage( iClient* Sender,
		Channel* theChan,
		const string& Message ) ;

	/**
	 * OnPrivateNotice is called when a NOTICE command
	 * is issued to the client.
	 */
	virtual void OnPrivateNotice( iClient* Sender,
		const string& Message, bool secure = false ) ;

	/**
	 * OnChannelNotice is called when a module receives
	 * channel notice, and is mode -d.
	 */
	virtual void OnChannelNotice( iClient* Sender,
		Channel* theChan, const string& Message ) ;

	/**
	 * OnServerMessage is called when a server message
	 * a client
	 */
	virtual void OnServerMessage( iServer* Sender,
		const string& Message, bool secure = false ) ;

	/**
	 * Handle a timer event.  The first argument is the
	 * handle for the timer registration, and the second is
	 * the arguments that were passed when registering the
	 * timer.
	 * This method overloads the pure virtual TimerHandler
	 * base class method declaration.
	 */
	virtual void	OnTimer( xServer::timerID, void* ) ;

	virtual void	OnTimerDestroy( xServer::timerID, void* ) ;

	/* Utility methods */

	/**
	 * Op a user on a channel, join/part the channel if necessary.
	 */
	virtual bool	Op( Channel*, iClient* ) ;

	/**
	 * Op one or more users on a channel, join/part the channel
	 * if necessary.
	 */
	virtual bool	Op( Channel*, const vector< iClient* >& ) ;

	/**
	 * Voice a user on a channel, join/part the channel if necessary.
	 */
	virtual bool	Voice( Channel*, iClient* ) ;

	/**
	 * Voice one or more users on a channel, join/part the channel
	 * if necessary.
	 */
	virtual bool	Voice( Channel*, const vector< iClient* >& ) ;
 
	/**
	 * Deop a user on a channel, join/part the channel if necessary.
	 */
	virtual bool	DeOp( Channel*, iClient* ) ;

	/**
	 * Deop a user on a channel, join/part the channel if necessary.
	 */
	virtual bool	DeOp( Channel*, const vector< iClient* >& ) ;

	/**
	 * Devoice a user on a channel, join/part the channel if necessary.
	 */
	virtual bool	DeVoice( Channel*, iClient* ) ;

	/**
	 * Devoice one or more users on a channel, join/part the channel
	 * if necessary.
	 */
	virtual bool	DeVoice( Channel*, const vector< iClient* >& ) ; 

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
	 * Remove a channel ban.
	 */
	virtual bool	UnBan( Channel*, const string& ) ;

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
	virtual bool	Join( const string& chanName,
				const string& modes = string(),
				const time_t& joinTime = 0,
				bool getOps = false ) ;

	/**
	 * Join the given channel.
	 */
	virtual bool	Join( Channel* theChan,
				const string& modes = string(),
				const time_t& joinTime = 0,
				bool getOps = false ) ;

	/**
	 * This method is called when the bot joins a channel.
	 */
	virtual void	OnJoin( Channel* ) ;

	/**
	 * This method is called when the bot joins a channel.
	 */
	virtual void	OnJoin( const string& ) ;

	/**
	 * Part will cause the client to part a channel.
	 */
	virtual bool	Part( const string&, const string& = "" ) ;

	/**
	 * Part the given channel.
	 */
	virtual bool	Part( Channel* ) ;

	/**
	 * This method is called when the bot parts a channel.
	 */
	virtual void	OnPart( Channel* ) ;

	/**
	 * This method is called when the bot parts a channel.
	 */
	virtual void	OnPart( const string& ) ;

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
	virtual bool DoCTCP( iClient* Target,
		const string& CTCP,
		const string& Message ) ;

	/**
	 * Message will PRIVMSG a string of data to the given iClient.
	 */
	virtual bool Message( const iClient* Target,
		const char* Message, ... ) ;

	/**
	 * Message will PRIVMSG a string of data to the given iClient.
	 */
	virtual bool Message( const iClient* Target,
		const string& Message ) ;

	/**
	 * This format of Message will write a string of data
	 * to a channel.
	 */
	virtual bool Message( const string& Channel,
		const char* Message, ... ) ;

	/**
	 * This format of Message will write a string of data
	 * to a channel.
	 */
	virtual bool Message( const string& Channel,
		const string& Message ) ;

	/**
	 * This format of Message will write a string of data
	 * to a channel.
	 */
	virtual bool Message( const Channel* theChan,
		const string& Message ) ;

	/**
	 * Notice will send a NOTICE command to the given iClient.
	 */
	virtual bool Notice( const iClient* Target,
		const char* Message, ... ) ;

	/**
	 * Notice will send a NOTICE command to the given iClient.
	 */
	virtual bool Notice( const iClient* Target, const string& ) ;

	/**
	 * This Notice() signature will send a channel NOTICE.
	 */
	virtual bool Notice( const string& Channel,
		const char* Message, ... ) ;

	/**
	 * This Notice() signature will send a channel NOTICE.
	 */
	virtual bool Notice( const Channel* theChan,
		const char* Message, ... ) ;

	/**
	 * Have this bot send a global wallops message.
	 */
	virtual bool Wallops( const string& ) ;

	/**
	 * Have this bot send a global wallops message.
	 */
	virtual bool Wallops( const char* Format, ... ) ;

	/**
	 * Have the server send a wallops.
	 */
	virtual bool WallopsAsServer( const string& ) ;

	/**
	 * Have the server send a wallops.
	 */
	virtual bool WallopsAsServer( const char* Format, ... ) ;

	/**
	 * Return true if this xClient is attached to a server.
	 */
	inline bool isConnected() const
		{ return Connected ; }

	/**
	 * Return this xClient's network instance (iClient*).
	 */
	inline iClient* getInstance() const
		{ return me ; }

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
	 * Return true if an arbitrary mode is set, false otherwise.
	 */
	inline bool	getMode( const modeType& whichMode ) const
		{ return ((mode & whichMode) == whichMode) ; }

	/**
	 * Obtain a pointer to the single xServer instance, which
	 * is the uplink of every services bot.
	 * Use of this method is discouraged.
	 */
	inline xServer* getUplink() const
		{ return MyUplink ; }

	/**
	 * Return the name of the configuration file from which this
	 * client derived its configuration information.
	 */
	inline const string& getConfigFileName() const
		{ return configFileName ; }

	friend ELog& operator<<( ELog& out,
		const xClient& theClient )
		{
		out     << theClient.nickName << '!'
			<< theClient.userName << '@'
			<< theClient.hostName
			<< " Numeric: " << theClient.getCharYYXXX()
			<< ", int YY/XXX/YYXXX: "
			<< theClient.getIntYY() << '/'
			<< theClient.getIntXXX() ;
		return out ;
		}

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
 	 * This method is called by the xServer, and its purpose is
	 * to reset its iClient instance.
	 */
	inline void resetInstance()
		{ me = 0 ; }

	/**
	 * ImplementServer is called by the xServer once
	 * the client has been attached to the server and
	 * has been assigned a YY numeric.  This method
	 * must then acquire the rest of its numeric information
	 * from the xServer.
	 */
	virtual void ImplementServer( xServer* Server ) ;

	/**
	 * This method is called to add a channel to the bot's
	 * internal channel database; typically called from
	 * OnJoin().  This is used to maintain the integrity
	 * of isOnChannel() calls.
	 */
	virtual bool	addChan( Channel* ) ;

	/**
	 * This method is called to remove a channel from the
	 * bot's internal database; typically called from OnPart().
	 * This is used to maintain the integrity of isOnChannel()
	 * calls.
	 */
	virtual bool	removeChan( Channel* ) ;

	/**
	 * This method sets the iClient instance of this xClient.
	 */
	virtual void	setInstance( iClient* me )
		{ this->me = me ; }

	/**
	 * The iClient representation of this xClient.
	 * This variable will be set by xNetwork once the
	 * xClient links to the xServer.
	 */
	iClient*	me ;

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

	/**
	 * The name of the config file from which this client read
	 * its configuration information.
	 */
	string		configFileName ;
} ;

} // namespace gnuworld

#endif // __CLIENT_H
