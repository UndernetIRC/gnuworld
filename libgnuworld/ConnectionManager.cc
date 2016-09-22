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
 * $Id: ConnectionManager.cc,v 1.20 2007/09/25 16:57:54 dan_karrels Exp $
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

#include	<new>
#include	<map>
#include	<string>
#include	<sstream>
#include	<iostream>

#include	<ctime>
#include	<cctype>
#include	<cstdlib>
#include	<cstring>
#include	<cstdio>
#include	<cassert>
#include	<cerrno>

#include	"ConnectionManager.h"
#include	"Connection.h"
#include	"ConnectionHandler.h"
#include	"Buffer.h"
#include	"ELog.h"

const char rcsId[] = "$Id: ConnectionManager.cc,v 1.20 2007/09/25 16:57:54 dan_karrels Exp $" ;

namespace gnuworld
{

using std::endl ;
using std::string ;
using std::map ;
using std::nothrow ;
using std::stringstream ;

ConnectionManager::ConnectionManager( const time_t defaultTimeoutDuration,
	const char defaultDelimiter )
:	timeoutDuration( defaultTimeoutDuration ),
	delimiter( defaultDelimiter ),
	inputBufferSize( 131072 )
{
inputBuffer = new char[ inputBufferSize ] ;
}

ConnectionManager::~ConnectionManager()
{
delete[] inputBuffer ; inputBuffer = 0 ;

// There is no reason to iterate through the eraseMap because
// that structure simply holds iterators to the connectionMap.
// Since the below loop invalidates all of those iterators, let's
// just be clean about it and clear the eraseMap now
eraseMap.clear() ;

// Iterate through the list of currently active/pending
// Connections
for( handlerMapIterator handlerItr = handlerMap.begin() ;
	handlerItr != handlerMap.end() ; ++handlerItr )
	{
	for( connectionMapIterator connectionItr = 
		handlerItr->second.begin() ;
		connectionItr != handlerItr->second.end() ;
		++connectionItr )
		{
		// Convenience variable
		// Obtain a pointer to the Connection object
		Connection* connectionPtr = *connectionItr ;

		// Close this Connection's socket
		closeSocket( connectionPtr->getSockFD() ) ;

		// Deallocate the space for this Connection object
		delete connectionPtr ;
		}
	}

// Clean up, even if not strictly necessary, it's good habit
handlerMap.clear() ;
}

Connection* ConnectionManager::Connect( ConnectionHandler* hPtr,
	const string& host,
	const unsigned short int remotePort )
{
// Handler must be valid
assert( hPtr != 0 ) ;

// An empty host is invalid
if( host.empty() )
	{
	return 0 ;
	}

//elog	<< "Connect> "
//	<< host
//	<< ":"
//	<< remotePort
//	<< endl ;

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

		elog	<< "ConnectionManager::Connect> Unable "
			<< "to find valid IP"
			<< endl ;

		return 0 ;
		}
	} // else()

// Open a non-blocking socket
int sockFD = openSocket() ;
if( -1 == sockFD )
	{
	delete newConnection ;

	elog	<< "Connect> openSocket() failed"
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

			elog	<< "ConnectionManager::Connect> Error "
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

if( newConnection != 0 )
	{
//	elog	<< "Connect> Adding new connection attempt: "
//		<< *newConnection
//		<< endl ;

	// Connection in progress
	// Add to the handlerMap
	bool insertOK = handlerMap[ hPtr ].insert( newConnection ).second ;

	// It's possible the connection could fail (?)
	// Either way, the container is nice enough to tell us
	// if it was successful, so let's check.
	if( !insertOK )
		{
		// Insertion failed
		// Close the socket
		closeSocket( newConnection->getSockFD() ) ;

		elog	<< "ConnectionManager::Connect> Failed to "
			<< "add new connection to handlerMap: "
			<< *newConnection
			<< endl ;

		// Deallocate and set to 0 for return to caller
		delete newConnection ; newConnection = 0 ;
		}
	}

if( newConnection != 0 )
	{
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
		elog	<< "finishConnect> getsockname() failed: "
			<< strerror( errno )
			<< endl ;

		closeSocket( newConnection->getSockFD() ) ;
		delete newConnection ; newConnection = 0 ;

		// Return failure
		return 0 ;
		}

	// Update remote port number for this Connection
	newConnection->setLocalPort( ntohs( sockAddr.sin_port ) ) ;
	}
