/**
 * nickUserCidr.h
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
 * $Id: nickUserCidr.h,v 1.3 2009/07/26 18:30:37 mrbean_ Exp $
 */

#ifndef __NICK_USER_CIDR_H
#define __NICK_USER_CIDR_H "$Id: nickUserCidr.h,v 1.3 2009/07/26 18:30:37 mrbean_ Exp $"

#include	<string>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<netdb.h>
#include	<arpa/inet.h>
#include	"cidr.h"

namespace gnuworld
{

/**
 * A utility class that is used to retrieve information about
 * CIDR addresses.
 */
class xNickUserCIDR
{

public:

	/**
	 * Construct an xNickUserCIDR instance given an address in std::string
	 * format.
	 */
	xNickUserCIDR( const std::string& nickUserCIDR) ;

	/**
	 * Checks whether or not the CIDR is valid.
	 */
	inline const bool& GetValid() const 
		{ return valid; }
		
	/**
	 * Retrieve the mask of this CIDR (the part after the /).
	 */
	inline int GetMask() const 
		{ return cidr.GetMask(); }

	/**
	 * Retrieve the prefix of this CIDR (the part before the /).
	 */
	inline const xIP& GetPrefix() const 
		{ return cidr.GetPrefix(); }

	/**
	 * Retrieve the nick 
	 */
	inline const std::string& GetNick() const 
		{ return nick; }

	/**
	 * Retrieve the user
	 */
	inline const std::string& GetUser() const 
		{ return user; }
	
	inline const std::string& AsString() const
		{ return str; }

		
	bool matches(const std::string& IP) ;	
		
protected:

	
	/**
	 * CIDR part 
	 */
	xCIDR cidr;
	
	/**
	 * The nick
	 */
	std::string nick ;
	
	/**
	 * user
	 */
	std::string	user ;
	
	std::string	str ;
	
	bool valid;
private:
	
	bool ExtractNickUser(const std::string&);

} ;

} // namespace gnuworld

#endif 
