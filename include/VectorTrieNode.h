/* VectorTrieNode.h
 * Author: Daniel Karrels dan@karrels.com
 * Special thanks to Carlo Wood for helping to
 *  streamline this class.
 * Requirements for keyType:
 * - operator[]
 */
 
#ifndef __VECTORTRIENODE_H
#define __VECTORTRIENODE_H "$Id: VectorTrieNode.h,v 1.1 2000/06/30 18:46:06 dan_karrels Exp $"

#include	<new>
#include	<cstring> // memset()

/// Provide a forward declaration of VectorTrie
/// for the friend declaration.
template< class keyType, class dataType, class charType >
	class VectorTrie ;

/**
 * Element is the data type to store data members
 * being stored in the Trie.
 */
template< class dataType >
struct Element
{
	typedef dataType value_type ;
	value_type	data ;

	Element( const value_type& _data )
	 : data( _data ) {}
	Element( const Element< dataType >& rhs )
	 : data( rhs.data ) {}
} ;

/**
 * This is a Node class to be used by class VectorTrie.
 * All three of VectorTrie's templated types are specified here
 * to satisfy C++'s template type definition semantics.
 */
template< class keyType, class dataType, class charType = char >
class VectorTrieNode
{

public:

	/**
	 * This is the type used to
	 * specify the number of elements
	 * needed in the root array.
	 */
	typedef size_t size_type ;

	/**
	 * This is the type of the
	 * objects to be stored in the node.
	 */
	typedef dataType value_type ;

	/**
	 * This is the type of node to store
	 * individual data items.
	 */
	typedef Element< value_type > elementType ;

protected:

	/**
	 * VectorTrieNode requires knowledge of (branchType)
	 * because it must hold an equivalent (root) array as
	 * that of class VectorTrie.
	 */
	typedef VectorTrieNode< keyType, dataType, charType > branchType ;

	/**
	 * Make VectorTrie a friend so it can access
	 * the root and data variables.
	 */
	friend VectorTrie< keyType, dataType, charType > ;

	/**
	 * This is the size of the character set being
	 * used in the VectorTrie, and is the number
	 * of nodes to be allocated in the (root) array.
	 */
	size_type	charSetSize ;

	/**
	 * This points to the elementType structure
	 * instance which holds the data object associated
	 * with this VectorTrieNode.
	 *
	 * Per class VectorTrie invariants:
	 * - (data) points always to NULL if the node
	 *   is not holding data;
	 * - otherwise, (data) points to an elementType holding
	 *   the data object associated with this node.
	 */
	elementType	*data ;

	/**
	 * This is the array of (branchType) pointers to
	 * sub tries.  Each element of this array is a pointer
	 * to an instance of a class VectorTrieNode.
	 * Per class VectorTrie invariants:		
	 * - each element of the (root) array points to
	 *   NULL if that sub trie is empty;
	 * - otherwise, each element points to a sub
	 *   trie of type (branchType).
	 */
	branchType	**root ;
	
	/**
	 * Disable the copy constructor.  Note that this method is
	 * declared but NOT defined.
	 */
	VectorTrieNode( const VectorTrieNode< keyType, dataType, charType >& ) ;

	/**
	 * Disable default assignment.  Note that this method is
	 * declared but NOT defined.
	 */
	VectorTrieNode operator=( const VectorTrieNode< keyType,
		dataType, charType >& ) ;

public:

	/**
	 * Constructor.
	 * The only data this node needs is the
	 * size of the character set to be used for
	 * allocating the (root) array.
	 */
	VectorTrieNode( const size_type& newCharSetSize )
	 : charSetSize( newCharSetSize ),
	   data( 0 )
	{
	// The initialization of the (data) variable
	// to NULL above satisfies the class
	// VectorTrie invariant:
	// - (data) points to NULL when no data is stored
	//   in this node.

	// Allow std::alloc to be thrown
	root = new branchType*[ charSetSize ] ;

	// Satisfy class VectorTrie invariants:
	// - All sub trie node pointers point to NULL
	//   when not in use.
	memset( root, 0, charSetSize * sizeof( branchType* ) ) ;

	}

	/**
	 * Destructor.
	 * Default behavior: delete each allocated
	 * node in the array of subtrie pointers.
	 */
	virtual ~VectorTrieNode()
	{
	for( register size_type i = 0 ; i < charSetSize ; ++i )
		{
		// OK to delete NULL
		delete root[ i ] ;
		}
	// Deallocate the table.
	delete[] root ;

	// Just to be safe.
	root = 0 ;

	}
	
} ;

#endif // __VECTORTRIENODE_H
