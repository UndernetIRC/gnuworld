/* VectorTrie.h
 * Author: Daniel Karrels dan@karrels.com
 * Special thanks to Carlo Wood for bug fixing.
 *
 * This structure uses radix key searching,
 * so operator[] must exist for keyType.
 * The character type of the key may be
 * specified as the third argument to the
 * template type of class VectorTrie.
 * The (charType) has a default value of (char).
 *
 * Each node stores a pointer to an instance of
 * type Element instead of a copy of a
 * dataType instance.  This is done for efficiency,
 * and for template implementation reasons:
 * - each empty node allocation requires only
 *   initializing a pointer, versus calling
 *   a default constructor;
 * This saves a bit of memory overhead, and
 * saves CPU time calling default constructors
 * of unknown size and complexity.
 * Also, removing an element from the tree
 * requires one fewer variables (comparison
 * agains NULL).
 * 
 * The algorithm used here, along with the
 * class invariants defined below, permit
 * this class to store elements without
 * storing their keys.
 *
 * Reasoning (weak proof):
 *  For each key inserted into the structure,
 *  its path to the correct end node is
 *  traversed completely.
 *  This is contrary to general trie insertion
 *  procedure: the key traversal for each
 *  insertion proceeds until a NULL node is
 *  encountered, or the end node is encountered,
 *  whichever comes first.  A data object stored
 *  using the general trie insertion method may
 *  be stored at a node whose full key
 *  has not been traversed.  That is, the data
 *  object is stored at a node whose key is
 *  a proper prefix (at the time) of the
 *  whole key.
 *  In class VectorTrie, each key maintains a static
 *  location in the trie, given a translation
 *  table.
 *  The location of any data object can
 *  be determined algorithmically, and without
 *  comparisons.  This eliminates the need
 *  for storing keys at tree nodes.
 *
 * Requirements for keyType:
 * - Must define operator[].
 *   Note that pointer types satisfy this
 *   condition.
 * - Key length must have given length > 0.
 *   To allow the use of pointer types as
 *   keys for this structure, the length
 *   of the key is specified as a separate
 *   argument to the Insert/Search/Delete
 *   methods.
 *   If VT_DEBUG is enable, not meeting this
 *   requirement will result in an assertion failure.
 * - Must be unique among keys stored
 *   in the trie.
 *   Attempting to Insert an object whose
 *   key corresponds to a data object already
 *   stored in the trie will result in failure.
 *
 * Requirements for dataType:
 * - Must be assignable.
 *   The copy constructor of any data type
 *   stored in this container as copy will
 *   be called during insertion into the trie.
 *   This is ok for pointers.
 * - The charSetSize is the number of significant
 *   elements in the character set.
 *   For the given translation tables, use
 *   the following charSetSize:
 *   caseAlphaTable: 52
 *   noCaseAlphaTable: 26
 *
 * This is a lazy Trie.  This means that
 *  removals from the trie do not result
 *  in removal of empty nodes.  The assumption
 *  here is that those empty nodes will be
 *  used again soon (this follows the pattern
 *  of the intended use for this structure).
 *
 * Invariants
 * ----------
 * - root != NULL
 * - Empty subtrie pointers point to NULL
 * - Sub tree (data) member points to NULL
 *   when the node is not storing data
 * - Length of each key is > 0
 * - No duplicate keys exist in the trie
 */
 
#ifndef __VECTORTRIE_H
#define __VECTORTRIE_H "$Id: VectorTrie.h,v 1.1 2000/06/30 18:46:06 dan_karrels Exp $"

#include	<new>
#include	<string>
#include	<cstring>
#include	<iostream>

#include	"VectorTrieNode.h"

// Below are several example translation
// tables for the key type (char).
// (charType) represents the type of
// elements of the (keyType).
//
// Each table is responsible for
// mapping elements of any key given to
// Insert, Search, or Delete to a low
// number array index.
// The array indices should begin at 0,
// and move contiguously to charSetSize - 1.
// This translation is done to save
// memory overhead in representing the
// trie internally.
//
// A value of (-1) as a translation
// table value represents a key
// element outside of the specified
// character set.
// An attempt to dereference an
// array index of (-1) in the trie
// will raise an exception.
// This is done for debugging purposes
// only.  In a production environment,
// specifying 0 as the default translation
// target can be used to catch inconsistencies
// in the character set definition.

// Case sensitive alphabetical only
// translation table.
// The number of valid elements in
// this table (charSetSize) is 52.
//

/**
 * This table is used to map (char) key
 * values from their true ASCII
 * value to an array index into the
 * trie.  Since this is case sensitive,
 * upper and lower case characters
 * represent different values.
 * The English alphabet has 52 characters,
 * including a-z, A-Z, excluding all
 * others, which gives a charSetSize of 52.
 */
static const char caseAlphaTable[] = {
 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 -1, -1, -1, -1, -1,  0,  1,  2,  3,  4,
  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
 25, -1, -1, -1, -1, -1, -1, 26, 27, 28,
 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
 49, 50, 51
 } ;

