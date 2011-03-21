/**
 * Network.h
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
 * $Id: Network.h,v 1.48 2007/04/27 19:30:43 mrbean_ Exp $
 */

#ifndef __NETWORK_H
#define __NETWORK_H "$Id: Network.h,v 1.48 2007/04/27 19:30:43 mrbean_ Exp $"

#include	<vector>
#include	<string>
#include	<set>
#include	<map>
#include	<list>

#include	<ctime>

#include	"iServer.h"
#include	"iClient.h"
#include	"Channel.h"
#include	"client.h"
#include	"misc.h" // struct noCaseCompare
#include	"gnuworld_config.h"

namespace gnuworld
{

class xServer ;

/**
 * This class is responsible for storing the network's iClient's,
 * iServer's, and local xClient's.  This is done using several
 * tables, some of which are a bit non-trivial to understand at
 * first, but are actually very simple.
 * This is a big container class, mutated primarily by the
 * xServer instance.
 *
 * The following semantics about the addition and removal of
 * fake clients and servers is followed:
 *
 * addFake{client,server}
 * - Adds fake element of the {client,server}
 * - Adds normal element of the {client,server}
 *
 * add{Client,Server}
 * - Adds normal element of the {client,server}
 *
 * removeFake{client,server}
 * - Removes fake element of {client,server}
 * 
 * remove{Client,Server}
 * - Removes normal element of {client,server}
 * - Removes fake element of {client,server}
 *
 * None of the remove* methods deallocate the {client,server}.
 * This also means that OnSplit() will properly clean up even
 * fake servers and clients.
 */
class xNetwork
{
	/// Allow xServer to access this class's private
	/// members/methods.
	friend class xServer ;

private:

	/**
	 * Type used to store all reserved intYY and intYYXXX's
	 * for the local server and juped iClient/xClient/iServer's.
	 * Keyed top level by the server's intYY, then each
	 * set is keyed by intXXX for that particular server.
	 */
	typedef std::map< unsigned int, std::set< unsigned int > >
		reservedNumericMapType ;

	/**
	 * This is an iterator type definition for the
	 * reservedNumericMapType.
	 */
	typedef reservedNumericMapType::iterator 
		reservedNumeric_iterator ;

	/**
	 * This type maps fake iClient's to their owning
	 * xClient's, keyed by the iClient's intYYXXX numeric.
	 */
	typedef std::map< unsigned int, std::pair< iClient*, xClient* > >
		fakeClientMapType ;

	/**
	 * This type stores fake (juped) servers.
	 */
	typedef std::map< unsigned int, std::pair< iServer*, xClient* > >
		fakeServerMapType ;

	/**
	 * This is the type of vector for storing
	 * local (services) clients.
	 */
	typedef std::map< unsigned int, xClient* > xClientMapType ;

	/**
	 * This is the type used to store the network Channel
	 * instances.
	 */
	typedef std::map< std::string, Channel*, noCaseCompare > 
			channelMapType ;

	/**
	 * This is the type used to store the nick name/iClient* pairs.
	 */
	typedef std::map< std::string, iClient*, noCaseCompare > 
			nickMapType ;

	/**
	 * The type used to store information about iClient's,
	 * keyed by integer numeric.
	 */
	typedef std::map< unsigned int, iClient* > numericMapType ;

	/**
	 * The type used to store information about iServer's,
	 * keyed by integer numeric.
	 */
	typedef std::map< unsigned int, iServer* > serverMapType ;

public:

	/**
	 * The default constructor handles all of the default
	 * allocation and initialization.
	 */
	xNetwork() ;

	/**
	 * The default destructor will deallocate all objects
	 * that are not explicitly removed from this structure.
	 * Therefore, it is highly recommend that the application
	 * programmer iterate through this structure and remove
	 * all items before destroying this structure.
	 */
	virtual ~xNetwork() ;

