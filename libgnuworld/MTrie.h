/**
 * MTrie.h
 *
 * $Id: MTrie.h,v 1.2 2003/07/20 23:00:34 dan_karrels Exp $
 */

#ifndef __MTRIE_H
#define __MTRIE_H

#include	<map>
#include	<list>
#include	<string>

using std::map ;
using std::list ;
using std::pair ;
using std::string ;

/**
 * This class is used to store elements keyed by a decimalized string,
 * for example a hostname like www.yahoo.com
 * The key advantage is that this class is designed to support
 * fast wildcard matching of these hosts.
 * For example, clients of this class can search for all elements
 * whose keys match 'ab*ef.foo.ba?.net'.
 */
template< typename _valueT >
class MTrie
{
public:
	/**
	 * Initialize an MTrie.
	 */
	MTrie() ;

	/**
	 * Destructor will deallocate all internally allocated
	 * memory, but will not touch data stored here on behalf
	 * the client.
	 */
	~MTrie() ;

	typedef _valueT		data_type ;

	/// The type of data being stored
	typedef pair< string, _valueT >	value_type ;

	/// The type used to represent the number of elements
	/// in this MTrie
	typedef size_t  	size_type ;

	/**
	 * Return the current number of elements being stored
	 * in this MTrie.
	 */
	inline size_type	size() const
		{ return numElements ; }

	/**
	 * Insert a key/value pair.  The key must NOT have any
	 * wildcard characters.
	 */
	bool	insert( const string& key, const data_type& value ) ;

	list< value_type >	find( const string& key ) const ;

protected:

	list< value_type >	wildCardFind( const string& key ) const ;
	string			getBase( const list< string >& ) const ;

	/// The number of elements stored in this MTrie
	size_type		numElements ;

	/// The type used to store values
	typedef list< data_type > valuesListType ;

	typedef typename valuesListType::const_iterator const_values_iterator ;

	/// The structure used to store values
	valuesListType		valuesList ;

	/// The type used to store pointers to nodes in the trie
	typedef map< string, MTrie< _valueT >* > nodesMapType ;

	/// An iterator to nodes in the trie
	typedef typename nodesMapType::iterator nodes_iterator ;

	typedef typename nodesMapType::const_iterator const_nodes_iterator ;

	/// The structure used to store pointers to nodes in the trie
	nodesMapType		nodesMap ;
} ;

// This is a template class, cannot compile, so include the source
// inline.
#include "MTrie.cc"

#endif // __MTRIE_H	
