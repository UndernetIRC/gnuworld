/* ccontrol.h
 * Author: Daniel Karrels dan@karrels.com
 */

#ifndef __CCONTROL_H
#define __CCONTROL_H "$Id: ccontrol.h,v 1.3 2000/07/10 22:41:29 dan_karrels Exp $"

#include	<string>
#include	<vector>
#include	<map>

#include	"client.h"
#include	"iClient.h"
#include	"server.h"
#include	"CControlCommands.h"
#include	"Channel.h"
#include    <stdio.h>

using std::string ;
using std::vector ;

namespace gnuworld
{
 
using namespace ccontrolns ;
class ccontrolns::Command ;

/**
 * A small class that contains information about the channels that
 * the ccontrol client is in.
 */
class e3Channel
{
public:
	/**
	 * Basic constructor receives name of the channel and
	 * default modes.
	 */
	e3Channel( const string& _chanName,
		const string& _modes )
	: chanName( _chanName ),
	  modes( _modes )
	{}
	/**
	 * This constructor just receives the name of the
	 * channel, no default modes.
	 */
	e3Channel( const string& _chanName )
	: chanName( _chanName )
	{}

	/**
	 * The name of the channel.
	 */
	string chanName ;

	/**
	 * The default modes of the channel, if any.
	 */
	string modes ;
} ;

/**
 * The ccontrol client class.  This is an operator service for network
 * operations.
 */
class ccontrol : public xClient
{

protected:

	/**
	 * The type used to store the client's command handlers.
	 */
	typedef map< string, Command*, noCaseCompare > commandMapType ;

	/**
	 * The value_type of the command table type.
	 * This type will be used to model key/value pairs
	 * for the command handler table.
	 */
	typedef commandMapType::value_type pairType ;

public:
	/**
	 * Default, and only constructor, receives the name
	 * of the configuration file for this client.
	 */
	ccontrol( const string& configFileName ) ;

	/**
	 * Destructor cleans up any allocated memory, etc.
	 */
	virtual ~ccontrol() ;

	/**
	 * This method is called by the xServer when it wants information
	 * about the channels this client will be on.
	 */
	virtual int BurstChannels() ;

	/**
	 * This method is invoked each time the client is sent a
	 * PRIVMSG.
	 */
	virtual int OnPrivateMessage( iClient*, const string& ) ;

	/**
	 * This method is invoked each time a network event occurs.
	 */
	virtual int OnEvent( const eventType&,
		void* = 0, void* = 0,
		void* = 0, void* = 0 ) ;

	/**
	 * This method is invoked each time a channel event occurs
	 * for one of the channels for which this client has registered
	 * to receive channel events.
	 */
	virtual int OnChannelEvent( const channelEventType&,
		const string&,
		void* = 0, void* = 0,
		void* = 0, void* = 0 ) ;

	/**
	 * This method is called once this client has been attached
	 * to an xServer.  This method is overloaded because the
	 * command handlers each require a reference to the xServer
	 * for efficiency.
	 */
	virtual void ImplementServer( xServer* ) ;

	/**
	 * Return true if the given channel name corresponds to a
	 * channel which is an IRCoperator only channel.
	 */
	inline bool isOperChan( const string& theChan ) const ;

	/**
	 * Return true if the given channel name corresponds to a
	 * channel that this client sits in, but is NOT an IRC operator
	 * only channel.
	 */
	inline bool isRegularChan( const string& theChan ) const ;

	/**
	 * Return true if this client is on the given channel.
	 */
	virtual bool isOnChannel( const string& chanName ) const
		{ return isOperChan( chanName ) || isRegularChan( chanName ) ; }

	/**
	 * This method will kick the given user from the given channel
	 * for the given reason (arg 3).
	 */
	virtual bool Kick( Channel*, iClient*, const string& ) ;

	/**
	 * This method will cause the bot to join the channel with
	 * the given name, if the client is not already on that
	 * channel.
	 */
	virtual bool Join( const string& ) ;

	/**
	 * This method will cause this client to part the given channel,
	 * if it is already on that channel.
	 */
	virtual bool Part( const string& ) ;

