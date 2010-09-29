/**
 * nickUserCidr.cc
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
 *			Orlando Bassotto
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
 * $Id: nickUserCidr.cc,v 1.2 2009/06/25 19:00:40 mrbean_ Exp $
 */

#include	<string>
#include	<sstream>

#include	<cstdio>
#include	<cstdlib>
#include	<cstring>

#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<netdb.h>
#include	<arpa/inet.h>


#include	"nickUserCidr.h"
#include 	"misc.h"
#include	"StringTokenizer.h"
#include	"gnuworld_config.h"
#include	"match.h"

RCSTAG( "$Id: nickUserCidr.cc,v 1.2 2009/06/25 19:00:40 mrbean_ Exp $" ) ;

namespace gnuworld
{

using std::string ;

xNickUserCIDR::xNickUserCIDR( const std::string& nickUserCidr ) :  cidr(""), nick("*"),user("*"),str("")
{
	StringTokenizer st = StringTokenizer(nickUserCidr,'@');
	if(st.size() == 1) //No @ , consider everything as a cidr
	{
		cidr = xCIDR(nickUserCidr);
		valid = cidr.GetValid();
	} else if(st.size() == 2) {
		ExtractNickUser(st[0]);
		cidr = xCIDR(st[1]);
		valid = cidr.GetValid();
		
	}
	str = nick + "!" + user + "@" + cidr.AsString();
	
}

bool xNickUserCIDR::ExtractNickUser(const std::string& nickUser)
{
	if(nickUser[0] == '!') //Starts with ! ? user = everything after the !
	{
		user = nickUser.substr(1);
		return true;
	}
	StringTokenizer st = StringTokenizer(nickUser,'!');
	if(st.size() == 1)
	{	//No ! user = everything
		user = st[0];
	} else {
		//We have ! - nick = up til !, user = rest
		nick = st[0];
		user = st.assemble(1);
	}
	return true;
}
bool xNickUserCIDR::matches(const std::string& IP)
{
	StringTokenizer st = StringTokenizer(IP,'@');
	if(st.size() == 1) 
	{	// WTF No @ ?! 
		return false;
	}
	if(!match(nick+"!"+user,st[0]))
	{ //User and nick match
		return cidr.matches(st[1]);
	}
	return false;
}
} // namespace gnuworld
