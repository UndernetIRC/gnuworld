/**
 * EConfig.h
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
 * $Id: EConfig.h,v 1.7 2004/01/16 00:59:47 dan_karrels Exp $
 */

#ifndef __ECONFIG_H
#define __ECONFIG_H "$Id: EConfig.h,v 1.7 2004/01/16 00:59:47 dan_karrels Exp $"

#include	<iostream>
#include	<fstream>
#include	<string>
#include	<list>
#include	<map>

#include	"ELog.h"
#include	"misc.h" // noCaseCompare

namespace gnuworld 
{

/**
 * A simple configuration file parser.
 *
 * Format of config file:
 * - Any line *beginning* with '#' is a comment
 *
 * Fields are of the form:
 * - variablename = value
 * - variablename must be one word
 * - value may be any number of words, but may
 *   NOT continue onto the next line
 * - value fields may NOT have a '=' character in it
 */
class EConfig
{

	/**
	 * The type of the map used to store the key/value pairs.
	 */
	typedef std::multimap< std::string, std::string, noCaseCompare > 
			mapType ;

	/**
	 * This struct is used to store information about an entry
	 * into the physical configuration file.
	 */
	struct lineInfo
	{
		/**
		 * The key for the key/value pair.
		 */
		std::string		key ;

		/**
		 * The value for the key/value pair.
		 * An empty value string indicates that this line
		 * is a comment, and key contains the comment
		 * (including preceeding '#' where necessary.
		 */
		std::string		value ;

		/**
		 * An iterator into the key/value map for this line,
		 * so that modifications to this key/value pair can
		 * can be placed in the correct location in the file.
		 */
		mapType::iterator	mapItr ;

	/**
	 * Default constructor relies upon the default constructors
	 * of the member variables.
	 */
	lineInfo()
	{}

	/**
	 * This constructor is called when there is a comment
	 * line.  Even though there are no value tokens, be sure
	 * to record whatever comment is present.
	 */
	lineInfo( const std::string& key )
	 : key( key )
	{}

	/**
	 * Copy constructor, used in assignment (as in the key/value map).
	 */
	lineInfo( const lineInfo& rhs )
	 : key( rhs.key ), value( rhs.value ), mapItr( rhs.mapItr )
	{}

	/**
	 * Constructor receives the key, value, and map iterator.
	 */
	lineInfo( const std::string& key, const std::string& value,
		const mapType::iterator& mapItr )
	 : key( key ), value( value ), mapItr( mapItr )
	{}

	/**
	 * Permit assignment.
	 */
	lineInfo& operator=( const lineInfo& rhs )
	{
		key = rhs.key ;
		value = rhs.value ;
		mapItr = rhs.mapItr ;
		return *this ;
	}

	} ;

	/**
	 * Type used to store configuration file format.
	 */
	typedef std::list< lineInfo >	lineListType ;

public:

	/**
	 * The pair type for the mapType, it holds the key/value pairs.
	 */
	typedef mapType::value_type mapPairType ;

	/**
	 * The type used to store the number of significant key/value
	 * pairs stored.
	 */
	typedef mapType::size_type	size_type ;

	/**
	 * A const iterator that users of this class may use
	 * to safely walk through the structure.
	 */
	typedef mapType::const_iterator const_iterator ;

	/**
	 * A non-const iterator for walking through this
	 * structure.
	 */
	typedef mapType::iterator iterator ;

	/**
	 * Create a new config parser for the given config file name.
	 */
	EConfig( const std::string& fileName ) ;

	/**
	 * Destroy the EConfig object.  This will also close the
	 * input file.
	 */
	virtual ~EConfig() ;

	/**
	 * Obtain a const iterator to the beginning of the map
	 * of key/value pairs.
	 */
	inline const_iterator begin() const
		{ return valueMap.begin() ; }

	/**
	 * Obtain a const iterator to the end of the map of
	 * key/value pairs.
	 */
	inline const_iterator end() const
		{ return valueMap.end() ; }

	/**
	 * Obtain a iterator to the beginning of the map
	 * of key/value pairs.
	 */
	inline iterator		begin()
		{ return valueMap.begin() ; }

	/**
	 * Obtain a iterator to the end of the map of
	 * key/value pairs.
	 */
	inline iterator		end()
		{ return valueMap.end() ; }

	/**
	 * Return the number of significant key/value pairs stored.
	 */
	inline size_type	size() const
		{ return valueMap.size() ; }

	/**
	 * Return true if there was an error in processing
	 * the file.
	 */
	inline bool		hasError() const
		{ return error ; }

	/**
	 * Find the first key/value pair for the given key.
	 */
	virtual iterator Find( const std::string& findMe ) ;

	/**
	 * Find the first key/value pair for the given key.
	 */
	virtual const_iterator Find( const std::string& findMe ) const ;

	/**
	 * Call this method to retrieve a const_iterator to the first
	 * value associated with the key (findMe).  If the key is
	 * not found, then an error message will be output and the
	 * program will terminate.
	 */
	virtual iterator Require( const std::string& findMe ) ;

	/**
	 * Add a key/value pair to the config file.
	 * Note that this will not delete any existing entry, but
	 * will add a duplicate, which is supported by the EConfig
	 * class.
	 */
	virtual bool	Add( const std::string& key,
				const std::string& value ) ;

	/**
	 * Add a comment to the end of the file.
	 * This comment line may be empty.
	 */
	virtual bool	AddComment( const std::string& newComment ) ;

	/**
	 * Delete a key/value pair by key.  Only one pair whose key
	 * matches the given key will be deleted, and no guarantee
	 * is made about which one.
	 * If you have duplicates, and would like to remove a specific
	 * entry, use the other form of Delete().
	 */
	virtual bool	Delete( const std::string& key ) ;

	/**
	 * Remove a key value pair given its iterator.
	 */
	virtual bool	Delete( iterator itr ) ;

	/**
	 * Replace the value of a given key/value pair.
	 * It is important to use this method rather than just
	 * modifying the iterator itself.
	 */
	virtual bool	Replace( iterator itr,
		const std::string& newValue ) ;

	/**
	 * Debugging function for outputting the entire map to
	 * an output stream.
	 */
	friend ELog& operator<<( ELog& out, const EConfig& conf )
		{
		EConfig::const_iterator ptr = conf.begin() ;
		while( ptr != conf.end() )
			{
			out	<< "Key: " << ptr->first << ", "
				<< "Value: " << ptr->second << std::endl ;
			++ptr ;
			}
		return out ;
		}

protected:

	/**
	 * Disable copying, this method is declared but NOT defined.
	 */
	EConfig( const EConfig& ) ;

	/**
	 * Disable assignment, this method is declared but NOT defined.
	 */
	EConfig operator=( const EConfig& ) ;

	/**
	 * Remove blank spaces from the line of text.
	 */
	virtual bool	removeSpaces( std::string& ) ;

	/**
	 * Parse the input file.
	 */
	virtual bool	readFile( std::ifstream& ) ;

	/**
	 * Write the current memory configuration to disk.
	 */
	virtual bool	writeFile() ;

	/**
	 * Record that an error has occured.
	 */
	inline void	setError()
		{ error = true ; }

	/**
	 * The name of the configuration file.
	 */
	std::string	configFileName ;

	/**
	 * The error status field.
	 */
	bool		error ;

	/**
	 * The map used to store the file's key/value pairs.
	 */
	mapType		valueMap ;

	/**
	 * The DS used to store the actual format of the config
	 * file.
	 */
	lineListType	lineList ;

} ; // class EConfig

} // namespace gnuworld

#endif // __ECONFIG_H
