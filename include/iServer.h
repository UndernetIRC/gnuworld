/**
 * iServer.h
 * iServer objects represent servers across the network, except
 * for the GNUWorld server itself (see class xServer).
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
 * $Id: iServer.h,v 1.14 2003/12/29 23:59:36 dan_karrels Exp $
 */

#ifndef __ISERVER_H
#define __ISERVER_H "$Id: iServer.h,v 1.14 2003/12/29 23:59:36 dan_karrels Exp $"

#include	<iostream>
#include	<string>

#include	<ctime>

#include	"ELog.h"
#include	"NetworkTarget.h"

namespace gnuworld
{

class xServer ;
class xNetwork ;

/**
 * This class represents a network server.
 */
class iServer : public NetworkTarget
{
	/**
	 * Allow xServer to directly manipulate the
	 * internal state of this iServer.
	 */
	friend class xServer ;

	/**
	 * xNetwork needs access to setIntYY().
	 */
	friend class xNetwork ;

public:

	/// Type used to hold flags
	typedef unsigned int	flagType ;

	/// Set if this iServer is juped, false otherwise
	static const flagType	FLAG_JUPE ;

	/**
	 * Construct an iServer given its vital state variables
	 * as parameters.
	 */
	iServer( const unsigned int& _uplink,
		const std::string& _yyxxx,
		const std::string& _name,
		const time_t& _connectTime,
		const std::string& description = std::string() ) ;

	/**
	 * Destruct this iServer instance.
	 */
	virtual ~iServer() ;

	/* Accessor methods */

	/**
	 * Return the iServer's flags.
	 */
	inline const flagType& getFlags() const
		{ return flags ; }

	/**
	 * Return true if a particular flag is set, false otherwise.
	 */
	inline bool getFlag( const flagType& whichFlag ) const
		{ return ((flags & whichFlag) == whichFlag) ; }

	/**
	 * Set a particular flags.
	 */
	inline void setFlag( const flagType& whichFlag )
		{ flags |= whichFlag ; }

	/**
	 * Remove a particular flag.
	 */
	inline void removeFlag( const flagType& whichFlag )
		{ flags &= ~whichFlag ; }

	/**
	 * Return true if this server is a jupe.
	 */
	inline bool isJupe() const
		{ return getFlag( FLAG_JUPE ) ; }

	/**
	 * Set this iServer as a juped server.
	 */
	inline void setJupe()
		{ setFlag( FLAG_JUPE ) ; }

	/**
	 * Return the server numeric of this server's uplink.
	 */
	inline const unsigned int& getUplinkIntYY() const
		{ return uplinkIntYY ; }

	/**
	 * Return the name of this server.
	 */
	inline const std::string& getName() const
		{ return name ; }

	/**
	 * Return the time at which this server connected to the network.
	 */
	inline const time_t& getConnectTime() const
		{ return connectTime ; }

	/**
	 * Return the time at which this server started.
	 */
	inline const time_t& getStartTime() const
		{ return startTime ; }

	/**
	 * Return the description of this server.
	 */
	inline const std::string& getDescription() const
		{ return description ; }

	/**
	 * Return true if no BURST state exists, false otherwise.
	 */
	virtual bool isBursting() const
		{ return bursting ; }

	/**
	 * This method is used by xServer to signify that this
	 * iServer is now in the bursting state.
	 */
	virtual void	startBursting()
		{ bursting = true ; }

	/**
	 * This method is called by xServer to signify that this
	 * iServer has completed bursting.
	 */
	virtual void	stopBursting()
		{ bursting = false ; }

	/**
	 * Permit setting an arbitrary value to the bursting
	 * variable.
	 */
	virtual void	setBursting( bool newVal )
		{ bursting = newVal ; }

	/**
	 * This method is called by class xServer once the uplink
	 * of the xServer is known.  This method is only called
	 * for the single instance of the iServer for the core
	 * xServer.
	 */
	virtual void	setUplinkIntYY( const unsigned int& newYY )
		{ uplinkIntYY = newYY ; }

	/**
	 * Nice debugging method for outputting the iServer's
	 * information to an ELog stream.
	 */
	friend ELog& operator<<( ELog& out,
		const iServer& serv )
		{
		out     << "Name: " << serv.getName() << ' '
			<< "intYY: " << serv.getIntYY() << ' '
			<< "uplinkIntYY: "
			<< serv.getUplinkIntYY() << ' '
			<< "charYY: " << serv.getCharYY() ;
		return out ;
		}

	/**
	 * Nice debugging method for outputting the iServer's
	 * information to a standard c++ output stream.
	 */
	friend std::ostream& operator<<( std::ostream& out,
		const iServer& serv )
		{
		out     << "Name: " << serv.getName() << ' '
			<< "intYY: " << serv.getIntYY() << ' '
			<< "uplinkIntYY: "
			<< serv.getUplinkIntYY() << ' '
			<< "charYY: " << serv.getCharYY() ;
		return out ;
		}

protected:

	/**
	 * Allow friends to modify the iServer's description.  This
	 * is used in the case of adding this server as a juped
	 * server.
	 */
	inline void	setDescription( const std::string& newDescription )
		{ description = newDescription ; }

	/**
	 * Integer numeric of this server's uplink.
	 */
	unsigned int	uplinkIntYY ;

	/**
	 * Name of this server.
	 */
	std::string	name ;

	/**
	 * The time at which this server joined the network.
	 */
	time_t		connectTime ;

	/**
	 * The time at which this server was started.
	 */
	time_t		startTime ;

	/**
	 * The server's description field.
	 */
	std::string	description ;

	/**
	 * This variable is true when this server is bursting.
	 */
	bool		bursting ;

	/**
	 * This server's flags.
	 */
	flagType	flags ;

} ;

} // namespace gnuworld

#endif // __ISERVER_H
