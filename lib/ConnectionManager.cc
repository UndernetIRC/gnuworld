/**
 * ConnectionManager.cc
 * Author: Daniel Karrels (dan@karrels.com)
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
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
 * $Id: ConnectionManager.cc,v 1.9 2002/06/06 14:34:36 dan_karrels Exp $
 */

#include	<unistd.h>
#include	<fcntl.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<sys/ioctl.h>
#include	<sys/time.h>
#include	<netinet/tcp.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<netdb.h>
#include	<pthread.h>

#include	<new>
#include	<map>
#include	<string>
#include	<sstream>
#include	<iostream>
#include	<algorithm>

#include	<ctime>
#include	<cctype>
#include	<cstdlib>
#include	<cstring>
#include	<cstdio>
#include	<cassert>
#include	<cerrno>

#include	"ConnectionManager.h"
#include	"Connection.h"
#include	"Buffer.h"

const char ConnectionManager_h_rcsId[] = __CONNECTIONMANAGER_H ;
const char ConnectionManager_cc_rcsId[] = "$Id: ConnectionManager.cc,v 1.9 2002/06/06 14:34:36 dan_karrels Exp $" ;
const char Connection_h_rcsId[] = __CONNECTION_H ;
const char Buffer_h_rcsId[] = __BUFFER_H ;

