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
 * $Id: EConfig.h,v 1.3 2003/06/18 01:08:48 dan_karrels Exp $
 */

#ifndef __ECONFIG_H
#define __ECONFIG_H "$Id: EConfig.h,v 1.3 2003/06/18 01:08:48 dan_karrels Exp $"

#include	<iostream>
#include	<fstream>
#include	<string>
#include	<list>
#include	<map>

#include	"ELog.h"
#include	"misc.h" // noCaseCompare

namespace gnuworld 
{

using std::list ;
using std::string ;
using std::multimap ;

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
 *
 */
class EConfig
{

	/**
	 * The type of the map used to store the key/value pairs.
	 */
	typedef multimap< string, string, noCaseCompare > mapType ;

	/**
	 * This struct is used to store information about an entry
	 * into the physical configuration file.
	 */
	struct lineInfo
	{
		string			key ;
		string			value ;
		mapType::iterator	mapItr ;

	lineInfo() {}

	// This constructor is called when there is a comment
	// line.  Even though there are no value tokens, be sure
	// to record whatever comment is present.
	lineInfo( const string& key )
	 : key( key ) {}
	lineInfo( const string& key, const string& value,
		const mapType::iterator& mapItr )
	 : key( key ), value( value ), mapItr( mapItr )
	{}
	} ;

	/**
	 * Type used to store configuration file format.
	 */
	typedef list< lineInfo >	fileListType ;

public:

	/**
	 * The pair type for the mapType, it holds the key/value pairs.
	 */
	typedef mapType::value_type mapPairType ;

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
	EConfig( const string& fileName ) ;

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
	 * Find the first key/value pair for the given key.
	 * Note that each key may have more than one value,
	 * so a const iterator is returned here.
	 */
	virtual const_iterator Find( const string& findMe ) const ;

	/**
	 * Call this method to retrieve a const_iterator to the first
	 * value associated with the key (findMe).  If the key is
	 * not found, then an error message will be output and the
	 * program will terminate.
	 */
	virtual const_iterator Require( const string& findMe ) const ;

	/**
	 * Add a key/value pair to the config file.
	 * Note that this will not delete any existing entry, but
	 * will add a duplicate, which is supported by the EConfig
	 * class.
	 */
	virtual bool	Add( const string& key, const string& value ) ;

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

	/// Default constructor implicitly disabled
	/// by presence of any other constructor.

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
	virtual bool	removeSpaces( string& ) ;

	/**
	 * Parse the input file.
	 */
	virtual bool	readFile( std::ifstream& ) ;

	/**
	 * Write the current memory configuration to disk.
	 */
	virtual bool	writeFile() ;

	/**
	 * The name of the configuration file.
	 */
	string		configFileName ;

	/**
	 * The map used to store the file's key/value pairs.
	 */
	mapType		valueMap ;

	/**
	 * The DS used to store the actual format of the config
	 * file.
	 */
	fileListType	fileList ;

} ;

} // namespace gnuworld

#endif // __ECONFIG_H
