/**
 * iClient.h
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
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
 * $Id: iClient.h,v 1.49 2009/07/26 18:30:37 mrbean_ Exp $
 */

#ifndef __ICLIENT_H
#define __ICLIENT_H "$Id: iClient.h,v 1.49 2009/07/26 18:30:37 mrbean_ Exp $"

#include	<string>
#include	<list>
#include	<map>

#include	<ctime>

#include	"gnuworld_config.h"
#include	"Channel.h"
#include	"iServer.h"
#include	"NetworkTarget.h"
#include	"ELog.h"

namespace gnuworld
{

class xClient ;
class Network ;

/**
 * iClient objects represent network users.  This class provides
 * basic functionality and state expected for a network client.
 * Note that iClient is NOT used for services clients, see
 * class xClient for that.
 */
class iClient : public NetworkTarget
{

protected:

	/**
	 * The type used to hold channel patronage information
	 * for each particular user.
	 */
	typedef std::list< Channel* > channelListType ;

public:

	/**
	 * Define a type to be used for storing the
	 * iClient's modes.
	 */
	typedef unsigned int modeType ;

	/// MODE_OPER is true if the iClient is an IRC operator.
	static const modeType	MODE_OPER ;

	/// MODE_WALLOPS is true if the iClient is receiving wallops.
	static const modeType	MODE_WALLOPS ;

	/// MODE_INVISIBLE is true if the iClient is user mode invisible.
	static const modeType	MODE_INVISIBLE ;

	/// MODE_DEAF is true if the iClient is not receiving messages.
	static const modeType	MODE_DEAF ;

	/// MODE_SERVICES is true if the iClient is a service agent.
	static const modeType	MODE_SERVICES ;

	/// MODE_REGISTERED is true if the iClient has an account set.
	static const modeType	MODE_REGISTERED ;

	/// MODE_HIDDEN_HOST is true if the iClient has HIDDEN_HOST (+x) set.
	static const modeType	MODE_HIDDEN_HOST ;

	/// MODE_G is true if the iCilent has user mode g set.
	static const modeType	MODE_G ;

	/// MODE_SERVNOTICES is true if this user is receiving server
	/// notices.  This may not be transmitted on all networks.
	static const modeType	MODE_SERVNOTICES ;
	
	/** MODE_FAKE is true if this user is a fake client. */
	static const modeType MODE_FAKE;

	/// Iterator for channels this user is on.
	typedef channelListType::iterator channelIterator ;

	/// Constant iterator for channels this user is on.
	typedef channelListType::const_iterator const_channelIterator ;

	/// Type used to store the number of channels for this iClient
	typedef channelListType::size_type channels_sizeType ;

	/**
	 * Construct a new iClient given a large list of
	 * parameters for the client's state.
	 */
	iClient( const unsigned int& _uplink,
		const std::string& _yyxxx,
		const std::string& _nickName,
		const std::string& _userName,
		const std::string& _hostBase64,
		const std::string& _insecureHost,
		const std::string& _realInsecureHost,
		const std::string& _mode,
		const std::string& _account,
		const time_t _account_ts,
		const std::string& _description,
		const time_t& _nick_ts ) ;

	/**
	 * Construct a new iClient given a large list of
	 * parameters for the client's state.
	 * This signature assumes that either/both of asuka are being
	 * used.
	 */
	iClient( const unsigned int& _uplink,
		const std::string& _yyxxx,
		const std::string& _nickName,
		const std::string& _userName,
		const std::string& _hostBase64,
		const std::string& _insecureHost,
		const std::string& _realInsecureHost,
		const std::string& _mode,
		const std::string& _account,
		const time_t _account_ts,
		const std::string& _setHost,
		const std::string& _fakeHost,
		const std::string& _description,
		const time_t& _nick_ts ) ;

	/**
	 * Destruct the iClient.
	 * This will call xClient::deleteCustomData() for each
	 * xClient which is storing a data element in this iClient.
	 */
	virtual ~iClient() ;

	/* Accessor methods */

	/**
	 * Retrieve the iClient's nick name.
	 */
	inline const std::string& getNickName() const
		{ return nickName ; }

	/**
	 * Retrieve the iClient's user name.
	 */
	inline const std::string& getUserName() const
		{ return userName ; }

	/**
	 * Retrieve the iClient's host name, NOT IP.
	 */
	inline const std::string& getInsecureHost() const
		{ return insecureHost ;}

	/**
	 * Retrieve the iClient's 'real' host name.
	 */
	inline const std::string& getRealInsecureHost() const
		{ return realInsecureHost ;}

	/**
	 * Retrieve a string of the form: nick!user for this user.
	 */
	inline const std::string getNickUser() const
		{ return (nickName + '!' + userName) ; }