namespace gnuworld
{

using std::cout ;
using std::endl ;
using std::string ;
using std::map ;
using std::nothrow ;
using std::stringstream ;
using gnuworld::Buffer ;

ConnectionManager::ConnectionManager( const time_t defaultTimeoutDuration,
	const char defaultDelimiter )
:	timeoutDuration( defaultTimeoutDuration ),
	delimiter( defaultDelimiter )
{
::pthread_cond_init( &condWait, 0 ) ;
::pthread_mutex_init( &condMutex, 0 ) ;
}

ConnectionManager::~ConnectionManager()
{
// Just be clean about it and clear the eraseMap now
eraseMap.clear() ;

// Iterate through the list of currently active/pending
// Connections
for( connectionMapIterator connectionItr = connectionMap.begin() ;
	connectionItr != connectionMap.end() ; ++connectionItr )
	{
	// Convenience variable
	// Obtain a pointer to the Connection object
	Connection* connectionPtr = *connectionItr ;

	// Close this Connection's socket
	closeSocket( connectionPtr->getSockFD() ) ;

	// Deallocate the space for this Connection object
	delete connectionPtr ;
	}

// Clean up, even if not strictly necessary, it's good habit
connectionMap.clear() ;

// No threads waiting on this condition
::pthread_cond_destroy( &condWait ) ;
}

Connection* ConnectionManager::Connect( const string& host,
	const unsigned short int remotePort )
{

// An empty host is invalid
if( host.empty() )
	{
	return 0 ;
	}

cout	<< "Connect> "
	<< host
	<< ":"
	<< remotePort
	<< endl ;

// Allocate a new Connection object
Connection* newConnection = new (nothrow)
	Connection( host, remotePort, delimiter ) ;
assert( newConnection != 0 ) ;

// Set the absolute time for this Connection's timeout to occur
newConnection->setAbsTimeout( time( 0 ) + timeoutDuration ) ;

// The new connection will automatically set itself to the pending state

// Check if the hostname given is a hostname or numbers and dots
if( isIpAddress( host ) )
	{
	// Numbers and dots
	newConnection->setIP( host ) ;
	}
else
	{
	// Canonical host name
	newConnection->setIP( ipAddrOf( host ) ) ;

	// Make sure that lookup was successful
	// If it fails, ipAddrOf() returns an empty
	// string, and inet_addr() will crash
	if( newConnection->getIP().empty() )
		{
		// No good
		delete newConnection ; newConnection = 0 ;

		cout	<< "ConnectionManager::Connect> Unable "
			<< "to find valid IP for "
			<< host
			<< endl ;

		return 0 ;
		}
	} // else()

// Open a non-blocking socket
int sockFD = openSocket() ;
if( -1 == sockFD )
	{
	delete newConnection ;

	cout	<< "Connect> openSocket() failed"
		<< endl ;

	return 0 ;
	}

// Setup some basic options
struct sockaddr_in* addr = newConnection->getAddr() ;
addr->sin_family = AF_INET ;
addr->sin_port = htons( static_cast< u_short >( remotePort ) ) ;
addr->sin_addr.s_addr = inet_addr( newConnection->getIP().c_str() ) ;

// Update the new Connection object
newConnection->setSockFD( sockFD ) ;

// Attempt to initiate the connect.
// The socket is non-blocking, so a failure is expected
// In the case of a UDP socket, this call to ::connect() will just
// fill the socket address structure with relevant information
if( ::connect( sockFD, reinterpret_cast< sockaddr* >( addr ),
	sizeof( struct sockaddr_in ) ) < 0 )
	{
	// Let's check what the error was
	switch( errno )
		{
		case EISCONN:
			// EISCONN means the connection is successful
			// This will still work fine when another
			// connect() is attempted in Poll()
			break ;

		// EINPROGRESS means that the socket is nonblocking and
		// the connection attempt is in progress
		case EINPROGRESS:
			break ;

		// All other errors are failures
		default:
			// Close the socket
			closeSocket( newConnection->getSockFD() ) ;

			// Deallocate the Connection object and
			// set it to NULL for return to caller
			delete newConnection ;
			newConnection = 0 ;

			cout	<< "ConnectionManager::Connect> Error "
				<< "detected in connect(): "
				<< strerror( errno )
				<< endl ;

			// No need to call OnConnectFail() here since
			// this method will return a NULL Connection
			// to the caller, which indicates that the
			// attempt failed.
			break ;
		} // switch()
	} // if()

if( 0 == newConnection )
	{
	return 0 ;
	}

//	cout	<< "Connect> Adding new connection attempt: "
//		<< *newConnection
//		<< endl ;

// Obtain the local machine's port number for this Connection
struct sockaddr_in sockAddr ;
memset( &sockAddr, 0, sizeof( struct sockaddr_in ) ) ;

socklen_t sockAddrLen =
	static_cast< socklen_t >( sizeof( struct sockaddr ) ) ;

// Get the information for the socket on this machine
if( ::getsockname( newConnection->getSockFD(),
	reinterpret_cast< struct sockaddr* >( &sockAddr ),
	&sockAddrLen ) < 0 )
	{
	// getsockname() failed
	cout	<< "finishConnect> getsockname() failed: "
		<< strerror( errno )
		<< endl ;

	// Close socket and clean up memory
	delete newConnection ; newConnection = 0 ;

	// Return failure
	return 0 ;
	}

// Update remote port number for this Connection
newConnection->setLocalPort( ntohs( sockAddr.sin_port ) ) ;

// Connection in progress
connectionMap.insert( newConnection ) ;

// Notify the thread, should it be waiting for something to do
::pthread_cond_signal( &condWait ) ;

// Return a pointer to the Connection object, whether or not
// it's NULL
return newConnection ;
}

bool ConnectionManager::DisconnectByHost( const string& hostname,
	const unsigned short int remotePort,
	const unsigned short int localPort )
{
// An empty hostname is ok here, it indicates that a listening
// Connection is to be removed from the given port

// This variable is used so that a pthread_cond_signal() can be
// invoked only from a single spot in this method
bool foundItem = false ;

// Walk the connectionMap, looking for one or more matching Connections
for( connectionMapIterator connectionItr = connectionMap.begin(),
	connectionEndItr = connectionMap.end() ;
	connectionItr != connectionEndItr ; ++connectionItr )
	{
	// Store the address of the Connection object in cPtr
	// Convenience variable
	Connection* cPtr = *connectionItr ;

	// If the hostname is empty, then we are attempting to remove
	// a listening socket
	if( hostname.empty() && cPtr->isListening() )
		{
		// We are looking for a listening Connection, and
		// we found one.
		// The only criteria for a listening Connection is
		// the localPort.
		if( localPort == cPtr->getLocalPort() )
			{
			// Found the listener we seek
			scheduleErasure( connectionItr ) ;

			foundItem = true ;
			break ;
			}
		}

	// Not looking for a listener
	// If localPort == 0, then remove all Connections matching
	// hostname/remotePort

	// First check for a remotePort match
	if( remotePort != cPtr->getRemotePort() )
		{
		// No match, continue with next item
		continue ;
		}

	// Look for a match on the hostname
	if( !strcasecmp( hostname.c_str(), cPtr->getHostname().c_str() ) )
		{
		// Found a match on the hostname

		// Check if we are to remove all Connections matching
		// the two previous criteria, regardless of localPort
		if( 0 == localPort )
			{
			// Remove all
			scheduleErasure( connectionItr ) ;

			foundItem = true ;
			// Keep going until end of connectionMap
			}

		// Check now if the localPorts match exactly
		else if( localPort == cPtr->getLocalPort() )
			{
			// Exact match
			scheduleErasure( connectionItr ) ;

			foundItem = true ;
			break ;
			}
		} // if( hostname == ... )
	} // for()

if( foundItem )
	{
	::pthread_cond_signal( &condWait ) ;
	}

return foundItem ;
}

bool ConnectionManager::DisconnectByIP( const string& IP,
	const unsigned short int remotePort,
	const unsigned short int localPort )
{
// An empty IP is ok here, it indicates that a listening Connection
// is to be removed from the given port

// This variable is used so that a pthread_cond_signal() can be
// invoked only from a single spot in this method
bool foundItem = false ;

// Walk the connectionMap, looking for one or more matching Connections
for( connectionMapIterator connectionItr = connectionMap.begin(),
	connectionEndItr = connectionMap.end() ;
	connectionItr != connectionEndItr ; ++connectionItr )
	{
	// Store the address of the Connection object in cPtr
	// Convenience variable
	Connection* cPtr = *connectionItr ;

	// If the IP is empty, then we are attempting to remove
	// a listening socket
	if( IP.empty() && cPtr->isListening() )
		{
		// We are looking for a listening Connection, and
		// we found one.
		// The only criteria for a listening Connection is
		// the localPort.
		if( localPort == cPtr->getLocalPort() )
			{
			// Found the listener we seek
			scheduleErasure( connectionItr ) ;

			foundItem = true ;
			break ;
			}
		}

	// Not looking for a listener
	// If localPort == 0, then remove all Connections matching
	// IP/remotePort

	// First check for a remotePort match
	if( remotePort != cPtr->getRemotePort() )
		{
		// No match, continue with next item
		continue ;
		}

	// Look for a match on the IP
	if( IP == cPtr->getIP() )
		{
		// Found a match on the IP

		// Check if we are to remove all Connections matching
		// the two previous criteria, regardless of localPort
		if( 0 == localPort )
			{
			// Remove all
			scheduleErasure( connectionItr ) ;

			foundItem = true ;
			// Keep going until end of connectionMap
			}

		// Check now if the localPorts match exactly
		else if( localPort == cPtr->getLocalPort() )
			{
			// Exact match
			scheduleErasure( connectionItr ) ;

			foundItem = true ;
			break ;
			}
		} // if( hostname == ... )
	} // for()

if( foundItem )
	{
	::pthread_cond_signal( &condWait ) ;
	}

return foundItem ;
}

bool ConnectionManager::Disconnect( Connection* cPtr )
{
// Public method, verify method arguments
assert( cPtr != 0 ) ;

// Attempt to locate the Connection in the connectionMap
connectionMapIterator connectionItr = connectionMap.find( cPtr ) ;
if( connectionItr == connectionMap.end() )
	{
	// Connection was not found for this handler
	return false ;
	}

cout	<< "ConnectionManager::Disconnect> Sheduling connection "
	<< "for removal: "
	<< *cPtr
	<< endl ;

// Schedule the connection to be erased during the next call
// to Poll()
scheduleErasure( connectionItr ) ;

// Connection located and scheduled for erasure, return success
return true ;
}

void ConnectionManager::Poll( const long seconds,
	const long milliseconds )
{

cout	<< "Poll()" << endl ;

// Only execute this method if:
// - The connectionMap is not empty, OR the eraseMap is not empty
// Either of these cases indicates that some processing must be
// performed.
if( connectionMap.empty() && eraseMap.empty() )
	{
	::pthread_cond_wait( &condWait, &condMutex ) ;
	// TODO: Check return value
	}

// highestFD is passed to select() as the largest FD for which to
// obtain state information.
int highestFD = 0 ;

// These variables are used for select()
fd_set writefds ;
fd_set readfds ;
FD_ZERO( &writefds ) ;
FD_ZERO( &readfds ) ;

// Iterate through the table of Connection's to setup select()
// FD information
for( connectionMapIterator connectionItr = connectionMap.begin(),
	connectionEndItr = connectionMap.end() ;
	connectionItr != connectionEndItr ; ++connectionItr )
	{
	// Create a couple of convenience variables
	Connection* connectionPtr = *connectionItr ;
	int tempFD = connectionPtr->getSockFD() ;

	// The order of the below if/else structure is important
	// First check for the fully connected sockets.
	if( connectionPtr->isConnected() )
		{
		// This connection already connected
		// Check to see if we can read
		FD_SET( tempFD, &readfds ) ;

		// Is the connection's output buffer empty?
		if( !connectionPtr->outputBuffer.empty() )
			{
			// There is data present to be written
			FD_SET( tempFD, &writefds ) ;
			}
		}
	// Because listening sockets are technically pending
	// sockets as well, process those first since they
	// are handled differently than outgoing pending
	// connections.
	else if( connectionPtr->isListening() )
		{
		// Server socket, it is always pending :)
		FD_SET( tempFD, &readfds ) ;
		}

	// Check for a still pending outgoing socket connection.
	else if( connectionPtr->isPending() )
		{
		// Not yet fully connected, check for write
		FD_SET( tempFD, &writefds ) ;
		}

	// Keep track of the highest FD
	if( tempFD > highestFD )
		{
		highestFD = tempFD ;
		}
	} // for( connectionItr )

//cout	<< "Poll> Attempting to read "
//	<< connectionMap.size()
//	<< " fd's"
//	<< endl ;

// fdCnt is the number of FD's returned by select()
// loopCnt is the max number of attempts for select()
int			fdCnt = -1,
			loopCnt = 10 ;

// Call select() repeatedly until either of the following:
// - select() returns a value >= 0
// - select() returns no error (other than EINTR)
// - loopCnt becomes negative
do
	{
	// timeval may be modified by select() on some systems,
	// so recreate it each time
	struct timeval to = { seconds, milliseconds } ;

	// Make sure to clear errno
	errno = 0 ;

	// Call select()
	// Block indefinitely if seconds is -1
	fdCnt = ::select( 1 + highestFD, &readfds, &writefds, 0,
		(-1 == seconds) ? NULL : &to ) ;
	} while( (EINTR == errno) && (--loopCnt >= 0) ) ;

// Is there still an error from select()?
if( fdCnt < 0 )
	{
	// Error in select()
	return ;
	}

// Continue with the rest of the loop, even if no connections were found
// to be awaiting service: a pending Connection still needs to have its
// timeout checked.

// This variable is used for checking timeouts
time_t now = ::time( 0 ) ;

for( connectionMapIterator connectionItr = connectionMap.begin() ;
	connectionItr != connectionMap.end() ; ++connectionItr )
	{

	// This variable indicates if the connection should be
	// kept (true) or removed (false) from the tables
	bool connectOK = true ;

	// Obtain a pointer to the Connection object being inspected
	Connection* connectionPtr = *connectionItr ;

	// Temporary variable, this is the value of the current
	// Connection's socket (file) descriptor
	int tempFD = connectionPtr->getSockFD() ;

	// Order of this if/else is important here.
	// First check for fully connected sockets
	if( connectionPtr->isConnected() )
		{
		// Check for ability to read
		if( FD_ISSET( tempFD, &readfds ) )
			{
			// Data available, or error
			// handleRead() will perform the read()/recv()
			// and distribute event to virtual methods
			// (including OnDisconnect())
			//
			// Connected sockets are always checked
			// for read
			connectOK = handleRead( connectionPtr ) ;

			// Attempt to write any buffered data
			// if the connection is valid
			// A Connection's outputBuffer cannot
			// be modified since when we last checked,
			// so no threat of a possibly blocking
			// call here.
			if( connectOK &&
				!connectionPtr->outputBuffer.empty() )
				{
				connectOK = handleWrite( connectionPtr ) ;
				}
			} // if( FD_ISSET() )
		} // if( connectionPtr->isConnected() )

	// Next let's check if this is a listening (server) socket
	else if( connectionPtr->isListening() )
		{
//		cout	<< "Poll> Checking listener: "
//			<< *connectionPtr
//			<< endl ;

		// Yup.  See if anyone is waiting to connect
		if( FD_ISSET( tempFD, &readfds ) )
			{
			// Could be, attempt an accept()
			connectOK = finishAccept( connectionPtr ) ;
				}
		}

	// Check for pending outgoing connection
	else if( connectionPtr->isPending() )
		{
//		cout	<< "Poll> Checking pending: "
//			<< *connectionPtr
//			<< endl ;

		// Ready for write state?
		if( FD_ISSET( tempFD, &writefds ) )
			{
			// Yup, attempt to complete the connection
			connectOK = finishConnect( connectionPtr ) ;
			} // if( FD_ISSET() )
		else
			{
			// FD is NOT set, and the connection is
			// still pending; let's check for a timeout
			if( now >= connectionPtr->getAbsTimeout() )
				{
				// This connection attempt has
				// timed out
				OnTimeout( connectionPtr ) ;

				// Mark this connection for erasure
				connectOK = false ;

				} // if( now >= ... )
			} // else()
		} // if( connectionPtr->isPending() )

	// Check if the connection is still valid
	if( !connectOK )
		{
		// Nope, the connection is invalid
		// Schedule it for erasure
		scheduleErasure( connectionItr ) ;
		} // if( !connectOK )

	} // for( connectionItr )

// Handler methods have the freedom to call Disconnect() at will,
// however disconnecting/deallocating Connection objects in a handler
// will invalidate the iterator used in the above loop (since handlers
// are called in the above loop).
// Therefore, we are using a separate container to hold pointers to
// the Connection objects to be erased, and are erasing them here
// in synchronous fashion to simplify the process.
// Elements can get themselves into the eraseMap by
// either having a read/write/accept/connect error, or by a client
// class calling some form of Disconnect().
// Either way, all terminal processing is done here.
//
// No increment is done in this for loop, because it is done
// in the inner for loop.
// Because a set<> is a unique associative container, eraseMap will
// never contain duplicate entries.
//
for( eraseMapIterator eraseItr = eraseMap.begin(),
	eraseEndItr = eraseMap.end() ;
	eraseItr != eraseEndItr ; ++eraseItr )
	{

	// There is no reason to call the notification methods of
	// the handlers here, all of that processing is performed
	// by the above for() loops (either directly, or through calls
	// to finishAccept(),finishConnect(),handleRead(),handleWrite())
	//
	// Obtain a convenience pointer for readability
	Connection* connectionPtr = *(*eraseItr) ;

//	cout	<< "Poll> Removing connection: "
//		<< *connectionPtr
//		<< endl ;

	// Close the Connection's socket (file) descriptor
	closeSocket( connectionPtr->getSockFD() ) ;

	// Deallocate the memory associated with the Connection
	delete connectionPtr ;

	// Remove the Connection from the connectionMap
	// for this handler
	connectionMap.erase( *eraseItr ) ;

	} // for( eraseItr != eraseEndItr )

// Now that all elements in the eraseMap have been handled, clear
// the map for future use.
eraseMap.clear() ;

//cout	<< "Poll> End"
//	<< endl ;

} // Poll()

int ConnectionManager::openSocket()
{

// Let's get right to it, open the socket
int sockFD = ::socket( AF_INET, SOCK_STREAM, 0 ) ;

// Was the socket creation successful?
if( sockFD < 0 )
	{
	// Nope
	cout	<< "openSocket> socket() failed: "
		<< strerror( errno )
		<< endl ;
	return -1 ;
	}

// Attempt to set this socket's options.
if( !setSocketOptions( sockFD ) )
	{
	// setSocketOptions() failed, the socket is now invalid.
	close( sockFD ) ;

	cout	<< "openSocket> Failed to set SO_LINGER: "
		<< strerror( errno )
		<< endl ;

	// Return error to the caller
	return -1 ;
	}

// The socket() was opened and configured properly, return the
// new sockFD to the caller.
return sockFD ;
}

// Static method, no const needed
bool ConnectionManager::isIpAddress( const string& host )
{
// decimalPoints counts the number of decimalPoints in the given
// string referred to by (host).  Note that a valid IP address
// has exactly 3 decimalPoints.
unsigned short int decimalPoints = 0 ;

// Iterate through the host string, looking for valid characters.
// Break when we reach the end of the string, when decimalPoints == 4,
// or we encounter an invalid character (non-digit).
for( string::size_type i = 0 ;
	(i < host.size()) && (decimalPoints < 4) ; ++i )
	{
	// characters in an IP address may only be
	// digits or decimal points
	if( '.' == host[ i ] )
		{
		++decimalPoints ;
		}
	else if( !isdigit( host[ i ] ) )
		{
		// Found a non-digit, return false, this is not a
		// valid IP address
		return false ;
		}
	}

// there are exactly three decimal points in an IP address
return (3 == decimalPoints) ;
}

// Static, so no const necessary
string ConnectionManager::ipAddrOf( const string& host )
{
// Attempt to lookup the hostname
struct hostent *hostEntry = ::gethostbyname( host.c_str() ) ;
if( NULL == hostEntry )
	{
	// hostname not found, return an empty string
	return string() ;
	}

// Attempt to extract the IP address from the hostent struct
struct in_addr in ;
char **p = hostEntry->h_addr_list ;
memcpy( &in.s_addr, *p, sizeof( in.s_addr ) ) ;

// This initial allocation is just for readability
char ipAddr[] = "000.000.000.000" ;
sprintf( ipAddr,"%s", inet_ntoa( in ) ) ;

// Return the ipAddr, wrapped in a std::string
return string( ipAddr ) ;
}

void ConnectionManager::closeSocket( int fd )
{
::close( fd ) ;
}

// Caller handles erasing/closing upon a failed call to handleRead().
bool ConnectionManager::handleRead( Connection* cPtr )
{
// protected member, no error checking

// Create and set a temporary buffer to 0
char buf[ 4096 ] ;
memset( buf, 0, 4096 ) ;

// Attempt the read from the socket
int readResult = ::recv( cPtr->getSockFD(), buf, 4096, 0 ) ;

// Check for error on read()
if( readResult < 0 )
	{
	// Error on read, socket no longer valid
	// Notify handler
	OnDisconnect( cPtr ) ;

	// Read error
	return false ;
	}

// Read was successful

// Null terminate the buffer
// This shouldn't be necessary since the buffer was cleared
// above, but who knows what the system is doing :)
buf[ readResult ] = 0 ;

// Add to Connection input buffer
cPtr->inputBuffer += buf ;

// Check if a complete command was read
string line ;
while( cPtr->inputBuffer.ReadLine( line ) )
	{
	// Line available
	// Notify handler
	OnRead( cPtr, line ) ;
	}

// The read was successful, return success
return true ;
}

// Caller handles erasing/closing upon a failed call to handleWrite().
bool ConnectionManager::handleWrite( Connection* cPtr )
{
// protected member, no error checking

// Attempt the write to the socket
// According to W. Richard Stevens, Unix Network Programming, Vol 1:
// If there is some room in the socket send buffer, the return value
// will be the number of bytes that the kernel was able to copy
// into the buffer. ( This is called a short count).
// Therefore, just place the entire Connection output buffer into
// the send(), and the system will send() as much as it can
// The above applies to NONBLOCKING sockets.
//
int writeResult = ::send( cPtr->getSockFD(),
	cPtr->outputBuffer.c_str(),
	cPtr->outputBuffer.size(),
	0 ) ;

// Check for write error
if( writeResult < 0 )
	{
	// Error on write, socket no longer valid
	// Notify the handler.
	OnDisconnect( cPtr ) ;

	// Write error
	return false ;
	}

// Successful write, update the Connection's output buffer
cPtr->outputBuffer.Delete( writeResult ) ;

// Write was successful, return succes
return true ;
}

// Caller handles erasing/closing upon a failed call to finishConnect().
bool ConnectionManager::finishConnect( Connection* cPtr )
{
// Protected member method, no error checking performed on
// method arguments

//cout	<< "ConnectionManager::finishConnect> "
//	<< *cPtr
//	<< endl ;

// Attempt to connect()
int connectResult = ::connect( cPtr->getSockFD(),
	reinterpret_cast< sockaddr* >( cPtr->getAddr() ),
	sizeof( struct sockaddr_in ) ) ;

// Was the connect successful?
if( connectResult < 0 )
	{
	// EISCONN means that the connection succeeded with the
	// first attempt
	if( errno != EISCONN )
		{
//		cout	<< "finishConnect> Failed connect: "
//			<< strerror( errno )
//			<< endl ;

		// Unable to establish connection
		// Notify handler
		OnConnectFail( cPtr ) ;

		// Return failure
		return false ;
		} // if( errno != EISCONN )
	}

// Update the Connection's state
cPtr->setConnected() ;

// Notify handler
OnConnect( cPtr ) ;

// Return success
return true ;
}

// Caller handles erasing/closing upon a failed call to finishAccept().
bool ConnectionManager::finishAccept( Connection* cPtr )
{
// Protected member, no arguments verified here
// (cPtr) is the server socket, which is listen()'ing

// Allocate and configure a new Connection object
Connection* newConnection = new (std::nothrow) Connection( delimiter ) ;
assert( newConnection != 0 ) ;

newConnection->setIncoming() ;
newConnection->setPending() ;

// len is the size of a sockaddr structure, for use by accept()
size_t len = sizeof( struct sockaddr ) ;

// Attempt to receive a pending connection.
// The socket descriptor passed in is that of the listening socket.
// The sockaddr structure is that of the new connection, and will be
// populated with host information of the connecting client.
// accept() returns the fd of the new connection, with "mostly"
// the same options as the server socket, or -1 on error.
int newFD = ::accept( cPtr->getSockFD(),
	reinterpret_cast< sockaddr* >( newConnection->getAddr() ),
	static_cast< socklen_t* >( &len ) ) ;

/*
 * From man page:
 * There may not always be a connection waiting after a SIGIO
 * is delivered or select(2) or poll(2) return a  readability
 * event because the connection might have been removed by an
 * asynchronous network error or another thread before accept
 * is called.  If this happens then the call will block waiting for
 * the next connection to  arrive.   To  ensure  that
 * accept never blocks, the passed socket s needs to have the
 * O_NONBLOCK flag set.
 */
if( (newFD < 0) && (EAGAIN != errno) && (EWOULDBLOCK != errno))
	{
	// The only use of notifying the handler here is
	// so the handler can check for newConnection->isIncoming()
	// to know that an incoming connection failed; it can
	// obtain no other information about the Connection.
	OnConnectFail( newConnection ) ;

	// Clean up memory
	delete newConnection ; newConnection = 0 ;

	// Return error
	return false ;
	}

// Connect OK, update newConnection accordingly
newConnection->setConnected() ;
newConnection->setSockFD( newFD ) ;

// Store the remote machine's IP address into the Connection's memory
const char* IP = inet_ntoa( newConnection->getAddr()->sin_addr ) ;
newConnection->setIP( IP ) ;

// Because a reverse lookup is flaky at best (and blocks), just assign
// the newConnection's hostname to be the same as its IP; this will
// allow DisconnectByHost() to still function properly.
newConnection->setHostname( IP ) ;

// Update remote port number for this Connection
newConnection->setRemotePort(
	ntohs( newConnection->getAddr()->sin_port ) ) ;

// The new connection's localPort is the port to which it connected,
// which is the port of the listener
newConnection->setLocalPort( cPtr->getLocalPort() ) ;

// From man page:
// Note that any per file descriptor flags (everything that can be  set
// with  the F_SETFL fcntl, like non blocking or async state)
// are not inherited across an accept().
if( !setSocketOptions( newFD ) )
	{
	// Failed to set socket options
	// The Connection now has the IP of the failed Connection
	// Notify handler
	OnConnectFail( newConnection ) ;

	cout	<< "ConnectionManager::finishAccept> Failed to set "
		<< "socket options for connection: "
		<< *newConnection
		<< endl ;

	// Close the socket associated with this connection
	closeSocket( newConnection->getSockFD() ) ;

	// Clean up
	delete newConnection ; newConnection = 0 ;

	// Return error
	return false ;
	}

// Attempt to insert the new Connection into the appropriate
// connectionMap for its handler
if( !connectionMap.insert( newConnection ).second )
	{
	// Notify the handler of a failure in connection
	// TODO: The handler expects errno to be set appropriately.
	OnConnectFail( newConnection ) ;

	cout	<< "ConnectionManager::finishAccept> Failed to add "
		<< "new connection to table: "
		<< *newConnection
		<< endl ;

	// Socket is valid, but something went wrong with the
	// insertion...make sure to close the socket.
	closeSocket( newConnection->getSockFD() ) ;

	// Clean up memory
	delete newConnection ; newConnection = 0 ;

	// Return error
	return false ;
	}

// Notify the responsible handler of the new incoming connection
OnConnect( newConnection ) ;

// Return success
return true ;
}

/**
 * Attempt to set all options for the given socket.
 * If we encounter an error, return false, and let the
 * calling method decide what to do with the socket.
 */
bool ConnectionManager::setSocketOptions( int sockFD )
{
// Protected member, no error checking on args

// Get current flags
int optval = ::fcntl( sockFD, F_GETFL, 0 ) ;
if( optval < 0 )
	{
	return false ;
	}

// Set to non-blocking
optval = ::fcntl( sockFD, F_SETFL, optval | O_NONBLOCK ) ;
if( optval < 0 )
	{
	return false ;
	}

// Detect closed connection
optval = ::setsockopt( sockFD, SOL_SOCKET, SO_KEEPALIVE,
	reinterpret_cast< const char* >( &optval ), sizeof( optval ) ) ;
if( optval < 0 )
	{
	return false ;
        }

// Disable linger
struct linger setLinger ;
setLinger.l_onoff = 0 ;
setLinger.l_linger = 0 ;

optval = ::setsockopt( sockFD, SOL_SOCKET, SO_LINGER,
	reinterpret_cast< const char* >( &setLinger ),
	sizeof( setLinger ) ) ;
if( optval < 0 )
	{
	return false ;
	}

// All options set successfully
return true ;
}

Connection* ConnectionManager::Listen( const unsigned short int localPort )
{
cout	<< "ConnectionManager::Listen> Port: "
	<< localPort
	<< endl ;

// Attempt to open a socket
// openSocket() will also call setSocketOptions()
int listenFD = openSocket() ;
if( listenFD < 0 )
	{
	cout	<< "ConnectionManager::Listen> Failed to open socket"
		<< endl ;
	return 0 ;
	}

// setSocketOptions() does not set SO_REUSEADDR.
// However, for server sockets, it's important to set SO_REUSEADDR,
// to allow fast rebinds.
int optVal = 0 ;
if( ::setsockopt( listenFD, SOL_SOCKET, SO_REUSEADDR,
	reinterpret_cast< const char* >( &optVal ),
	sizeof( optVal ) ) < 0 )
	{
	// Failed to set SO_REUSEADDR
	cout	<< "ConnectionManager::Listen> Failed to set "
		<< "SO_REUSEADDR: "
		<< strerror( errno )
		<< endl ;

	// Close the socket
	closeSocket( listenFD ) ;

	// Return failure
	return 0 ;
	}

// Create and configure a new Connection
Connection* newConnection = new (std::nothrow) Connection( delimiter ) ;
assert( newConnection != 0 ) ;

newConnection->setSockFD( listenFD ) ;
newConnection->setListen() ;
newConnection->setPending() ;
newConnection->setLocalPort( localPort ) ;
// Leave hostname/IP empty for the new Connection.  This will
// distinguish the connection more easily for use in
// DisconnectByHost() and DisconnectByIP()

// Setup the sockaddr structure
struct sockaddr_in* addr = newConnection->getAddr() ;
addr->sin_family = AF_INET ;
addr->sin_port = htons( static_cast< u_short >( localPort ) ) ;

// Attempt to bind to the given port
if( ::bind( listenFD,
	reinterpret_cast< sockaddr* >( newConnection->getAddr() ),
	static_cast< socklen_t >( sizeof( struct sockaddr ) ) ) < 0 )
	{
	// bind() failed
	cout	<< "ConnectionManager::Listen> bind() failed: "
		<< strerror( errno )
		<< endl ;

	// Close the socket
	closeSocket( listenFD ) ;

	// Clean up memory area
	delete newConnection ; newConnection = 0 ;

	// Return failure
	return 0 ;
	}

// Attempt to establish listener on the given socket
if( ::listen( listenFD, 5 ) < 0 )
	{
	// listen() failed
	cout	<< "ConnectionManager::Listen> listen() failed: "
		<< strerror( errno )
		<< endl ;

	// Close the socket
	closeSocket( listenFD ) ;

	// Clean up memory
	delete newConnection ; newConnection = 0 ;

	// Return error
	return 0 ;
	}

// Attempt to add this new connection into the handler map for
// the given handler
if( !connectionMap.insert( newConnection ).second )
	{
	cout	<< "ConnectionManager::Listen> Failed to add new "
		<< "connection to connectionMap: "
		<< *newConnection
		<< endl ;

	// Close the socket
	closeSocket( listenFD ) ;

	// Clean up memory
	delete newConnection ; newConnection = 0 ;

	// Return error
	return 0 ;
	}

// There is no need to call accept() once here, as we did
// with connect(), because the Poll() loop will work properly
// without it.

// Return the new connection to the caller
return newConnection ;
}

void ConnectionManager::scheduleErasure(
	connectionMapIterator connectionItr )
{
if( std::find( eraseMap.begin(), eraseMap.end(),
	connectionItr ) == eraseMap.end() )
	{
	eraseMap.push_back( connectionItr ) ;
	}
}

void ConnectionManager::OnConnect( Connection* )
{}

void ConnectionManager::OnDisconnect( Connection* )
{}

void ConnectionManager::OnConnectFail( Connection* )
{}

void ConnectionManager::OnTimeout( Connection* )
{}

void ConnectionManager::OnRead( Connection*, const string& )
{}

void ConnectionManager::Write( Connection* cPtr, const string& data )
{
assert( cPtr != 0 ) ;

// Ok to append an empty string
cPtr->outputBuffer += data ;
}

void ConnectionManager::Write( Connection* cPtr,
	const stringstream& data )
{
assert( cPtr != 0 ) ;

// Ok to append an empty string
cPtr->outputBuffer += data.str() ;
}

} // namespace gnuworld