/**
 * Alphabetical, case insensitive
 * The number of valid elements in
 * this table (charSetSize) is 26.
 *
 * a-z and A-Z are the same here since
 * this table is case insensitive.
 * Nothing else is allowed.
 */
static const char noCaseAlphaTable[] = {
 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 -1, -1, -1, -1, -1,  0,  1,  2,  3,  4,
  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
 25, -1, -1, -1, -1, -1, -1,  0,  1,  2,
  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
 23, 24, 25
 } ;

/**
 * This class is a templated Trie (N-Way Tree).
 *
 * Template arguments:
 * - (keyType)
 *   The type of the key.
 * - (dataType)
 *   The type to be stored in the structure.
 * - (charType)
 *   The type of each key element (radix searching).
 *   The (charType) has a default argument of (char).
 */
template< class keyType, class dataType, class charType = char >
class VectorTrie
{

// First, the public typedef's
public:

	/**
	 * This is used as the type
	 * to store the size of the character set.
	 */
	typedef size_t size_type ;

	/**
	 * This is the type of the keys
	 * to be used for searching, inserting,
	 * and deleting.
	 */
	typedef keyType key_type ;

	/**
	 * This is the type of the data
	 * to be stored in the trie.
	 */
	typedef dataType value_type ;

	/**
	 * This is the type for key elements
	 * to be used for indexing and radix
	 * searching.
	 */
	typedef charType char_type ;

	/**
	 * This is the type of the element
	 * structure used to hold the data object
	 * internally.
	 */
	typedef VectorTrieNode< keyType, dataType, charType >::elementType
		elementType ;

protected:

	// Default construction automatically
	// disabled by definition of any other
	// constructor.

	// Disable a few methods.
	// Note that these methods are declared, but
	// not defined.  This prevents accidental
	// execution by any of the private or protected
	// methods.

	/**
	 * Disables the copy constructor.  Note that this
	 * method is declared but NOT defined.
	 */
	VectorTrie( const VectorTrie< keyType, dataType,
		charType >& ) ;

	/**
	 * Disable the copy assignment operator.  Note that
	 * this method is declared but NOT defined.
	 */
	VectorTrie operator=( const VectorTrie< keyType, dataType,
		charType >& ) ;

	/**
	 * The type for the sub-trie node class.
	 */
	typedef VectorTrieNode< keyType, dataType, charType > branchType ;

	// Next the variables, at the top
	// for convenience.

	/**
	 * This is the size of the character set.
	 * This is correspondingly the size of the root
	 * array in this class, as well as VectorTrieNode.
	 */
	size_type		charSetSize ;

	/**
	 * This is a pointer to the table which
	 * contains (charSetSize) valid characters.
	 * This table is used to convert key elements
	 * to a low numbered index into the root array.
	 */
	const charType		*transTable ;

	/**
	 * Pointer to array of pointers,
	 *  allocated exactly once, at instantiation.
	 * Per invariant:
	 *  Each element of the root array
	 *  is of type branchType*, and initially
	 *  NULL.
	 */
	branchType		**root ;

// Public methods
public:

	/**
	 * The only constructor defined in this class.
	 * The constructor receives as arguments:
	 * - (newCharSetSize)
	 *    The size of the character set
	 * - (newTransTable)
	 *    A pointer to the translation table
	 */
	VectorTrie( const size_type& newCharSetSize,
		const charType* newTransTable )
	 : charSetSize( newCharSetSize ),
	   transTable( newTransTable )
	{
	// Allow std::bad_alloc to be thrown
	root = new branchType*[ charSetSize ] ;

	// Initialize each element in the table
	// to NULL.
	memset( root, 0, charSetSize * sizeof( branchType* ) ) ;
	}

	/**
	 * Destructor.
	 * Deallocate each node.
	 * Attempting to delete a NULL pointer is valid according to
	 * the C++ standard.
	 */
	virtual ~VectorTrie()
	{
	for( register size_type i = 0 ; i < charSetSize ; ++i )
		{
		delete root[ i ] ;
		}

	// Deallocate the root tree itself.
	delete[] root ;

	// Just for good measure.
	root = 0 ;
	}

