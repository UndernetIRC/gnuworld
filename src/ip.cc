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

RCSTAG( "$Id: ip.cc,v 1.11 2005/01/12 03:50:38 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::string ;

xIP::xIP( const unsigned int& IP )
{
this->IP = ntohl( IP ) ;
}

xIP::xIP( const unsigned int& a,
	const unsigned int& b,
	const unsigned int& c,
	const unsigned int& d )
{
IP = ntohl( (a << 24) | (b << 16) | (c << 8) | d ) ;
}

xIP::xIP( const xIP& rhs )
 : IP( rhs.IP )
{}

xIP::xIP( const string& IP, bool Base64 )
{
if( Base64 )
	{
	// 30 24 18 12 6
	this->IP = (convert2n[ static_cast< size_t >( IP[ 0 ] ) ] << 30) +
		  (convert2n[ static_cast< size_t >( IP[ 1 ] ) ] << 24) +
		  (convert2n[ static_cast< size_t >( IP[ 2 ] ) ] << 18) +
		  (convert2n[ static_cast< size_t >( IP[ 3 ] ) ] << 12) +
		  (convert2n[ static_cast< size_t >( IP[ 4 ] ) ] << 6) +
		  (convert2n[ static_cast< size_t >( IP[ 5 ] ) ] ) ;

	this->IP = ntohl( this->IP ) ;
	return ;
	}

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

}

string xIP::GetIP() const
{
struct hostent* hp = ::gethostbyaddr(
	reinterpret_cast< const char* >( &IP ),
	sizeof( unsigned int ), AF_INET ) ;

return (hp ? string( hp->h_name ) : GetNumericIP()) ;
}

string xIP::GetNumericIP() const
{
std::stringstream s ;
s	<< static_cast< int >( (IP >> 24) & 0xff ) << '.'
	<< static_cast< int >( (IP >> 16) & 0xff ) << '.'
	<< static_cast< int >( (IP >>  8) & 0xff ) << '.'
	<< static_cast< int >( (IP & 0xff) ) ;
return s.str() ;
}

const unsigned int& xIP::GetLongIP() const
{
return IP ;
}

void xIP::GetIP( unsigned int& a,
	unsigned int& b,
	unsigned int& c,
	unsigned int& d ) const
{
a = (IP >> 24) & 0xff ;
b = (IP >> 16) & 0xff ;
c = (IP >>  8) & 0xff ;
d = IP & 0xff ;
}

string xIP::GetBase64IP() const
{
std::stringstream s ;
s	<< convert2y[ ((IP >> 30) & 0x3f) ]
	<< convert2y[ ((IP >> 24) & 0x3f) ]
	<< convert2y[ ((IP >> 18) & 0x3f) ]
	<< convert2y[ ((IP >> 12) & 0x3f) ]
	<< convert2y[ ((IP >> 6 ) & 0x3f) ]
	<< convert2y[ (IP & 0x3f) ] ;

return s.str() ;
}

} // namespace gnuworld
