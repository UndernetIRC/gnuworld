/**
 * EConfig.cc
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
 * $Id: EConfig.cc,v 1.4 2003/06/18 01:08:48 dan_karrels Exp $
 */

#include	<string>
#include	<fstream>
#include	<stdexcept>
#include	<iostream>
#include	<map>

#include	<cstdlib>
#include	<cstdio> // rename()
#include	<cstring> // strerror()
#include	<cerrno>

#include	"EConfig.h"
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"misc.h"

const char rcsId[] = "$Id: EConfig.cc,v 1.4 2003/06/18 01:08:48 dan_karrels Exp $" ;

namespace gnuworld
{

using std::ifstream ;
using std::ofstream ;
using std::string ;
using std::endl ;
using std::map ;

EConfig::EConfig( const string& _configFileName )
 : configFileName( _configFileName )
{
ifstream configFile( configFileName.c_str() ) ;
if( !configFile.is_open() )
	{
	elog	<< "EConfig: Unable to open file: "
		<< configFileName
		<< endl ;
	::exit( 0 ) ;
	}
if( !readFile( configFile ) )
	{
	::exit( 0 ) ;
	}
configFile.close() ;
}

EConfig::~EConfig()
{
// No heap space allocated
valueMap.clear() ;
fileList.clear() ;
}

EConfig::const_iterator EConfig::Find( const string& findMe ) const
{
return valueMap.find( findMe ) ;
}

EConfig::const_iterator EConfig::Require( const string& key ) const
{
// Attempt to find the key in the map
const_iterator ptr = valueMap.find( key ) ;

// Was it found?
if( ptr == valueMap.end() )
	{
	// Nope, this method is intended to "require" the config file
	// to have a certain key/value pair.  Since this is not the
	// case, print out an error and quit.
	elog	<< "EConfig::Require> Configuration requires value "
		<< "for key \""
		<< key << "\""
		<< endl ;
	::exit( 0 ) ;
	}

// At least one key/value pair for this key exists; return it
return ptr ;
}

bool EConfig::readFile( ifstream& configFile )
{
size_t lineNumber = 0 ;
string tmp ;

while( getline( configFile, tmp ) )
	{
	// Increment lineNumber before
	// any continue statements
	lineNumber++ ;

	if( !removeSpaces( tmp ) )
		{
		// Parse error
		elog	<< "EConfig: Parse error at line: "
			<< lineNumber
			<< endl ;
		return false ;
		}

	if( tmp.empty() || '#' == tmp[ 0 ] )
		{
		fileList.push_back( lineInfo( tmp ) ) ;

		// Ignore this line
		continue ;
		}

	// The line should now be of the form:
	// variablename=value
	StringTokenizer st( tmp, '=' ) ;

	if( st.size() < 2 )
		{
		elog	<< "EConfig: Improper number of fields "
			<< "at line: "
			<< lineNumber
			<< std::endl ;
		return false ;
		}

	// Looks ok
	mapType::iterator mapItr =
		valueMap.insert( mapPairType( st[ 0 ],
			st.assemble( 1 ) ) ) ;

	fileList.push_back( lineInfo( st[ 0 ], st.assemble( 1 ),
		mapItr ) ) ;
	}

return true ;

}

bool EConfig::removeSpaces( string& line )
{

// If the first character is '#', just return true
if( !line.empty() && ('#' == line[ 0 ]) )
	{
	return true ;
	}

string::iterator ptr = line.begin() ;

bool inValue = false ;

// Only continue up until the value field
while( ptr != line.end() )
	{
	if( ('=' == *ptr) && !inValue )
		{
		// We've reached the value field
		// Only remove comments from this point
		// on.
		if( ((ptr + 1) != line.end()) && (*(ptr + 1) == ' ') )
			{
			ptr = line.erase( ptr + 1 ) ;
			}
		inValue = true ;
		}

	// Remove any white space characters, so long
	// as we're not in the value field
	else if( (' ' == *ptr || '\t' == *ptr) && !inValue )
		{

		// erase() invalidates the iterator
		ptr = line.erase( ptr ) ;

		continue ;
		}

	else
		{
		++ptr ;
		}
	}

// Remove trailing whitespace
while( !line.empty()
	&& ((' ' == line[ line.size() - 1 ])
	|| ('\t' == line[ line.size() - 1 ])) )
	{
	line.erase( line.size() - 1 ) ;
	}

return true ;
}

bool EConfig::Add( const string& key, const string& value )
{
// TODO: Add timestamp info?
// Update the valueMap, which is used by clients of this class
iterator mapItr =
	valueMap.insert( mapType::value_type( key, value ) ) ;

// Update the fileMap, which is used to keep track of the
// format of the config file
lineInfo addMe( key, value, mapItr ) ;
fileList.push_back( addMe ) ;

return writeFile() ;
}

bool EConfig::writeFile()
{
// Move the old file to the /tmp directory to ensure we have
// a backup.
string shortName( configFileName ) ;

// Remove any leading directory path information
string::size_type slashPos = shortName.find_last_of( '/' ) ;
if( string::npos != slashPos )
	{
	// There is leading path information
	shortName = shortName.substr( slashPos, string::npos ) ;
	}
shortName = string( "/tmp/" ) + shortName ;

//elog	<< "EConfig::writeFile> Renaming "
//	<< configFileName
//	<< " to "
//	<< shortName
//	<< endl ;

if( ::rename( configFileName.c_str(), shortName.c_str() ) < 0 )
	{
	elog	<< "EConfig::writeFile> Unable to rename "
		<< configFileName
		<< " to "
		<< shortName
		<< " because: "
		<< strerror( errno )
		<< endl ;

	// Ok to leave the file open per class conditions.
	return false ;
	}

std::ofstream configFile( configFileName.c_str() ) ;
if( !configFile.is_open() )
	{
	elog	<< "EConfig::writeFile> Unable to open file: "
		<< configFileName
		<< ", because: "
		<< strerror( errno )
		<< endl ;

	if( ::rename( shortName.c_str(), configFileName.c_str() ) < 0 )
		{
		elog	<< "EConfig::writeFile> Unable to restore "
			<< "original file \""
			<< configFileName
			<< "\" from backup \""
			<< shortName
			<< "\" because: "
			<< strerror( errno )
			<< endl ;
		}
	return false ;
	}

for( fileListType::const_iterator fileItr = fileList.begin() ;
	fileItr != fileList.end() ; ++fileItr )
	{
	const lineInfo& theInfo = *fileItr ;
	if( theInfo.value.empty() )
		{
//		elog	<< "EConfig::writeFile> Writing: "
//			<< theInfo.key
//			<< endl ;

		// comment line
		configFile	<< theInfo.key
				<< endl ;

		continue ;
		}

//	elog	<< "EConfig::writeFile> Writing: "
//		<< theInfo.key
//		<< " = "
//		<< theInfo.value
//		<< endl ;

	configFile	<< theInfo.key
			<< " = "
			<< theInfo.value
			<< endl ;
	} // for()

configFile.close() ;
return true ;
} // writeFile()

} // namespace gnuworld
