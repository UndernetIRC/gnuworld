/**
 * Buffer.h
 * Author: Daniel Karrels dan@karrels.com
 *
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
 * $Id: Buffer.h,v 1.7 2003/12/29 23:59:36 dan_karrels Exp $
 */

#ifndef __BUFFER_H
#define __BUFFER_H "$Id: Buffer.h,v 1.7 2003/12/29 23:59:36 dan_karrels Exp $"

#include	<iostream>
#include	<string>

namespace gnuworld
{

/**
 * This class represents a dynamic buffer capable or parsing out
 * (delimiter) delimited lines of text.
 * This is a very useful class for using as an input buffer because
 * of the ReadLine() method, and a useful class for ouput buffering
 * because it is unlimited in size and provides a Delete() method
 * to delete an arbitrary number of bytes.
 */
class Buffer
{

public:

	/**
	 * The type used to represent the string, could be useful
	 * if this class is ever templated (again).
	 */
	typedef std::string stringType ;

	/**
	 * Initialize the Buffer with the given string and
	 * delimiting character
	 */
	Buffer( char = '\n' ) ;

	/**
	 * Destroy this Buffer object.  No internal allocation
	 * is done, so this method is a no-op.
	 */
	virtual ~Buffer() ;

	/**
	 * Read a (delimiter) delimited token from the
	 * buffer and write it into the string
	 * passed as argument, erasing the line from
	 * the Buffer once complete.
	 */
	virtual bool		ReadLine( std::string& ) ;

	/**
	 * Define the type of the variable used to maintain
	 * size (number of bytes) of Buffer.
	 */
	typedef std::string::size_type size_type ;

	/**
	 * Return the the number of characters currently in
	 * the Buffer.
	 */
	virtual size_type		size() const
		{ return buf.size() ; }

	/**
	 * Return the the number of characters currently in
	 * the Buffer.
	 */
	virtual size_type		length() const
		{ return buf.length() ; }

	/**
	 * Return true if the buffer is empty, false otherwise.
	 */
	virtual bool			empty() const
		{ return buf.empty() ; }

	/**
	 * Clear all data from the Buffer.
	 * For some reason, GNU gcc's implementation of std::string doesn't
	 * include clear(), do it the old fashion way here.
	 */
	virtual void			clear()
		{ buf.erase() ; }

	/**
	 * Remove the given number of bytes from the
	 * beginning of the Buffer.
	 */
	virtual void		Delete( const size_type& ) ;

	/**
	 * Return a substring of this Buffer, starting at (index),
	 * and continuing (len) characters into the Buffer.
	 * This method is const, and therefore does not alter the
	 * Buffer.
	 */
	inline std::string	substr( const size_type& index,
				const size_type& len ) const
		{ return buf.substr( index, len ) ; }

	/**
	 * Return a const reference to a C++ string
	 * representation of this Buffer.
	 */
	inline const std::string&	toString() const
		{ return buf ; }

	/**
	 * Return a const reference to a C++ string representation
	 * of this Buffer.
	 */
	inline const std::string&	operator*() const
		{ return buf ; }

	/**
	 * Return a pointer to the C NULL terminated character
	 * array representation of this array.  Note that the
	 * data is const, and thus cannot be altered by the
	 * pointer returned by this method.
	 */
	inline const char*	c_str() const
		{ return buf.c_str() ; }

	/**
	 * Return a pointer to the character array representation
	 * of this buffer.
	 * Note that conforming to the C++ standard on std::string,
	 * this data may NOT be null terminated.
	 */
	inline const char*	data() const
		{ return buf.data() ; }

	/**
	 * Append a give number of bytes from appendMe to this
	 * Buffer.  This is useful for storing binary data in an
	 * object of this class.
	 */
	inline void		append( const char* appendMe, size_t numBytes )
		{ buf.append( appendMe, numBytes ) ; }

	/**
	 * Concatenate the given C++ string to the end of the
	 * Buffer.
	 */
	inline Buffer&		operator+=( const std::string& addMe )
		{ buf += addMe ; return *this ; }

	/**
	 * Concatenate the given Buffer onto the end of this
	 * Buffer.
	 */
	inline Buffer&		operator+=( const Buffer& addMe )
		{ buf += addMe.buf ; return *this ; }

	/**
	 * Assign to this Buffer the contents of the C++ string
	 * passed as argument.
	 */
	inline Buffer&		operator=( const std::string& replaceWithMe )
		{ buf = replaceWithMe ; return *this ; }

	/**
	 * Assign to this Buffer the contents of the Buffer passed
	 * as argument.
	 */
	inline Buffer&		operator=( const Buffer& replaceWithMe )
		{ buf = replaceWithMe.buf ; return *this ; }

	/**
	 * Debugging function for output of this Buffer to a C++
	 * output stream.
	 */
	inline friend std::ostream& operator<<( std::ostream& o,
			const Buffer& b )
		{
		o << b.buf ;
		return o ;
		}

protected:

	/**
	 * This is the internal representation of this Buffer.
	 */
	std::string	buf ;

	/**
	 * This is the delimiting character.
	 */
	char		delim ;

} ;

} // namespace gnuworld

#endif /* __BUFFER_H */
