/* ip.h
 * This stuff should probably be moved to a static class.
 * xIP provides mostly utilitarian functionality for IP
 * addresses.
 * This class originally created by Orlando Bassotto.
 */

#ifndef __XIP_H
#define __XIP_H "$Id: ip.h,v 1.1 2000/06/30 18:46:06 dan_karrels Exp $"

#include	<string>

using std::string ;

namespace gnuworld
{

/**
 * A utility class that is used to retrieve information about
 * IP addresses.
 */
class xIP
{

public:

	/**
	 * Construct an xIP instance given an address in std::string
	 * format.
	 */
	xIP( const string& IP, bool Base64 = false ) ;

	/**
 	 * Construct an xIP instance given an IP in long int format.
	 */
	xIP( const long int& IP ) ;

	/**
	 * Construct an xIP instance given the four octets of
	 * an IP address.
	 */
	xIP( const int& a, const int& b,
		const int& c, const int& d ) ;

	/**
	 * Copy constructor.
	 */
	xIP( const xIP& IP ) ;

	/**
	 * Destruct this xIP instance.  No heap space is allocated.
	 */
	~xIP() {}

	/**
	 * Retrieve the IP in character array format.
	 */
	const char* GetIP() const ;

	/**
	 * Retrieve the IP in xxx.xxx.xxx.xxx character array format.
	 */
	const char* GetNumericIP() const ;

	/**
	 * Retrieve the IP as a long int.
	 */
	const long int& GetLongIP() const ;

	/**
	 * Place into the four variables passed to this method the
	 * four octets of the IP.
	 */
	void GetIP( int& a, int& b, int& c, int& d ) const ;

	/**
	 * Return a character array representation of the base 64
	 * IP.
	 */
	const char* GetBase64IP() const ;

protected:

	/**
	 * The IP number itself.
	 */
	long int	IP ;

} ;

} // namespace gnuworld

#endif // __XIP_H