	/**
	 * Insert a new data item into the trie.
	 * Requirements:
	 *  - This is a unique associative container
	 *   -> Each key must be unique
	 * - Each key must have positive length (len)
	 *
	 * Returns:
	 *  true on success
	 *  false on invalid key length
	 *  false on attempt to insert non-unique key
	 *
	 * The search for a unique key is included inline
	 * in this method.  This proves to be extremeley
	 * efficient.
	 */
	bool Insert( const keyType& key, const dataType& data,
		const size_type& len )
	{

	// Make sure that the key is not empty
	// Behavior: Be nice in the production
	// environment, drop core in debug environment.
	#ifdef VT_DEBUG
		assert( len >= 1 ) ;
	#else
		if( len < 1 )
			{
			// std::clog << "Insert: Key is empty()\n" ;
			return false ;
			}
	#endif // VT_DEBUG

	// A pointer for iterating the structure's
	// internal pointer tables.
	//
	// Note that this statement is valid because
	// root is allocated in the constructor, and the
	// key has been verified as positive length.
	//
	register branchType* ptr = root[ transTable[ key[ 0 ] ] ] ;

	// Make sure the root node of the first
	// subtrie is allocated properly.
	if( 0 == ptr )
		{
		// Note that both ptr and root[...]
		// are being assigned to the same new
		// node.
		// Allow std::bad_alloc to be thrown.
		ptr = root[ transTable[ key[ 0 ] ] ] =
			new branchType( charSetSize ) ;
		}

	// Iterate through the charType's in
	// the key, allocating nodes where needed.
	for( register size_type i = 1 ; i < len ; ++i )
		{

		// If this particular node doesn't already exist,
		// create it.
		if( 0 == ptr->root[ transTable[ key[ i ] ] ] )
			{
			ptr->root[ transTable[ key[ i ] ] ] = new
				branchType( charSetSize ) ;
			}

		// Move to the next link
		ptr = ptr->root[ transTable[ key[ i ] ] ] ;
		}

	// The variable (i) is now equivalent to the key's
	// length (len).
	// The variable (ptr) points to the the VectorTrieNode
	// that will hold the new data.

	// Here is the test for key uniqueness.
	//
	// Class invariant:
	// - Sub trie (data) member points to NULL
	//   when the node is not storing data.
	//
	// If the (data) variable is non-NULL, it
	// points to data, and this key is a
	// duplicate.
	//
	if( 0 != ptr->data )
		{
		// This key is non-unique, return false
		// std::clog << "Element already exists\n" ;
		return false ;
		}

	// This uses dataType's copy constructor
	// Allow bad_alloc to be thrown.
	ptr->data = new elementType( data ) ;

	// The Insert method has succeeded.
	return true ;

	} // Insert()

	/**
	 * Search the structure for a given key (key),
	 * and of given length (len).
	 *
	 * Per the requirements of the key type, len
	 * must be greater than zero.
	 *
	 * Returns:
	 *  NULL if (len) of invalid length
	 *  NULL if key not found
	 *  Pointer to elementType which holds
	 *   data object being retrieved.
	 *  Use the elementType's (data) member
	 *   to access the data object. 
	 */
	elementType* Search( const key_type& key, const size_type& len ) const
	{

	// Make sure that the key is not empty.
	//
	// Be gracious if this class is being
	// used in a production environment,
	// drop core otherwise.
	#ifdef VT_DEBUG
		assert( len >= 1 ) ;
	#else
		if( len < 1 )
			{
			return 0 ;
			}
	#endif // VT_DEBUG

	// (ptr) is initialized to the first
	// node pointer given the first element
	// of the key.
	register const branchType* ptr = root[ transTable[ key[ 0 ] ] ] ;
		
	// Move until we can't move any further:
	// to the node representing the correct key,
	// or a node along the way that points to NULL.
	// We start at the second charType of the key
	// because ptr is already pointing at the node
	// corresponding to the key's first element
	// from the top of the tree (this->root).
	//
	// Continue while (i) is less than the length
	// of the key being searched for, and while
	// ptr is non-NULL
	//
	for( register size_type i = 1 ; ptr && i < len ;
		ptr = ptr->root[ transTable[ key[ i ] ] ], ++i )
		/* NO-OP */ ;
		
	// (ptr) now either points to NULL or it
	// points to the node representing the key.
	//
	// Return NULL if the node is empty
	// Return a pointer to the elementType holding
	// the saught after data.
	return (ptr ? ptr->data : 0) ;

	} // Search()

	/**
	 * Remove a given <key, data> pair from the trie.
	 * (key) must have length (len) greater than zero.
	 * 
	 * Returns:
	 *  void: Make sure any deallocation of data objects
	 *  stored by pointer is done before calling Delete.
	 */
	void Delete( const key_type& key, const size_type& len )
	{

	#ifdef VT_DEBUG
		// Class invariant: Each key stored
		// in this trie has length > 0
		assert( len >= 1 ) ;
	#else
		// In production, allow this without
		// failing.
		if( len < 1 )
			{
			return ;
			}
	#endif // VT_DEBUG

	// Initialize the root iterator
	register branchType* ptr = root[ transTable[ key[ 0 ] ] ] ;

	// Continue until we find either the correct node (i < len
	// will fail), or an empty node ((ptr) will fail).
	for( register size_type i = 1 ; ptr && i < len ;
		ptr = ptr->root[ transTable[ key[ i ] ] ], ++i )
		/* NO-OP */ ;

	// "ptr" now either points to NULL, or to a node with
	// the correct key, by definition.
	if( 0 != ptr )
		{

		// Deallocate the storage element.
		// This is ok even if ptr->data
		// is empty.
		// In that case, per class invariant:
		// ptr->data points to NULL
		delete ptr->data ;

		// Set it to NULL
		ptr->data = 0 ;

		}
	} // Delete()

} ; // class VectorTrie

#endif // __VECTORTRIE_H
