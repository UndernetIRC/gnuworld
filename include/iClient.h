/* iClient.h
 */

#ifndef __ICLIENT_H
#define __ICLIENT_H "$Id: iClient.h,v 1.3 2000/07/31 15:17:25 dan_karrels Exp $"

#include	<string>
#include	<list>
#include	<iostream>

#include	<ctime>

#include	"Numeric.h"
#include	"Channel.h"

using std::string ;
using std::list ;

namespace gnuworld
{

/**
 * iClient objects represent network users.  This class provides
 * basic functionality and state expected for a network client.
 * Note that iClient is NOT used for services clients, see
 * class xClient for that.
 */
class iClient
{

protected:

	/**
	 * The type used to hold channel patronage information
	 * for each particular user.
	 */
	typedef list< Channel* > channelListType ;

public:

	/**
	 * Define a type to be used for storing the
	 * iClient's modes.
	 */
	typedef unsigned int modeType ;

	/// MODE_OPER is true if the iClient is an IRC operator.
	const static modeType	MODE_OPER	= 0x01 ;

	/// MODE_WALLOPS is true if the iClient is receiving wallops.
	const static modeType	MODE_WALLOPS	= 0x02 ;

	/// MODE_INVISIBLE is true if the iClient is user mode invisible.
	const static modeType	MODE_INVISIBLE	= 0x04 ;

	/// MODE_DEAF is true if the iClient is not receiving messages.
	const static modeType	MODE_DEAF	= 0x10 ;

	/// MODE_SERVICES is true if the iClient is a service agent.
	const static modeType	MODE_SERVICES	= 0x20 ;

	/// Iterator for channels this user is on.
	typedef channelListType::iterator channelIterator ;

	/// Constant iterator for channels this user is on.
	typedef channelListType::const_iterator const_channelIterator ;

	/**
	 * Construct a new iClient given a large list of
	 * parameters for the client's state.
	 */
	iClient( const unsigned int& _uplink,
		const string& _yyxxx,
		const string& _nickName,
		const string& _userName,
		const string& _hostBase64,
		const string& _insecureHost,
		const string& _mode,
		const time_t& _connectTime ) ;

	/**
	 * Destruct the iClient.
	 */
	virtual ~iClient() ;

	/* Accessor methods */

	/**
	 * Retrieve the iClient's nick name.
	 */
	inline const string& getNickName() const
		{ return nickName ; }

	/**
	 * Retrieve the iClient's user name.
	 */
	inline const string& getUserName() const
		{ return userName ; }

	/**
	 * Retrieve the iClient's host name, NOT IP.
	 */
	inline const string& getInsecureHost() const
		{ return insecureHost ;}

	/**
	 * Retrieve a string of the form: nick!user@host for this user.
	 */
	inline const string getNickUserHost() const
		{ return (nickName + '!' + userName + '@' + insecureHost) ; }

	/**
	 * Retrieve the iClient's connection time.
	 */
	inline const time_t& getConnectTime() const
		{ return connectTime ; }

	/**
	 * Retrieve the iClient's IP.  A network byte ordered
	 * one-word mask is returned.
	 * TODO: Make sure that a word is 32 bits, adjusting where
	 * necessary for > 32bit architectures.
	 */
	inline const long int& getIP() const
		{ return IP ; }

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

	/* Mutator methods */

	/**
	 * Add a channel to this user's channel patronage structure.
	 */
	inline void addChannel( Channel* newChannel )
		{ channelList.push_front( newChannel ) ; }

	/**
	 * Remove a channel from this user's channel patronage structure.
	 */
	inline void removeChannel( Channel* theChannel ) ;

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
	inline void setNickName( const string& newNick )
		{ nickName = newNick ; }

	// The following methods are used to access and mutate
	// the client's modes.

	/**
	 * Return true if this client has the given mode set, false
	 * otherwise.
	 */
	inline bool getMode( const modeType& theMode ) const
		{ return (mode & theMode) ; }

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
	inline void addMode( const modeType& newMode )
		{ mode |= newMode ; }

	/**
	 * Remove a user mode for this iClient.
	 */
	inline void removeMode( const modeType& theMode )
		{ mode &= ~theMode ; }

	/**
	 * Return a string representation of this iClient's user
	 * modes.
	 */
	inline const string getCharModes() const ;

	/**
	 * Return this client's uplink server's integer numeric.
	 */
	inline const unsigned int& getIntYY() const
		{ return intYY ; }

	/**
	 * Return this client's integer per-server numeric.
	 */
	inline const unsigned int& getIntXXX() const
		{ return intXXX ; }

	/**
	 * Return this clients integer network numeric.
	 */
	inline const unsigned int& getIntYYXXX() const
		{ return intYYXXX ; }

	/**
	 * Return this client's uplink server's character numeric.
	 */
	inline const char* getCharYY() const
		{ return charYY ; }

	/**
	 * Return this client's character per-server numeric.
	 */
	inline const char* getCharXXX() const
		{ return charXXX ; }

	/**
	 * Return this client's full character numeric.
	 */
	inline const string getCharYYXXX() const
		{ return( string( charYY ) + charXXX ) ; }

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
			<< std::endl ;
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
	void		setModes( const string& ) ;

	// The below variables are ordered to provide
	// efficient instantiation.  Do not alter order.

	/**
	 * This is the integer representation of this
	 * client's uplink server.
	 */
	unsigned int	intYY ;

	/**
	 * This client's nick name.
	 */
	string		nickName ;

	/** This client's user name.
	 */
	string		userName ;

	/**
	 * This client's 32 bit IP, stored in
	 *  network (bigendian) byte order.
	 */
	long int	IP ;

	/**
	 * This client's hostname as it appears to network users.
	 */
	string		insecureHost ;

	/**
	 * The time at which this iClient connected to the network.
	 */
	time_t		connectTime ;

	/**
	 * This client's integer per-server numeric.
	 */
	unsigned int	intXXX ;

	/**
	 * This client's uplink server's character numeric.
	 */
	char		charYY[ 3 ] ;

	/**
	 * This client's per-server character numeric.
	 */
	char		charXXX[ 4 ] ;

	/**
	 * This client's current user modes.
	 */
	modeType	mode ;

	/**
	 * This client's integer network numeric.
	 */
	unsigned int	intYYXXX ;

	/**
	 * The structure used to store which channels this user is in.
	 */
	channelListType	channelList ;

} ;

} // namespace gnuworld

#endif // _ICLIENT_H
