/**
 * StringTokenizer.h
 * Author: Daniel Karrels dan@karrels.com
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
 * $Id: StringTokenizer.h,v 1.4 2002/05/27 17:18:12 dan_karrels Exp $
 */

#ifndef __STRINGTOKENIZER_H
#define __STRINGTOKENIZER_H "$Id: StringTokenizer.h,v 1.4 2002/05/27 17:18:12 dan_karrels Exp $"

#include	<vector>
#include	<string>
#include	<iostream>

namespace gnuworld
{

using std::string ;
using std::vector ;

/**
 * This class provides a clean mechanism for parsing
 * C++ strings based on a given delimiter.
 * It overloads operator[], and does not affect the
 * original string passed as argument to the constructor.
 * The tokens maintained by this class are zero indexed.
 * This class is immutable.
 */
class StringTokenizer
{

private:
	/**
	 * This is the type that will be used to store
	 * the tokens in the StringTokenizer object.
	 */
	typedef vector< string > vectorType ;

public:

	/**
	 * Constructor receives the string to be
	 * tokenized, and the delimiter by which
	 * tokens will be generated.
	 */
	StringTokenizer( const string& = string(), char = ' ' ) ;

	/**
	 * The destructor is a NOOP because no streams have been
	 * opened, and no memory dynamically explicitly allocated.
	 */
	virtual ~StringTokenizer() ;

	/**
	 * This is the type of the variable used for
	 * representing the size (number of tokens) of
	 * the StringTokenizer object.
	 */
	typedef vectorType::size_type size_type ;

	/**
	 * Retrieve a const reference to a given token, zero-indexed.
	 * This method will assert(false) if the requested index is
	 * out of bounds.  This is for debugging, and may be conditionally
	 * compiled at a later time to throw an exception while
	 * in a production environment.
	 */
	const string&		getToken( const size_type& ) const ;

	/**
	 * This method allows StringTokenizer objects to be used like
	 * arrays.  This method just calls the getToken() method, and
	 * its semantics are the same.
	 */
	inline const string&	operator[]( const size_type& sub ) const
		{ return getToken( sub ) ; }

	/**
	 * Return a const reference to the original C++ string before
	 * tokenization.
	 */
	inline const string&	getOriginal() const
		{ return original ; }

	/**
	 * Return a const reference to the original C++ string before
	 * tokenization.
	 */
	inline const string&	toString() const
		{ return original ; }

	/**
	 * Return the number of tokens in this StringTokenizer object.
	 */
	inline size_type	size() const
		{ return array.size() ; }

	/**
	 * Return true if the StringTokenizer holds no tokens,
	 * false otherwise.
	 * This is equivalent to (size() == 0).
	 */
	inline bool		empty() const
		{ return array.empty() ; }

	/**
	 * Determine if the subscript argument is within the
	 * bounds [0,size()).
	 * Return true if so, false otherwise.
	 * This method works also even if the StringTokenizer is empty.
	 */
	inline bool		validSubscript( const size_type& sub ) const
		{ return (sub < size()) ; }

	/**
	 * This method builds and returns a C++ string starting at the given
	 * index, and continuing until the last token, placing the
	 * appropriate delimiter between each token.
	 */
	string			assemble( const size_type& ) const ;

	/**
	 * The immutable iterator type to use for walking through
	 * this object's tokens.
	 */
	typedef vectorType::const_iterator const_iterator ;

	/**
	 * Retrieve an immutable iterator to the beginning of this
	 * object's token structure.
	 */
	inline const_iterator	begin() const
		{ return array.begin() ; }

	/**
	 * Retrieve an immutable iterator to the end of this object's
	 * token structure.
	 */
	inline const_iterator	end() const
		{ return array.end() ; }

	/**
	 * Convenience method for debugging purposes.
	 */
	friend std::ostream& operator<<( std::ostream& out,
		const StringTokenizer& rhs )
		{
		for( size_type i = 0, end = rhs.size() ; i < end ; ++i )
			{
			out << rhs.array[ i ] ;
			if( i < (rhs.size() - 1) )
				{
				out << rhs.delimiter ;
				}
			}
		return out ;
		}

protected:

	/**
	 * Disable the copy constructor.  This method is declared
	 * but NOT defined.
	 */
	StringTokenizer( const StringTokenizer& ) ;

	/**
	 * Protected method called internally by the constructor once
	 * at object instantiation to tokenize the given C++ string.
	 */
	virtual void		Tokenize( const string& ) ;

	/**
	 * The non-tokenized string.
	 */
	const string		original ;

	/**
	 * The delimiter by which the (original) string is tokenized.
	 */
	const char		delimiter ;

	/**
	 * The structure for holding the tokens.
	 */
	vectorType		array ;

} ;

} // namespace gnuworld

#endif /* __STRINGTOKENIZER_H */
