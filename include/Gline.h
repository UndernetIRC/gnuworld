/**
 * Gline.h
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
 * $Id: Gline.h,v 1.7 2003/12/29 23:59:36 dan_karrels Exp $
 */

#ifndef __GLINE_H
#define __GLINE_H "$Id: Gline.h,v 1.7 2003/12/29 23:59:36 dan_karrels Exp $"

#include	<string>

#include	<ctime>

#include	"misc.h"

namespace gnuworld
{

/**
 * A simple data storage class which represents a network G-Line.
 */
class Gline
{

public:

	/**
	 * Construct a Gline instance given the nick!user@host of the
	 * user requesting it, the userHost being glined, th reason
	 * for the gline, and the duration (in seconds) of the
	 * gline.
	 */
	Gline( const std::string& _setBy,
		const std::string& _userHost,
		const std::string& _reason,
		const time_t& _duration ,
		const time_t& _lastmod = ::time( 0 ))
	: setBy( _setBy ),
	  userHost( _userHost ),
	  reason( _reason ),
	  expiration( _duration + ::time( 0 ) ),
	  lastmod( _lastmod )
	{}

	/**
	 * Copy constructor.
	 * Construct a Gline given another Gline.
	 */
	Gline( const Gline& rhs )
	: setBy( rhs.setBy ),
	  userHost( rhs.userHost ),
	  reason( rhs.reason ),
	  expiration( rhs.expiration ),
	  lastmod (rhs.lastmod) 
	{}

	/**
	 * Destroy this gline.  No heap space allocated.
	 */
	virtual ~Gline()
	{}

	/**
	 * Return true if this Gline is equivalent (case insensitive)
	 * to the given userHost mask.
	 * This does NOT perform a wildcard match.
	 */
	inline bool operator==( const std::string& _userHost ) const
		{ return (0 == strcasecmp( userHost, _userHost )) ; }

	/**
	 * Retrieve the nick!user@host of the user who set this gline.
	 */
	inline const std::string& getSetBy() const
		{ return setBy ; }

	/**
	 * Retrieve the user@host which has been glined.
	 */
	inline const std::string& getUserHost() const
		{ return userHost ; }

	/**
	 * Retrieve the reason for this gline.
	 */
	inline const std::string& getReason() const
		{ return reason ; }

	/**
	 * Retrieve the time at which this gline will expire.
	 */
	inline const time_t& getExpiration() const
		{ return expiration ; }

	/**
	 * Retrieve the time at which the gline was last modified
	 */
	inline const time_t& getLastmod() const
		{ return lastmod ; }

	/**
	 * Retrieve the user/server who set this gline.
	 */
	inline void setSetBy( const std::string& _setBy )
		{ setBy = _setBy; }

	/**
	 * Set the hostmask affected by this gline.
	 */
	inline void setHost( const std::string& _host )
		{ userHost = _host; }

	/**
	 * Set the reason for this gline.
	 */
	inline void setReason( const std::string& _reason )
		{ reason = _reason; }

	/**
	 * Set the expiration time on this gline.
	 */
	inline void setExpiration( const time_t _expiration )
		{ expiration = _expiration; }

	/**
	 * Set the lastmod time
	 */
	inline void setLastmod( const time_t _lastmod )
		{ lastmod = _lastmod; }

	/**
	 * Convenience operator method which allows the output
	 * of a Gline instance to a C++ standard output stream.
	 */
	friend ELog& operator<<( ELog& out, const Gline& rhs )
	{
	out	<< "Mask: " << rhs.userHost << ' '
		<< "Reason: " << rhs.reason << ' '
		<< "Set by: " << rhs.setBy << ' '
		<< "Expires: " << rhs.expiration << ' '
		<< "Last modified: " << rhs.lastmod ;
	return out ;
	}
	
protected:

	/**
	 * The nick!user@host of the user who set this gline.
	 */
	std::string	setBy ;

	/**
	 * The banned user@host.
	 */
	std::string	userHost ;

	/**
	 * The reason this gline was set.
	 */
	std::string	reason ;

	/**
	 * The time at which this gline expires.
	 */
	time_t		expiration ;

	/**
	 * The time at which this gline was last modified
	 */
	time_t		lastmod ;

} ;

} // namespace gnuworld

#endif // __GLINE_H
