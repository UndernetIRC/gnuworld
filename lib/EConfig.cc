/* EConfig.cc
 */

#include	<string>
#include	<fstream>
#include	<map>
#include	<stdexcept>
#include	<iostream>
#include	<cstdlib>

#include	"EConfig.h"
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"misc.h"

const char EConfig_h_rcsId[] = __ECONFIG_H ;
const char EConfig_cc_rcsId[] = "$Id: EConfig.cc,v 1.4 2001/01/08 00:01:07 dan_karrels Exp $" ;

using std::string ;
using std::fstream ;
using std::map ;
using std::clog ;
using std::endl ;

namespace gnuworld
{

EConfig::EConfig( const string& _configFileName )
 : configFileName( _configFileName ),
   configFile( configFileName.c_str(), ios::in | ios::out )
{
if( !configFile.is_open() )
	{
	elog	<< "EConfig: Unable to open file: "
		<< configFileName << endl ;
	::exit( 0 ) ;
	}
if( !ReadFile() )
	{
	::exit( 0 ) ;
	}
}

EConfig::~EConfig()
{
// No heap space allocated
CloseFile() ;
}

void EConfig::CloseFile()
{
// configFile is known to be open
configFile.close() ;
valueMap.clear() ;
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
	clog	<< "EConfig::Require> Configuration requires value "
		<< "for key \"" << key << "\"\n" ;
	::exit( 0 ) ;
	}

// At least one key/value pair for this key exists; return it
return ptr ;
}

bool EConfig::ReadFile()
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
			<< lineNumber << std::endl ;
		return false ;
		}

	if( tmp.empty() || '#' == tmp[ 0 ] )
		{
		// Ignore this line
		continue ;
		}

	// The line should now be of the form:
	// variablename=value
	StringTokenizer st( tmp, '=' ) ;

	if( st.size() != 2 )
		{
		elog	<< "EConfig: Improper number of fields "
			<< "at line: " << lineNumber << std::endl ;
		return false ;
		}

	// Looks ok
	valueMap.insert( mapPairType( st[ 0 ], st[ 1 ] ) ) ;

	}

return true ;

}

bool EConfig::removeSpaces( string& line )
{

// If the first character is '#', just return true
if( !line.empty() && '#' == line[ 0 ] )
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
return true ;
}

} // namespace gnuworld
