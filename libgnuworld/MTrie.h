/**
 * MTrie.h
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
 * $Id: MTrie.h,v 1.13 2004/05/18 16:50:57 dan_karrels Exp $
 */

#ifndef __MTRIE_H
#define __MTRIE_H

#include	<map>
#include	<list>
#include	<string>
#include	<iostream>

#include	"StringTokenizer.h"

using gnuworld::StringTokenizer ;

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

	/**
	 * The type being stored in this structure
	 */
	typedef _valueT		data_type ;

	/**
	 * The key/value pair type
	 */
	typedef std::pair< std::string, _valueT >	value_type ;

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
	bool	insert( const std::string& key, const data_type& value ) ;

	/**
	 * Return a list of the key/value pairs that match the given
	 * wildcard key.  The key may contain '?' and '*', but need
	 * not do so.  In general, the most specific searches will
	 * be fastest.
	 */
	virtual std::list< value_type >	find( const std::string& key ) const ;

	/**
	 * Remove all values associated with key's that match the given
	 * key.  The key may have wildcards '?' and '*', and follow
	 * the same semantics as find().  Be sure to deallocate the
	 * values if necessary before calling erase(), this method will
	 * not deallocate the items stored in this structure.
	 * The number of elements erased is returned.
	 */
	virtual size_type		erase( const std::string& key ) ;

	/**
	 * Produce output about for each node in the trie in the
	 * following format:
	 * <level number> <nodesmap size> <valueslist size>
	 * This method is used for statistical analysis.
	 */
	virtual void			dumpDebug( std::ostream& ) const ;

	/**
	 * Send the key of each node at the given level to the
	 * output stream.  Also send the total number of values
	 * under each subtree at those nodes.
	 */
	virtual void	levelDebug( std::ostream&, size_t searchLevel )
				const ;

	/**
	 * Return all keys that have at least as many tokens as
	 * minLength.
	 */
	virtual std::list< std::string > findMinLength( size_t minLength )
				const ;
protected:

	/**
	 * Recursive helper method to the public levelDebug()
	 */
	virtual void	levelDebug( std::ostream&, size_t currentLevel,
				size_t searchLevel,
				const MTrie< data_type >* ) const ;

	/**
	 * Return the total number of values under the given node
	 */
	virtual size_t	value_size( const MTrie< data_type >* ) const ;

	/**
	 * Recursive helper method to the public findMinLength()
	 */
	virtual void	findMinLength( size_t minLength,
				std::list< std::string >& retMe,
				const MTrie< data_type >* ) const ;

	/**
	 * Recursive find method that handles all the hard work
	 * matching across multiple levels.
	 */
	virtual void	find( const MTrie< data_type >*,
				const StringTokenizer&,
				StringTokenizer::const_reverse_iterator )
				const ;

	/**
	 * Recursive erase method that handles all the hard work
	 * matching across multiple levels.
	 */
	virtual size_type erase( MTrie< data_type >*,
				std::list< std::string >&,
				const std::string& origKey,
				const std::string& key ) ;

	/**
	 * Convenience method that puts together a list of
	 * string tokens into a single string.
	 */
	std::string		getBase() const ;

	/**
	 * Recursive method used only for searching for '*' matched
	 * strings.
	 */
	virtual void	recursiveFind( const MTrie< data_type >*,
				bool blindRecursion = false )
				const ;

	/**
	 * Recursive method used only for erasing '*' matched
	 * strings.
	 */
	virtual size_type recursiveErase( MTrie< data_type >*,
				std::list< std::string >& base,
				const std::string& key ) ;

	/**
	 * A recursive method used to output some debugging information.
	 */
	virtual void	recursiveDebug( std::ostream&,
				size_t levelNum,
				const MTrie< data_type >* ) const ;

	/**
	 * The number of elements stored in this MTrie
	 */
	size_type		numElements ;

	/**
	 * The parent node to this node (NULL if root)
	 */
	MTrie< data_type >*	parent ;

	/**
	 * The string delimiter.
	 */
	char			delimiter ;

	/**
	 * The type used to store values
	 */
	typedef std::list< data_type > valuesListType ;

	/**
	 * An iterator for iterating the valuesList
	 */
	typedef typename valuesListType::iterator values_iterator ;

	/**
	 * A const iterator for iterating the valuesList
	 */
	typedef typename valuesListType::const_iterator const_values_iterator ;

	/**
	 * The structure used to store values
	 */
	valuesListType		valuesList ;

	/**
	 * The type used to store pointers to levels in the trie
	 */
	typedef std::map< std::string, MTrie< _valueT >* > nodesMapType ;

	/**
	 * An iterator to nodes in the trie
	 */
	typedef typename nodesMapType::iterator nodes_iterator ;

	/**
	 * A const iterator to nodes in the trie
	 */
	typedef typename nodesMapType::const_iterator const_nodes_iterator ;

	/**
	 * The structure used to store levels in the trie
	 */
	nodesMapType		nodesMap ;

	/**
	 * This variable is used in find() and erase() to reduce the 
	 * number of arguments passed to the internal recursive methods.
	 * It stores the current key associated with the node
	 * being examined.
	 */
	mutable std::list< std::string >	base ;

	/**
	 * This variable is used in find() and erase() to reduce the 
	 * number of arguments passed to the internal recursive methods.
	 * It stores the values to be returned from the methods.
	 */
	mutable std::list< value_type >	returnMe ;

	/**
	 * This variable is used in find() and erase() to reduce the 
	 * number of arguments passed to the internal recursive methods.
	 * It stores the key being searched for in the overall
	 * find/erase.
	 */
	mutable std::string	origKey ;

	/**
	 * The tokens object used in find() and erase().
	 */
	StringTokenizer		tokens ;

} ;

// This is a template class, cannot compile, so include the source
// inline.
#include "MTrie.cc"

#endif // __MTRIE_H	
