/* Buffer.h
 * Author: Daniel Karrels dan@karrels.com
 * Synapsis:
 * History: Mon Jan 25 23:29:47 EST 1999 -- beginning
 */

#ifndef __BUFFER_H
#define __BUFFER_H "$Id: Buffer.h,v 1.3 2001/07/29 22:44:06 dan_karrels Exp $"

#include	<iostream>
#include	<string>

namespace gnuworld
{

using std::basic_string ;

/**
 * This class represents a dynamic buffer capable or parsing out
 * (delimiter) delimited lines of text.
 * This is a very useful class for using as an input buffer because
 * of the ReadLine() method, and a useful class for ouput buffering
 * because it is unlimited in size and provides a Delete() method
 * to delete an arbitrary number of bytes.
 */
template< class charType = char >
class Buffer
{

public:

	typedef basic_string< charType > stringType ;

	/**
	 * Initialize the Buffer with the given string and
	 * delimiting character
	 */
	Buffer( const basic_string< charType >& =
		basic_string< charType >(), charType = '\n' ) ;

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
	virtual bool		ReadLine( basic_string< charType >& ) ;

	/**
	 * Define the type of the variable used to maintain
	 * size (number of bytes) of Buffer.
	 */
	typedef basic_string< charType >::size_type size_type ;

	/**
	 * Return the the number of characters currently in
	 * the Buffer.
	 */
	inline size_type		size() const
		{ return buf.size() ; }

	/**
	 * Return the the number of characters currently in
	 * the Buffer.
	 */
	inline size_type		length() const
		{ return buf.length() ; }

	/**
	 * Return the number of bytes that this Buffer can hold
	 * before having to reallocate internally.
	 */
	inline size_type		capacity() const
		{ return buf.capacity() ; }

	/**
	 * Return true if the buffer is empty, false otherwise.
	 */
	inline bool			empty() const
		{ return buf.empty() ; }

	/**
	 * Clear all data from the Buffer.
	 * For some reason, GNU gcc's implementation of std::string doesn't
	 * include clear(), do it the old fashion way here.
	 */
	inline void			clear()
		{ buf.erase( buf.begin(), buf.end() ) ; }

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
	inline basic_string< charType >	substr( const size_type& index,
				const size_type& len ) const
		{ return buf.substr( index, len ) ; }

	/**
	 * Return a const reference to a C++ string
	 * representation of this Buffer.
	 */
	inline const basic_string< charType >&	toString() const
		{ return buf ; }

	/**
	 * Return a const reference to a C++ string representation
	 * of this Buffer.
	 */
	inline const basic_string< charType >&	operator*() const
		{ return buf ; }

	/**
	 * Return a pointer to the C NULL terminated character
	 * array representation of this array.  Note that the
	 * data is const, and thus cannot be altered by the
	 * pointer returned by this method.
	 */
	inline const charType*	c_str() const
		{ return buf.c_str() ; }

	/**
	 * Concatenate the given character array onto the end of
	 * the Buffer.
	 */
	inline Buffer&		operator+=( const charType* addMe )
		{ buf += addMe ; return *this ; }

	/**
	 * Concatenate the given C++ string to the end of the
	 * Buffer.
	 */
	inline Buffer&		operator+=(
			const basic_string< charType >& addMe )
		{ buf += addMe ; return *this ; }

	/**
	 * Concatenate the given Buffer onto the end of this
	 * Buffer.
	 */
	inline Buffer&		operator+=( const Buffer& addMe )
		{ buf += addMe.buf ; return *this ; }

	/**
	 * Assign to this Buffer the contents of the character
	 * array passed as argument.
	 */
	inline Buffer&		operator=( const charType* replaceWithMe )
		{ buf = replaceWithMe ; return *this ; }

	/**
	 * Assign to this Buffer the contents of the C++ string
	 * passed as argument.
	 */
	inline Buffer&		operator=(
			const basic_string< charType >& addMe )
		{ buf = addMe ; return *this ; }

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
		{ o << b.buf ; return o ; }

protected:

	/**
	 * This is the internal representation of this Buffer.
	 */
	basic_string< charType >	buf ;

	/**
	 * This is the delimiting character.
	 */
	charType			delim ;

} ;

} // namespace gnuworld

#include	"Buffer.cc"

#endif /* __BUFFER_H */