	/**
	 * Retrieve a string of the form: nick!user@host for this user.
	 */
	inline const std::string getNickUserHost() const
		{ return (nickName + '!' + userName + '@' + insecureHost) ; }

	/**
	 * Retrieve a string of the form: user@real-host for this user.
	 */
	inline const std::string getRealUserHost() const
		{ return (userName + '@' + realInsecureHost) ; }

	/**
	 * Retrieve a string of the form: nick!user@real-host for this user.
	 */
	inline const std::string getRealNickUserHost() const
		{ return (nickName + '!' + userName + '@' + realInsecureHost) ; }

	/**
	 * Retrieve client's 'real-name' field.
	 */
	inline const std::string& getDescription() const
		{ return description ; }

	/**
	 * Overwrite this clients "Real Host" with a "Hidden Host".
	 */
	inline void setHiddenHost()
		{ insecureHost = account + hiddenHostSuffix ; }

	/**
	 * This method will set the hidden host suffix.  This value
	 * is only modified by the xServer on startup.
	 */
	inline static void	setHiddenHostSuffix( const std::string& newVal )
		{ hiddenHostSuffix = newVal ; }

	/**
	 * Retrieve client's 'account' field.
	 */
	inline const std::string& getAccount() const
		{ return account ; }

	/**
	 * Retrieve client's account timestamp.
	 *
	 * @return the timestamp of the account if set, else 0.
	 * Note: the value in ircu is a timestamp, but gnuworld uses it as the account ID
	 */
	inline const time_t& getAccountID() const
		{ return account_ts; }

	/**
	 * Return the suffix hostname to be appended to the
	 * user's account name for use with host hiding.
	 */
	inline static const std::string& getHiddenHostSuffix()
		{ return hiddenHostSuffix ; }

	/**
	 * This method sets user mode +r and records the account
	 * domain for this network client.
	 */
	inline void setAccount( const std::string& _account )
		{
		account = _account ;
		setModeR() ;

		// We know that the user is mode R already
		if (isModeX()) setHiddenHost();
		}

	/**
	 * Set the account ID for this iClient.
	 * Only valid if the iClient isModeR()
	 * Note: the value in ircu is a timestamp, but gnuworld uses it as the account ID
	 */
	inline void setAccountID( const time_t _account_ts )
		{
		if( ! isModeR() ) { return ; }
		account_ts = _account_ts;
		}

#ifdef ASUKA
	/**
	 * Retrieve the iClient's sethost.
	 */
	inline const std::string& getSetHost() const
		{ return setHost ; }
#endif

#ifdef SRVX
	/**
	 * Retrieve the iClient's fakehost.
	 */
	inline const std::string& getFakeHost() const
		{ return fakeHost ; }
#endif

	/**
	 * Retrieve the iClient's server
	 */
	const iServer* getServer();

	/**
	 * Retrieve the iClient's nickname timestamp.
	 */
	inline const time_t& getNickTS() const
		{ return nick_ts ; }

	/**
	 * Retrieve the first nickname timestamp recorded
	 * for an iClient.
	 * For clients having connected prior to gnuworld being linked,
	 * the timestamp will be the latter of their connection timestamp
	 * and the timestamp of any nickname changes after connection but
	 * prior to gnuworld linking.
	 *
	 * For clients connecting after gnuworld being linked, this will be
	 * their connection timestamp.
	 */
	inline const time_t& getFirstNickTS() const
		{ return first_nick_ts ; }

	/**
	 * Set the iClient's nickname timestamp.
	 */
	inline void setNickTS( const time_t& newTS )
		{ nick_ts = newTS ; }

	/**
	 * Retrieve the iClient's IP. Network byte ordered
	 */
	inline const irc_in_addr& getIP() const
		{ return IP ; }

	/*
	 * Cut down all the numeric conversions, get it directly
	 */
	inline const std::string& getNumericIP() const
		{ return numericIP ; }

	/**
	 * Obtain const iterator to beginning of this user's channel
	 * membership structure.
	 */
	inline const_channelIterator channels_begin() const
		{ return channelList.begin() ; }

	/**
	 * Obtain const iterator to end of this user's channel
	 * membership structure.
	 */
	inline const_channelIterator channels_end() const
		{ return channelList.end() ; }

	/**
	 * Obtain mutable iterator to beginning of this user's channel
	 * membership structure.
	 */
	inline channelIterator channels_begin()
		{ return channelList.begin() ; }

	/**
	 * Obtain mutable iterator to end of this user's channel
	 * membership structure.
	 */
	inline channelIterator channels_end()
		{ return channelList.end() ; }

	/**
	 * Return the number of channels which this user is on.
	 */
	inline channels_sizeType channels_size() const
		{ return channelList.size() ; }

	/* Mutator methods */

	/**
	 * Add a channel to this user's channel patronage structure.
	 */
	bool addChannel( Channel* newChannel ) ;

