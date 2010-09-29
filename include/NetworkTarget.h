/**
 * NetworkTarget.h
 * Author: Daniel Karrels (dan@karrels.com)
 *
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
 * $Id: NetworkTarget.h,v 1.2 2003/12/29 23:59:36 dan_karrels Exp $
 */

#ifndef __NETWORKTARGET_H
#define __NETWORKTARGET_H

#include	<string>

#include	"Numeric.h"

namespace gnuworld
{

class xNetwork ;

/**
 * This class is basically a convenience class for addressing
 * targets according to the ircu network protocol.  That is,
 * this class supports integer and string numerics.
 */
class NetworkTarget
{
	/// Allow xNetwork to call the protected mutator methods
	friend class xNetwork ;

private:

	/**
	 * The integer base 64 representation of this target's
	 * server.
	 */
	unsigned int		intYY ;

	/**
	 * The integer base 64 representation of this target's
	 * client ID (relative to its server).
	 */
	unsigned int		intXXX ;

	/**
	 * The char array base 64 representation of this target's
	 * server.
	 */
	char			charYY[ 3 ] ;

	/**
	 * The char array base 64 representation of this target's
	 * client ID (relative to its server).
	 */
	char			charXXX[ 4 ] ;

public:
	/**
	 * Instantiate a NetworkTarget with no arguments.
	 * All variables will be set to 0.
	 */
	NetworkTarget()
	: intYY( 0 ),
	  intXXX( 0 )
	{
	charYY[ 0 ] = charYY[ 1 ] = charYY[ 2 ] = 0 ;
	charXXX[ 0 ] = charXXX[ 1 ] = charXXX[ 2 ] = charXXX[ 3 ] = 0 ;
	}

	/**
	 * Instantiate a NetworkTarget given its base 64 integer
	 * numerics.
	 */
	NetworkTarget( const unsigned int& intYY,
		const unsigned int& intXXX )
	{
		setIntYY( intYY ) ;
		setIntXXX( intXXX ) ;
	}

	/**
	 * Instantiate a NetworkTarget given its base 64 char array
	 * numerics.
	 */
	NetworkTarget( const std::string& stringYYXXX )
	{
		setCharYY( stringYYXXX.substr( 0, 2 ) ) ;
		setCharXXX( stringYYXXX.substr( 2, 3 ) ) ;
	}

	/**
	 * Destroy this NetworkTarget object.
	 */
	virtual ~NetworkTarget()
	{}

	/**
	 * Return the integer base 64 representation of this
	 * target's server.
	 */
	inline unsigned int	getIntYY() const
		{ return intYY ; }

	/**
	 * Return the integer base 64 representation of this
	 * target's client identifier (relative to its server).
	 */
	inline unsigned int	getIntXXX() const
		{ return intXXX ; }

	/**
	 * Return the integer base 64 representation of this
	 * target's full client/server numeric.
	 */
	inline unsigned int	getIntYYXXX() const
		{ return combinebase64int( intYY, intXXX ) ; }

	/**
	 * Return the char array base 64 representation of this
	 * target's server.
	 */
	inline const std::string	getCharYY() const
		{ return std::string( charYY ) ; }

	/**
	 * Return the char array base 64 representation of this
	 * target's client identifier (relative to its server).
	 */
	inline const std::string	getCharXXX() const
		{ return std::string( charXXX ) ; }

	/**
	 * Return the char array base 64 representation of this
	 * target's full client/server numeric.
	 */
	inline const std::string	getCharYYXXX() const
		{ return (getCharYY() + getCharXXX()) ; }

protected:

	/**
	 * Set the integer base 64 representation of this
	 * target's server.
	 */
	inline void		setIntYY( unsigned int newIntYY )
	{
		intYY = newIntYY ;
		inttobase64( charYY, intYY, 2 ) ;
		charYY[ 2 ] = 0 ;
	}

	/**
	 * Set the integer base 64 representation of this
	 * target's client identifier (relative to its server).
	 */
	inline void		setIntXXX( unsigned int newIntXXX )
	{
		intXXX = newIntXXX ;
		inttobase64( charXXX, intXXX, 3 ) ;
		charXXX[ 3 ] = 0 ;
	}

	/**
	 * Set the char array base 64 representation of this
	 * target's server.
	 */
	inline void		setCharYY( const std::string& stringYY )
	{
		charYY[ 0 ] = stringYY[ 0 ] ;
		charYY[ 1 ] = stringYY[ 1 ] ;
		charYY[ 2 ] = 0 ;
		intYY = base64toint( stringYY.c_str(), 2 ) ;
	}

	/**
	 * Set the char array base 64 representation of this
	 * target's client identifier (relative to its server).
	 */
	inline void		setCharXXX( const std::string& stringXXX )
	{
		charXXX[ 0 ] = stringXXX[ 0 ] ;
		charXXX[ 1 ] = stringXXX[ 1 ] ;
		charXXX[ 2 ] = stringXXX[ 2 ] ;
		charXXX[ 3 ] = 0 ;
		intXXX = base64toint( stringXXX.c_str(), 3 ) ;
	}

} ;

} // namespace gnuworld

#endif // __NETWORKTARGET_H
