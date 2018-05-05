/**
 * ChannelUser.h
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
 * $Id: ChannelUser.h,v 1.16 2009/07/26 18:30:37 mrbean_ Exp $
 */

#ifndef __CHANNELUSER_H
#define __CHANNELUSER_H "$Id: ChannelUser.h,v 1.16 2009/07/26 18:30:37 mrbean_ Exp $"

#include	<string>

#include	"Numeric.h"

#include	"ELog.h"

namespace gnuworld
{

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
	static const modeType	MODE_O ;

	/// Bit representing channel user mode +v
	static const modeType	MODE_V ;

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

	/**
	 * Return true if this user has mode +o on this channel,
	 * false otherwise.
	 */
	inline bool isModeO() const
		{ return getMode( MODE_O ) ; }

	/**
	 * Return true if this user has mode +v on this channel,
	 * false otherwise.
	 */
	inline bool isModeV() const
		{ return getMode( MODE_V ) ; }

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

	/**
	 * Set the user's mode +o state in this channel.
	 */
	inline void setModeO()
		{ setMode( MODE_O ) ; }

	/**
	 * Set the user's mode +v state in this channel.
	 */
	inline void setModeV()
		{ return setMode( MODE_V ) ; }

	/**
	 * Remove a given channel user mode.
	 */
	inline void removeMode( const modeType& whichMode )
		{ modes &= ~whichMode ; }

	/**
	 * Remove the user's mode +o state in this channel.
	 */
	inline void removeModeO()
		{ removeMode( MODE_O ) ; }

	/**
	 * Remove the user's mode +v state in this channel.
	 */
	inline void removeModeV()
		{ removeMode( MODE_V ) ; }

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
	const std::string& getNickName() const ;

	/**
	 * Retrieve this user's user name.
	 */
	const std::string& getUserName() const ;

	/**
	 * Retrieve this channel user's host name.
	 */
	const std::string& getHostName() const ;

	/**
	 * Retrieve this channel user's network numeric.
	 */
	const std::string getCharYYXXX() const ;

	/**
	 * Retrieve this channel user's IP.
	 */
	const irc_in_addr& getIP() const ;

	/**
	 * Retrieve this channel user's integer representation
	 * of its server numeric.
	 */
	unsigned int getIntYY() const ;

	/**
	 * Retrieve this channel user's integer representation
	 * of its client numeric.
	 */
	unsigned int getIntXXX() const ;

	/**
	 * Retrieve this channel user's integer representation
	 * of its network numeric.
	 */
	unsigned int getIntYYXXX() const ;

	/**
	 * Return true if this client is an oper.
 	 */
	bool isOper() const ;

	/**
	 * Return a pointer to the iClient to which this ChannelUser
	 * is associated.
	 * Use of this method is strongly discouraged.
	 */
	inline iClient* getClient() const
		{ return theClient ; }

	/**
	 * Return a human readable string of modes this user has
	 * in the channel.
	 * Valid modes are:
	 *  - "op"
	 *  - "voice"
	 *  - "oper"
	 */
	std::string	getModeString() const ;

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
		<< rhs.getCharYYXXX()
		<< " user modes: "
		<< rhs.getModeString() ;
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
