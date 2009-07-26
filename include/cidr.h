/**
 * cidr.h
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
 * $Id: cidr.h,v 1.4 2009/07/26 18:30:37 mrbean_ Exp $
 */

#ifndef __CIDR_H
#define __CIDR_H "$Id: cidr.h,v 1.4 2009/07/26 18:30:37 mrbean_ Exp $"

#include	<string>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<netdb.h>
#include	<arpa/inet.h>
#include	"ip.h"

namespace gnuworld
{

/**
 * A utility class that is used to retrieve information about
 * CIDR addresses.
 */
class xCIDR
{

public:

	/**
	 * Construct an xIP instance given an address in std::string
	 * format.
	 */
	xCIDR( const std::string& IP) ;

	/**
	 * Checks whether or not the CIDR is valid.
	 */
	inline const bool& GetValid() const 
		{ return valid; }
		
	/**
	 * Retrieve the mask of this CIDR (the part after the /).
	 */
	inline const int& GetMask() const 
		{ return mask; }

	/**
	 * Retrieve the prefix of this CIDR (the part before the /).
	 */
	inline const xIP& GetPrefix() const 
		{ return prefix; }

	inline const std::string& AsString() const
		{ return str; }
		
	bool matches(const std::string& IP) ;	
		
protected:

	/**
	 * Wheter this CIDR is valid.
	 */
	bool	valid ;
	
	/**
	 * The mask
	 */
	int mask ;
	
	std::string	str; 
	
	/**
	 * The prefix
	 */
	xIP prefix ;
	
	
	
private:
	
	bool ExtractMask(const std::string&);
	
	bool  ExtractPrefix(const std::string&,const unsigned int&,unsigned int&);

} ;

} // namespace gnuworld

#endif // __XCIDR_H