	/* Addition methods
	 * These methods return true if addition is successful,
	 * false otherwise.
	 */

	/**
	 * Add a new iClient to the network.  This will also handle
	 * any additions into the nickname table, if required.
	 * Returns false if the addition fails.
	 */
	virtual bool		addClient( iClient* ) ;

	/**
	 * Add a fake client to this (or a juped) server.  The
	 * xClient is the "owner" of the iClient.
	 * This method will fill in the iClient's intXXX/charXXX,
	 * but the intYY/charYY must be valid.
	 */
	virtual bool		addFakeClient( iClient* theClient,
					xClient* ownerClient ) ;

	/**
	 * Add a services client to the network table.
	 * This is stored in the table of local clients.
	 * Returns false if the addition fails.
	 */
	virtual bool		addClient( xClient* ) ;

	/**
	 * Add a new server to the network table.
	 * Returns false if the addition fails.
	 */
	virtual bool		addServer( iServer* ) ;

	/**
	 * Add a new fake (juped) server to the structure
	 * of fake servers.
	 * Returns true on success, false on failure.
	 */
	virtual bool		addFakeServer( iServer*, xClient* ) ;

	/**
	 * Add a new channel to the network table.
	 * Returns false if the addition fails.
	 */
	virtual bool		addChannel( Channel* ) ;

	/*
	 * All nickname based searches are case insensitive.
	 */

	/**
	 * Find a remote client given the client's integer numeric.
	 * Returns NULL if not found.
	 */
	virtual iClient*	findClient( const unsigned int& YY,
					const unsigned int& XXX ) const ;

	/**
	 * Find a remote client given the client's integer numeric.
	 * Returns NULL if not found.
	 */
	virtual iClient*	findClient( const unsigned int& intYYXXX )
					const ;

	/**
	 * Find a fake client given the client's integer numeric.
	 * Returns NULL if not found.
	 */
	virtual iClient*	findFakeClient( const unsigned int& YY,
					const unsigned int& XXX ) const ;

	/**
	 * Find a remote client given the client's character numeric.
	 * Returns NULL if not found.
	 */
	virtual iClient*	findClient( const std::string& yyxxx ) 
					const ;

	/**
	 * Find a fake client by numeric.
	 * Returns NULL if not found.
	 */
	virtual iClient*	findFakeClient( const std::string& yyxxx )
					const ;

	/**
	 * Lookup a fake (juped) iClient.
	 */
	virtual iClient*	findFakeClient( iClient* ) const ;

	/**
	 * Lookup the xClient owner of a particular iClient.
	 */
	virtual xClient*	findFakeClientOwner( iClient* ) const ;

	/**
	 * Lookup all fake iClients belonging to an xClient.
	 * If the xClient is NULL, return all fake clients.
	 */
	virtual std::list< iClient* > findFakeClients( xClient* ) const ;

	/**
	 * Retrieve a pointer to an iClient given its nick name.
	 * Returns NULL if not found.
	 */
	virtual iClient*	findNick( const std::string& nickName ) 
					const ;

	/**
	 * Find a local (services) client by its character numeric.
	 * Returns NULL if not found.
	 */
	virtual xClient*	findLocalClient( const std::string& yyxxx )
					const ;

	/**
	 * Find a local (services) client by its integer numeric.
	 * Returns NULL if not found.
	 */
	virtual xClient*	findLocalClient( const unsigned int& intYYXXX )
					const ;

	/**
	 * Find a local (services) client by its case-insensitive nickname.
	 * Returns NULL if not found.
	 */
	virtual xClient*	findLocalNick( const std::string& nickName )
					const ;

	/**
	 * Lookup a fake (juped) iClient by nickname.
	 */
	virtual iClient*	findFakeNick( const std::string& nickName )
					const ;

	/**
	 * Find a remote server by its integer numeric.
	 * Returns NULL if not found.
	 */
	virtual iServer*	findServer( const unsigned int& YY ) const ;

