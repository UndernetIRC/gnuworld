/* Gline.h */

#ifndef __GLINE_H
#define __GLINE_H "$Id: Gline.h,v 1.4 2002/04/27 14:54:49 dan_karrels Exp $"

#include	<string>
#include	<iostream>

#include	<ctime>

#include	"misc.h"

namespace gnuworld
{

using std::string ;

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
	Gline( const string& _setBy,
		const string& _userHost,
		const string& _reason,
		const time_t& _duration )
	: setBy( _setBy ),
	  userHost( _userHost ),
	  reason( _reason ),
	  expiration( _duration + ::time( 0 ) )
	{}

	/**
	 * Copy constructor.
	 * Construct a Gline given another Gline.
	 */
	Gline( const Gline& rhs )
	: setBy( rhs.setBy ),
	  userHost( rhs.userHost ),
	  reason( rhs.reason ),
	  expiration( rhs.expiration )
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
	inline bool operator==( const string& _userHost ) const
		{ return (0 == strcasecmp( userHost, _userHost )) ; }

	/**
	 * Retrieve the nick!user@host of the user who set this gline.
	 */
	inline const string& getSetBy() const
		{ return setBy ; }

	/**
	 * Retrieve the user@host which has been glined.
	 */
	inline const string& getUserHost() const
		{ return userHost ; }

	/**
	 * Retrieve the reason for this gline.
	 */
	inline const string& getReason() const
		{ return reason ; }

	/**
	 * Retrieve the time at which this gline will expire.
	 */
	inline const time_t& getExpiration() const
		{ return expiration ; }

	/**
	 * Retrieve the user/server who set this gline.
	 */
	inline void setSetBy( const string& _setBy )
		{ setBy = _setBy; }

	/**
	 * Set the hostmask affected by this gline.
	 */
	inline void setHost( const string& _host )
		{ userHost = _host; }

	/**
	 * Set the reason for this gline.
	 */
	inline void setReason( const string& _reason )
		{ reason = _reason; }

	/**
	 * Set the expiration time on this gline.
	 */
	inline void setExpiration( const time_t _expiration )
		{ expiration = _expiration; }

	/**
	 * Convenience operator method which allows the output
	 * of a Gline instance to a C++ standard output stream.
	 */
	friend ELog& operator<<( ELog& out, const Gline& rhs )
	{
	out	<< "Mask: " << rhs.userHost << ' '
		<< "Reason: " << rhs.reason << ' '
		<< "Set by: " << rhs.setBy << ' '
		<< "Expires: " << rhs.expiration ;
	return out ;
	}
	
protected:

	/**
	 * The nick!user@host of the user who set this gline.
	 */
	string		setBy ;

	/**
	 * The banned user@host.
	 */
	string		userHost ;

	/**
	 * The reason this gline was set.
	 */
	string		reason ;

	/**
	 * The time at which this gline expires.
	 */
	time_t		expiration ;

} ;

} // namespace gnuworld

#endif // __GLINE_H
