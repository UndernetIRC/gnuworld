/* xparameters.h
 * This class originally written by Orlando Bassotto.
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
 * $Id: xparameters.h,v 1.5 2002/05/27 17:18:12 dan_karrels Exp $
 */

#ifndef __XPARAMETERS_H
#define __XPARAMETERS_H "$Id: xparameters.h,v 1.5 2002/05/27 17:18:12 dan_karrels Exp $"

#include        <vector>
#include	<cassert>

namespace gnuworld
{

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
	 * Set a particular element to a new value, NULL is
	 * permitted.
	 */
	void setValue( const size_t pos, char* newVal )
	{ assert( validSubscript( pos ) ) ; myVector[ pos ] = newVal ; }

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
	 * Return true if there exist no tokens represented by this
	 * class.
	 */
	inline bool empty() const
		{ return myVector.empty() ; }

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

} // namespace gnuworld

#endif // __XPARAMETERS_H