	/**
	 * Find a fake server, return that server if it is fake (juped),
	 * or NULL otherwise.
	 */
	virtual iServer*	findFakeServer( const iServer* ) const ;

	/**
	 * Find a fake server described by the given intYY numeric.
	 * Return 0 on failure.
	 */
	virtual iServer*	findFakeServer( unsigned int intYY ) const ;

	/**
	 * Find a remote server by its character numeric.
	 * Returns NULL if not found.
	 */
	virtual iServer*	findServer( const std::string& YY ) const ;

	/**
	 * Find a remote server by its case-insensitive name.
	 * Returns NULL if not found.
	 */
	virtual iServer*	findServerName( const std::string& name ) 
					const ;

	/**
	 * Find a fake server by (case insensitive) name.
	 */
	virtual iServer*	findFakeServerName( const std::string& ) 
					const ;

	/**
	 * Find a remote server by a wildmask name.
	 * Returns NULL if not found.
	 */
	virtual iServer*	findExpandedServerName(
					const std::string& name ) const ;
	/**
	 * Returns all the servers which are currently in bursting state
	 */
	virtual std::list< iServer* > getAllBurstingServers();

	/**
	 * Find a global channel by case insensitive name.
	 * Returns NULL if not found.
	 */
	virtual Channel*	findChannel( const std::string& name ) 
					const ;

	/* Removal methods. */

	/**
	 * Remove a remote client by integer numeric and return a
	 * pointer to the iClient.	
	 * Return NULL if not found.
	 */
	virtual iClient*	removeClient(
					const unsigned int& intYYXXX ) ;

	/**
	 * Remove a remote client by integer server and client numerics,
	 * and return a pointer to the iClient.
	 * Returns NULL if not found.
	 */
	virtual iClient*	removeClient( const unsigned int& YY,
					const unsigned int& XXX ) ;

	/**
	 * Remove a remote client by character numeric, and return a
	 * pointer to it.
	 * Returns NULL if not found.
	 */
	virtual iClient*	removeClient( const std::string& yyxxx ) ;

	/**
	 * Remove a remote client by its pointer.
	 * This will NOT deallocate the iClient instance.
	 */
	virtual iClient*	removeClient( iClient* ) ;

	/**
	 * Remove a local client from the network data tables.
	 */
	virtual xClient*	removeLocalClient( xClient* ) ;

	/**
	 * Remove a fake client from the network data structures
	 * and deallocate its reserved numeric.
	 */
	virtual iClient*	removeFakeClient( iClient* ) ;

	/**
	 * Remove a fake server from the network data structures
	 * and deallocate its reserved numeric.
	 */
	virtual iServer*	removeFakeServer( iServer* ) ;

	/**
	 * Remove a fake iServer from the network data structures
	 * by (case-insensitive) name, and remove its reserved
	 * numeric.
	 */
	virtual iServer*	removeFakeServerName( const std::string& ) ;

	/**
	 * Remove a nick name from the internal nick name table.
	 * Note that this returns void.  This is because nick names
	 * are cross referenced in the nick name table.  That is,
	 * no heap space is allocated for elements in the nick name
	 * table, they simply point to iClient instances.
	 */
	virtual void		removeNick( const std::string& nickName ) ;

	/**
	 * Remove a remote server by its integer numeric, and
	 * return a pointer to it.
	 * When a server is removed, all its clients are also
	 * removed.  Each client is deleted, so make sure that
	 * the clients added to the network table (this) are
	 * allocated on the heap, and that there are no
	 * other stale pointers to any clients.
	 * If postEvent is true, then an EVT_QUIT will be generated
	 * for each client removal.
	 * Returns NULL if not found.
	 */
	virtual iServer*	removeServer( const unsigned int& YY,
					bool postEvent = false ) ;