	/**
	 * Remove a channel from this user's channel patronage structure.
	 */
	bool removeChannel( Channel* theChannel ) ;

	/**
	 * Return true if this iClient is in the given channel.
	 */
	bool findChannel( const Channel* theChannel ) const ;

	/**
	 * Clear this client's list of channels.  No heap space needs
	 * to be deallocated here.
	 */
	inline void clearChannels()
		{ channelList.clear() ; }

	/**
	 * Change this iClient's nick name.
	 * During an iClient's lifetime on the network, the only
	 * thing that may change is its nickname.
	 */
	inline void setNickName( const std::string& newNick )
		{ nickName = newNick ; }

	// The following methods are used to access and mutate
	// the client's modes.

	/**
	 * Return true if this client has the given mode set, false
	 * otherwise.
	 */
	inline bool getMode( const modeType& theMode ) const
		{ return (theMode == (mode & theMode)) ; }

	/**
	 * Return true if this client has the +i mode set, false otherwise.
	 */
	inline bool isModeI() const
		{ return getMode( MODE_INVISIBLE ) ; }

	/**
	 * Return true if this client has the +w mode set, false otherwise.
	 */
	inline bool isModeW() const
		{ return getMode( MODE_WALLOPS ) ; }

	/**
	 * Return true if this client has the +k mode set, false otherwise.
	 */
	inline bool isModeK() const
		{ return getMode( MODE_SERVICES ) ; }

	/**
	 * Return true if this client has the +o mode set, false otherwise.
	 */
	inline bool isModeO() const
		{ return getMode( MODE_OPER ) ; }

	/**
	 * Return true if this client has the +d mode set, false otherwise.
	 */
	inline bool isModeD() const
		{ return getMode( MODE_DEAF ) ; }

	/**
	 * Return true if this client has the +r mode set, false otherwise.
	 */
	inline bool isModeR() const
		{ return getMode( MODE_REGISTERED ) ; }

	/**
	 * Return true if this client has the +x mode set, false otherwise.
	 */
	inline bool isModeX() const
		{ return getMode( MODE_HIDDEN_HOST ) ; }

	/**
	 * Return true if this client has the +g mode set, false otherwise.
	 */
	inline bool isModeG() const
		{ return getMode( MODE_G ) ; }

	/**
	 * Return true if this iClient is a fake, false otherwise.
	 */
	inline bool isFake() const
		{ return getMode( MODE_FAKE ) ; }

	/**
	 * Return true if this iClient is an oper, false otherwise.
	 */
	inline bool isOper() const
		{ return getMode( MODE_OPER ) ; }

	/**
	 * Return this iClient's current user modes.
	 */
	inline const modeType& getModes() const
		{ return mode ; }

	/**
	 * Set a user mode for this iClient.
	 */
	inline void setMode( const modeType& newMode )
		{ mode |= newMode ; }

	/**
	 * Set mode +i for this user.
	 */
	inline void setModeI()
		{ setMode( MODE_INVISIBLE ) ; }

	/**
	 * Set mode +w for this user.
	 */
	inline void setModeW()
		{ setMode( MODE_WALLOPS ) ; }

	/**
	 * Set mode +k for this user.
	 */
	inline void setModeK()
		{ setMode( MODE_SERVICES ) ; }

	/**
	 * Set mode +d for this user.
	 */
	inline void setModeD()
		{ setMode( MODE_DEAF ) ; }

	/**
	 * Set mode +g for this user.
	 */
	inline void setModeG()
		{ setMode( MODE_G ) ; }

	/**
	 * Set mode +o for this user.
	 */
	inline void setModeO()
		{ setMode( MODE_OPER ) ; }

	/**
	 * Set mode +x for this user.
	 */
	inline void setModeX()
		{
		setMode( MODE_HIDDEN_HOST ) ;
		if (isModeR() && isModeX()) setHiddenHost();
		}

	/**
	 * Set mode +r for this user.
	 */
	inline void setModeR()
		{ setMode( MODE_REGISTERED ) ; }

	/**
	 * Designate this iClient as a fake.
	 */
	inline void setFake()
		{ setMode( MODE_FAKE ) ; }

	/**
	 * Remove a user mode for this iClient.
	 */
	inline void removeMode( const modeType& theMode )
		{ mode &= ~theMode ; }

	/**
	 * Remove user mode 'i'.
	 */
	inline void removeModeI()
		{ removeMode( MODE_INVISIBLE ) ; }

	/**
	 * Remove user mode 'w'.
	 */
	inline void removeModeW()
		{ removeMode( MODE_WALLOPS ) ; }

	/**
	 * Remove user mode 'k'.
	 */
	inline void removeModeK()
		{ removeMode( MODE_SERVICES ) ; }

	/**
	 * Remove user mode 'd'.
	 */
	inline void removeModeD()
		{ removeMode( MODE_DEAF ) ; }

