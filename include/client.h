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
 * $Id: client.h,v 1.59 2005/09/29 17:40:06 kewlio Exp $
 */

#ifndef __CLIENT_H
#define __CLIENT_H "$Id: client.h,v 1.59 2005/09/29 17:40:06 kewlio Exp $"

#include	<map>
#include	<sstream>
#include	<string>

#include	"NetworkTarget.h"
#include	"server.h"
#include	"iClient.h"
#include	"events.h"
#include	"TimerHandler.h"

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
class xClient : public TimerHandler, public NetworkTarget
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
	xClient( const std::string& ) ;

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
	 * This method must call xServer::BurstChannel() with appropriate
	 * channel modes for any channel it wishes to own.
	 */
	virtual void BurstChannels() ;

	/**
	 * BurstGlines is called before eob, for the client to burst
	 * all of its glines
	 */
	virtual bool BurstGlines() ;

	/**
	 * Kill will issue a KILL command to the network for
	 * the given iClient (network generic client).
	 */
	virtual bool Kill( iClient*, const std::string& ) ;
	virtual bool Kill( iClient*, const std::string&, bool ) ;

	/**
	 * QuoteAsServer will send data to the network as the
	 * server itself.  Try to avoid using this method.
	 */
	virtual bool QuoteAsServer( const std::string& Command );

	/**
	 * Write a string of data to the network.
	 */
	virtual bool Write( const std::string& s )
		{ return QuoteAsServer( s ) ; }

	/**
	 * Write a string of data to the network.
	 */
	virtual bool Write( const std::stringstream& s )
		{ return QuoteAsServer( s.str() ) ; }

	/**
	 * Write a variable length argument list to the network.
	 */
	virtual bool Write( const char*, ... ) ;

	/**
	 * This method will change modes in a channel.
	 * If the fourth argument is true, then the modes will be
	 * changed as the server, otherwise the client will set
	 * the modes (joining and parting the channel if necessary).
	 * Removing mode 'k' expects an argument, but it doesn't matter
	 * what the argument is.
	 * Removing mode 'l' requires NO argument to be issued.
	 */
	virtual bool Mode( const std::string& chanName,
			const std::string& modes,
			const std::string& args,
			bool modeAsServer = false ) ;

	/**
	 * This method will change modes in a channel.
	 * If the fourth argument is true, then the modes will be
	 * changed as the server, otherwise the client will set
	 * the modes (joining and parting the channel if necessary).
	 * Removing mode 'k' expects an argument, but it doesn't matter
	 * what the argument is.
	 * Removing mode 'l' requires NO argument to be issued.
	 */
	virtual bool Mode( Channel*,
			const std::string& modes,
			const std::string& args,
			bool modeAsServer = false ) ;

	/**
	 * Mode is used to set the bot's modes.  If connected to the
	 * network already, these new modes will be written to the
	 * network.
	 */
	virtual bool Mode( const std::string& Mode ) ;

	/**
	 * Issue a clearmode (CM) for the given channel and update
	 * all internal structures.
	 * This method will also post a channel event for each
	 * mode.
	 */
	virtual bool ClearMode( Channel* theChan,
				const std::string& modes,
				bool modeAsServer = false ) ;

	/**
	 * OnConnect is called when the server connects to the
	 * network, during burst time.  The client's NICK
	 * information has already been sent to the network.
	 */
	virtual void	OnConnect() ;

	/**
	 * Invoked when the uplink has been terminated.
	 */
	virtual void	OnDisconnect() ;

	/**
	 * This method is invoked when the server has been requested
	 * to shutdown.  If currently connected to the network, this
	 * method gives xClient's a chance to gracefully QUIT from
	 * the network, or whatever other processing is useful.
	 * To force data to be written before final shutdown (again,
	 * if connected), set xServer::FlushData().
	 * Timers will be executed after this method is invoked, once,
	 * depending upon target time of course :)
	 */
	virtual void	OnShutdown( const std::string& reason ) ;

	/**
	 * Invoked after the client has been loaded, perform
	 * initialization stuff here.
	 */
	virtual void	OnAttach() ;

	/**
	 * This method will be invoked when the server is unloading
	 * the client for whatever reason.
	 */
	virtual void	OnDetach( const std::string& =
				std::string( "Server Shutdown") ) ;

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
			const std::string& kickMessage,
			bool authoritative ) ;

	/**
	 * This method is invoked when one or more "simple" (no argument)
	 * channel modes are set or unset.
	 * The ChannelUser* (source) user may be NULL if the modes
	 * are being changed by a server.
	 */
	virtual void OnChannelMode( Channel*, ChannelUser*,
			const xServer::modeVectorType& ) ;

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
			ChannelUser*, const std::string& ) ;

	/**
	 * This method is invoked when a user sets or removes
	 * channel mode A (Apass).  Keep in mind that the
	 * source ChannelUser may be NULL if a server is
	 * setting the mode.
	 * If the mode is being removed, the Apass argument will
	 * be empty.
	 */
	virtual void OnChannelModeA( Channel*, bool polarity,
			ChannelUser*, const std::string& ) ;

	/**
	 * This method is invoked when a user sets or removes
	 * channel mode U (Upass).  Keep in mind that the
	 * source ChannelUser may be NULL if a server is
	 * setting the mode.
	 * If the mode is being removed, the Upass argument will
	 * be empty.
	 */
	virtual void OnChannelModeU( Channel*, bool polarity,
			ChannelUser*, const std::string& ) ;

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
		const std::string& CTCP,
		const std::string& Message,
		bool Secure = false ) ;

	/**
	 * OnFakeCTCP is called when a CTCP command is issued to
	 * one of this xClient's fake clients.
	 */
	virtual void OnFakeCTCP( iClient* Sender,
		iClient* fakeClient,
		const std::string& CTCP,
		const std::string& Message,
		bool Secure = false ) ;

	/**
	 * This method is called when a channel CTCP occurs
	 * in a channel in which an xClient resides, and the
	 * xClient is user mode -d.
	 */
	virtual void OnChannelCTCP( iClient* Sender,
		Channel* theChan,
		const std::string& CTCPCommand,
		const std::string& Message ) ;

	/**
	 * This method is called when a channel CTCP occurs
	 * in a channel in which an xClient resides, and the
	 * xClient is user mode -d.
	 */
	virtual void OnFakeChannelCTCP( iClient* Sender,
		iClient* fakeClient,
		Channel* theChan,
		const std::string& CTCPCommand,
		const std::string& Message ) ;

	/**
	 * OnPrivateMessage is called when a PRIVMSG command
	 * is issued to the client.
	 */
	virtual void OnPrivateMessage( iClient* Sender,
		const std::string& Message,
		bool secure = false ) ;

	/**
	 * Invoked when a private message arives for a fake client
	 * owned by this xClient.
	 */
	virtual void OnFakePrivateMessage( iClient* Sender,
		iClient* Target,
		const std::string& Message,
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
	 * Invoked when a fake client in a channel receives
	 * a channel message.
	 */
	virtual void OnFakeChannelMessage( iClient* Sender,
		iClient* Target,
		Channel* theChan,
		const std::string& Message ) ;

	/**
	 * OnPrivateNotice is called when a NOTICE command
	 * is issued to the client.
	 */
	virtual void OnPrivateNotice( iClient* Sender,
		const std::string& Message,
		bool secure = false ) ;

	/**
	 * Invoked when a private notice arives for a fake client
	 * owned by this xClient.
	 */
	virtual void OnFakePrivateNotice( iClient* Sender,
		iClient* Target,
		const std::string& Message,
		bool secure = false ) ;

	/**
	 * OnChannelNotice is called when a module receives
	 * channel notice, and is mode -d.
	 */
	virtual void OnChannelNotice( iClient* Sender,
		Channel* theChan,
		const std::string& Message ) ;

	/**
	 * Invoked when a fake client in a channel receives
	 * a channel notice.
	 */
	virtual void OnFakeChannelNotice( iClient* Sender,
		iClient* Target,
		Channel* theChan,
		const std::string& Message ) ;

	/**
	 * OnServerMessage is called when a server messages
	 * a client.
	 */
	virtual void OnServerMessage( iServer* Sender,
		const std::string& Message, bool secure = false ) ;

	/**
	 * Handle a timer event.  The first argument is the
	 * handle for the timer registration, and the second is
	 * the arguments that were passed when registering the
	 * timer.
	 * This method overloads the pure virtual TimerHandler
	 * base class method declaration.
	 */
	virtual void	OnTimer( const xServer::timerID& , void* ) ;

	/**
	 * A timer has been destroyed by the server (such as during
	 * a shutdown).  Perform cleanup for the timer here.
	 */
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
	virtual bool	Op( Channel*, const std::vector< iClient* >& ) ;

	/**
	 * Voice a user on a channel, join/part the channel if necessary.
	 */
	virtual bool	Voice( Channel*, iClient* ) ;

	/**
	 * Voice one or more users on a channel, join/part the channel
	 * if necessary.
	 */
	virtual bool	Voice( Channel*, const std::vector< iClient* >& ) ;
 
	/**
	 * Deop a user on a channel, join/part the channel if necessary.
	 */
	virtual bool	DeOp( Channel*, iClient* ) ;

	/**
	 * Deop a user on a channel, join/part the channel if necessary.
	 */
	virtual bool	DeOp( Channel*, const std::vector< iClient* >& ) ;

	/**
	 * Devoice a user on a channel, join/part the channel if necessary.
	 */
	virtual bool	DeVoice( Channel*, iClient* ) ;

	/**
	 * Devoice one or more users on a channel, join/part the channel
	 * if necessary.
	 */
	virtual bool	DeVoice( Channel*, const std::vector< iClient* >& ) ; 

	/**
	 * Set a ban on a channel, join/part the channel if necessary.
	 */
	virtual bool	Ban( Channel*, iClient* ) ;

	/**
	 * Set a ban on a channel, join/part the channel if necessary.
	 */
	virtual bool	Ban( Channel*, const std::vector< iClient* >& ) ;

	/**
	 * Ban kick a client from a channel for the given reason.
	 */
	virtual bool	BanKick( Channel*, iClient*, const std::string& ) ;

	/**
	 * Remove a channel ban.
	 */
	virtual bool	UnBan( Channel*, const std::string& ) ;

	/**
	 * Kick a user from a channel, join/part if necessary.
	 */
	virtual bool	Kick( Channel*, iClient*, const std::string&, bool modeAsServer = false);

	/**
	 * Kick several users from a channel, join/part if necessary.
	 */
	virtual bool	Kick( Channel*, const std::vector< iClient* >&,	const std::string&, bool modeAsServer = false);

	/**
	 * Kick all users from a channel that matches the specified IP, join/part if necessary.
	 */
	virtual bool	Kick( Channel*, const string&,
				const std::string&, bool modeAsServer = false ) ;

	/**
	 * Set the topic in a channel, joining, opping, and parting
	 * the client if necessary.
	 */
	virtual bool	Topic( Channel*, const std::string& ) ;

	/**
	 * Join will cause the client to join a channel.
	 */
	virtual bool	Join( const std::string& chanName,
				const std::string& modes = std::string(),
				const time_t& joinTime = 0,
				bool getOps = false ) ;

	/**
	 * Join the given channel.
	 */
	virtual bool	Join( Channel* theChan,
				const std::string& modes = std::string(),
				const time_t& joinTime = 0,
				bool getOps = false ) ;

	/**
	 * This method is called when the bot joins a channel.
	 */
	virtual void	OnJoin( Channel* ) ;

	/**
	 * This method is called when the bot joins a channel.
	 */
	virtual void	OnJoin( const std::string& ) ;

	/**
	 * Part will cause the client to part a channel.
	 */
	virtual bool	Part( const std::string&,
				const std::string& = std::string() ) ;

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
	virtual void	OnPart( const std::string& ) ;

	/**
	 * Invite a user to a channel.  Join the channel if necessary
	 * (and then part).
	 */
	virtual bool	Invite( iClient*, const std::string& ) ;

	/**
	 * Invite a user to a channel.  Join the channel if necessary
	 * (and then part).
	 */
	virtual bool	Invite( iClient*, Channel* ) ;

	/**
	 * Return true if the bot is on the given channel.
	 */
	virtual bool	isOnChannel( const std::string& chanName ) const ;

	/**
	 * Return true if the bot is on the given channel.
	 */
	virtual bool	isOnChannel( const Channel* theChan ) const ;

	/**
	 * DoCTCP will issue a CTCP (reply) to the given iClient.
	 */
	virtual bool DoCTCP( iClient* Target,
		const std::string& CTCP,
		const std::string& Message ) ;

	/**
	 * DoFakeCTCP will issue a CTCP (reply) to the given
	 * iClient with a fake client interface.
	 */
	virtual bool DoFakeCTCP( const iClient* Target,
		const iClient* srcClient,
		const std::string& CTCP,
		const std::string& Message ) ;

	/**
	 * Message will PRIVMSG a string of data to the given iClient.
	 */
	virtual bool Message( const iClient* Target,
		const char* Message, ... ) ;

	/**
	 * Message an iClient with a fake client interface.
	 */
	virtual bool FakeMessage( const iClient* Target,
		const iClient* srcClient,
		const std::string& Message ) ;

	/**
	 * Message a channel with a fake client interface.
	 */
	virtual bool FakeMessage( const Channel* theChan,
		const iClient* srcClient,
		const std::string& Message ) ;

	/**
	 * Notice an iClient with a fake client interface.
	 */
	virtual bool FakeNotice( const iClient* Target,
		const iClient* srcClient,
		const std::string& Message ) ;

	/**
	 * Notice a channel with a fake client interface.
	 */
	virtual bool FakeNotice( const Channel* theChan,
		const iClient* srcClient,
		const std::string& Message ) ;

	/**
	 * Message will PRIVMSG a string of data to the given iClient.
	 */
	virtual bool Message( const iClient* Target,
		const std::string& Message ) ;

	/**
	 * This format of Message will write a string of data
	 * to a channel.
	 */
	virtual bool Message( const std::string& Channel,
		const char* Message, ... ) ;

	/**
	 * This format of Message will write a string of data
	 * to a channel.
	 */
	virtual bool Message( const std::string& Channel,
		const std::string& Message ) ;

	/**
	 * This format of Message will write a string of data
	 * to a channel.
	 */
	virtual bool Message( const Channel* theChan,
		const std::string& Message ) ;

	/**
	 * Have this module message a channel.
	 */
	virtual bool Message( const Channel* theChan,
		const char* Format, ... ) ;

	/**
	 * Notice will send a NOTICE command to the given iClient.
	 */
	virtual bool Notice( const iClient* Target,
		const char* Message, ... ) ;

	/**
	 * Notice will send a NOTICE command to the given iClient.
	 */
	virtual bool Notice( const iClient* Target, const std::string& ) ;

	/**
	 * This Notice() signature will send a channel NOTICE.
	 */
	virtual bool Notice( const std::string& Channel,
		const char* Message, ... ) ;

	/**
	 * This Notice() signature will send a channel NOTICE.
	 */
	virtual bool Notice( const Channel* theChan,
		const char* Message, ... ) ;

	/**
	 * Notice channel operators with given message.
	 */
	virtual bool NoticeChannelOps( const Channel* theChan,
		const char* Message, ... ) ;

	/**
	 * Notice channel operators with given message.
	 */
	virtual bool NoticeChannelOps( const string& chanName,
		const char* Message, ... ) ;

	/**
	 * This Notice() signature will send a channel NOTICE.
	 */
	virtual bool Notice( const Channel*, const std::string& ) ;

	/**
	 * Have this bot send a global wallops message.
	 */
	virtual bool Wallops( const std::string& ) ;

	/**
	 * Have this bot send a global wallops message.
	 */
	virtual bool Wallops( const char* Format, ... ) ;

	/**
	 * Have the server send a wallops.
	 */
	virtual bool WallopsAsServer( const std::string& ) ;

	/**
	 * Have the server send a wallops.
	 */
	virtual bool WallopsAsServer( const char* Format, ... ) ;

	/**
	 * Return this xClient's network instance (iClient*).
	 */
	inline iClient* getInstance() const
		{ return me ; }

	/**
	 * Return this bot's nick name.
	 */
	inline const std::string& getNickName() const
		{ return nickName ; }

	/**
	 * Retrieve this bot's user name.
	 */
	inline const std::string& getUserName() const
		{ return userName ; }

	/**
	 * Retrieve this bot's host name.
	 */
	inline const std::string& getHostName() const
		{ return hostName ; }

	/**
	 * Retrieve this bot's description.
	 */
	inline const std::string& getDescription() const
		{ return userDescription ; }

	/**
	 * Retrieve this bot's uplink's numeric, integer format.
	 */
	unsigned int getUplinkIntYY() const
		{ return MyUplink->getIntYY() ; }

	/**
	 * Retrieve this bot's uplink's highest client count
	 * numeric, integer format.
	 */
	inline unsigned int getUplinkIntXXX() const
		{ return MyUplink->getIntXXX() ; }

	/**
	 * Retrieve this bot's uplink's numeric, character
	 * array format.
	 */
	inline const std::string getUplinkCharYY() const
		{ return MyUplink->getCharYY() ; }

	/**
	 * Retrieve this bot's uplink's client count numeric,
	 * character array format.
	 */
	inline const std::string getUplinkCharXXX() const
		{ return MyUplink->getCharXXX() ; }

	/**
	 * Retrieve this bot's uplink's network numeric,
	 * std::string format.
	 */
	inline const std::string getUplinkCharYYXXX() const
		{ return MyUplink->getCharYYXXX() ; }

	/**
	 * Retrieve this bot's uplink's server name.
	 */
	inline const std::string& getUplinkName() const
		{ return MyUplink->getName() ; }

	/**
	 * Retrieve this bot's uplink's description.
	 */
	inline const std::string& getUplinkDescription() const
		{ return MyUplink->getDescription() ; }

	/**
	 * Accessor method for the bot's user modes.
	 */
	virtual std::string getModes() const ;

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
	inline const std::string& getConfigFileName() const
		{ return configFileName ; }

	/**
	 * Return true if the server is connected to a network.
	 */
	inline bool isConnected() const
		{ return (MyUplink && MyUplink->isConnected()) ; }

	/**
	 * Utility method for outputting client information to
	 * a gnuworld logging stream.
	 */
	friend ELog& operator<<( ELog& out,
		const xClient& theClient )
		{
		out     << theClient.nickName << '!'
			<< theClient.userName << '@'
			<< theClient.hostName
			<< " Numeric: " << theClient.getCharYYXXX()
			<< ", int YY/XXX/YYXXX: "
			<< theClient.getIntYY() << '/'
			<< theClient.getIntXXX() << '/'
			<< theClient.getIntYYXXX() ;
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
 	 * This method is called by the xServer, and its purpose is
	 * to reset its iClient instance.
	 */
	inline void resetInstance()
		{ me = 0 ; }

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
	std::string	nickName ;

	/**
	 * This bot's user name.
	 */
	std::string	userName ;

	/**
	 * This bot's host name.
	 */
	std::string	hostName ;

	/**
	 * This bot's description.
	 */
	std::string	userDescription ;

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
	 * The name of the config file from which this client read
	 * its configuration information.
	 */
	std::string	configFileName ;
} ;

} // namespace gnuworld

#endif // __CLIENT_H
