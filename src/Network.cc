/**
 * Network.cc
 * Author: Daniel Karrels dan@karrels.com
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
 * $Id: Network.cc,v 1.78 2008/04/16 20:29:38 danielaustin Exp $
 */

#include	<new>
#include	<set>
#include	<map>
#include	<list>
#include	<string>
#include	<vector>
#include	<iostream>
#include	<algorithm>

#include	<cassert>

#include	"Network.h"
#include	"ELog.h"
#include	"iClient.h"
#include	"iServer.h"
#include	"Channel.h"
#include	"client.h"
#include	"misc.h"
#include	"Numeric.h"
#include	"match.h"
#include	"StringTokenizer.h"
#include	"ip.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: Network.cc,v 1.78 2008/04/16 20:29:38 danielaustin Exp $" ) ;

namespace gnuworld
{

using std::set ;
using std::map ;
using std::make_pair ;
using std::string ;
using std::endl ;
using std::list ;
using std::unary_function ;

xNetwork::xNetwork()
{}

xNetwork::~xNetwork()
{}

bool xNetwork::addClient( iClient* newClient )
{
assert( NULL != newClient ) ;

if( !numericMap.insert( numericMapType::value_type(
	newClient->getIntYYXXX(), newClient ) ).second )
	{
	elog	<< "xNetwork::addClient> Insert into numericMap failed for numeric "
		<< newClient->getIntYYXXX()
		<< endl ;
	return false ;
	}

//elog	<< "xNetwork::addClient> Added client: "
//	<< *newClient
//	<< endl ;

addNick( newClient ) ;

return true ;
}

// xClients are local to this server, they
// must be handled a little differently.
// This method needs to assign numerics
// to the client -- the client doesn't
// already have a numeric.
bool xNetwork::addClient( xClient* newClient )
{
assert( NULL != newClient ) ;

if( findLocalNick( newClient->getNickName() ) != 0 )
	{
	// Nickname already exists on this serer
	elog	<< "xNetwork::addClient(xClient*)> Found existing "
		<< "nickname: "
		<< newClient->getNickName()
		<< endl ;
	return false ;
	}

// Find a new numeric for this client
// The client's intYY/charYY should already be set.
unsigned int intXXX = 0 ;

if( !allocateClientNumeric( newClient->getIntYY(), intXXX ) )
	{
	elog	<< "xNetwork::addClient(xClient)> Unable to "
		<< "allocate local client numeric for: "
		<< *newClient
		<< endl ;
	return false ;
	}

// Give the new client a numeric
newClient->setIntXXX( intXXX ) ;

if( !localClients.insert(
	make_pair( newClient->getIntYYXXX(), newClient ) ).second )
	{
	elog	<< "xNetwork::addClient(xClient)> Unable to insert "
		<< "new client into localClients: "
		<< *newClient
		<< endl ;
	newClient->setIntXXX( 0 ) ;
	return false ;
	}

//elog	<< "xNetwork::addClient(xClient)> Added client: "
//	<< *newClient
//	<< endl ;

return true ;
}

bool xNetwork::addServer( iServer* newServer )
{
assert( newServer != NULL ) ;

//elog	<< "xNetwork::addServer> Adding server: "
//	<< *newServer
//	<< endl ;

if( !serverMap.insert( serverMapType::value_type(
	newServer->getIntYY(), newServer ) ).second )
	{
	elog	<< "xNetwork::addServer> Insert into serverMap failed"
		<< " for server: "
		<< *newServer
		<< endl ;
	return false ;
	}

//elog	<< "xNetwork::addServer> Added server: "
//	<< *newServer
//	<< endl ;

return true ;
}

bool xNetwork::addChannel( Channel* theChan )
{
assert( theChan != NULL ) ;

//elog	<< "Adding channel: "
//	<< *theChan
//	<< endl ;

return channelMap.insert(
	channelMapType::value_type( theChan->getName(),
		theChan ) ).second ;
}

iClient* xNetwork::findClient( const unsigned int& intYYXXX ) const
{
numericMapType::const_iterator ptr = numericMap.find( intYYXXX ) ;
if( ptr == numericMap.end() )
	{
	return 0 ;
	}
return ptr->second ;
}

iClient* xNetwork::findClient( const unsigned int& intYY,
	const unsigned int& intXXX ) const
{
unsigned int intYYXXX = combinebase64int( intYY, intXXX ) ;

numericMapType::const_iterator ptr = numericMap.find( intYYXXX ) ;
if( ptr == numericMap.end() )
	{
	return 0 ;
	}
return ptr->second ;
}

iClient* xNetwork::findFakeClient( const unsigned int& intYY,
	const unsigned int& intXXX ) const
{
unsigned int intYYXXX = combinebase64int( intYY, intXXX ) ;

fakeClientMapType::const_iterator ptr = fakeClientMap.find( intYYXXX ) ;
if( ptr == fakeClientMap.end() )
	{
	return 0 ;
	}
return ptr->second.first ;
}

iClient* xNetwork::findClient( const string& yyxxx ) const
{
unsigned int intYYXXX = base64toint( yyxxx.c_str(), yyxxx.size() ) ;
numericMapType::const_iterator ptr = numericMap.find( intYYXXX ) ;
if( ptr == numericMap.end() )
	{
	return 0 ;
	}
return ptr->second ;
}

iClient* xNetwork::findFakeClient( const string& yyxxx ) const
{
unsigned int intYYXXX = base64toint( yyxxx.c_str(), yyxxx.size() ) ;
fakeClientMapType::const_iterator ptr = fakeClientMap.find( intYYXXX ) ;
if( ptr == fakeClientMap.end() )
	{
	return 0 ;
	}
return ptr->second.first ;
}

iClient* xNetwork::findNick( const string& nick ) const
{
//elog	<< "xNetwork::findNick> "
//	<< nick
//	<< endl ;

const_clientIterator ptr = nickMap.find( nick ) ;
if( ptr == clients_end() )
	{
	return 0 ;
	}
return ptr->second ;
}

xClient* xNetwork::findLocalClient( const string& yyxxx ) const
{
unsigned int intYYXXX = base64toint( yyxxx.c_str(), 5 ) ;

const_localClientIterator cItr = localClients.find( intYYXXX ) ;
if( cItr == localClient_end() )
	{
//	elog	<< "xNetwork::findLocalClient> Unable to find "
//		<< "client numeric: "
//		<< yyxxx
//		<< endl ;
	return 0 ;
	}
return cItr->second ;
}

xClient* xNetwork::findLocalClient( const unsigned int& intYYXXX ) const
{
const_localClientIterator cItr = localClients.find( intYYXXX ) ;
if( cItr == localClient_end() )
	{
//	elog	<< "xNetwork::findLocalClient> Unable to find "
//		<< "client numeric: "
//		<< intYYXXX
//		<< endl ;
	return 0 ;
	}
return cItr->second ;
}

xClient* xNetwork::findLocalNick( const string& nickName ) const
{
for( const_localClientIterator cItr = localClient_begin() ;
	cItr != localClient_end() ; ++cItr )
	{
	if( !strcasecmp( cItr->second->getNickName(), nickName ) )
		{
		return cItr->second ;
		}
	}
return 0 ;
}

iServer* xNetwork::findServer( const string& YY ) const
{
return findServer( base64toint( YY.c_str(), YY.size() ) ) ;
}

iServer* xNetwork::findServer( const unsigned int& intYY ) const
{
const_serverIterator ptr = serverMap.find( intYY ) ;
if( ptr == servers_end() )
	{
	return 0 ;
	}
return ptr->second ;
}

iServer* xNetwork::findServerName( const string& name ) const
{
for( const_serverIterator ptr = servers_begin() ;
	ptr != servers_end() ; ++ptr )
	{
	if( !strcasecmp( ptr->second->getName(), name ) )
		{
		return ptr->second ;
		}
	}
return 0 ;
}

iServer* xNetwork::findExpandedServerName( const string& name ) const
{
for( const_serverIterator ptr = servers_begin() ;
	ptr != servers_end() ; ++ptr )
	{
	if( !match( ptr->second->getName(), name ) )
		{
		return ptr->second ;
		}
	}
return 0 ;
}

std::list< iServer* > xNetwork::getAllBurstingServers()
{
std::list< iServer* > burstingList;
for( const_serverIterator ptr = servers_begin() ;
	ptr != servers_end() ; ++ptr )
	{
	if(ptr->second->isBursting())
		{
		burstingList.push_back(ptr->second);
		}
	}
return burstingList;

}


Channel* xNetwork::findChannel( const string& name ) const
{
const_channelIterator ptr = channelMap.find( name ) ;
if( ptr == channels_end() )
	{
//	elog	<< "xNetwork::findChannel> Failed to find: "
//		<< name << endl ;
	return 0 ;
	}
return ptr->second ;
}

iClient* xNetwork::removeClient( const unsigned int& intYY,
	const unsigned int& intXXX )
{
return removeClient( combinebase64int( intYY, intXXX ) ) ;
}

iClient* xNetwork::removeClient( const string& yyxxx )
{
return removeClient( base64toint( yyxxx.c_str(), 5 ) ) ;
}

iClient* xNetwork::removeClient( const unsigned int& intYYXXX )
{
numericMapType::iterator ptr = numericMap.find( intYYXXX ) ;
if( ptr == numericMap.end() )
	{
	elog	<< "xNetwork::removeClient> Unable to find client "
		<< "numeric: "
		<< intYYXXX
		<< endl ;
	return 0 ;
	}

//elog	<< "xNetwork::removeClient> Removing client: "
//	<< *(ptr->second)
//	<< endl ;

iClient* retMe = ptr->second ;

numericMap.erase( ptr ) ;

removeNick( retMe->getNickName() ) ;

if( findFakeClient( retMe ) != 0 )
	{
	removeFakeClient( retMe ) ;
	}

// Remove all associations between client->channel
iClient::channelIterator chanPtr = retMe->channels_begin() ;
while( chanPtr != retMe->channels_end() )
	{
//	elog	<< "xNetwork::removeClient> Removing user "
//		<< retMe->getCharYYXXX()
//		<< " from channel "
//		<< (*chanPtr)->getName()
//		<< endl ;

	ChannelUser* theChanUser = (*chanPtr)->removeUser( retMe ) ;
	if( NULL == theChanUser )
		{
		elog	<< "xNetwork::removeClient> Unable to find "
			<< "ChannelUser in channel "
			<< (*chanPtr)->getName()
			<< ", for iClient: "
			<< *retMe
			<< endl ;
		}
	delete theChanUser ; theChanUser = 0 ;

	if( (*chanPtr)->empty() )
		{
//		elog	<< "xNetwork::removeClient> Removing channel "
//			<< (*chanPtr)->getName()
//			<< endl ;

		delete removeChannel( (*chanPtr)->getName() ) ;
		}
	++chanPtr ;
	}

retMe->clearChannels() ;
return retMe ;
}

iClient* xNetwork::removeClient( iClient* theClient )
{
assert( theClient != 0 ) ;

return removeClient( theClient->getIntYYXXX() ) ;
}

xClient* xNetwork::removeLocalClient( xClient* theClient )
{
assert( theClient != 0 ) ;

localClientIterator cItr =
	localClients.find( theClient->getIntYYXXX() ) ;
if( cItr == localClient_end() )
	{
	// client not found
	elog	<< "xNetwork::removeLocalClient> Unable to "
		<< "find local client: "
		<< *theClient
		<< endl ;
	return 0 ;
	}
localClients.erase( cItr ) ;

if( !freeClientNumeric( theClient->getIntYYXXX() ) )
	{
	elog	<< "xNetwork::removeLocalClient> Failed to free "
		<< "client numeric for client: "
		<< *theClient
		<< endl ;
	return 0 ;
	}

return theClient ;
}

void xNetwork::removeNick( const string& nick )
{
nickMap.erase( nick ) ;
}

/**
 * Remove a server by numeric from the network tables.
 * This will deallocate any clients on that server.
 * A pointer to the server being removed is returned.
 * The memory space for this iServer is NOT deallocated
 * by this method, so it's up to the caller to deallocate
 * any heap memory.
 */
iServer* xNetwork::removeServer( const unsigned int& YY,
	bool postEvent )
{
// Attempt to find the server being removed
serverIterator sItr = serverMap.find( YY ) ;

// Did we find the server?
if( sItr == servers_end() )
	{
	// Nope, log an error
	elog	<< "xNetwork::removeServer> Failed to find server "
		<< "numeric: "
		<< YY
		<< endl ;

	// Let the caller know that the remove failed
	return 0 ;
	}

// Grab a pointer to the iServer for convenience and readability
iServer* serverPtr = sItr->second ;

// Remove the server from the internal table
serverMap.erase( sItr ) ;

if( findFakeServer( serverPtr->getIntYY() ) )
	{
	removeFakeServer( serverPtr ) ;
	}

// Verbose debugging information
//elog	<< "xNetwork::removeServer> Removing server: "
//	<< *serverPtr
//	<< endl ;

// Walk through the numericMap looking for clients which are on
// the server being removed.
// This algorithm is O(N) :(
for( numericMapType::iterator clientIterator = numericMap.begin() ; 
	clientIterator != numericMap.end() ; )
	{
	// Is this client on the server that is being removed?
	if( YY != clientIterator->second->getIntYY() )
		{
		// Nope, move to next client
		++clientIterator ;

		// Skip rest of the loop
		continue ;
		}

	// Let removeClient() handle:
	// - Removing the client<->channel interactions
	// - Removing the channel itself, if empty
	// - Removing the client from the internal tables
	//
	// The removal of an element from a hash_map<> invalidates
	// the iterator (or perhaps all iterators, still searching
	// for documentation on this).  However, hash_map::erase()
	// does not return an updated iterator, so we have to make
	// a little work around here to make sure we keep a valid
	// iterator.

	// nextIterator is the iterator past the current iterator
	numericMapType::iterator nextIterator( clientIterator ) ;
	++nextIterator ;

	// Remove the iClient and perform functions described above.
	// This method calls numericMap.erase(), and so after the
	// call to removeClient(), the clientIterator is no longer
	// valid.
	// removeClient() also calls removeFakeClient()
	iClient* theClient = removeClient( clientIterator->second ) ;

	// Point the clientIterator to the nextIterator
	clientIterator = nextIterator ;

	// Should we post this as an EVT_QUIT event?
	// It seems that this may add unneeded complexity in handling
	// requests of the client modules since they can call methods
	// which may attempt to access the server to which this client
	// is/was connected (the server is no longer in the tables)
	if( postEvent )
		{
		// Yes, post the event
		theServer->PostEvent( EVT_QUIT,
			static_cast< void* >( theClient ) ) ;
		}

	// Be sure to deallocate the iClient's allocated heap space
	delete theClient ;
	}

// Return the server being removed
return serverPtr ;
}

iServer* xNetwork::removeServer( const string& YY )
{
return removeServer( base64toint( YY.c_str() ) ) ;
}

iServer* xNetwork::removeServerName( const string& name )
{
iServer* serverPtr = findServerName( name ) ;
if( NULL == serverPtr )
	{
	return NULL ;
	}
return removeServer( serverPtr->getIntYY() ) ;
}

Channel* xNetwork::removeChannel( const string& name )
{
Channel* tmpChan;

channelIterator ptr = channelMap.find( name ) ;
if( ptr == channels_end() )
	{
	elog	<< "xNetwork::removeChannel> Failed to find channel: "
		<< name
		<< endl ;
	return 0 ;
	}
tmpChan = ptr->second;
channelMap.erase( ptr ) ;
return tmpChan;
}

Channel* xNetwork::removeChannel( const Channel* theChan )
{
assert( theChan != 0 ) ;

return removeChannel( theChan->getName() ) ;
}

void xNetwork::rehashNick( const string& yyxxx,
	const string& newNick )
{
//elog	<< "xNetwork::rehashNick> yyxxx: "
//	<< yyxxx
//	<< ", newNick: "
//	<< newNick
//	<< endl ;

iClient* theClient = findClient( yyxxx ) ;
if( NULL == theClient )
	{
	elog	<< "xNetwork::rehashNick> Unable to find numeric: "
		<< yyxxx
		<< ", new nick: "
		<< newNick
		<< endl ;
	return ;
	}

// No need to keep track of the return value here,
// we already have it.
removeNick( theClient->getNickName() ) ;

// oldNick is used in the event posting
string oldNick = theClient->getNickName() ;

// Change the client's nickname
theClient->setNickName( newNick ) ;

// Add the client back to the nickname table
addNick( theClient ) ;

// Note that the user's numeric never changes,
// so no need to rehash numeric

// Go ahead and post this event
theServer->PostEvent( EVT_CHNICK,
	static_cast< void* >( theClient ),
	static_cast< void* >( &oldNick ) ) ;

// TODO: theServer->PostNickChange() --> OnNickChange()
}

void xNetwork::addNick( iClient* theClient )
{
// This is a protected method, theClient is guaranteed to
// be non-NULL.
if( !nickMap.insert( nickMapType::value_type(
	theClient->getNickName(), theClient ) ).second )
	{
	elog	<< "xNetwork::addNick> Failed to add nick: "
		<< theClient->getNickName()
		<< endl ;
	}
}

/**
 * Handle a netsplit.
 * This method removes and deallocated the server corresponding
 * to the numeric (intYY), and all of its leaf servers (and all
 * of their leaf servers, etc).
 * Our uplink server has its own numeric as its uplinkIntYY.
 * This method was once a disturbingly complicated recursive method.
 * It turned out that the previous implementation was not robust
 * to modifications made to internal data structure types.
 * Therefore, it has been greatly simplified here, to where even
 * someone like me can understand it.
 * The idea here is to use a simple recursive method to gather the
 * server numerics which need to be removed, and then perform the
 * removals in an iterative manner.
 * (intYY) is the server numeric of the server to be removed.
 * The caller of this method is responsible for posting the
 * EVT_NETBREAK event.
 */
void xNetwork::OnSplit( const unsigned int& intYY )
{
// yyVector will be used to hold server numerics of servers
// to be removed.
typedef std::vector< unsigned int > yyVectorType ;
yyVectorType yyVector ;

// Of course add the top level server to the list of servers
// to be removed.
yyVector.push_back( intYY ) ;

// Recursive method to find all leaf servers of intYY, and all
// of each of those servers' leaf servers.
// This is much simpler than having the entire OnSplit() method
// recursive.
findLeaves( yyVector, intYY ) ;

// yyVector should now have all leaf servers (if any) of intYY,
// each of which must be removed.

// Now iterate through the server numeric vector once more and
// remove the servers from the network data tables.
// Note that this will call removeServer() for each server,
// which will remove that server from the data tables, and also
// deallocate the server pointer.
// removeServer() will also remove all iClient's resident on that
// server (and deallocate them), and also remove any channels
// which are determined to be empty as a result of the netsplit.

for( yyVectorType::const_iterator yyIterator = yyVector.begin() ;
	yyIterator != yyVector.end() ; ++yyIterator )
	{
	// Obtain a pointer to the server in question for convenience
	// and readability
	iServer* removeMe = findServer( *yyIterator ) ;
	assert( removeMe != 0 ) ;

	// Generate some debugging information
//	elog	<< "xNetwork::OnSplit> Removing server: "
//		<< *removeMe
//		<< endl ;

	// Remove the server, its clients, any empty channels,
	// and post events for all of the above.
	iServer* tmpServer =  removeServer( removeMe->getIntYY(), true ) ;

	// Dont post an event for the actual server that is being
	// squit, let the msg_SQ handle that.
	if( intYY != tmpServer->getIntYY() )
		{
		string Reason( "Uplink Squit" ) ;
	
		theServer->PostEvent(EVT_NETBREAK,
			  static_cast<void *>(tmpServer),
			  static_cast<void*>(findServer(intYY)),
			  static_cast<void*>(&Reason));
		}
	delete tmpServer;		  
	}
}

/**
 * This is a simple recursive method which traverses the
 * serverMap looking for all leaves to server whose numeric
 * is uplinkIntYY.  Any server which is found to be a leaf
 * to uplinkIntYY is then passed into a recursive call to
 * findLeaves() for that particular leaf server, etc.
 * This method does not modify any data tables, it only
 * walks the serverMap table.
 */
void xNetwork::findLeaves( std::vector< unsigned int >& yyVector,
	const unsigned int uplinkIntYY ) const
{
// Begin our walk down the serverMap looking for leaf servers
// of uplinkIntYY.
for( const_serverIterator sItr = servers_begin() ;
	sItr != servers_end() ; ++sItr )
	{
	// Obtain a pointer to this iServer for convenience and
	// readability
	const iServer* serverPtr = sItr->second ;

	// Check to see if this server is our uplink, don't want
	// to remove that one :)
	if( theServer->getUplinkIntYY() == serverPtr->getIntYY() )
		{
		// It's our uplink.  Prevent infinite recursion here
		// by just ignoring this server (base case).

//		elog	<< "xServer::findLeaves> Found uplink"
//			<< endl ;
		}
	else if( serverPtr->getUplinkIntYY() == uplinkIntYY )
		{
		// serverPtr is a leaf to uplinkIntYY

		// Add serverPtr to the list of leaves to server
		// uplinkIntYY
		yyVector.push_back( serverPtr->getIntYY() ) ;

		// Call this method recursively to find leaves
		// of serverPtr.
		findLeaves( yyVector, serverPtr->getIntYY() ) ;

		} // else if()

	} // for()

} // findLeaves()

size_t xNetwork::serverList_size() const
{
return static_cast< size_t >( serverMap.size() ) ;
}

size_t xNetwork::clientList_size() const
{
return static_cast< size_t >( numericMap.size() ) ;
}

size_t xNetwork::countClients( const iServer* serverPtr ) const
{
assert( theServer != 0 ) ;

// Server numeric for which to search
const unsigned int YY = serverPtr->getIntYY() ;

// The number of clients found for the given server
size_t numClients = 0 ;

for( numericMapType::const_iterator ptr = numericMap.begin(),
	endPtr = numericMap.end() ; ptr != endPtr ; ++ptr )
	{
	if( YY == ptr->second->getIntYY() )
		{
		++numClients ;
		}
	}
return numClients ;
}

list< const iClient* > xNetwork::matchHost( const string& wildHost ) const
{
list< const iClient* > retMe ;

for( numericMapType::const_iterator ptr = numericMap.begin(),
	endPtr = numericMap.end() ; ptr != endPtr ; ++ptr )
	{
	const iClient* clientPtr = ptr->second ;
	if( !match( wildHost, clientPtr->getInsecureHost() ) ||
		!match( wildHost,
			xIP( clientPtr->getIP() ).GetNumericIP() ) )
		{
		// Found a match
		retMe.push_back( clientPtr ) ;
		}
	}

return retMe ;
}

list< const iClient* > xNetwork::matchUserHost(
	const string& wildUserHost ) const
{
// Tokenize the wildUserHost into username and hostname
StringTokenizer st( wildUserHost, '@' ) ;

// Make sure there are exactly two tokens
if( st.size() != 2 )
	{
	// Invalid format of wildUserHost, return empty list
	return list< const iClient* >() ;
	}

// Get a list of matching hosts to that of wildUserHost
list< const iClient* > matchingHosts = matchHost( st[ 1 ] ) ;

// Were any found?
if( matchingHosts.empty() )
	{
	// No matching hosts found, return the empty list
	return matchingHosts ;
	}

// Create a list to return to the caller, create matchingHosts.size()
// empty slots to speed up memory allocation
list< const iClient* > retMe ;

// Iterate through the list of matching hostnames
for( list< const iClient* >::const_iterator ptr = matchingHosts.begin() ;
	ptr != matchingHosts.end() ; ++ptr )
	{
	// Does this iClient's username also match that of the
	// wildUserHost username?
	if( !match( st[ 0 ], (*ptr)->getUserName() ) )
		{
		// Found a matching username
		retMe.push_back( *ptr ) ;
		}
	}

// Return the list of matching username and hostnames
return retMe ;
}

size_t xNetwork::countMatchingUserHost( const string& wildUserHost ) const
{
return static_cast< size_t >( matchUserHost( wildUserHost ).size() ) ;
}

list< const iClient* > xNetwork::findHost( const string& hostName ) const
{
list< const iClient* > retMe ;

for( numericMapType::const_iterator ptr = numericMap.begin(),
	endPtr = numericMap.end() ; ptr != endPtr ; ++ptr )
	{
	const iClient* clientPtr = ptr->second ;

	if( !strcasecmp( hostName, clientPtr->getInsecureHost() ) )
		{
		// Found a match
		retMe.push_back( clientPtr ) ;
		}
	}

return retMe ;
}

size_t xNetwork::countMatchingHost( const string& wildHost ) const
{
return static_cast< size_t >( matchHost( wildHost ).size() ) ;
}

size_t xNetwork::countHost( const string& hostName ) const
{
return static_cast< size_t >( findHost( hostName ).size() ) ;
}

list< const iClient* > xNetwork::matchRealHost( const string& wildHost )
	const
{
list< const iClient* > retMe ;

for( numericMapType::const_iterator ptr = numericMap.begin(),
	endPtr = numericMap.end() ; ptr != endPtr ; ++ptr )
	{
	const iClient* clientPtr = ptr->second ;
	if( !match( wildHost, clientPtr->getRealInsecureHost() ) ||
		!match( wildHost,
			xIP( clientPtr->getIP() ).GetNumericIP() ) )
		{
		// Found a match
		retMe.push_back( clientPtr ) ;
		}
	}

return retMe ;
}

list< const iClient* > xNetwork::matchRealUserHost(
	const string& wildUserHost ) const
{
// Tokenize the wildUserHost into username and hostname
StringTokenizer st( wildUserHost, '@' ) ;

// Make sure there are exactly two tokens
if( st.size() != 2 )
	{
	// Invalid format of wildUserHost, return empty list
	return list< const iClient* >() ;
	}

/* iterate through all users and try to match ident + host */
list< const iClient* > retMe;
bool host_is_ip = false;

for (numericMapType::const_iterator ptr = numericMap.begin();
	ptr != numericMap.end(); ++ptr)
{
	const iClient* clientPtr = ptr->second;

	/* match ident first - dont call match() if ident = '*' */
	if ((!strcmp(st[0].c_str(),"*")) || (!match(st[0], clientPtr->getUserName())))
	{
		/* ident matches, check if host/ip matches */
		/* to attempt to minimise cpu, if ip==host only check once */
		if (!strcmp(xIP(clientPtr->getIP()).GetNumericIP().c_str(), clientPtr->getRealInsecureHost().c_str()))
			host_is_ip = true;
		else
			host_is_ip = false;

		if (!match(st[1], xIP(clientPtr->getIP()).GetNumericIP()) ||
			(!host_is_ip && (!match(st[1], clientPtr->getRealInsecureHost()))))
		{
			/* match - push it back */
			retMe.push_back(clientPtr);
		}
	}
}

// Return the list of matching username and hostnames
return retMe ;
}

size_t xNetwork::countMatchingRealUserHost( const string& wildUserHost ) const
{
return static_cast< size_t >( matchRealUserHost( wildUserHost ).size() ) ;
}

list< const iClient* > xNetwork::findRealHost( const string& hostName ) const
{
list< const iClient* > retMe ;

for( numericMapType::const_iterator ptr = numericMap.begin(),
	endPtr = numericMap.end() ; ptr != endPtr ; ++ptr )
	{
	const iClient* clientPtr = ptr->second ;

	if( !strcasecmp( hostName, clientPtr->getRealInsecureHost() ) )
		{
		// Found a match
		retMe.push_back( clientPtr ) ;
		}
	}

return retMe ;
}

size_t xNetwork::countMatchingRealHost( const string& wildHost ) const
{
return static_cast< size_t >( matchRealHost( wildHost ).size() ) ;
}

size_t xNetwork::countRealHost( const string& hostName ) const
{
return static_cast< size_t >( findRealHost( hostName ).size() ) ;
}

list< const iClient* > xNetwork::matchRealName( const string& realName )
	const
{
list< const iClient* > retMe ;

for( numericMapType::const_iterator ptr = numericMap.begin(),
	endPtr = numericMap.end() ; ptr != endPtr ; ++ptr )
	{
	const iClient* clientPtr = ptr->second ;

	if( !match( realName, clientPtr->getDescription() ) )
		{
		retMe.push_back( clientPtr ) ;
		}
	}
return retMe;
}

bool xNetwork::allocateClientNumeric( unsigned int intYY,
	unsigned int& newIntXXX )
{
// First verify that the given intYY corresponds to a
// fake server (including the xServer).
reservedNumeric_iterator rsItr = reservedNumericMap.find( intYY ) ;
if( rsItr == reservedNumericMap.end() )
	{
	// Can't find the fake server to which this client
	// is to be attached.
	elog	<< "xNetwork::allocateClientNumeric> Unable to "
		<< "find intYY: "
		<< intYY
		<< endl ;
	return false ;
	}

// Look for an unassigned numeric.
// If the for loop traverses all possible values of
// unsigned int, it will eventually hit 0 again, and
// the loop will terminate.
unsigned int maxIntXXX = base64toint("]]]", 3);
for( newIntXXX = 0 ; newIntXXX <= maxIntXXX ; ++newIntXXX )
	{
//	elog	<< "xNetwork::allocateClientNumeric> Checking: "
//		<< newIntXXX
//		<< endl ;

	if( rsItr->second.find( newIntXXX ) == rsItr->second.end() )
		{
		// Unused numeric
		// By definition this insert() must succeed, since
		// the only reason for it to fail is if the
		// numeric already existed.
		rsItr->second.insert( newIntXXX ) ;

		break ;
		}
	}

// Check if all values were examined
if( newIntXXX > maxIntXXX )
	{
	elog	<< "xNetwork::allocateClientNumeric> Exceeded maxIntXXX "
		<< endl ;
	return false ;
	}

//elog	<< "xNetwork::allocateClientNumeric> Allocated numeric: "
//	<< "intYY: "
//	<< intYY
//	<< ", intXXX: "
//	<< newIntXXX
//	<< endl ;

// newIntXXX holds the unused numeric
return true ;
}

void xNetwork::setServer( xServer* _theServer )
{
assert( _theServer != 0 ) ;

// When this method is invoked, the xServer's iServer instance
// has already been added to normal server list
theServer = _theServer ;

// Reserve the server's numeric
if( !reservedNumericMap.insert( make_pair( theServer->getIntYY(), 
	set< unsigned int>() ) ).second )
	{
	elog	<< "xNetwork::setServer> Failed to add core server "
		<< "numeric to reservedNumericMap"
		<< endl ;
	}
}

bool xNetwork::addFakeClient( iClient* fakeClient,
	xClient* ownerClient )
{
// precondition: fakeClient has an assigned intYY already, but
// not an intXXX
assert( fakeClient != 0 ) ;
// ownerClient can be NULL

// This is a protected method, the presence of the necessary
// variables in the iClient have been met.
// Verify the integrity of a few key elements.

// Make sure the fake server to which this iClient is being
// associated at least has its server intYY numeric reserved
if( reservedNumericMap.find( fakeClient->getIntYY() ) ==
	reservedNumericMap.end() )
	{
	elog	<< "xNetwork::addFakeClient> Unable to find "
		<< "fakeServer intYY "
		<< fakeClient->getIntYY()
		<< " for fake client: "
		<< *fakeClient
		<< endl ;
	return false ;
	}

// Make sure the nickname does not collide
if( findNick( fakeClient->getNickName() ) != 0 )
	{
	elog	<< "xNetwork::addFakeClient> Found matching nickname: "
		<< fakeClient->getNickName()
		<< endl ;
	return false ;
	}

// fakeServer now points to a valid fake server
// Get an intXXX
unsigned int intXXX = 0 ;
if( !allocateClientNumeric( fakeClient->getIntYY(), intXXX ) )
	{
	elog	<< "xNetwork::addFakeClient> Unable to "
		<< "allocate client numeric for: "
		<< *fakeClient
		<< endl ;
	return false ;
	}

// pos
// Give the new client a numeric
// This call will set both intXXX and charXXX
fakeClient->setIntXXX( intXXX ) ;

// Add this client to the fake client table
if( !fakeClientMap.insert(
	make_pair( fakeClient->getIntYYXXX(),
		make_pair( fakeClient, ownerClient ) ) ).second )
	{
	elog	<< "xNetwork::addFakeClient> Failed to insert into "
		<< "fakeClientMap: "
		<< *fakeClient
		<< ", with controlling xClient: " ;

	if( 0 == ownerClient )
		{
		elog	<< "NULL" ;
		}
	else
		{
		elog	<< *ownerClient ;
		}
	elog	<< endl ;
	return false ;
	}

if( !numericMap.insert( make_pair( fakeClient->getIntYYXXX(),
	fakeClient ) ).second )
	{
	elog	<< "xNetwork::addFakeClient> Failed to add client "
		<< "to the numericMap: "
		<< *fakeClient
		<< endl ;

	fakeClientMap.erase( fakeClient->getIntYYXXX() ) ;
	freeClientNumeric( fakeClient->getIntYYXXX() ) ;

	return false ;
	}

addNick( fakeClient ) ;

/* Mark the iClient as being fake */
fakeClient->setFake();

return true ;
}

iServer* xNetwork::findFakeServer( const iServer* theServer ) const
{
assert( theServer != 0 ) ;
return findFakeServer( theServer->getIntYY() ) ;
}

iServer* xNetwork::findFakeServer( unsigned int intYY ) const
{
const_fakeServerIterator sItr = fakeServerMap.find( intYY ) ;
if( sItr == fakeServers_end() )
	{
	return 0 ;
	}
return sItr->second.first ;
}

iClient* xNetwork::removeFakeClient( iClient* fakeClient )
{
assert( fakeClient != 0 ) ;

//elog	<< "xNetwork::removeFakeClient> Removing client: "
//	<< *fakeClient
//	<< endl ;

fakeClientIterator cItr = fakeClientMap.find( 
	fakeClient->getIntYYXXX() ) ;
if( cItr == fakeClient_end() )
	{
	elog	<< "xNetwork::removeFakeClient> Unable to find fake "
		<< "client: "
		<< *fakeClient
		<< endl ;
	}
else
	{
	fakeClientMap.erase( cItr ) ;
	}

if( !freeClientNumeric( fakeClient->getIntYYXXX() ) )
	{
	elog	<< "xNetwork::removeFakeClient> Failed to free "
		<< "client numeric: "
		<< fakeClient->getIntYYXXX()
		<< endl ;
	}

// All successful
return fakeClient ;
}

iClient* xNetwork::findFakeNick( const string& nickName ) const
{
for( const_fakeClientIterator cItr = fakeClient_begin() ;
	cItr != fakeClient_end() ; ++cItr )
	{
	std::pair< iClient*, xClient* > clientPair = cItr->second ;
	if( !strcasecmp( clientPair.first->getNickName(),
		nickName ) )
		{
		return clientPair.first ;
		}
	}
return 0 ;
}

bool xNetwork::addFakeServer( iServer* fakeServer,
	xClient* owningClient )
{
assert( fakeServer != 0 ) ;
assert( owningClient != 0 ) ;

// Verify that the server name does not exist
if( findServerName( fakeServer->getName() ) != 0 )
	{
	elog	<< "xNetwork::addFakeServer> Server name already "
		<< "exists in normal list of iServers: "
		<< *fakeServer
		<< endl ;
	return false ;
	}

//elog	<< "xNetwork::addFakeServer> No matching name found for: "
//	<< *fakeServer
//	<< endl ;

// Allocate a new numeric
unsigned int intYY = 0 ;
if( !allocateServerNumeric( intYY ) )
	{
	elog	<< "xNetwork::addFakeServer> Failed to "
		<< "allocate fake numeric"
		<< endl ;
	return false ;
	}

// Set the intYY/charYY of the fakeServer
fakeServer->setIntYY( intYY ) ;

// Add the fakeServer into the fakeServerMap
if( !fakeServerMap.insert( make_pair( fakeServer->getIntYY(),
	make_pair( fakeServer, owningClient ) ) ).second )
	{
	elog	<< "xNetwork::addFakeServer> Failed to insert "
		<< "new server into fakeServerMap: "
		<< *fakeServer
		<< endl ;

	// A numeric had been allocated for this server,
	// free that numeric.
	freeServerNumeric( fakeServer->getIntYY() ) ;

	return false ;
	}

if( !serverMap.insert( make_pair( fakeServer->getIntYY(),
	fakeServer ) ).second )
	{
	elog	<< "xNetwork::addFakeServer> Failed to add new server "
		<< "to serverMap: "
		<< *fakeServer
		<< endl ;

	freeServerNumeric( fakeServer->getIntYY() ) ;
	fakeServerMap.erase( fakeServer->getIntYY() ) ;

	return false ;
	}

//elog	<< "xNetwork::addFakeServer> Successfully added fake "
//	<< "server: "
//	<< *fakeServer
//	<< endl ;

return true ;
}

iServer* xNetwork::findFakeServerName( const string& name ) const
{
for( const_fakeServerIterator sItr = fakeServers_begin() ;
	sItr != fakeServers_end() ; ++sItr )
	{
	if( !strcasecmp( name, sItr->second.first->getName() ) )
		{
		// Found it
		elog	<< "xNetwork::findServerName> Found name: "
			<< name
			<< ", matching server: "
			<< *(sItr->second.first)
			<< endl ;
		return sItr->second.first ;
		}
	} // for()

elog	<< "xNetwork::findFakeServerName> Unable to find server name: "
	<< name
	<< endl ;

return 0 ;
}

bool xNetwork::freeClientNumeric( unsigned int intYYXXX )
{
unsigned int intYY = 0 ;
unsigned int intXXX = 0 ;

splitbase64int( intYYXXX, intYY, intXXX ) ;

reservedNumeric_iterator rsItr =
	reservedNumericMap.find( intYY ) ;
if( rsItr == reservedNumericMap.end() )
	{
	elog	<< "xNetwork::freeClientNumeric> Unable to find "
		<< "server intYY for intYY/intXXX/intYYXXX: "
		<< intYY << '/'
		<< intXXX << '/'
		<< intYYXXX
		<< endl ;
	return false ;
	}

set< unsigned int >::iterator numItr = rsItr->second.find( intXXX ) ;
if( numItr == rsItr->second.end() )
	{
	elog	<< "xNetwork::freeClientNumeric> Unable to find "
		<< "client intXXX for intYY/intXXX/intYYXXX: "
		<< intYY << '/'
		<< intXXX << '/'
		<< intYYXXX
		<< endl ;
	return false ;
	}

//elog	<< "xNetwork::freeClientNumeric> Removing "
//	<< "intYY/intXXX/intYYXXX: "
//	<< intYY << '/'
//	<< intXXX << '/'
//	<< intYYXXX
//	<< endl ;

rsItr->second.erase( numItr ) ;
return true ;
}

bool xNetwork::allocateServerNumeric( unsigned int& intYY )
{
// Arbitrarily choose 200 as base numeric

// Look for an unassigned numeric.
// If the for loop traverses all possible values of
// unsigned int, it will eventually hit 0 again, and
// the loop will terminate.
for( intYY = 2000 ; intYY != 0 ; ++intYY )
	{
//	elog	<< "xNetwork::allocateServerNumeric> Checking: "
//		<< intYY
//		<< endl ;

	if( serverMap.find( intYY ) != serverMap.end() )
		{
		continue ;
		}
	if( reservedNumericMap.find( intYY ) != reservedNumericMap.end() )
		{
		continue ;
		}

	// Found an unused numeric
//	elog	<< "xNetwork::allocateServerNumeric> Found "
//		<< "unused numeric: "
//		<< intYY
//		<< endl ;

	if( !reservedNumericMap.insert( make_pair( intYY,
		set< unsigned int >() ) ).second )
		{
		elog	<< "xNetwork::allocateServerNumeric> Failed "
			<< "to reserve intYY: "
			<< intYY
			<< endl ;
		return false ;
		}
	return true ;
	} // for()

elog	<< "xNetwork::allocateServerNumeric> Looped unsigned "
	<< "int"
	<< endl ;
return false ;
}

bool xNetwork::freeServerNumeric( unsigned int intYY )
{
reservedNumeric_iterator sItr = reservedNumericMap.find( intYY ) ;
if( sItr == reservedNumericMap.end()  )
	{
	elog	<< "xNetwork::freeServerNumeric> Unable to find "
		<< "numeric: "
		<< intYY
		<< endl ;
	return false ;
	}

if( !sItr->second.empty() )
	{
	elog	<< "xNetwork::freeServerNumeric> Releasing numeric "
		<< intYY
		<< " which has "
		<< sItr->second.empty()
		<< " client numerics reserved"
		<< endl ;
	}

reservedNumericMap.erase( sItr ) ;
return true ;
}

iServer* xNetwork::removeFakeServer( iServer* fakeServer )
{
assert( fakeServer != 0 ) ;

fakeServerIterator sItr = fakeServerMap.find( fakeServer->getIntYY() ) ;
if( sItr == fakeServers_end() )
	{
	elog	<< "xNetwork::removeFakeServer> Unable to find fake "
		<< "server: "
		<< *fakeServer
		<< endl ;
	}
else
	{
	fakeServerMap.erase( sItr ) ;
	}

if( !freeServerNumeric( fakeServer->getIntYY() ) )
	{
	elog	<< "xNetwork::removeFakeServer> Failed to free "
		<< "server numeric: "
		<< fakeServer->getIntYY()
		<< endl ;
	}

// All successful
return fakeServer ;
}

iServer* xNetwork::removeFakeServerName( const string& name )
{
if( name.empty() )
	{
	elog	<< "xNetwork::removeFakeServerName> Empty name"
		<< endl ;
	return 0 ;
	}

fakeServerIterator sItr = fakeServers_begin() ;
for( ; sItr != fakeServers_end() ; ++sItr )
	{
	if( !strcasecmp( sItr->second.first->getName(), name ) )
		{
		// Found it
		elog	<< "xNetwork::removeFakeServerName> Found "
			<< "matching server name for name: "
			<< name
			<< ", server: "
			<< *(sItr->second.first)
			<< endl ;

		return removeFakeServer( sItr->second.first ) ;
		}
	} // for()

elog	<< "xNetwork::removeFakeServerName> Unable to find "
	<< "matching server name for name: "
	<< name
	<< endl ;

return 0 ;
}

iClient* xNetwork::findFakeClient( iClient* theClient ) const
{
assert( theClient != 0 ) ;

const_fakeClientIterator cItr = fakeClientMap.find( 
	theClient->getIntYYXXX() ) ;
if( cItr == fakeClientMap.end() )
	{
	return 0 ;
	}
return cItr->second.first ;
}

xClient* xNetwork::findFakeClientOwner( iClient* theClient ) const
{
assert( theClient != 0 ) ;

const_fakeClientIterator cItr = fakeClientMap.find( 
	theClient->getIntYYXXX() ) ;
if( cItr == fakeClientMap.end() )
	{
	return 0 ;
	}
return cItr->second.second ;
}

list< iClient* > xNetwork::findFakeClients( xClient* owningClient ) const
{
list< iClient* > retMe ;

// This hurts my brain a bit...
for( fakeClientMapType::const_iterator cItr = fakeClientMap.begin() ;
	cItr != fakeClientMap.end() ; ++cItr )
	{
	if( 0 == owningClient
		|| owningClient == cItr->second.second )
		{
		retMe.push_back( cItr->second.first ) ;
		}
	} // for
return retMe ;
}

/* function to search channels for a certain key */
list<const Channel*> xNetwork::getChannelsWithKey(const string& key) const
{
	list<const Channel*> retMe;

	for (const_channelIterator cptr = channels_begin(); (cptr != channels_end()); cptr++)
	{
		if ((cptr->second->getMode(Channel::MODE_K)) && (!match(key,cptr->second->getKey())))
			retMe.push_back(cptr->second);
	}
	return retMe;
}

/* function to search channels for matching modes */
list<const Channel*> xNetwork::getChannelsWithModes(const string& modes) const
{
	bool modeflag = false, foundMatch;
	const char *c;
	char ch;
	list<const Channel*> retMe;

	for (const_channelIterator cptr = channels_begin(); (cptr != channels_end()); cptr++)
	{
		/* parse the modes and check against the channel modes */
		/* don't need to initialise modeflag, as we know 'modes' starts with + or - */
		foundMatch = true;
		c = modes.c_str();
		while ((ch = *c++))
		{
			/* handle change of flags (+/-) */
			if (ch == '+')
			{
				modeflag = true;
				continue;
			}
			if (ch == '-')
			{
				modeflag = false;
				continue;
			}
			/* check modes */
			switch (ch) {
				case 'k':	/* keyed */
						if ((!modeflag && cptr->second->getMode(Channel::MODE_K))
							|| (modeflag && !cptr->second->getMode(Channel::MODE_K)))
						{
							foundMatch = false;
						}
						break;
				case 'l':	/* limit */
						if ((!modeflag && cptr->second->getMode(Channel::MODE_L))
							|| (modeflag && !cptr->second->getMode(Channel::MODE_L)))
						{
							foundMatch = false;
						}
						break;
				case 'i':	/* invite */
						if ((!modeflag && cptr->second->getMode(Channel::MODE_I))
							|| (modeflag && !cptr->second->getMode(Channel::MODE_I)))
						{
							foundMatch = false;
						}
						break;
				case 'm':	/* moderated */
						if ((!modeflag && cptr->second->getMode(Channel::MODE_M))
							|| (modeflag && !cptr->second->getMode(Channel::MODE_M)))
						{
							foundMatch = false;
						}
						break;
				case 'n':	/* no outside messages */
						if ((!modeflag && cptr->second->getMode(Channel::MODE_N))
							|| (modeflag && !cptr->second->getMode(Channel::MODE_N)))
						{
							foundMatch = false;
						}
						break;
				case 'p':	/* private */
						if ((!modeflag && cptr->second->getMode(Channel::MODE_P))
							|| (modeflag && !cptr->second->getMode(Channel::MODE_P)))
						{
							foundMatch = false;
						}
						break;
				case 's':	/* secret */
						if ((!modeflag && cptr->second->getMode(Channel::MODE_S))
							|| (modeflag && !cptr->second->getMode(Channel::MODE_S)))
						{
							foundMatch = false;
						}
						break;
				case 't':	/* no outside topics */
						if ((!modeflag && cptr->second->getMode(Channel::MODE_T))
							|| (modeflag && !cptr->second->getMode(Channel::MODE_T)))
						{
							foundMatch = false;
						}
						break;
				case 'r':	/* registered users only */
						if ((!modeflag && cptr->second->getMode(Channel::MODE_R))
							|| (modeflag && !cptr->second->getMode(Channel::MODE_R)))
						{
							foundMatch = false;
						}
						break;
				case 'R':	/* registered channel */
						if ((!modeflag && cptr->second->getMode(Channel::MODE_REG))
							|| (modeflag && !cptr->second->getMode(Channel::MODE_REG)))
						{
							foundMatch = false;
						}
						break;
				case 'D':	/* new .12 mode for large channels */
						if ((!modeflag && cptr->second->getMode(Channel::MODE_D))
							|| (modeflag && !cptr->second->getMode(Channel::MODE_D)))
						{
							foundMatch = false;
						}
						break;
				case 'A':	/* Apass for oplevels */
						if ((!modeflag && cptr->second->getMode(Channel::MODE_A))
							|| (modeflag && !cptr->second->getMode(Channel::MODE_A)))
						{
							foundMatch = false;
						}
						break;
				case 'U':	/* Upass for oplevels */
						if ((!modeflag && cptr->second->getMode(Channel::MODE_U))
							|| (modeflag && !cptr->second->getMode(Channel::MODE_U)))
						{
							foundMatch = false;
						}
						break;
				default:	/* other - should never happen */
						break;
			}
		}
		if (foundMatch)
			retMe.push_back(cptr->second);
	}
	return retMe;
}

#ifdef TOPIC_TRACK
/* function to search channels for a certain topic */
list<const Channel*> xNetwork::getChannelsWithTopic(const string& topic) const
{
	list<const Channel*> retMe;

	for (const_channelIterator cptr = channels_begin(); (cptr != channels_end()); cptr++)
	{
		if (!match(topic, cptr->second->getTopic()))
			retMe.push_back(cptr->second);
	}
	return retMe;
}

/* function to search channels with a certain topic setter */
list<const Channel*> xNetwork::getChannelsWithTopicBy(const string& topicby) const
{
	list<const Channel*> retMe;

	for (const_channelIterator cptr = channels_begin(); (cptr != channels_end()); cptr++)
	{
		if (!match(topicby, cptr->second->getTopicWhoSet()))
			retMe.push_back(cptr->second);
	}
	return retMe;
}
#endif
} // namespace gnuworld
