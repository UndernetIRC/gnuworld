/**
 * ip.cc
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
 * $Id: ip.cc,v 1.11 2005/01/12 03:50:38 dan_karrels Exp $
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

#include	"ip.h"
#include	"Numeric.h"
#include	"gnuworld_config.h"
#include	"misc.h"

RCSTAG( "$Id: ip.cc,v 1.11 2005/01/12 03:50:38 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::string ;

xIP::xIP( const irc_in_addr& IP )
{
	this->IP = IP;
}

xIP::xIP( const xIP& rhs )
 : IP( rhs.IP )
{}

xIP::xIP( const string& IP, bool Base64 )
{
	if( Base64 )
	{
		base64toip(IP.c_str(), &this->IP);
		return ;
	}
//TODO: This is the ELSE part: When do we get here, if we even ever get, and HOW do we deal with this?!
/*
struct hostent* hp = ::gethostbyname( IP.c_str() ) ;
if( NULL == hp )
	{
	this->IP = ntohl( inet_addr( IP.c_str() ) ) ;
	}
else
	{
	struct
		{
		unsigned char a, b, c, d;
		} ip ;
	::memcpy( &ip, hp->h_addr, sizeof( unsigned int ) ) ;

	this->IP = ntohl( 
		((ip.a << 24) |
		(ip.b << 16) |
		(ip.c << 8) |
		ip.d) ) ;
	}
	*/
	//for now i just put a simply base64 again
	base64toip(IP.c_str(), &this->IP);
}

string xIP::GetNumericIP(bool fixedToCIDR64) const
{
	if (fixedToCIDR64)
		return fixToCIDR64(ircd_ntoa(&this->IP));
	else
		return ircd_ntoa(&this->IP);
}

const irc_in_addr& xIP::GetLongIP() const
{
	return IP ;
}

string xIP::GetBase64IP() const
{
	char ip_base64[25];	//64
	int IsIPv6 = 1;
	if (irc_in_addr_is_ipv4(&this->IP)) IsIPv6 = 0;
	return iptobase64(ip_base64, &this->IP, sizeof(ip_base64), IsIPv6);
}

unsigned int xIP::getIP32() const
{
	// 30 24 18 12 6
	if (!irc_in_addr_is_ipv4(&this->IP)) return 0;
	string StrIP32 = GetNumericIP();
	unsigned int IP32 = (convert2n[ static_cast< size_t >( StrIP32[ 0 ] ) ] << 30) +
		  (convert2n[ static_cast< size_t >( StrIP32[ 1 ] ) ] << 24) +
		  (convert2n[ static_cast< size_t >( StrIP32[ 2 ] ) ] << 18) +
		  (convert2n[ static_cast< size_t >( StrIP32[ 3 ] ) ] << 12) +
		  (convert2n[ static_cast< size_t >( StrIP32[ 4 ] ) ] << 6) +
		  (convert2n[ static_cast< size_t >( StrIP32[ 5 ] ) ] ) ;

	IP32 = ntohl( IP32 ) ;
	return IP32;
}
} // namespace gnuworld
