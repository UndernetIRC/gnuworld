/* Network.cc
 * Author: Daniel Karrels dan@karrels.com
 */

#include	<iostream>
#include	<string>
#include	<hash_map>

#include	<cstring>
#ifndef NDEBUG
  #include	<cassert>
#endif

#include	"Network.h"
#include	"ELog.h"
#include	"iClient.h"
#include	"iServer.h"
#include	"Channel.h"
#include	"client.h"
#include	"misc.h"

const char xNetwork_h_rcsId[] = __XNETWORK_H ;
const char xNetwork_cc_rcsId[] = "$Id: Network.cc,v 1.2 2000/07/06 19:13:07 dan_karrels Exp $" ;

using std::string ;
using std::endl ;
using std::hash_map ;
using std::hash ;

namespace gnuworld
{

xNetwork::xNetwork()
{}

xNetwork::~xNetwork()
{}

bool xNetwork::addClient( iClient* newClient )
{
#ifndef NDEBUG
  assert( NULL != newClient ) ;
#endif

// Make sure the client is on a valid server
networkVectorType::size_type YY =
	static_cast< networkVectorType::size_type >( newClient->getIntYY() ) ;

// We should definitely have all servers before receiving
// client bursts.
if( YY >= clients.size() )
	{
	// No server!
	elog	<< "xNetwork::addClient> Server not found." << endl ;
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
	elog	<< "xNetwork::addClient: Numeric collision" << endl ;

	// Delete the old one
	delete clients[ YY ][ XXX ] ;
	}

clients[ YY ][ XXX ] = newClient ;
servers[ YY ]->incrementClients() ;

addNick( newClient ) ;

return true ;
}

// xClients are local to this server, they
// must be handled a little differently
// This method needs to assign numerics
// to the client -- the client doesn't
// already have a numeric.
bool xNetwork::addClient( xClient* newClient )
{
#ifndef NDEBUG
  assert( NULL != newClient ) ;
#endif

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

newClient->setIntXXX( pos ) ;

return true ;

}

bool xNetwork::addServer( iServer* newServer )
{
#ifndef NDEBUG
  assert( newServer != NULL ) ;
#endif

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
	elog	<< "xNetwork::addServer> Numeric collission!\n" ;

	// The show must go on!
	delete servers[ YY ] ;

	}

servers[ YY ] = newServer ;

//elog << "addServer> " << *newServer << endl ;

return true ;
}

bool xNetwork::addChannel( Channel* theChan )
{
#ifndef NDEBUG
  assert( theChan != NULL ) ;
#endif

//elog << "Adding channel: " << *theChan << endl ;

return channelMap.insert(
	channelMapType::value_type( theChan->getName().c_str(),
	theChan ) ).second ;

}

iClient* xNetwork::findClient( const unsigned int& YY,
	const unsigned int& XXX ) const
{
if( static_cast< serverVectorType::size_type >( YY ) >= servers.size()
	|| NULL == servers[ YY ] )
	{
	elog		<< "xNetwork::findClient> Server not found: "
			<< YY << endl ;
	return 0 ;
	}

// Server exists, but does the client?
if( static_cast< networkVectorType::size_type >( XXX ) >= clients[ YY ].size() )
	{
	// Client not found
//	elog	<< "xNetwork::findClient> Client not found: "
//		<< XXX << endl ;
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
nickMapType::const_iterator ptr = nickMap.find( nick.c_str() ) ;
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
	if( !strcasecmp( nickName.c_str(), localClients[ i ]->getNickName().c_str() ) )
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
	if( !strcasecmp( servers[ i ]->getName().c_str(), name.c_str() ) )
		{
		return servers[ i ] ;
		}
	}
return NULL ;
}

Channel* xNetwork::findChannel( const string& name ) const
{
return channelMap.find( name.c_str() )->second ;
}

iClient* xNetwork::removeClient( const unsigned int& YY,
	const unsigned int& XXX )
{

if( static_cast< networkVectorType::size_type >( YY ) >= clients.size() )
	{
	elog	<< "xNetwork::removeClient> Bad YY\n" ;
	return 0 ;
	}
if( static_cast< clientVectorType::size_type >( XXX ) >= clients[ YY ].size() )
	{
	elog	<< "xNetwork::removeClient> Bad XXX\n" ;
	return 0 ;
	}

iClient* retMe = clients[ YY ][ XXX ] ;
clients[ YY ][ XXX ] = NULL ;

if( retMe != NULL )
	{
	removeNick( retMe->getNickName() ) ;
	}

#ifdef EDEBUG
if( NULL == retMe )
	{
	elog	<< "xNetwork::removeClient( int, int )> Unable to find user "
		<< *retMe << endl ;
	// Non-fatal
	}
#endif

servers[ YY ]->decrementClients() ;

// Remove all associations between client->channel
iClient::channelIterator chanPtr = retMe->channels_begin() ;
while( chanPtr != retMe->channels_end() )
	{
	delete (*chanPtr)->removeUser( retMe ) ;

	if( (*chanPtr)->empty() )
		{
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

void xNetwork::removeNick( const string& nick )
{
nickMap.erase( nick.c_str() ) ;
}

/**
 * Remove a server by numeric from the network tables.
 * This will deallocate any clients on that server.
 * A pointer to the server being removed is returned.
 * The memory space for this iServer is NOT deallocated
 * by this method, so it's up to the caller to deallocate
 * any heap memory.
 */
iServer* xNetwork::removeServer( const unsigned int& YY )
{

// Make sure the server numeric (YY) is valid.
if( static_cast< serverVectorType::size_type >( YY ) >= servers.size() )
	{
	elog	<< "xNetwork::removeServer> Bad YY\n" ;
	return 0 ;
	}

// Delete each client on this server
for( clientVectorType::size_type i = 0 ; i < clients[ YY ].size() ; i++ )
	{

	// It's possible that this iClient* may point to NULL.
	iClient* theClient = clients[ YY ][ i ] ;
	if( NULL == theClient )
		{
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

	delete theClient ;
	clients[ YY ][ i ] = NULL ;
	}	

// Erase the client vector associated with this server
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
channelMapType::iterator ptr = channelMap.find( name.c_str() ) ;
if( ptr == channelMap.end() )
	{
	return 0 ;
	}
channelMap.erase( ptr ) ;
return ptr->second ;
}

void xNetwork::rehashNick( const string& yyxxx,
	const string& newNick )
{
iClient* theClient = findClient( yyxxx ) ;
if( NULL == theClient )
	{
	elog	<< "xNetwork::rehashNick> Unable to find nick\n" ;
	return ;
	}

// No need to keep track of the return value here,
// we already have it.
removeNick( theClient->getNickName() ) ;

// Change the client's nickname
theClient->setNickName( newNick ) ;

// Add the client back to the nickname table
addNick( theClient ) ;

// Note that the user's numeric never changes,
// so no need to rehash numeric

}

void xNetwork::addNick( iClient* theClient )
{
nickMap.insert( nickMapType::value_type(
	theClient->getNickName().c_str(), theClient ) ) ;
}

/**
 * Handle a netsplit.
 * Per requirements, this method does NOT remove or deallocate
 * the server referenced by intYY.
 * Our uplink server has its own numeric as its uplinkIntYY.
 */
void xNetwork::OnSplit( const unsigned int& intYY )
{
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
		delete removeServer( servers[ i ]->getIntYY() ) ;

		// Remove servers[ i ]'s leaf servers.
		OnSplit( removeIntYY ) ;

		}
	}
}

} // namespace gnuworld