	/**
	 * Remove user mode 'g'.
	 */
	inline void removeModeG()
		{ removeMode( MODE_G ) ; }

	/**
	 * Remove user mode 'o'.
	 */
	inline void removeModeO()
		{ removeMode( MODE_OPER ) ; }

	/**
	 * Remove user mode 'x'.
	 */
	inline void removeModeX()
		{ removeMode( MODE_HIDDEN_HOST ) ; }

	/**
	 * Remove user mode 'r'.
	 */
	inline void removeModeR()
		{ removeMode( MODE_REGISTERED ) ; }

	/**
	 * Return a string representation of this iClient's user
	 * modes.
	 */
	const std::string getStringModes() const ;

	/**
	 * Add an xClient's personal data representation to this
	 * iClient.  Only one element per xClient is permitted.
	 * This method will return false if the internal allocation
	 * fails, or if the xClient in question is already using
	 * a data element in this iClient, or if the addition to the
	 * internal data structure fails.
	 * Add a NULL value to this iClient is permitted.
	 */
	bool		setCustomData( xClient*, void* ) ;

	/**
	 * Retrieve the custom data element for the given xClient.
	 * If none is found, NULL is returned.
	 */
	void*		getCustomData( xClient* ) const ;

	/**
	 * Retrieve, remove, and return the custom data element
	 * for the given xClient.  If none is found, NULL is
	 * returned.
	 */
	void*		removeCustomData( xClient* ) ;

	/**
	 * operator<< is overloaded for debugging purposes,
	 * this makes it extremely easy to output this client's
	 * information to an output stream.
	 */
	friend ELog& operator<<( ELog& out,
		const iClient& theClient )
		{
		out     << theClient.nickName << '!'
			<< theClient.userName << '@'
			<< theClient.insecureHost
			<< " Numeric: " << theClient.getCharYYXXX()
			<< ", int YY/XXX/YYXXX: "
			<< theClient.getIntYY() << '/'
			<< theClient.getIntXXX() << '/'
			<< theClient.getIntYYXXX() ;
		return out ;
		}

protected:

	/**
	 * Disable copy constructor.  This method is declared but
	 * NOT defined.
	 */
	iClient( const iClient& ) ;

	/**
	 * Disable default assignment.  This method is declared but
	 * NOT defined.
	 */
	iClient operator=( const iClient& ) ;

	/**
	 * Parse a string of modes and set this iClient's
	 * modes appropriately.
	 * This is called by the constructor only.
	 */
	void		setModes( const std::string& ) ;

	// The below variables are ordered to provide
	// efficient instantiation.  Do not alter order.

	/**
	 * This client's nick name.
	 */
	std::string	nickName ;

	/** This client's user name.
	 */
	std::string	userName ;

	/**
	 * This client's 128 bit IP, stored in
	 * network (bigendian) byte order.
	 */
	irc_in_addr	IP ;

	/*
	 * Converted to string, for smoother usage everywhere
	 */
	std::string	numericIP;

	/**
	 * This client's hostname as it appears to network users.
	 * (Possibly a hidden-hostname is the user is +x)
	 */
	std::string	insecureHost ;

	/**
	 * This client's actual network hostname, unhidden and
	 * exposed.
	 */
	std::string	realInsecureHost ;

	/**
	 * This client's 'real-name' field data.
	 */
	std::string	description ;

	/**
	 * The time at which this iClient took its current nickname.
	 */
	time_t		nick_ts ;

	/**
	 * The first nickname timestamp received for a client either at burst
	 * or at connect.
	 */
	time_t		first_nick_ts ;

	/**
	 * This client's current user modes.
	 */
	modeType	mode ;

	/** This client's "Account". */
	std::string	account ;
	
	/** The timestamp of this client's account. */
	time_t		account_ts ;

#ifdef ASUKA
	/**
	 * This client's sethost.
	 */
	std::string	setHost ;
#endif

#ifdef SRVX
	/**
	 * This client's fakehost.
	 */
	std::string	fakeHost ;
#endif

	/**
	 * The structure used to store which channels this user is in.
	 */
	channelListType	channelList ;

	/**
	 * This is the type used to represent the custom data map.
	 */
	typedef std::map< xClient*, void* >	customDataMapType ;

	/**
	 * This structure is used to store generic data for
	 * individual xClient's.  Only one element per xClient is
	 * permitted to be stored in this structure.  A pointer is
	 * used here to reduce memory footprint for those iClient's
	 * whose customDataMap's aren't used.
	 */
	customDataMapType		*customDataMap ;

	/**
	 * The suffix for all hidden host names.
	 * This variable is read from the .conf file, and
	 * has the form of "mynetwork.org".
	 */
	static std::string		hiddenHostSuffix ;

} ;

} // namespace gnuworld

#endif // __ICLIENT_H
