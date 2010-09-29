/**
 * testBot.h
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
 * $Id: testbot.h,v 1.4 2005/01/12 04:36:48 dan_karrels Exp $
 */

#ifndef __TESTBOT_H
#define __TESTBOT_H "$Id: testbot.h,v 1.4 2005/01/12 04:36:48 dan_karrels Exp $"

#include	<string>
#include	<list>

#include	"ConnectionManager.h"
#include	"ConnectionHandler.h"
#include	"Connection.h"

using namespace gnuworld ;

/**
 * This is a little client bot that connects to an irc server
 * like all normal bots.  This is just a proof of concept for
 * the ConnectionManager subsystem.  It is clean and small, just
 * for fun :).
 */
class testBot : public ConnectionHandler
{

public:
	/**
	 * Create a new testBot with the given configuration  file
	 * name.
	 */
	testBot( const std::string& ) ;

	/**
	 * Do the usual destructor stuff.
	 */
	virtual ~testBot() ;

	/* The following connection method are called by the
	 * ConnectionManager subsystem, there is no need to call
	 * them directly.
	 */

	/**
	 * This method is called when a connection is established.
	 */
	virtual void	OnConnect( Connection* ) ;

	/**
	 * This method is invoked when a connection attempt fails.
	 */
	virtual void	OnConnectFail( Connection* ) ;

	/**
	 * This method is called when a line of data is read from
	 * the connection.  No need to buffer this data any
	 * further, this is a complete '\n' delimited command.
	 */
	virtual void	OnRead( Connection*, const std::string& ) ;

	/**
	 * This method is called when the connection terminates
	 * on the other end.
	 */
	virtual void	OnDisconnect( Connection* ) ;

	/**
	 * This method is called if a timeout occurs during
	 * connection attempt.
	 */
	virtual void	OnTimeout( Connection* ) ;

	/**
	 * This method contains the main loop of the testbot.
	 */
	virtual void	Run() ;

	/**
	 * Return the name of the current uplink.
	 */
	inline const std::string&	getUplinkName() const
		{ return uplinkName ; }

	/**
	 * Return the current uplink's port.
	 */
	inline unsigned short int getUplinkPort() const
		{ return uplinkPort ; }

	/**
	 * Return the bot's current nickname.
	 */
	inline const std::string&	getNickName() const
		{ return nickName ; }

	/**
	 * Return the bot's username.
	 */
	inline const std::string&	getUserName() const
		{ return userName ; }

	/**
	 * Return the bot's real name.
	 */
	inline const std::string&	getRealName() const
		{ return realName ; }

	/**
	 * Set the name of the uplink.
	 */
	inline void	setUplinkName( const std::string& newVal )
		{ uplinkName = newVal ; }

	/**
	 * Set the port on the uplink to which the bot is
	 * connected or connecting.
	 */
	inline void	setUplinkPort( unsigned short int newVal )
		{ uplinkPort = newVal ; }

	/**
	 * Set the bot's nickname.
	 */
	inline void	setNickName( const std::string& newVal )
		{ nickName = newVal ; }

	/**
	 * Set the bot's username.
	 */
	inline void	setUserName( const std::string& newVal )
		{ userName = newVal ; }

	/**
	 * Set the bot's real name.
	 */
	inline void	setRealName( const std::string& newVal )
		{ realName = newVal ; }

protected:

	/// This method outputs the NICK/USER lines when the
	/// bot is first connected.
	virtual void	login() ;

	/// This method is called when a 376 is read from the
	/// uplink, and it attempts to join all channels found
	/// in the configuration file.
	virtual void	joinChannels() ;

	/// processInput() attempts to parse the line of data
	/// received in OnRead(), and interpret some basic
	/// IRC commands.
	virtual void	processInput( const std::string& ) ;

	/// The bot will continue running while this variable is
	/// true.  Setting it to false will cause the bot to
	/// terminate.
	bool			keepRunning ;

	/// The name of the current uplink server
	std::string		uplinkName ;

	/// The port on the uplink server to which we are connected
	/// or connecting.
	unsigned short int	uplinkPort ;

	/// The bot's current nickname.
	std::string		nickName ;

	/// The bot's userrname.
	std::string		userName ;

	/// The bot's realname.
	std::string		realName ;

	/// Pointer to the uplink connection
	Connection*		myConnect ;

	/// The ConnectionManager subsystem instance for this
	/// testbot.
	ConnectionManager	cm ;

	/// The list of channels the bot will attempt to join.
	std::list< std::string >	channels ;
} ;

#endif // __TESTBOT_H