// Return a pointer to the Connection object, whether or not
// it's NULL
return newConnection ;
}

bool ConnectionManager::DisconnectByHost( ConnectionHandler* hPtr,
	const string& hostname,
	const unsigned short int remotePort,
	const unsigned short int localPort )
{
// Public method, check method arguments
assert( hPtr != 0 ) ;

// An empty hostname is ok here, it indicates that a listening
// Connection is to be removed from the given port

// Attempt to find the handler in the handlerMap
handlerMapIterator handlerItr = handlerMap.find( hPtr ) ;
if( handlerItr == handlerMap.end() )
	{
	// This ConnectionHandler has no Connections registered
	// *shrug*
	return false ;
	}

// Walk the connectionMap, looking for one or more matching Connections
for( connectionMapIterator connectionItr = handlerItr->second.begin(),
	connectionEndItr = handlerItr->second.end() ;
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
			scheduleErasure( hPtr, connectionItr ) ;

			// Since a machine (in most cases, in this
			// one at least) may only have a single
			// listener for a given port, there is no
			// use in continuing with this loop.
			// Return success
			return true ;
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
			scheduleErasure( hPtr, connectionItr ) ;

			// Keep going until end of connectionMap
			}

		// Check now if the localPorts match exactly
		else if( localPort == cPtr->getLocalPort() )
			{
			// Exact match
			scheduleErasure( hPtr, connectionItr ) ;

			// Since there can only be a single (by
			// definition of TCP connections) connection
			// based on a unique set of hostname/localport/
			// remoteport, there is no need to continue
			// looking for more matches: there are none.
			return true ;
			}
		} // if( hostname == ... )
	} // for()

// Unable to find a matching Connection, return failure
return false ;
}

bool ConnectionManager::DisconnectByIP( ConnectionHandler* hPtr,
	const string& IP,
	const unsigned short int remotePort,
	const unsigned short int localPort )
{
// Public method, check args
assert( hPtr != 0 ) ;

// An empty IP is ok here, it indicates that a listening Connection
// is to be removed from the given port

// Attempt to lookup the given handler
// handlerItr will be equivalent to handlerMap.end() if the handler
// is not found.
handlerMapIterator handlerItr = handlerMap.find( hPtr ) ;
if( handlerItr == handlerMap.end() )
	{
	// This ConnectionHandler has no Connections registered
	// *shrug*
	return false ;
	}

// Walk the connectionMap, looking for one or more matching Connections
for( connectionMapIterator connectionItr = handlerItr->second.begin(),
	connectionEndItr = handlerItr->second.end() ;
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
			scheduleErasure( hPtr, connectionItr ) ;

			// Since a machine (in most cases, in this
			// one at least) may only have a single
			// listener for a given port, there is no
			// use in continuing with this loop.
			// Return success
			return true ;
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
			scheduleErasure( hPtr, connectionItr ) ;

			// Keep going until end of connectionMap
			}

		// Check now if the localPorts match exactly
		else if( localPort == cPtr->getLocalPort() )
			{
			// Exact match
			scheduleErasure( hPtr, connectionItr ) ;

			// Since there can only be a single (by
			// definition of TCP connections) connection
			// based on a unique set of IP/localport/
			// remoteport, there is no need to continue
			// looking for more matches: there are none.
			return true ;
			}
		} // if( hostname == ... )
	} // for()

// Unable to find a matching Connection, return failure
return false ;
}

bool ConnectionManager::disconnectAll( ConnectionHandler* hPtr )
{
// Precondition: hPtr != 0
handlerMapType::iterator hmItr = handlerMap.find( hPtr ) ;
if( handlerMap.end() == hmItr )
	{
	// No Connections owned by the ConnectionHandler
	return true ;
	}

bool returnMe = true ;

// This is safe because Disconnect() calls scheduleErasure()
// which modifies only the eraseMap.
for( connectionMapIterator connItr = hmItr->second.begin() ;
	connItr != hmItr->second.end() ; ++connItr )
	{
	// Track if any of the Disconnect()'s fail
	bool disReturn = Disconnect( hPtr, *connItr ) ;
	if( !disReturn )
		{
		returnMe = false ;
		}
	}
return returnMe ;
}

