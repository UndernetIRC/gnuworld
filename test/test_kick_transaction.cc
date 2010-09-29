/**
 * test_kick_transaction.cc
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
 * $Id: test_kick_transaction.cc,v 1.3 2004/05/18 16:51:05 dan_karrels Exp $
 */

#include	<iostream>
#include	<string>
#include	<fstream>
#include	<map>

#include	"StringTokenizer.h"
#include	"ELog.h"

using namespace std ;
using namespace gnuworld ;

void usage( const string& progName )
{
cout	<< "Usage: "
	<< progName
	<< " <socket log file>"
	<< endl ;
}

struct kick
{
string	destNumeric ;
string	srcNumeric ;
string	chan ;
size_t	lineNumber ;

kick( const string& destNumeric,
	const string& srcNumeric,
	const string& chan,
	const size_t& lineNumber )
 : destNumeric( destNumeric ),
   srcNumeric( srcNumeric ),
   chan( chan ),
   lineNumber( lineNumber )
{}

inline bool operator()( const kick& lhs, const kick& rhs ) const
	{ return (lhs.destNumeric < rhs.destNumeric) ; }

friend ostream& operator<<( ostream& out, const kick& rhs )
	{
	out	<< rhs.lineNumber << ":"
		<< rhs.srcNumeric
		<< " K "
		<< rhs.destNumeric << " "
		<< rhs.chan ;
	return out ;
	}
} ;

typedef multimap< string, kick > kickMapType ;
kickMapType	kickMap ;
size_t lineNumber = 0 ;

void	checkCorrespondingKick( const StringTokenizer& ) ;
void	pruneKickMap( size_t ) ;

int main( int argc, char** argv )
{
if( argc != 2 )
	{
	usage( argv[ 0 ] ) ;
	return 0 ;
	}

ifstream inFile( argv[ 1 ] ) ;
if( !inFile )
	{
	cout	<< "Unable to open input file: "
		<< argv[ 1 ]
		<< endl ;
	return 0 ;
	}

string line ;

cout	<< "Reading input file..."
	<< endl ;

while( std::getline( inFile, line ) )
	{
	++lineNumber ;

	if( (lineNumber > 0) && (0 == (lineNumber % 100000) ) )
		{
		cout	<< "Line "
			<< lineNumber
			<< ", kickMap.size(): "
			<< kickMap.size()
			<< endl ;

		pruneKickMap( lineNumber - 25000 ) ;
		}

	// abcde K fghij #chan :reason
	StringTokenizer st( line ) ;
	if( st.size() < 5 )
		{
		continue ;
		}

	if( st[ 1 ] == "K" )
		{
		// Found kick
		// Add to the kickMap for comparing later
		kickMap.insert( make_pair( st[ 0 ],
			kick( st[ 3 ], // destNumeric
				st[ 0 ], // srcNumeric
				st[ 2 ], // chan
				lineNumber ) ) ) ;
		}

	if( st[ 1 ] == "L" )
		{
		// See if this part corresponds to a previous
		// kick
		checkCorrespondingKick( st ) ;
		}

	} // while()

inFile.close() ;
return 0 ;
}

void checkCorrespondingKick( const StringTokenizer& st )
{
// st[ 0 ] is the numeric if the client parting the channel
// Check for matching clients that have been kicked
kickMapType::iterator kItr = kickMap.find( st[ 0 ] ) ;
if( kItr == kickMap.end() )
	{
	// Not found
	return ;
	}

// Found the numeric in the kick map
for( ; (kItr != kickMap.end()) && (kItr->first == st[ 0 ]) ; ++kItr )
	{
//	cout	<< "Found destNumerics: "
//		<< kItr->second
//		<< " and "
//		<< lineNumber << ":"
//		<< st[ 0 ] << " L "
//		<< st[ 2 ]
//		<< endl ;

	if( kItr->second.chan != st[ 2 ] )
		{
		// Different channel
		continue ;
		}

	// Else, possible match
	cout	<< "Possible match: "
		<< kItr->second
		<< " with "
		<< lineNumber << ":"
		<< st[ 0 ] << " L "
		<< st[ 2 ]
		<< endl ;
	} // for()

}

// Remove any kicks from the kickMap if they are at least
// as old as "age"
void pruneKickMap( size_t age )
{
for( kickMapType::iterator kItr = kickMap.begin() ;
	kItr != kickMap.end() ; ++kItr )
	{
	if( kItr->second.lineNumber <= age )
		{
		kickMap.erase( kItr ) ;
		}
	}
}
