/* Network.cc
 * Author: Daniel Karrels dan@karrels.com
 */

#include	<new>
#include	<iostream>
#include	<string>
#include	<list>
#include	<hash_map>
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

const char xNetwork_h_rcsId[] = __NETWORK_H ;
const char xNetwork_cc_rcsId[] = "$Id: Network.cc,v 1.28 2001/06/14 22:14:13 dan_karrels Exp $" ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char iClient_h_rcsId[] = __ICLIENT_H ;
const char Channel_h_rcsId[] = __CHANNEL_H ;
const char client_h_rcsId[] = __CLIENT_H ;
const char misc_h_rcsId[] = __MISC_H ;
const char Numeric_h_rcsId[] = __NUMERIC_H ;
const char match_h_rcsId[] = __MATCH_H ;
const char StringTokenizer_cc_rcsId[] = __STRINGTOKENIZER_H ;

namespace gnuworld
{

using std::string ;
using std::endl ;
using std::list ;
using std::hash_map ;
using std::hash ;
using std::unary_function ;

xNetwork::xNetwork()
{}

xNetwork::~xNetwork()
{
// TODO: Establish protocol here about who deletes all clients/server/etc
}

bool xNetwork::addClient( iClient* newClient )
{
assert( NULL != newClient ) ;

// Make sure the client is on a valid server
networkVectorType::size_type YY =
	static_cast< networkVectorType::size_type >( newClient->getIntYY() ) ;

// We should definitely have all servers before receiving
// client bursts.
if( YY >= clients.size() )
	{
	char charYY[ 3 ] = { 0 } ;

	// No server!
	elog	<< "xNetwork::addClient> Server not found: "
		<< YY
		<< "("
		<< inttobase64( charYY, YY, 2 )
		<< ")"
		<< endl ;
	return false ;
	}

// Do we need to add this client to the end of the list,
// i.e., a new numeric?
clientVectorType::size_type XXX =
	static_cast< clientVectorType::size_type >( newClient->getIntXXX() ) ;

// Is this a new numeric?
while( XXX >= clients[ YY ].size() )
	{
	clients[ YY ].push_back( NULL ) ;
	}

// Is another client already occupying this slot?
if( clients[ YY ][ XXX ] != NULL )
	{
	// Numeric collission
	elog	<< "xNetwork::addClient> Numeric collision: " 
		<< newClient->getCharYYXXX()
		<< endl ;

	// Delete the old one
	delete clients[ YY ][ XXX ] ;
	}

clients[ YY ][ XXX ] = newClient ;
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

// First, find a new numeric for this client
xClientVectorType::size_type pos = 0 ;
for( ; pos < localClients.size() ; pos++ )
	{
	if( NULL == localClients[ pos ] )
		{
		break ;
		}
	}

// Were there any spots available?
if( pos >= localClients.size() )
	{
	// Nope, make a new one
	localClients.push_back( NULL ) ;
	}

// pos is now the index of the next empty
// slot in the localClients vector
localClients[ pos ] = newClient ;

// Give the new client a numeric
newClient->setIntXXX( pos ) ;

return true ;
}

bool xNetwork::addServer( iServer* newServer )
{
assert( newServer != NULL ) ;

serverVectorType::size_type YY =
	static_cast< serverVectorType::size_type >( newServer->getIntYY() ) ;

// It is possible that numerics received are out of order
while( YY >= servers.size() )
	{
	// Make sure to update both the servers and
	// clients array as they are synchronized
	// with each other.
	servers.push_back( NULL ) ;
	clients.push_back( clientVectorType() ) ;
	}

if( NULL != servers[ YY ] )
	{
	elog	<< "xNetwork::addServer> Numeric collission: "
		<< newServer->getCharYY()
		<< endl ;

	// The show must go on!
	delete servers[ YY ] ;
	// TODO: Clear the clients for this particular server

	}

servers[ YY ] = newServer ;
clients[ YY ].clear() ;

// Optimize a bit
clients[ YY ].reserve( newServer->getIntXXX() ) ;

//elog << "addServer> " << *newServer << endl ;

return true ;
}

bool xNetwork::addChannel( Channel* theChan )
{
assert( theChan != NULL ) ;

//elog << "Adding channel: " << *theChan << endl ;

return channelMap.insert(
	channelMapType::value_type( theChan->getName(),
		theChan ) ).second ;
}

iClient* xNetwork::findClient( const unsigned int& YY,
	const unsigned int& XXX ) const
{
if( static_cast< serverVectorType::size_type >( YY ) >= servers.size()
	|| NULL == servers[ YY ] )
	{
//	elog	<< "xNetwork::findClient> Server not found: "
//		<< YY << endl ;
	return 0 ;
	}

// Server exists, but does the client?
if( static_cast< networkVectorType::size_type >( XXX ) >= clients[ YY ].size() )
	{
	// Client not found
//	elog	<< "xNetwork::findClient> Client not found: "
//		<< "YY: " << YY << ", XXX: " << XXX << endl ;
	return 0 ;
	}

// This could still return NULL
return clients[ YY ][ XXX ] ;
}

iClient* xNetwork::findClient( const string& yyxxx ) const
{
unsigned int	yy = 0,
		xxx = 0 ;

if( yyxxx.size() == 5 )
	{
	// n2k
	yy = base64toint( yyxxx.c_str(), 2 ) ;
	xxx = base64toint( yyxxx.c_str() + 2, 3 ) ;
	}
else if( yyxxx.size() == 3 )
	{
	// yxx
	yy = base64toint( yyxxx.c_str(), 1 ) ;
	xxx = base64toint( yyxxx.c_str() + 1, 2 ) ;
	}
else
	{
	return NULL ;
	}

return findClient( yy, xxx ) ;
}

iClient* xNetwork::findNick( const string& nick ) const
{
nickMapType::const_iterator ptr = nickMap.find( nick ) ;
if( ptr == nickMap.end() )
	{
	return 0 ;
	}
return ptr->second ;
}

xClient* xNetwork::findLocalClient( const unsigned int& YY,
	const unsigned int& XXX ) const
{
// Nothing fancy for now, just linear iteration
for( xClientVectorType::size_type i = 0 ; i < localClients.size() ; i++ )
	{
	if( NULL == localClients[ i ] )
		{
		continue ;
		}
	if( YY == localClients[ i ]->getIntYY() &&
		XXX == localClients[ i ]->getIntXXX() )
		{
		return localClients[ i ] ;
		}
	}
return 0 ;
}

xClient* xNetwork::findLocalClient( const string& yyxxx ) const
{
unsigned int	yy = 0,
		xxx = 0 ;

if( yyxxx.size() == 5 )
	{
	// n2k
	yy = base64toint( yyxxx.c_str(), 2 ) ;
	xxx = base64toint( yyxxx.c_str() + 2, 3 ) ;
	}
else
	{
	// yxx
	yy = base64toint( yyxxx.c_str(), 1 ) ;
	xxx = base64toint( yyxxx.c_str() + 1, 2 ) ;
	}

return findLocalClient( yy, xxx ) ;
}

xClient* xNetwork::findLocalNick( const string& nickName ) const
{
for( xClientVectorType::size_type i = 0 ; i < localClients.size() ; i++ )
	{
	if( NULL == localClients[ i ] )
		{
		continue ;
		}
	if( !strcasecmp( nickName, localClients[ i ]->getNickName() ) )
		{
		return localClients[ i ] ;
		}
	}
return 0 ;
}

iServer* xNetwork::findServer( const string& YY ) const
{
return findServer( base64toint( YY.c_str(), YY.size() ) ) ;
}

iServer* xNetwork::findServer( const unsigned int& YY ) const
{
if( static_cast< serverVectorType::size_type >( YY ) >= servers.size() )
	{
	return 0 ;
	}
return servers[ YY ] ;
}

iServer* xNetwork::findServerName( const string& name ) const
{
for( serverVectorType::size_type i = 0 ; i < servers.size() ; i++ )
	{
	if( NULL == servers[ i ] )
		{
		continue ;
		}
	if( !strcasecmp( servers[ i ]->getName(), name ) )
		{
		return servers[ i ] ;
		}
	}
return NULL ;
}

Channel* xNetwork::findChannel( const string& name ) const
{
channelMapType::const_iterator ptr = channelMap.find( name ) ;
if( ptr == channelMap.end() )
	{
//	elog	<< "xNetwork::findChannel> Failed to find: "
//		<< name << endl ;
	return 0 ;
	}
return ptr->second ;
}

iClient* xNetwork::removeClient( const unsigned int& YY,
	const unsigned int& XXX )
{

if( static_cast< networkVectorType::size_type >( YY ) >= clients.size() )
	{
	elog	<< "xNetwork::removeClient> Bad YY: "
		<< YY
		<< endl ;
	return 0 ;
	}
if( static_cast< clientVectorType::size_type >( XXX ) >= clients[ YY ].size() )
	{
	elog	<< "xNetwork::removeClient> Bad XXX: "
		<< XXX
		<< endl ;
	return 0 ;
	}

iClient* retMe = clients[ YY ][ XXX ] ;
clients[ YY ][ XXX ] = NULL ;

if( retMe != NULL )
	{
	removeNick( retMe->getNickName() ) ;
	}

if( NULL == retMe )
	{
	elog	<< "xNetwork::removeClient( int, int )> Unable to find user "
		<< "YY: "
		<< YY
		<< ", XXX: "
		<< XXX
		<< endl ;
	return 0 ;
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
	delete theChanUser ;

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

iClient* xNetwork::removeClient( const string& yyxxx )
{
unsigned int	yy = 0,
		xxx = 0 ;

if( yyxxx.size() == 5 )
	{
	// n2k
	yy = base64toint( yyxxx.c_str(), 2 ) ;
	xxx = base64toint( yyxxx.c_str() + 2, 3 ) ;
	}
else
	{
	// yxx
	yy = convert2n[ yyxxx[ 0 ] ] ;
	xxx = base64toint( yyxxx.c_str() + 1, 2 ) ;
	}
return removeClient( yy, xxx ) ;
}

iClient* xNetwork::removeClient( iClient* theClient )
{
assert( theClient != 0 ) ;

return removeClient( theClient->getIntYY(), theClient->getIntXXX() ) ;
}

xClient* xNetwork::removeLocalClient( xClient* theClient )
{
assert( theClient != 0 ) ;

for( xClientVectorType::size_type i = 0 ; i < localClients.size() ; ++i )
	{
	if( localClients[ i ] == theClient )
		{
		localClients[ i ] = 0 ;
		return theClient ;
		}
	}
return 0 ;
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

// Make sure the server numeric (YY) is valid.
if( static_cast< serverVectorType::size_type >( YY ) >= servers.size() )
	{
	elog	<< "xNetwork::removeServer> Bad YY: "
		<< YY
		<< endl ;
	return 0 ;
	}

//elog	<< "xNetwork::removeServer> Removing server " << YY
//	<< ", name: " << servers[ YY ]->getName() << endl ;

// Delete each client on this server
for( clientVectorType::size_type i = 0 ; i < clients[ YY ].size() ; i++ )
	{

	// It's possible that this iClient* may point to NULL.
	iClient* theClient = clients[ YY ][ i ] ;
	if( NULL == theClient )
		{
		// Unused numeric, no big deal
//		elog << "xNetwork::removeServer> Found NULL iClient\n" ;
		continue ;
		}

	// Remove channel->client associations
	iClient::channelIterator chanPtr = theClient->channels_begin() ;
	while( chanPtr != theClient->channels_end() )
		{
		delete (*chanPtr)->removeUser( theClient ) ;

		// Is the channel now empty?
		if( (*chanPtr)->empty() )
			{
			// The channel is now empty, there is no worry
			// of finding another user on this server
			// which is in this channel (it's empty!)
			// This removal should be safe...
			delete removeChannel( (*chanPtr)->getName() ) ;
			}

		// Even if the channel is deallocated and removed
		// from the channel table, this increment here is still
		// safe.
		++chanPtr ;
		}
	theClient->clearChannels() ;

	// Should we post this as an EVT_QUIT event?
	if( postEvent )
		{
		// Yes, post the event
		theServer->PostEvent( EVT_QUIT,
			static_cast< void* >( theClient ) ) ;
		}

	removeNick( theClient->getNickName() ) ;
	delete theClient ;
	clients[ YY ][ i ] = NULL ;
	}	

// Clear the client vector associated with this server
clients[ YY ].clear() ;

iServer* retMe = servers[ YY ] ;
servers[ YY ] = NULL ;
return retMe ;
}

iServer* xNetwork::removeServer( const string& YY )
{
return removeServer( base64toint( YY.c_str() ) ) ;
}

iServer* xNetwork::removeServerName( const string& name )
{
iServer* theServer = findServerName( name ) ;
if( NULL == theServer )
	{
	return NULL ;
	}
return removeServer( theServer->getIntYY() ) ;
}

Channel* xNetwork::removeChannel( const string& name )
{
channelMapType::iterator ptr = channelMap.find( name ) ;
if( ptr == channelMap.end() )
	{
	elog	<< "xNetwork::removeChannel> Failed to find channel: "
		<< name
		<< endl ;
	return 0 ;
	}
channelMap.erase( ptr ) ;
return ptr->second ;
}

Channel* xNetwork::removeChannel( const Channel* theChan )
{
assert( theChan != 0 ) ;

return removeChannel( theChan->getName() ) ;
}

void xNetwork::rehashNick( const string& yyxxx,
	const string& newNick )
{
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

}

void xNetwork::addNick( iClient* theClient )
{
// This is a protected method, theClient is guaranteed to 
// be non-NULL
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
 * Per requirements, this method does NOT remove or deallocate
 * the server referenced by intYY.
 * Our uplink server has its own numeric as its uplinkIntYY.
 */
void xNetwork::OnSplit( const unsigned int& intYY )
{
// TODO: Post events
for( serverVectorType::size_type i = 0 ; i < servers.size() ; ++i )
	{
	if( NULL == servers[ i ] )
		{
		}
	else if( servers[ i ]->getUplinkIntYY() == servers[ i ]->getIntYY() )
		{
		// It's our uplink.  Prevent infinite recursion here
		// by just ignoring this server.
//		elog	<< "xServer::OnSplit> Found uplink\n" ;
		}
	else if( servers[ i ]->getUplinkIntYY() == intYY )
		{
		// servers[ i ] is a leaf to intYY

//		elog	<< "xNetwork::OnSplit> Removing "
//			<< servers[ i ]->getName() << endl ;

		// Remember the numeric to be removed.
		const unsigned int removeIntYY = servers[ i ]->getIntYY() ;

		// Remove servers[ i ].
		delete removeServer( servers[ i ]->getIntYY(), true ) ;

		// Remove servers[ i ]'s leaf servers.
		OnSplit( removeIntYY ) ;

		}
	}
}

size_t xNetwork::serverList_size() const
{
size_t i = 0 ;
for( serverVectorType::const_iterator ptr = servers.begin() ;
	ptr != servers.end() ; ++ptr )
	{
	if( *ptr != NULL )
		{
		++i ;
		}
	}
return i ;
}

size_t xNetwork::clientList_size() const
{
size_t i = 0 ;
for( networkVectorType::const_iterator ptr = clients.begin() ;
	ptr != clients.end() ; ++ptr )
	{
	for( clientVectorType::size_type cPtr = 0 ; cPtr < (*ptr).size() ;
		++cPtr )
		{
		if( (*ptr)[ cPtr ] != NULL )
			{
			++i ;
			}
		}
	}
return i ;
}

void xNetwork::foreach_xClient( xNetwork::fe_xClientBase f )
{
std::for_each( localClients.begin(), localClients.end(), f ) ;
}

size_t xNetwork::countClients( const iServer* theServer ) const
{
assert( theServer != 0 ) ;

// Find the server in the internal server table
serverVectorType::size_type serverID = 0 ;
for( ; serverID < servers.size() ; ++serverID )
	{
	if( theServer == servers[ serverID ] )
		{
		// Found it
		break ;
		}
	}

// Was the server found?
if( serverID >= servers.size() )
	{
	// Did not find the server
	return 0 ;
	}

// Iterate through this server's info and count the number
// of clients

// numClients will hold the number of clients found
size_t numClients = 0 ;

for( clientVectorType::size_type clientID = 0 ;
	clientID < clients[ serverID ].size() ; ++clientID )
	{
	// Only increment numClients if this client is non-NULL
	if( NULL != clients[ serverID ][ clientID ] )
		{
		++numClients ;
		}
	}

// Return the number of clients found to the caller
return numClients ;
}

list< const iClient* > xNetwork::matchHost( const string& wildHost ) const
{
list< const iClient* > retMe ;

for( networkVectorType::const_iterator sPtr = clients.begin() ;
	sPtr != clients.end() ; ++sPtr )
	{

	for( clientVectorType::const_iterator cPtr = (*sPtr).begin() ;
		cPtr != (*sPtr).end() ; ++cPtr )
		{
		if( NULL == *cPtr )
			{
			// No client present at this numeric,
			// no big deal
			continue ;
			}

		if( !match( wildHost, (*cPtr)->getInsecureHost() ) )
			{
			// Found a match
			retMe.push_back( *cPtr ) ;
			}
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
list< const iClient* > retMe( matchingHosts.size() ) ;

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
return matchUserHost( wildUserHost ).size() ;
}

list< const iClient* > xNetwork::findHost( const string& hostName ) const
{
list< const iClient* > retMe ;

for( networkVectorType::const_iterator sPtr = clients.begin() ;
	sPtr != clients.end() ; ++sPtr )
	{

	for( clientVectorType::const_iterator cPtr = (*sPtr).begin() ;
		cPtr !=(*sPtr).end() ; ++cPtr )
		{
		if( NULL == *cPtr )
			{
			// No client present at this numeric,
			// no big deal
			continue ;
			}

		if( !strcasecmp( hostName, (*cPtr)->getInsecureHost() ) )
			{
			// Found a match
			retMe.push_back( *cPtr ) ;
			}
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

} // namespace gnuworld