bool ConnectionManager::Disconnect( ConnectionHandler* hPtr,
	Connection* cPtr )
{
// Public method, verify method arguments
assert( hPtr != 0 ) ;

if( 0 == cPtr )
	{
	// Disconnect all Connections held by the ConnectionHandler
	return disconnectAll( hPtr ) ;
	}

// Attempt to locate the handler in the handler map
handlerMapIterator handlerItr = handlerMap.find( hPtr ) ;
if( handlerItr == handlerMap.end() )
	{
	// Handler not found
	return false ;
	}

// Attempt to locate the Connection in the handler's connectionMap
connectionMapIterator connectionItr = handlerItr->second.find( cPtr ) ;
if( connectionItr == handlerItr->second.end() )
	{
	// Connection was not found for this handler
	return false ;
	}

elog	<< "ConnectionManager::Disconnect> Scheduling connection "
	<< "for removal: "
	<< *cPtr
	<< endl ;

// Schedule the connection to be erased during the next call
// to Poll()
scheduleErasure( hPtr, connectionItr ) ;

// Connection located and scheduled for erasure, return success
return true ;
}

void ConnectionManager::Poll( const long seconds,
	const long milliseconds )
{
//elog	<< "Poll()" << endl ;

// Only execute this method if:
// - The handlerMap is not empty, OR the eraseMap is not empty
// Either of these cases indicates that some processing must be
// performed.
if( handlerMap.empty() && eraseMap.empty() )
	{
//	elog	<< "ConnectionManager::Poll> handlerMap.empty()"
//		<< endl ;
	return ;
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
for( constHandlerMapIterator handlerItr = handlerMap.begin(),
	handlerEndItr = handlerMap.end() ;
	handlerItr != handlerEndItr ; ++handlerItr )
	{
	for( constConnectionMapIterator connectionItr =
		handlerItr->second.begin(),
		connectionEndItr = handlerItr->second.end() ;
		connectionItr != connectionEndItr ;
		++connectionItr )
		{
		// Create a couple of convenience variables
		const Connection* connectionPtr = *connectionItr ;
		int tempFD = connectionPtr->getSockFD() ;

//		elog	<< "Poll> Before select(), tempFD: "
//			<< tempFD
//			<< endl ;

		assert( tempFD >= 0 ) ;

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
	} // for( handlerItr )

//elog	<< "ConnectionManager::Poll> select()"
//	<< endl ;

int selectRet = 0 ;
struct timeval to = { seconds, milliseconds * 1000 } ;

// Call select()
selectRet = ::select( 1 + highestFD, &readfds, &writefds, 0,
		(-1 == seconds) ? NULL : &to ) ;

//elog	<< "ConnectionManager::Poll()> seconds: "
//	<< seconds
//	<< ", selectRet: "
//	<< selectRet
//	<< endl ;

// Continue even if select() returned 0, there may be connections
// waiting to be erased

// Is there an error from select()?
if( selectRet < 0 )
	{
	// Error in select()
	elog	<< "ConnectionManager::Poll> Error in Poll(): "
		<< strerror( errno )
		<< endl ;
	return ;
	}

// Continue with the rest of the loop, even if no connections were found
// to be awaiting service: a pending Connection still needs to have its
// timeout checked.

// This variable is used for checking timeouts
time_t now = ::time( 0 ) ;

// Walk the handler list, checking connections for each connectionMap.
for( constHandlerMapIterator handlerItr = handlerMap.begin(),
	handlerEndItr = handlerMap.end() ;
	handlerItr != handlerEndItr ; ++handlerItr )
	{
	// Convenience variable for the long loop ahead
	ConnectionHandler* hPtr = handlerItr->first ;

	// Iterate through this handler's connectionMap, similar
	// as the above loops.
	for( connectionMapIterator connectionItr =
		handlerItr->second.begin(),
		connectionEndItr = handlerItr->second.end() ;
		connectionItr != connectionEndItr ;
		++connectionItr )
		{

		// This variable indicates if the connection should be
		// kept (true) or removed (false) from the tables
		bool connectOK = true ;

		// Obtain a pointer to the Connection object being inspected
		Connection* connectionPtr = *connectionItr ;

		// Temporary variable, this is the value of the current
		// Connection's socket (file) descriptor
		int tempFD = connectionPtr->getSockFD() ;

//		elog	<< "Poll> Before select(), tempFD: "
//			<< tempFD
//			<< endl ;

		assert( tempFD >= 0 ) ;

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
				connectOK = handleRead( hPtr,
					connectionPtr ) ;
				} // if( FD_ISSET( read ) )

			// Attempt to write any buffered data
			// if the connection is valid
			// A Connection's outputBuffer cannot
			// be modified since when we last checked,
			// so no threat of a possibly blocking
			// call here.
			if( connectOK && FD_ISSET( tempFD, &writefds ) )
				{
				connectOK = handleWrite( hPtr,
					connectionPtr ) ;
				}
			} // if( connectionPtr->isConnected() )

		// Next let's check if this is a listening (server) socket
		else if( connectionPtr->isListening() )
			{
//			elog	<< "Poll> Checking listener: "
//				<< *connectionPtr
//				<< endl ;

			// Yup.  See if anyone is waiting to connect
			if( FD_ISSET( tempFD, &readfds ) )
				{
				// Could be, attempt an accept()
				connectOK = finishAccept( hPtr,
					connectionPtr ) ;
				}
			}

		// Check for pending outgoing connection
		else if( connectionPtr->isPending() )
			{
//			elog	<< "Poll> Checking pending: "
//				<< *connectionPtr
//				<< endl ;

			// Ready for write state?
			if( FD_ISSET( tempFD, &writefds ) )
				{
				// Yup, attempt to complete the connection
				connectOK = finishConnect( hPtr,
					connectionPtr ) ;
				} // if( FD_ISSET() )
			else
				{
				// FD is NOT set, and the connection is
				// still pending; let's check for a timeout
				if( now >= connectionPtr->getAbsTimeout() )
					{
					// This connection attempt has
					// timed out
					// Notify the handler
					hPtr->OnTimeout( connectionPtr ) ;

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
			scheduleErasure( hPtr, connectionItr ) ;
			} // if( !connectOK )

		} // for( connectionItr )
	} // for( handlerItr )

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
// Note that the reason for using an associative container for the
// eraseMap, instead of a linear one, is to increase efficiency here.
// If the Connection*'s were unsorted in the eraseMap, then this
// loop would be performing many handlerMap.find()'s looking for the
// ConnectionHandler for each Connection.
// With the eraseMap being associative, we can work a single
// ConnectionHandler at a time to its completion, which saves
// lookups.
// No increment is done in this for loop, because it is done
// in the inner for loop.
// Because all Connections are erased by calling scheduleErasure(),
// we are guaranteed that an iterator to a connection is not entered
// into the eraseMap more than once.
//
for( eraseMapIterator eraseItr = eraseMap.begin(),
	eraseEndItr = eraseMap.end() ;
	eraseItr != eraseEndItr ; )
	{
	// First lookup the handlerItr.
	// No error checking is performed here, because the above
	// loop was able to find the handler, and no removes have
	// been done (it is done here).
	// Because the lookups are performed based on what is found
	// in the eraseMap, handlerItr is guaranteed to have
	// at least one Connection which must be removed.
	//
	handlerMapIterator handlerItr = handlerMap.find(
		eraseItr->first ) ;

	// Continue in the loop, erasing elements, until the
	// handler in the eraseMap changes
	// This loop guard will be evaluated to true at least once.
	// The below loop will terminate when either of two conditions
	// are present after incrementing eraseItr:
	// - eraseItr is equivalent to eraseEndItr
	// - eraseItr->first != handlerItr->first
	// Be sure to check both, beginning with the case that could
	// crash the process :)
	// There is no reason to call the notification methods of
	// the handlers here, all of that processing is performed
	// by the above for() loops (either directly, or through calls
	// to finishAccept(),finishConnect(),handleRead(),handleWrite())
	//
	for( ; (eraseItr != eraseEndItr) &&
		(eraseItr->first == handlerItr->first) ; ++eraseItr )
		{
		// Obtain a convenience pointer for readability
		Connection* connectionPtr = *(eraseItr->second) ;

//		elog	<< "Poll> Removing connection: "
//			<< *connectionPtr
//			<< endl ;

		// Close the Connection's socket (file) descriptor
		closeSocket( connectionPtr->getSockFD() ) ;

		// Deallocate the memory associated with the Connection
		delete connectionPtr ;

		// Remove the Connection from the connectionMap
		// for this handler
		handlerItr->second.erase( eraseItr->second ) ;

		} // for( ; eraseItr->first == handlerItr->first ; )

	} // for( eraseItr != eraseEndItr )

// Now that all elements in the eraseMap have been handled, clear
// the map for future use.
eraseMap.clear() ;

// Check if the connectionMap for any particular handler is empty
for( handlerMapIterator handlerItr = handlerMap.begin() ;
	handlerItr != handlerMap.end() ; ++handlerItr )
	{
	// From SGI STL website:
	// Map has the important property that inserting a new element 
	// into a map does not invalidate iterators that point to 
	// existing elements.  Erasing an element from a map also does
	// not invalidate any iterators, except, of course, for iterators 
	// that actually point to the element that is being erased. 
	if( handlerItr->second.empty() )
		{
		// The connectionMap for this handler is empty
		// Erase it
		handlerMap.erase( handlerItr ) ;
		}
	if (handlerMap.begin() == handlerMap.end())
		break;
	} // for()

//elog	<< "Poll> End"
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
	elog	<< "openSocket> socket() failed: "
		<< strerror( errno )
		<< endl ;
	return -1 ;
	}

// Attempt to set this socket's options.
if( !setSocketOptions( sockFD ) )
	{
	// setSocketOptions() failed, the socket is now invalid.
	close( sockFD ) ;

	elog	<< "openSocket> Failed to set SO_LINGER: "
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
bool ConnectionManager::handleRead( ConnectionHandler* hPtr,
	Connection* cPtr )
{
// protected member, no error checking

// Attempt the read from the socket
errno = 0 ;
int readResult = -1 ;

// Check for simulation mode
if( cPtr->isFile() )
	{
	// Connected to file
	readResult = ::read( cPtr->getSockFD(), inputBuffer,
		inputBufferSize ) ;
	}
else
	{
	// Network connection
	readResult = ::recv( cPtr->getSockFD(), inputBuffer,
		inputBufferSize, 0 ) ;
	}

if( EAGAIN == errno )
	{
	// Nonblocking type error
	// Ignore it
//	elog	<< "ConnectionManager::handleRead> EAGAIN"
//		<< endl ;
	return true ;
	}

//elog	<< "ConnectionManager::handleRead> Read "
//	<< readResult
//	<< " bytes"
//	<< endl ;

// Check for error on read()
if( readResult <= 0 )
	{
	elog	<< "ConnectionManager::handleRead> Read error: "
		<< strerror( errno )
		<< endl ;

	// Error on read, socket no longer valid
	// Notify handler
	hPtr->OnDisconnect( cPtr ) ;

	// Read error
	return false ;
	}

// Read was successful

// Add to Connection input buffer
cPtr->inputBuffer.append( inputBuffer, readResult ) ;
cPtr->bytesRead += readResult ;

// Check if a complete command was read
string line ;
while( cPtr->inputBuffer.ReadLine( line ) )
	{
	// Line available
	// Notify handler
	hPtr->OnRead( cPtr, line ) ;
	}

// The read was successful, return success
return true ;
}

// Caller handles erasing/closing upon a failed call to handleWrite().
bool ConnectionManager::handleWrite( ConnectionHandler* hPtr,
	Connection* cPtr )
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

// Does this Connection represent a file?
if( cPtr->isFile() )
	{
	// Just ignore writes to the file
	cPtr->outputBuffer.clear() ;

	return true ;
	}

if( cPtr->isFlush() )
	{
	return handleFlush( hPtr, cPtr ) ;
	}

errno = 0 ;
int writeResult = ::send( cPtr->getSockFD(),
	cPtr->outputBuffer.data(),
	cPtr->outputBuffer.size(),
	0 ) ;

if( (ENOBUFS == errno) || (EWOULDBLOCK == errno) || (EAGAIN == errno) )
	{
	// Nonblocking type error
	// Ignore it for now
	elog	<< "ConnectionManager::handleWrite> errno: "
		<< strerror( errno )
		<< endl ;
	return true ;
	}

//elog	<< "ConnectionManager::handleWrite> Wrote "
//	<< writeResult
//	<< " bytes, remaining in buffer: "
//	<< (cPtr->outputBuffer.size() - writeResult)
//	<< endl ;

// Check for write error
if( writeResult < 0 )
	{
	// Error on write, socket no longer valid
	// Notify the handler.
	hPtr->OnDisconnect( cPtr ) ;

	// Write error
	return false ;
	}

//elog	<< "ConnectionManager::handleWrite> Wrote: "
//	<< cPtr->outputBuffer.substr( 0, writeResult )
//	<< endl ;

// Successful write, update the Connection's output buffer
cPtr->outputBuffer.Delete( writeResult ) ;
cPtr->bytesWritten += writeResult ;

// Write was successful, return succes
return true ;
}

// Caller handles erasing/closing upon a failed call to handleWrite().
bool ConnectionManager::handleFlush( ConnectionHandler* hPtr,
	Connection* cPtr )
{
// protected member, no error checking

// Make sure the Connection's F_FLUSH flag is cleared, so do it first
cPtr->removeFlag( Connection::F_FLUSH ) ;

// Set to blocking so that the send() will block

// Retrieve the current flags
int flags = ::fcntl( cPtr->getSockFD(), F_GETFL, 0 ) ;
if( flags < 0 )
	{
	elog	<< "ConnectionManager::handleFlush> Unable to set "
		<< "blocking for connection: "
		<< strerror( errno )
		<< endl ;

	// Terminal error
	hPtr->OnDisconnect( cPtr ) ;
	return false ;
	}

// Remove nonblocking flag
flags &= ~O_NONBLOCK ;

// Attempt to set new flag (blocking)
if( ::fcntl( cPtr->getSockFD(), F_SETFL, flags ) < 0 )
	{
	elog	<< "ConnectionManger::handleFlush> Failed to set "
		<< "to blocking: "
		<< strerror( errno )
		<< endl ;

	// Terminal error
	hPtr->OnDisconnect( cPtr ) ;
	return false ;
	}

while( !cPtr->outputBuffer.empty() )
	{
	errno = 0 ;
	int writeResult = ::send( cPtr->getSockFD(),
		cPtr->outputBuffer.data(),
		cPtr->outputBuffer.size(),
		0 ) ;

	if( (ENOBUFS == errno) || (EWOULDBLOCK == errno) || (EAGAIN == errno) )
		{
		// Nonblocking type error
		// Ignore it for now
		elog	<< "ConnectionManager::handleFlush> errno: "
			<< strerror( errno )
			<< endl ;
		return true ;
		}

	//elog	<< "ConnectionManager::handleFlush> Wrote "
	//	<< writeResult
	//	<< " bytes, remaining in buffer: "
	//	<< (cPtr->outputBuffer.size() - writeResult)
	//	<< endl ;

	// Check for write error
	if( writeResult < 0 )
		{
		// Error on write, socket no longer valid
		// Notify the handler.
		hPtr->OnDisconnect( cPtr ) ;

		// Write error
		return false ;
		}

	//elog	<< "ConnectionManager::handleFlush> Wrote: "
	//	<< cPtr->outputBuffer.substr( 0, writeResult )
	//	<< endl ;

	// Successful write, update the Connection's output buffer
	cPtr->outputBuffer.Delete( writeResult ) ;
	cPtr->bytesWritten += writeResult ;
	} // while( !empty() )

// Reset to nonblocking
flags |= O_NONBLOCK ;

if( ::fcntl( cPtr->getSockFD(), F_SETFL, flags ) < 0 )
	{
	elog	<< "ConnectionManager::handleFlush> Failed to set to "
		<< "nonblocking: "
		<< strerror( errno )
		<< endl ;

	// Terminal error
	hPtr->OnDisconnect( cPtr ) ;
	return false ;
	}

// Write was successful, return succes
return true ;
}

// Caller handles erasing/closing upon a failed call to finishConnect().
bool ConnectionManager::finishConnect( ConnectionHandler* hPtr,
	Connection* cPtr )
{
// Protected member method, no error checking performed on
// method arguments

//elog	<< "ConnectionManager::finishConnect> "
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
//		elog	<< "finishConnect> Failed connect: "
//			<< strerror( errno )
//			<< endl ;

		// Unable to establish connection
		// Notify handler
		hPtr->OnConnectFail( cPtr ) ;

		// Return failure
		return false ;
		} // if( errno != EISCONN )
	}

// Update the Connection's state
cPtr->setConnected() ;
cPtr->connectTime = ::time( 0 ) ;

// Notify handler
hPtr->OnConnect( cPtr ) ;

// Return success
return true ;
}

