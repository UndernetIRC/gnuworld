/* xparameters.h
 */

#ifndef __xPARAMETERS_H
#define __xPARAMETERS_H "$Id: xparameters.h,v 1.1 2000/06/30 18:46:06 dan_karrels Exp $"

#include        <vector>
#include	<cassert>

/**
 * This class is similar to StringTokenizer, except that it accepts
 * pointers to already tokenized C character arrays.  This class
 * will not attempt to copy the arrays.  This is done for efficiency.
 * The tokens kept by instances of this class are indexed starting
 * at zero.
 * This class is mutable.
 */
class xParameters
{

private:
	/**
	 * The structure type for storing the tokens internally.
	 */
	typedef std::vector< char* > vectorType ;

public:

	/**
	 * The variable type used to represent this class's
	 * size (number of tokens in the object).
	 */
	typedef vectorType::size_type size_type ;

	/**
	 * Construct an xParameters object.
	 */
	xParameters() {}

	/**
	 * Destroy an xParameters object.
	 * No streams have been opened, and no memory explicitly
	 * dynamically allocated, so this method is a NOOP.
	 */
	~xParameters() {}

	/**
	 * Insert a pointer to a character array (token) into the
	 * xParameters instance.
	 */
	inline xParameters& operator<<( char* Parameter )
		{ myVector.push_back( Parameter ) ; return *this ; }

	/**
	 * Return a pointer to a mutable character array token.
	 * The tokens are indexed beginning at zero.
	 * This method will assert false if the requested index
	 * is out of bounds, according to validSubscript.
	 */
	inline char* operator[]( const size_type& pos ) const
	{ assert( validSubscript( pos ) ) ; return myVector[ pos ] ; }

	/**
	 * Remove all tokens, if any, from this xParameters object.
	 */
	inline void Clear()
		{ myVector.clear() ; }

	/**
	 * Return the number of tokens held in this xParameters object.
	 */
	inline size_type Count() const
		{ return myVector.size() ; }

	/**
	 * Return the number of tokens held in this xParameters object.
	 */
	inline size_type size() const
		{ return myVector.size() ; }

	/**
	 * Return true if the given variable (i) is a valid index
	 * into the table of tokens.
	 * Return false otherwise.
	 */
	inline bool validSubscript( const size_type& i ) const
		{ return (i < myVector.size()) ; }

protected:

	/**
	 * Disable copy constructing.
	 * This method is declared, but NOT defined.
	 */
	xParameters( const xParameters& ) ;

	/**
	 * Disable default assignment.
	 * This method is declared but NOT defined.
	 */
	xParameters operator=( const xParameters& ) ;

	/**
	 * This is the structure used to store tokens internally.
	 */
	vectorType      myVector ;

} ;

#endif // __xPARAMETERS_H
