/* iServer.h
 * iServer objects represent servers across the network, except
 * for the EWorld server itself (see class xServer).
 */

#ifndef __ISERVER_H
#define __ISERVER_H "$Id: iServer.h,v 1.1 2000/06/30 18:46:06 dan_karrels Exp $"

#include	<iostream>
#include	<string>

#include	<ctime>

#include	"Numeric.h"
#include	"ELog.h"

using std::string ;

namespace gnuworld
{

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
	iServer( const int& _uplink,
		const string& _yxx,
		const string& _name,
		const time_t& _connectTime,
		const time_t& _startTime,
		const int& _version ) ;

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
	 * Return the version of this server.
	 */
	inline const int& getVersion() const
		{ return version ; }

	/**
	 * Return the IP of this server.
	 */
	inline const long int& getIP() const
		{ return IP ; }

	/**
	 * Return the number of clients that this server
	 * holds.
	 */
	inline const size_t& getClients() const
		{ return clients ; }

	/**
	 * Return the number of leaf servers this server holds.
	 */
	inline const size_t& getServers() const
		{ return servers ; }

	/**
	 * Increment the number of clients this server holds.
	 */
	inline void incrementClients()
		{ clients++ ; }

	/**
	 * Decrement the number of clients this server holds.
	 */
	inline void decrementClients()
		{ clients-- ; }

	/**
	 * Increment the number of servers this server holds.
	 */
	inline void incrementServers()
		{ servers++ ; }

	/**
	 * Decrement the number of servers this server holds.
	 */
	inline void decrementServers()
		{ servers-- ; }

	/**
	 * Set the number of leaf servers this server holds to
	 * the given number.
	 */
	inline void setServers( const size_t& _servers )
		{ servers = _servers ; }

	/**
	 * Set the number of clients this server holds to the
	 * given number.
	 */
	inline void setClients( const size_t& _clients )
		{ clients = _clients ; }

	/**
	 * Nice debugging method for outputting the iServer's
	 * information to a C++ output stream.
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

protected:

	// Do not alter the order of these declarations

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
	 * The version number of this server.
	 */
	int		version ;

	/**
	 * Number of clients attached to this server.
	 */
	size_t		clients ;

	/**
	 * Number of servers attached to this server.
	 */
	size_t		servers ;

	/**
	 * The IP of this server.
	 */
	long int	IP ;

	/**
	 * This server's integer numeric.
	 */
	unsigned int	intYY ;

	/**
	 * This server's character array numeric.
	 */
	char		charYY[ 3 ] ;

} ;

} // namespace gnuworld

#endif // __ISERVER_H
