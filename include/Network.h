/* xNetwork.h
 * Author: Daniel Karrels dan@karrels.com
 */

#ifndef __XNETWORK_H
#define __XNETWORK_H "$Id: Network.h,v 1.2 2000/07/06 19:13:07 dan_karrels Exp $"

#include	<vector>
#include	<string>
#include	<map>
#include	<hash_map>

#include	<ctime>

#include	"iServer.h"
#include	"iClient.h"
#include	"Channel.h"
#include	"client.h"
#include	"VectorTrie.h"
#include	"misc.h" // struct noCaseCompare

using std::string ;
using std::vector ;
using std::map ;
using std::hash ;
using std::hash_map ;

namespace gnuworld
{

/**
 * This class is responsible for storing the network's iClient's,
 * iServer's, and local xClient's.  This is done using several
 * tables, some of which are a bit non-trivial to understand at
 * first, but are actually very simple.
 * This is a big container class, mutated primarily by the
 * xServer instance.
 */
class xNetwork
{

	/**
	 * This is the vector type that will hold
	 * pointers to iServer's.
	 */
	typedef vector< iServer* > serverVectorType ;

	/**
	 * This is the vector type that will hold
	 * pointers to iClient's.
	 */
	typedef vector< iClient* > clientVectorType ;

	/**
	 * This is a vector of clientVectorType's.
	 * This type will be used to hold vectors of iClients,
	 * one for each server. (Described below.)
	 */
	typedef vector< clientVectorType > networkVectorType ;

	/**
	 * This is the type of vector for storing
	 * local (services) clients.
	 */
	typedef vector< xClient* > xClientVectorType ;

	/**
	 * This is the type used to store the network Channel
	 * instances.
	 * TODO: This is not too scalable (need templated hashtable).
	 */
	typedef hash_map< const char*, Channel*, hash< const char* >, eqstr >
		channelMapType ;
//	typedef map< string, Channel*, noCaseCompare > channelMapType ;

	/**
	 * This is the type used to store the nick name/iClient* pairs.
	 */
	typedef hash_map< const char*, iClient*, hash< const char* >, eqstr >
		nickMapType ;
//	typedef VectorTrie< string, iClient* > nickMapType ;

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
	 * Add a new channel to the network table.
	 * Returns false if the addition fails.
	 */
	virtual bool		addChannel( Channel* ) ;

	/*
	 * All nickname based searches are case sensitive,
	 * be careful how you place items into this structure.
	 * Note that this is not true, at present, for server
	 * name based searches.
	 */

	/**
	 * Find a remote client given the client's integer numeric.
	 * Returns NULL if not found.
	 */
	virtual iClient*	findClient( const unsigned int& YY,
					const unsigned int& XXX ) const ;

	/**
	 * Find a remote client given the client's character numeric.
	 * Returns NULL if not found.
	 */
	virtual iClient*	findClient( const string& yyxxx ) const ;

	/**
	 * Retrieve a pointer to an iClient given its nick name.
	 * Returns NULL if not found.
	 */
	virtual iClient*	findNick( const string& nickName ) const ;

	/**
	 * Find a local (services) client by its numeric.
	 * Returns NULL if not found.
	 */
	virtual xClient*	findLocalClient( const unsigned int& YY,
					const unsigned int& XXX ) const ;

	/**
	 * Find a local (services) client by its character numeric.
	 * Returns NULL if not found.
	 */
	virtual xClient*	findLocalClient( const string& yyxxx ) const ;

	/**
	 * Find a local (services) client by its case-sensitive nickname.
	 * Returns NULL if not found.
	 */
	virtual xClient*	findLocalNick( const string& nickName ) const ;

	/**
	 * Find a remote server by its integer numeric.
	 * Returns NULL if not found.
	 */
	virtual iServer*	findServer( const unsigned int& YY ) const ;

	/**
	 * Find a remote server by its character numeric.
	 * Returns NULL if not found.
	 */
	virtual iServer*	findServer( const string& YY ) const ;

	/**
	 * Find a remote server by its case-insensitive name.
	 * Returns NULL if not found.
	 */
	virtual iServer*	findServerName( const string& name ) const ;

