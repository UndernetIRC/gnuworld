/* ChannelUser.h */

#ifndef __CHANNELUSER_H
#define __CHANNELUSER_H "$Id: ChannelUser.h,v 1.5 2001/06/23 16:27:52 dan_karrels Exp $"

#include	<string>

#include	"ELog.h"

namespace gnuworld
{

using std::string ;

/// Forward declaration of class iClient.
class iClient ;

/**
 * This class represents an individual channel patron, one
 * instance for each user in each channel.
 */
class ChannelUser
{

public:

	/**
	 * The type to be used for this ChannelUser's channel
	 * modes.
	 */
	typedef unsigned char modeType ;

	/// Bit representing channel user mode +o
	static const modeType	MODE_O		= 0x01 ;

	/// Bit representing channel user mode +v
	static const modeType	MODE_V		= 0x02 ;

	/// ZOMBIE is true if the iClient is in the zombie state
	const static modeType   ZOMBIE          = 0x04 ;

	/**
	 * Construct a ChannelUser given an iClient
	 * pointer.
	 */
	ChannelUser( iClient* ) ;

	/**
	 * Destroy this ChannelUser.
	 * No heap space allocated.
	 */
	virtual ~ChannelUser() ;

	/**
	 * Retrieve a given channel user mode.
	 */
	inline bool getMode( const modeType& whichMode ) const
		{ return (whichMode == (modes & whichMode)) ; }

	inline bool isModeO() const
		{ return getMode( MODE_O ) ; }

	inline bool isModeV() const
		{ return getMode( MODE_V ) ; }

	inline bool isZombie() const
		{ return getMode( ZOMBIE ) ; }

	/**
	 * Retrieve this ChannelUser's current modes in
	 * its channel.
	 */
	inline const modeType& getModes() const
		{ return modes ; }

	/**
	 * Set a given channel user mode.
	 */
	inline void setMode( const modeType& whichMode )
		{ modes |= whichMode ; }

	inline void setModeO()
		{ setMode( MODE_O ) ; }

	inline void setModeV()
		{ return setMode( MODE_V ) ; }

	inline void setZombie()
		{ setMode( ZOMBIE ) ; }

	/**
	 * Remove a given channel user mode.
	 */
	inline void removeMode( const modeType& whichMode )
		{ modes &= ~whichMode ; }

	inline void removeModeO()
		{ removeMode( MODE_O ) ; }

	inline void removeModeV()
		{ removeMode( MODE_V ) ; }

	inline void removeZombie()
		{ removeMode( ZOMBIE ) ; }

	/*
	 * These are not defined in the header file to avoid
	 * recursive includes.
	 */

	/**
	 * Retrieve this user's nick name.
	 * Note that this method will simply query this
	 * instance's iClient object for the nick name.
	 * Thus, if the user changes its nickname, this
	 * will be reflected automatically.
	 */
	inline const string& getNickName() const ;

	/**
	 * Retrieve this user's user name.
	 */
	inline const string& getUserName() const ;

	/**
	 * Retrieve this channel user's host name.
	 */
	inline const string& getHostName() const ;

	/**
	 * Retrieve this channel user's network numeric.
	 */
	inline const string getCharYYXXX() const ;

	/**
	 * Retrieve this channel user's IP.
	 */
	inline const unsigned int& getIP() const ;

	/**
	 * Retrieve this channel user's integer representation
	 * of its server numeric.
	 */
	inline const unsigned int& getIntYY() const ;

	/**
	 * Retrieve this channel user's integer representation
	 * of its client numeric.
	 */
	inline const unsigned int& getIntXXX() const ;

	/**
	 * Retrieve this channel user's integer representation
	 * of its network numeric.
	 * BUG: Do not use this method, it is still being tested.
	 */
	inline const unsigned int getIntYYXXX() const ;

	/**
	 * Return true if this client is an oper.
 	 */
	inline bool isOper() const ;

	/**
	 * Return a pointer to the iClient to which this ChannelUser
	 * is associated.
	 * Use of this method is strongly discouraged.
	 */
	inline iClient* getClient() const
		{ return theClient ; }

	/**
	 * Convenience operator method for outputting this
	 * ChannelUser's information to a C++ standard
	 * output stream.
	 */
	friend ELog& operator<<( ELog& out, const ChannelUser& rhs )
	{
	out	<< rhs.getNickName() << '!'
		<< rhs.getUserName() << '@'
		<< rhs.getHostName() << ' '
		<< rhs.getCharYYXXX() ;
	return out ;
	}

protected:

	/**
	 * The iClient to which this ChannelUser instance is associated.
	 */
	iClient*	theClient ;

	/**
	 * This channel user's modes in the current channel.
	 */
	modeType	modes ;

} ; 

} // namespace gnuworld

#endif // __CHANNELUSER_H