	/**
	 * Remove a remote server by its character numeric, and
	 * return a pointer to it.
	 * When a server is removed, all its clients are also
	 * removed.  Each client is deleted, so make sure that
	 * the clients added to the network table (this) are
	 * allocated on the heap, and that there are no
	 * other stale pointers to any clients.
	 * Returns NULL if not found.
	 */
	virtual iServer*	removeServer( const std::string& YY ) ;

	/**
	 * Remove a remote server by its case-insensitive name, and
	 * return a pointer to it.
	 * When a server is removed, all its clients are also
	 * removed.  Each client is deleted, so make sure that
	 * the clients added to the network table (this) are
	 * allocated on the heap, and that there are no
	 * other stale pointers to any clients.
	 * Returns NULL if not found.
	 */
	virtual iServer*	removeServerName( const std::string& name ) ;

	/**
	 * Remove a channel from the network table, keyed by its
	 * case insensitive name.
	 * Returns NULL if not found.
	 */
	virtual Channel*	removeChannel( const std::string& name ) ;

	/**
	 * Remove a channel from the network table.
	 * Returns the Channel which has been removed, or NULL if the
	 * channel was not found.
	 */
	virtual Channel*	removeChannel( const Channel* theChan ) ;

	/* Utility methods */

	/**
	 * Change a remote client's nickname, given its character
	 * numeric and new nickname.
	 * This method does NOT work for local clients.
	 */
	virtual void		rehashNick( const std::string& yyxxx,
					const std::string& newNick ) ;

	/**
	 * This method performs a recursive removal of all servers
	 * which are uplinked by intYY.
	 * The server referenced by intYY is deallocated in this
	 * method.
	 */
	virtual void		OnSplit( const unsigned int& intYY ) ;

	/**
	 * An iterator to a network (iClient), which allows mutation.
	 * This iterator is a pair< const string (nickname), iClient* >.
	 */
	typedef nickMapType::iterator	clientIterator ;

	/**
	 * Return a mutator to the beginning of the network client
	 * data structure.  The client referenced is mutable.
	 */
	inline clientIterator		clients_begin()
		{ return nickMap.begin() ; }

	/**
	 * Return a mutator to the end of the network client
	 * data structure.  The client referenced is mutable.
	 */
	inline clientIterator		clients_end()
		{ return nickMap.end() ; }

	/**
	 * An iterator to a network (iClient), which disallows mutation.
	 * This iterator is a pair< const string (nickname), iClient* >.
	 */
	typedef nickMapType::const_iterator	const_clientIterator ;

	/**
	 * Return a mutator to the beginning of the network client
	 * data structure.  The client referenced is immutable.
	 */
	inline const_clientIterator	clients_begin() const
		{ return nickMap.begin() ; }

	/**
	 * Return a mutator to the end of the network client
	 * data structure.  The client referenced is immutable.
	 */
	inline const_clientIterator	clients_end() const
		{ return nickMap.end() ; }

	/**
	 * Define a non-const iterator for walking through the
	 * structure of remote servers.
	 */
	typedef serverMapType::iterator serverIterator ;

	/**
	 * Define a const iterator for walking through the structure
	 * of remote servers.
	 */
	typedef serverMapType::const_iterator const_serverIterator ;

	/**
	 * Return a non-const iterator to the beginning of the
	 * remote servers table.
	 */
	inline serverIterator servers_begin()
		{ return serverMap.begin() ; }

	/**
	 * Return a non-const iterator to the end of the
	 * remote servers table.
	 */
	inline serverIterator servers_end()
		{ return serverMap.end() ; }

	/**
	 * Return a const iterator to the beginning of the
	 * remote servers table.
	 */
	inline const_serverIterator servers_begin() const
		{ return serverMap.begin() ; }

	/**
	 * Return a const iterator to the end of the remote
	 * servers table.
	 */
	inline const_serverIterator servers_end() const
		{ return serverMap.end() ; }

