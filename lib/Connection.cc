/**
 * Connection.cc
 * Author: Daniel Karrels (dan@karrels.com)
 * $Id: Connection.cc,v 1.3 2002/05/28 20:25:48 dan_karrels Exp $
 */

#include	<sys/types.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<sys/socket.h>
#include	<netdb.h>

#include	<string>

#include	<cstring>

#include	"Connection.h"
#include	"Buffer.h"

const Connection::flagType	Connection::F_PENDING = 0x01 ;
const Connection::flagType	Connection::F_CONNECTED = 0x02 ;
const Connection::flagType	Connection::F_INCOMING = 0x04 ;
const Connection::flagType	Connection::F_LISTEN = 0x08 ;

using std::string ;

// Simply initialize the object
Connection::Connection( const string& _hostname,
	const unsigned short int _port,
	const bool _TCP,
	const char _delimiter )
: hostname( _hostname ),
	port( _port ),
	TCP( _TCP ),
	inputBuffer( string(), _delimiter ),
	outputBuffer( string(), _delimiter ),
	IP( string() ),
	sockFD( -1 ),
	flags( F_PENDING )
{
memset( &addr, 0, sizeof( struct sockaddr_in ) ) ;
}

Connection::Connection( const char _delimiter )
: port( 0 ),
	TCP( true ),
	inputBuffer( string(), _delimiter ),
	outputBuffer( string(), _delimiter ),
	sockFD( -1 ),
	flags( F_PENDING )
{
memset( &addr, 0, sizeof( struct sockaddr_in ) ) ;
}

Connection::~Connection()
{
/* No work to be done, no heap space allocated */
}

