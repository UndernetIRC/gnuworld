/**
 * MTrie.h
 *
 * $Id: MTrie.h,v 1.6 2003/07/27 22:26:45 dan_karrels Exp $
 */

#ifndef __MTRIE_H
#define __MTRIE_H

#include	<map>
#include	<list>
#include	<string>
#include	<iostream>

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
	MTrie( const char delimiter = '.' ) ;

	/**
	 * Destructor will deallocate all internally allocated
	 * memory, but will not touch data stored here on behalf
	 * the client.
	 */
	virtual ~MTrie() ;

	/// The type being stored in this structure
	typedef _valueT		data_type ;

	/// The key/value pair type
	typedef pair< string, _valueT >	value_type ;

	/**
	 * The type used to represent the number of elements
	 * in this MTrie
	 */
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

	/**
	 * Return a list of the key/value pairs that match the given
	 * wildcard key.  The key may contain '?' and '*', but need
	 * not do so.  In general, the most specific searches will
	 * be fastest.
	 */
	virtual list< value_type >	find( const string& key ) const ;

	/**
	 * Remove all values associated with key's that match the given
	 * key.  The key may have wildcards '?' and '*', and follow
	 * the same semantics as find().  Be sure to deallocate the
	 * values if necessary before calling erase(), this method will
	 * not deallocate the items stored in this structure.
	 * The number of elements erased is returned.
	 */
	virtual size_type		erase( const string& key ) ;

	virtual void			dumpDebug( std::ostream& ) const ;

protected:

	/**
	 * Recursive find method that handles all the hard work
	 * matching across multiple levels.
	 */
	virtual void	find( const MTrie< data_type >*,
				list< value_type >&,
				list< string >&,
				const string& origKey,
				const string& key ) const ;

	/**
	 * Recursive erase method that handles all the hard work
	 * matching across multiple levels.
	 */
	virtual size_type erase( MTrie< data_type >*,
				list< string >&,
				const string& origKey,
				const string& key ) ;

	/**
	 * Convenience method that puts together a list of
	 * string tokens into a single string.
	 */
	string			getBase( const list< string >& ) const ;

	/**
	 * Recursive method used only for searching for '*' matched
	 * strings.
	 */
	virtual void	recursiveFind( const MTrie< data_type >*,
				list< string >& base,
				list< value_type >& returnMe,
				const string& key ) const ;

	/**
	 * Recursive method used only for erasing '*' matched
	 * strings.
	 */
	virtual size_type recursiveErase( MTrie< data_type >*,
				list< string >& base,
				const string& key ) ;

	virtual void	recursiveDebug( std::ostream&,
				size_t levelNum,
				const MTrie< data_type >* ) const ;

	/// The number of elements stored in this MTrie
	size_type		numElements ;

	/// The parent node to this node (NULL if root)
	MTrie< data_type >*	parent ;

	/// The string delimiter
	char			delimiter ;

	/// The type used to store values
	typedef list< data_type > valuesListType ;

	/// An iterator for iterating the valuesList
	typedef typename valuesListType::iterator values_iterator ;

	/// A const iterator for iterating the valuesList
	typedef typename valuesListType::const_iterator const_values_iterator ;

	/// The structure used to store values
	valuesListType		valuesList ;

	/// The type used to store pointers to levels in the trie
	typedef map< string, MTrie< _valueT >* > nodesMapType ;

	/// An iterator to nodes in the trie
	typedef typename nodesMapType::iterator nodes_iterator ;

	/// A const iterator to nodes in the trie
	typedef typename nodesMapType::const_iterator const_nodes_iterator ;

	/// The structure used to store levels in the trie
	nodesMapType		nodesMap ;
} ;

// This is a template class, cannot compile, so include the source
// inline.
#include "MTrie.cc"

#endif // __MTRIE_H	