	/**
	 * Define a non-const iterator for walking through the
	 * structure of local clients (xClients).
	 */
	typedef xClientMapType::iterator localClientIterator ;

	/**
	 * Define a const iterator for walking through the
	 * structure of local clients (xClients).
	 */
	typedef xClientMapType::const_iterator const_localClientIterator ;

	/**
	 * Return a non-const iterator to the beginning of the
	 * local clients table.
	 */
	inline localClientIterator localClient_begin()
		{ return localClients.begin() ; }

	/**
	 * Return a non-const iterator to the end of the local
	 * clients table.
	 */
	inline localClientIterator localClient_end()
		{ return localClients.end() ; }

	/**
	 * Return a const iterator the beginning of the
	 * local clients table.
	 */
	inline const_localClientIterator localClient_begin() const
		{ return localClients.begin() ; }

	/**
	 * Return a const iterator to the end of the local
	 * clients table.
	 */
	inline const_localClientIterator localClient_end() const
		{ return localClients.end() ; }

	/**
	 * Define a const iterator for walking through the 
	 * channels structure
	 */
	typedef channelMapType::const_iterator	const_channelIterator;
	
	/**
	 * Returns an iterator to the begining of the channels structure
	 */
	inline const_channelIterator		channels_begin() const
		{ return channelMap.begin(); }
	
	/**
	 * Returns an iterator to the end of the channels structure
	 */
	inline const_channelIterator		channels_end() const
		{ return channelMap.end(); }

	/**
	 * Define a mutating iterator for walking through the 
	 * channels structure
	 */
	typedef channelMapType::iterator	channelIterator;
	
	/**
	 * Returns an iterator to the begining of the channels structure
	 */
	inline channelIterator			channels_begin()
		{ return channelMap.begin(); }
	
	/**
	 * Returns an iterator to the end of the channels structure
	 */
	inline channelIterator			channels_end()
		{ return channelMap.end(); }

	/**
	 * const_iterator to a fake client.
	 */
	typedef fakeClientMapType::const_iterator const_fakeClientIterator ;

	/**
	 * Return a const iterator the beginning of the fake
	 * client structure.
	 */
	inline const_fakeClientIterator	fakeClient_begin() const
		{ return fakeClientMap.begin() ; }

	/**
	 * Return a const iterator the end of the fake
	 * client structure.
	 */
	inline const_fakeClientIterator	fakeClient_end() const
		{ return fakeClientMap.end() ; }

	/**
	 * Mutable iterator to a fake client.
	 */
	typedef fakeClientMapType::iterator	fakeClientIterator ;

	/**
	 * Return a mutable iterator the beginning of the fake
	 * client structure.
	 */
	inline fakeClientIterator	fakeClient_begin()
		{ return fakeClientMap.begin() ; }

	/**
	 * Return a mutable iterator the end of the fake
	 * client structure.
	 */
	inline fakeClientIterator	fakeClient_end()
		{ return fakeClientMap.end() ; }

	/**
	 * const_iterator to the fakeServerMap.
	 */
	typedef fakeServerMapType::const_iterator
		const_fakeServerIterator ;

	/**
	 * Return a const_iterator to the beginning of the fake
	 * server structure.
	 */
	inline const_fakeServerIterator	fakeServers_begin() const
		{ return fakeServerMap.begin() ; }

	/**
	 * Return a const_iterator to the end of the fake
	 * server structure.
	 */
	inline const_fakeServerIterator fakeServers_end() const
		{ return fakeServerMap.end() ; }

	/**
	 * iterator to the fakeServerMap.
	 */
	typedef fakeServerMapType::iterator fakeServerIterator ;

	/**
	 * Return a mutable iterator to the beginning of the fake
	 * server structure.
	 */
	inline fakeServerIterator	fakeServers_begin()
		{ return fakeServerMap.begin() ; }

