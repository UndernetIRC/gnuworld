/* ip.cc
 */

#include	<string>
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

const char xIP_h_rcsId[] = __IP_H ;
const char xIP_cc_rcsId[] = "$Id: ip.cc,v 1.4 2001/03/31 01:26:10 dan_karrels Exp $" ;
const char Numeric_h_rcsId[] = __NUMERIC_H ;

namespace gnuworld
{

using std::string ;

xIP::xIP( const unsigned int& IP )
{
this->IP = htonl( IP ) ;
}

xIP::xIP( const unsigned int& a,
	const unsigned int& b,
	const unsigned int& c,
	const unsigned int& d )
{
IP = htonl( (a << 24) | (b << 16) | (c << 8) | d ) ;
}

xIP::xIP( const xIP& rhs )
 : IP( rhs.IP )
{}

xIP::xIP( const string& IP, bool Base64 )
{
if( Base64 )
	{
	// TOOD: ntohl()?
	// 30 24 18 12 6
	this->IP = (convert2n[ IP[ 0 ] ] << 30) +
		  (convert2n[ IP[ 1 ] ] << 24) +
		  (convert2n[ IP[ 2 ] ] << 18) +
		  (convert2n[ IP[ 3 ] ] << 12) +
		  (convert2n[ IP[ 4 ] ] << 6) +
		  (convert2n[ IP[ 5 ] ] ) ;
	return ;
	}

struct hostent* hp = ::gethostbyname( IP.c_str() ) ;
if( NULL == hp )
	{
	this->IP = htonl( inet_addr( IP.c_str() ) ) ;
	}
else
	{
	struct
		{
		unsigned char a, b, c, d;
		} ip ;
	::memcpy( &ip, hp->h_addr, sizeof( unsigned int ) ) ;
	this->IP = htonl( ((ip.a << 24) | (ip.b << 16) | (ip.c << 8) | ip.d) ) ;
	} // close else

}

const char* xIP::GetIP() const
{
struct hostent* hp = ::gethostbyaddr(
	reinterpret_cast< const char* >( &IP ),
	sizeof( unsigned int ), AF_INET ) ;

return (hp ? hp->h_name : GetNumericIP()) ;
}

const char* xIP::GetNumericIP() const
{
static char buf[ 16 ] ;

sprintf( buf, "%d.%d.%d.%d",
	static_cast< int >( (IP >> 24) & 0xff ),
	static_cast< int >( (IP >> 16) & 0xff ),
	static_cast< int >( (IP >>  8) & 0xff ),
	static_cast< int >( (IP & 0xff) ) ) ;
return buf ;
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

const char* xIP::GetBase64IP() const
{
static char buf[ 128 ] ;

sprintf( buf, "%c%c%c%c%c%c",
	convert2y[ ((IP >> 30) & 0x3f) ],
	convert2y[ ((IP >> 24) & 0x3f) ],
	convert2y[ ((IP >> 18) & 0x3f) ],
	convert2y[ ((IP >> 12) & 0x3f) ],
	convert2y[ ((IP >> 6 ) & 0x3f) ],
	convert2y[ (IP & 0x3f) ] ) ;

return buf ;
}

} // namespace gnuworld