// Caller handles erasing/closing upon a failed call to finishAccept().
bool ConnectionManager::finishAccept( ConnectionHandler* hPtr,
	Connection* cPtr )
{
// Protected member, no arguments verified here
// (cPtr) is the server socket, which is listen()'ing

// Allocate and configure a new Connection object
Connection* newConnection = new (std::nothrow) Connection( delimiter ) ;
assert( newConnection != 0 ) ;

newConnection->setIncoming() ;
newConnection->setPending() ;

// len is the size of a sockaddr structure, for use by accept()
socklen_t len = sizeof( struct sockaddr ) ;

// Attempt to receive a pending connection.
// The socket descriptor passed in is that of the listening socket.
// The sockaddr structure is that of the new connection, and will be
// populated with host information of the connecting client.
// accept() returns the fd of the new connection, with "mostly"
// the same options as the server socket, or -1 on error.
int newFD = ::accept( cPtr->getSockFD(),
	reinterpret_cast< sockaddr* >( newConnection->getAddr() ),
	&len ) ;

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
	hPtr->OnConnectFail( newConnection ) ;

	// Clean up memory
	delete newConnection ; newConnection = 0 ;

	// Return error
	return false ;
	}

// Connect OK, update newConnection accordingly
newConnection->setConnected() ;
newConnection->setSockFD( newFD ) ;
newConnection->connectTime = ::time( 0 ) ;

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
	hPtr->OnConnectFail( newConnection ) ;

	elog	<< "ConnectionManager::finishAccept> Failed to set "
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
if( !handlerMap[ hPtr ].insert( newConnection ).second )
	{
	// Failed to insert into handlerMap
	// Notify the handler of a failure in connection
	// TODO: The handler expects errno to be set appropriately.
	hPtr->OnConnectFail( newConnection ) ;

	elog	<< "ConnectionManager::finishAccept> Failed to add "
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
hPtr->OnConnect( newConnection ) ;

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

optval = 1 ;
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

Connection* ConnectionManager::Listen( ConnectionHandler* hPtr,
	const unsigned short int localPort )
{
// Public method, check arguments
assert( hPtr != 0 ) ;

elog	<< "ConnectionManager::Listen> Port: "
	<< localPort
	<< endl ;

// Attempt to open a socket
// openSocket() will also call setSocketOptions()
int listenFD = openSocket() ;
if( listenFD < 0 )
	{
	elog	<< "ConnectionManager::Listen> Failed to open socket"
		<< endl ;
	return 0 ;
	}

// setSocketOptions() does not set SO_REUSEADDR.
// However, for server sockets, it's important to set SO_REUSEADDR,
// to allow fast rebinds.
int optVal = 1 ;
if( ::setsockopt( listenFD, SOL_SOCKET, SO_REUSEADDR,
	reinterpret_cast< const char* >( &optVal ),
	sizeof( optVal ) ) < 0 )
	{
	// Failed to set SO_REUSEADDR
	elog	<< "ConnectionManager::Listen> Failed to set "
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
	elog	<< "ConnectionManager::Listen> bind() failed: "
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
	elog	<< "ConnectionManager::Listen> listen() failed: "
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
if( !handlerMap[ hPtr ].insert( newConnection ).second )
	{
	// Addition to handlerMap failed :(
	elog	<< "ConnectionManager::Listen> Failed to add new "
		<< "connection to handlerMap: "
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

void ConnectionManager::scheduleErasure( ConnectionHandler* hPtr,
	connectionMapIterator connectionItr )
{
// Protected member, no error checking performed on arguments
eraseMapIterator eraseItr = eraseMap.find( hPtr ) ;

// Walk through the container
// Continue walking until we find the duplicate (in which case
// return from this method), we encounter the end of the eraseMap,
// or we are no longer examining the connections for the given
// handler (we can do this because a multimap guarantees its
// entries to be kept in a strict weak order)
for( ; (eraseItr != eraseMap.end()) && (eraseItr->first == hPtr) ;
	++eraseItr )
	{
	if( eraseItr->second == connectionItr )
		{
		// Found a duplicate
		// No need to perform any further processing,
		// this connection is already scheduled for
		// erasure and will be removed on the next call
		// to Poll() (assuming this method isn't already
		// being called from Poll() <G>)
		return ;
		}
	} // while()

// The connectionItr is not present in the eraseMap, go ahead
// and add it to the eraseMap to be erased by Poll()
eraseMap.insert( eraseMapType::value_type( hPtr, connectionItr ) ) ;
}

/**
 * This method is used for debugging in general, but can certainly
 * be used to read a file.
 */
Connection* ConnectionManager::ConnectToFile( ConnectionHandler* hPtr,
	const string& fileName )
{
// public method, verify parameter
assert( hPtr != 0 ) ;

// Make sure the filename is valid (or at least, not blatantly invalid)
if( fileName.empty() )
	{
	return 0 ;
	}

//elog	<< "Connecting to file: "
//	<< fileName
//	<< endl ;

// Open the file
int fd = ::open( fileName.c_str(), O_CREAT ) ;
if( fd < 0 )
	{
	// Failed to open the file
	elog	<< "ConnectToFile> Unable to open file "
		<< fileName
		<< ": "
		<< strerror( errno )
		<< endl ;
	return 0 ;
	}

// Create a new Connection object to represent this open file
Connection* newConnect = new (std::nothrow)
	Connection( fileName, fd, delimiter ) ;
assert( newConnect != 0 ) ;

// Set the Connection's state
newConnect->setConnected() ;
newConnect->setSockFD( fd ) ;

// Tag the new Connection as a file
newConnect->setFile() ;

// Insert the new Connection into the Connection map
bool insertOK = handlerMap[ hPtr ].insert( newConnect ).second ;
if( !insertOK )
	{
	elog	<< "ConnectToFile> Failed to insert new Connection"
		<< endl ;

	// Posting OnConnectFail() doesn't make sense here because
	// we haven't notified the handler of the Connection in
	// the first place
	closeSocket( fd ) ;

	// Clean up memory
	delete newConnect ; newConnect = 0 ;
	}

// Should we notify the handler of the new conncetion?
if( newConnect != 0 )
	{
	// Yes, all is valid
	hPtr->OnConnect( newConnect ) ;
	}

// Return the new connection to the caller
return newConnect ;
}

// Retrieve the number of connections that a given handler
// present has outstanding
size_t ConnectionManager::numConnections( ConnectionHandler* hPtr ) const
{
// public method, check parameter
assert( hPtr != 0 ) ;

// Lookup the handler
constHandlerMapIterator hItr = handlerMap.find( hPtr ) ;
if( hItr == handlerMap.end() )
	{
	// Handler not found
	return 0 ;
	}

// Return the number of active/pending connections the handler posesses
return hItr->second.size() ;
}

} // namespace gnuworld