	/**
	 * This method will register a given command handler, removing
	 * (and deallocating) the existing handler for this command,
	 * should one exist.
	 */
	virtual bool RegisterCommand( Command* ) ;

	/**
	 * This method will unregister the command handler for the command
	 * of the given command name, deallocating the object from the
	 * heap as well.
	 */
	virtual bool UnRegisterCommand( const string& ) ;

	/**
	 * This method will add the given channel name as an IRC operator
	 * only channel.
	 * This method will cause the client to join the channel and
	 * kick all non-IRCops if it is not already on the channel.
	 */
	virtual bool addOperChan( const string& chanName ) ;

	/**
	 * This method will add the channel with the given name as an IRC
	 * operator only channel for the given reason.
	 * This method will cause the client to join the channel and
	 * kick all non-IRCops if it is not already on the channel.
	 */
	virtual bool addOperChan( const string& chanName, const string& reason ) ;

	/**
	 * This method will remove the channel with the given name from the
	 * list of IRCoperator only channels.
	 * This method will cause the client to part from the channel, if it
	 * is already in the channel.
	 */
	virtual bool removeOperChan( const string& ) ;

	/**
	 * This method will cause the client to join the given channel
	 * (if it's not already in the channel), and op itself.  All IRCops
	 * which join the channel will be opped.
	 */
	virtual bool addChan( const string& ) ;

	/**
	 * This method will cause the bot part the given channel added
	 * using the addChan() method.
	 */
	virtual bool removeChan( const string& ) ;

	/**
	 * This is a constant iterator type used to perform a read-only
	 * iteration of the operchan structure.
	 */
	typedef vector< string >::const_iterator const_operChanIterator ;

	/**
	 * This it an iterator type used to perform a mutating
	 * iteration of the operchan structure.
	 */
	typedef vector< string >::size_type operChanSizeType ;

	/**
	 * Retrieve a constant iterator to the beginning of the oper
	 * chan structure.
	 */
	inline const_operChanIterator operChan_begin() const
		{ return operChans.begin() ; }

	/**
	 * Retrieve a constant iterator to the end of the oper chan
	 * structure.
	 */
	inline const_operChanIterator operChan_end() const
		{ return operChans.end() ; }

	/**
	 * Retrieve the size of the operchan structure.
	 */
	inline operChanSizeType operChan_size() const
		{ return operChans.size() ; }

	/**
	 * Return true if the oper chan structure is empty.
	 */
	inline bool operChan_empty() const
		{ return operChans.empty() ; }

	/**
	 * The type of a constant iterator to the command map.
	 */
	typedef commandMapType::const_iterator constCommandIterator ;

	/**
	 * Retrieve a constant iterator to the beginning of the command
	 * table.
	 */
	constCommandIterator command_begin() const
		{ return commandMap.begin() ; }

	/**
	 * Retrieve a constant iterator to the end of the command table.
	 */
	constCommandIterator command_end() const
		{ return commandMap.end() ; }

	/**
	 * Retrieve a constant iterator to a command handler for the
	 * given command token.
	 */
	constCommandIterator findCommand( const string& theComm ) const
		{ return commandMap.find( theComm ) ; }

	/**
	 * Retrieve the default length of time for glines.
	 */
	inline const time_t& getDefaultGlineLength() const
		{ return gLength ; }

protected:

	/**
	 * The name of the channel to which to send network events.
	 * This is also an oper chan.
	 */
	string			msgChan ;

	/**
	 * The modes to enforce in an oper channel.
	 */
	string			operChanModes ;

	/**
 	 * The kick reason for kicking (-o) users from oper
	 * only channel.
	 */
	string			operChanReason ;

	/**
	 * The default length of time for glines.
	 */
	time_t			gLength ;

	/**
	 * The table of oper only channels.
	 */
	vector< string >	operChans ;

	/**
	 * The table which stores information about the regular
	 * channels this client is on.
	 */
	vector< e3Channel* >	channels ;

	/**
	 * The command handler table.
	 */
	commandMapType		commandMap ;

} ; 
 
} // namespace gnuworld
 
#endif // __CCONTROL_H