	/**
	 * Find a global channel by case sensitive name.
	 * TODO: Change to case insensitive.
	 * Returns NULL if not found.
	 */
	virtual Channel*	findChannel( const string& name ) const ;

	/* Removal methods. */

	/**
	 * Remove a remote client by integer numeric, and return a pointer
	 * to it.
	 * Returns NULL if not found.
	 */
	virtual iClient*	removeClient( const unsigned int& YY,
					const unsigned int& XXX ) ;

	/**
	 * Remove a remote client by character numeric, and return a
	 * pointer to it.
	 * Returns NULL if not found.
	 */
	virtual iClient*	removeClient( const string& yyxxx ) ;

	/**
	 * Remove a nick name from the internal nick name table.
	 * Note that this returns void.  This is because nick names
	 * are cross referenced in the nick name table.  That is,
	 * no heap space is allocated for elements in the nick name
	 * table, they simply point to iClient instances.
	 */
	virtual void		removeNick( const string& nickName ) ;

	/**
	 * Remove a remote server by its integer numeric, and
	 * return a pointer to it.
	 * When a server is removed, all its clients are also
	 * removed.  Each client is deleted, so make sure that
	 * the clients added to the network table (this) are
	 * allocated on the heap, and that there are no
	 * other stale pointers to any clients.
	 * Returns NULL if not found.
	 */
	virtual iServer*	removeServer( const unsigned int& YY ) ;

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
	virtual iServer*	removeServer( const string& YY ) ;

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
	virtual iServer*	removeServerName( const string& name ) ;

	/**
	 * Remove a channel from the network table, keyed by its
	 * case sensitive name.
	 * Returns NULL if not found.
	 */
	virtual Channel*	removeChannel( const string& name ) ;

	/* Utility methods */

	/**
	 * Change a remote client's nickname, given its character
	 * numeric and new nickname.
	 * This method does NOT work for local clients.
	 */
	virtual void		rehashNick( const string& yyxxx,
					const string& newNick ) ;

	/**
	 * This method performs a recursive removal of all servers
	 * which are uplinked by intYY.
	 * The original server referenced by intYY is neither removed
	 * nor deleted.
	 */
	virtual void		OnSplit( const unsigned int& intYY ) ;

	/**
	 * Define a non-const iterator for walking through the
	 * structure of remote servers.
	 */
	typedef serverVectorType::iterator serverIterator ;

	/**
	 * Define a const iterator for walking through the structure
	 * of remote servers.
	 */
	typedef serverVectorType::const_iterator const_serverIterator ;

	/**
	 * Return a non-const iterator to the beginning of the
	 * remote servers table.
	 */
	inline serverIterator server_begin()
		{ return servers.begin() ; }

	/**
	 * Return a non-const iterator to the end of the
	 * remote servers table.
	 */
	inline serverIterator server_end()
		{ return servers.end() ; }

	/**
	 * Return a const iterator to the beginning of the
	 * remote servers table.
	 */
	inline const_serverIterator server_begin() const
		{ return servers.begin() ; }

	/**
	 * Return a const iterator to the end of the remote
	 * servers table.
	 */
	inline const_serverIterator server_end() const
		{ return servers.end() ; }

	/**
	 * Define a non-const iterator for walking through the
	 * structure of local clients (xClients).
	 */
	typedef xClientVectorType::iterator localClientIterator ;

	/**
	 * Define a const iterator for walking through the
	 * structure of local clients (xClients).
	 */
	typedef xClientVectorType::const_iterator const_localClientIterator ;

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

//	typedef clientVectorType::iterator clientIterator ;
//	typedef clientVectorType::const_iterator const_clientIterator ;

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
	 * This method is used internally when a client is added to
	 * the structure.
	 */
	void addNick( iClient* ) ;

	/**
	 * This is a vector of iServer*, one for each server
	 * on the network.
	 */
	serverVectorType		servers ;

	/**
	 * This is a vector of vector of iClient*.
	 * The index into clients is the server on which
	 * the client resides.
	 */
	networkVectorType		clients ;

	/**
	 * The vector of local clients.
	 */
	xClientVectorType		localClients ;

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

} ;

/**
 * Declare the single xNetwork instance as global, for now.
 */
extern xNetwork* Network ;

} // namespace gnuworld

#endif // __XNETWORK_H