	/**
 	 * Return a mutable iterator to the end of the fake
	 * server structure.
	 */
	inline fakeServerIterator fakeServers_end()
		{ return fakeServerMap.end() ; }

	/**
	 * Return the number of channels currently stored in the
	 * the channel table.
	 */
	inline size_t		channelList_size() const
		{ return channelMap.size() ; }

	/**
	 * Return the number of servers currently stored in the
	 * the server table.
	 */
	size_t		serverList_size() const ;

	/**
	 * Return the number of network clients currently stored
	 * in the client table.
	 */
	size_t		clientList_size() const ;

	/**
	 * Retrieve the number of clients on the given server.
	 */
	size_t		countClients( const iServer* theServer ) const ;

	/**
	 * This method is used to set the xServer used for
	 * backwards communication.  This is bad, and I would like
	 * very much to get rid of it.
	 */
	virtual void	setServer( xServer* _theServer ) ;

	/**
	 * Attempt to match the hostname, which may include wildcard
	 * characters, with any clients on the network.  Return a
	 * list of pointers to const iClient's which match.
	 */
	virtual std::list< const iClient* > matchHost(
			const std::string& wildHost ) const ;

	/**
	 * Match the given user@host string, which may include
	 * wildcards, to each client on the network.  Return a
	 * list of pointers to const iClient's which match.
	 */
	virtual std::list< const iClient* >	matchUserHost(
			const std::string& ) const ;

	/**
	 * Match the given channel key, which may include wildcards,
	 * to each channel on the network.  Return a list of pointers
	 * to const Channel's which match.
	 */
	virtual std::list<const Channel*> getChannelsWithKey(
			const std::string& key) const;

	/**
	 * Match the given channel modes, which may NOT include wildcards,
	 * to each channel on the network.  Return a list of pointers
	 * to const Channel's which match.
	 */
	virtual std::list<const Channel*> getChannelsWithModes(
			const std::string& modes) const;

#ifdef TOPIC_TRACK
	/**
	 * Match the given channel topic, which may include wildcards,
	 * to each channel on the network.  Return a list of pointers
	 * to const Channel's which match.
	 */
	virtual std::list<const Channel*> getChannelsWithTopic(
			const std::string& topic) const;

	/**
	 * Match the given channel topic setter, which may include wildcards,
	 * to each channel on the network.  Return a list of pointers
	 * to const Channel's which match.
	 */
	virtual std::list<const Channel*> getChannelsWithTopicBy(
			const std::string& topicby) const;
#endif
	/**
	 * Attempt to find hostnames which are equivalent to the given
	 * hostname, found using a case insensitive search.
	 * Return a list of pointers to const iClient's which are
	 * found.
	 */
	virtual std::list< const iClient* > findHost(
			const std::string& hostName ) const ;

	/**
	 * Perform a similar match as to matchHost(), except return
	 * only the number of matches found.
	 */
	virtual size_t	countMatchingHost( const std::string& wildHost ) 
				const ;

	/**
	 * Perform a similar match as to matchUserHost(), except return
	 * only the number of matches found.
	 */
	virtual size_t	countMatchingUserHost( const std::string& wildUserHost )
				const ;
	/**
	 * Perform a similar operation as to findHost(), except return
	 * only the number of hosts found.
	 */
	virtual size_t	countHost( const std::string& hostName ) const ;

	/**
	 * Attempt to match the real hostname, which may include wildcard
	 * characters, with any clients on the network.  Return a
	 * list of pointers to const iClient's which match.
	 */
	virtual std::list< const iClient* > matchRealHost(
			const std::string& wildHost ) const ;

	/**
	 * Match the given user@host string, which may include
	 * wildcards, to each client on the network.  Return a
	 * list of pointers to const iClient's which match.
	 */
	virtual std::list< const iClient* >	matchRealUserHost(
			const std::string& ) const ;

