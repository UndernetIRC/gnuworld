/* iServer.h
 * iServer objects represent servers across the network, except
 * for the GNUWorld server itself (see class xServer).
 */

#ifndef __ISERVER_H
#define __ISERVER_H "$Id: iServer.h,v 1.8 2001/08/25 18:07:14 dan_karrels Exp $"

#include	<iostream>
#include	<string>

#include	<ctime>

#include	"Numeric.h"
#include	"ELog.h"

namespace gnuworld
{

using std::string ;

/**
 * This class represents a network server.
 */
class iServer
{

	/**
	 * Allow xServer to directly manipulate the
	 * internal state of this iServer.
	 */
	friend class xServer ;

public:

	/**
	 * Construct an iServer given its vital state variables
	 * as parameters.
	 */
	iServer( const unsigned int& _uplink,
		const string& _yxx,
		const string& _name,
		const time_t& _connectTime ) ;

	/**
	 * Destruct this iServer instance.
	 */
	virtual ~iServer() ;

	/* Accessor methods */

	/**
	 * Return the integer representation of this iServer's uplink
	 * server.
	 */
	inline const unsigned int& getUplinkIntYY() const
		{ return uplinkIntYY ; }

	/**
	 * Return the integer representation of this iServer's network
	 * numeric.
	 */
	inline const unsigned int& getIntYY() const
		{ return intYY ; }

	/**
	 * Return the integer representation of this iServer's network
	 * max client numeric.
	 */
	inline const unsigned int& getIntXXX() const
		{ return intXXX ; }

	/**
	 * Return the character array representation of this iServer's
	 * network numeric.
	 */
	inline const char* getCharYY() const
		{ return charYY ; }

	/**
	 * Return the name of this server.
	 */
	inline const string& getName() const
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
	 * Return true if no BURST state exists, false otherwise.
	 */
	virtual bool isBursting() const
		{ return bursting ; }

	/**
	 * Nice debugging method for outputting the iServer's
	 * information to an ELog stream.
	 */
	friend ELog& operator<<( ELog& out,
		const iServer& serv )
		{
		out     << "Name: " << serv.name << ' '
			<< "intYY: " << serv.intYY << ' '
			<< "uplinkIntYY: " << serv.uplinkIntYY << ' '
			<< "charYY: " << serv.charYY ;
		return out ;
		}

	/**
	 * Nice debugging method for outputting the iServer's
	 * information to an ELog stream.
	 */
	friend std::ostream& operator<<( std::ostream& out,
		const iServer& serv )
		{
		out     << "Name: " << serv.name << ' '
			<< "intYY: " << serv.intYY << ' '
			<< "uplinkIntYY: " << serv.uplinkIntYY << ' '
			<< "charYY: " << serv.charYY ;
		return out ;
		}

protected:

	/**
	 * This method is called by class xServer once the uplink
	 * of the xServer is known.  This method is only called
	 * for the single instance of the iServer for the core
	 * xServer.
	 */
	virtual void	setUplinkIntYY( const unsigned int& newYY )
		{ uplinkIntYY = newYY ; }

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
	 * Integer numeric of this server's uplink.
	 */
	unsigned int	uplinkIntYY ;

	/**
	 * Name of this server.
	 */
	string		name ;

	/**
	 * The time at which this server joined the network.
	 */
	time_t		connectTime ;

	/**
	 * The time at which this server was started.
	 */
	time_t		startTime ;

	/**
	 * This server's integer numeric.
	 */
	unsigned int	intYY ;

	/**
	 * This server's integer max number of clients.
	 */
	unsigned int	intXXX ;

	/**
	 * This server's character array numeric.
	 */
	char		charYY[ 3 ] ;

	/**
	 * This variable is true when this server is bursting.
	 */
	bool		bursting ;

} ;

} // namespace gnuworld

#endif // __ISERVER_H