	/**
	 * Attempt to find real hostnames which are equivalent to the given
	 * hostname, found using a case insensitive search.
	 * Return a list of pointers to const iClient's which are
	 * found.
	 */
	virtual std::list< const iClient* > findRealHost(
			const std::string& hostName ) const ;

	/**
	 * Perform a similar match as to matchRealHost(), except return
	 * only the number of matches found.
	 */
	virtual size_t	countMatchingRealHost( const std::string& wildHost )
				const ;

	/**
	 * Perform a similar match as to matchRealUserHost(), except return
	 * only the number of matches found.
	 */
	virtual size_t	countMatchingRealUserHost( const std::string& wildUserHost )
				const ;
	/**
	 * Perform a similar operation as to findRealHost(), except return
	 * only the number of hosts found.
	 */
	virtual size_t	countRealHost( const std::string& hostName ) const ;

	/**
	 * Match the given client real name string, which may include
	 * wildcards, to each client on the network.  Return a
	 * list of pointers to const iClient's which match.
	 */
	virtual std::list< const iClient* >	matchRealName(
			const std::string& ) const ;

	/**
	 * Find a new server (iServer) numeric.
	 * The new numeric is assigned to intYY.
	 * On success, true is returned, false otherwise.
	 */
	virtual bool	allocateServerNumeric( unsigned int& intYY ) ;

protected:

	/**
	 * Disable copy constructing, this method is declared
	 * but NOT defined.
	 */
	xNetwork( const xNetwork& ) ;

	/**
	 * Disable copy assignment, this method is declared but
	 * NOT defined.
	 */
	xNetwork operator=( const xNetwork& ) ;

	/**
	 * Find a new client (iClient or xClient) numeric.
	 * The new numeric is assigned to newIntYYXXX.
	 * On success, true is returned, false otherwise.
	 */
	virtual bool	allocateClientNumeric( unsigned int intYY,
				unsigned int& newIntXXX ) ;

	/**
	 * Free a reserved client (iClient or xClient) numeric.
	 */
	virtual bool	freeClientNumeric( unsigned int intYYXXX ) ;


	/**
	 * Free a reserved server (iServer) numeric.
	 */
	virtual bool	freeServerNumeric( unsigned int intYYXXX ) ;

	/**
	 * This method is used internally when a client is added to
	 * the structure.
	 */
	virtual void addNick( iClient* ) ;

	/**
	 * Perform a simple recursive search for all leaves of
	 * the server whose numeric is the second arguments, and
	 * place each of those servers' numerics into the vector.
	 */
	virtual void	findLeaves( std::vector< unsigned int >& yyVector,
			const unsigned int intYY ) const ;

	/**
	 * The vector of local clients.
	 */
	xClientMapType			localClients ;

	/**
	 * The structure used to store the network channel
	 * information.
	 */
	channelMapType			channelMap ;

	/**
	 * The structure used to store the nick name->iClient
	 * cross references.
	 */
	nickMapType			nickMap ;

	/**
	 * The container used to store the network iClient's,
	 * keyed by iClient numeric.
	 */
	numericMapType			numericMap ;

	/**
	 * The container used to store the network iServer's,
	 * keyed by iServer numeric.
	 */
	serverMapType			serverMap ;

	/**
	 * This variable is used backwards calls to the main server.
	 */
	xServer				*theServer ;

	/**
	 * This structure maps fake iClient's to their owning
	 * xClient's.
	 */
	fakeClientMapType		fakeClientMap ;

	/**
	 * This structure stores fake (juped) servers.
	 */
	fakeServerMapType		fakeServerMap ;

	/**
	 * Structure used to store all reserved intYY and intYYXXX's
	 * for the local server and juped iClient/xClient/iServer's.
	 */
	reservedNumericMapType		reservedNumericMap ;	
} ;

/**
 * Declare the single xNetwork instance as global, for now.
 */
extern xNetwork* Network ;

} // namespace gnuworld

#endif // __NETWORK_H
