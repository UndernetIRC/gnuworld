/**
 * mtrie_perf_summary.cc
 *
 * $Id: mtrie_perf_summary.cc,v 1.4 2004/05/18 16:51:05 dan_karrels Exp $
 */

#include	<string>
#include	<vector>
#include	<fstream>
#include	<iostream>
#include	<iterator>
#include	<algorithm>
#include	<functional>

#include	"ELog.h"
#include	"StringTokenizer.h"

using namespace std ;
using namespace gnuworld ;

void usage( const string& progName )
{
cout	<< "Usage: "
	<< progName
	<< " <mtrie performance file>"
	<< endl ;
cout	<< "This program accepts any number of performance files "
	<< "as input"
	<< endl ;
}

int main( int argc, char** argv )
{
if( argc < 2 )
	{
	usage( argv[ 0 ] ) ;
	return 0 ;
	}
size_t numFiles = static_cast< size_t >( argc - 1 ) ;

size_t totalNodes = 0 ;
size_t totalValues = 0 ;
size_t totalSubtrees = 0 ;

size_t maxLevel = 0 ;
size_t maxNumSubtrees = 0 ;
size_t maxNumValues = 0 ;

// strip off ".log"
StringTokenizer fileTokens( argv[ 1 ], '.' ) ;
string fileNameBase( fileTokens[ 0 ] ) ;

if( argc > 2 )
	{
	fileNameBase = "mtrie_perf_summary" ;
	}

string levelFileName( fileNameBase + "_level.dat" ) ;
string valueFileName( fileNameBase + "_value.dat" ) ;
string subtreeFileName( fileNameBase + "_subtree.dat" ) ;

ofstream levelFile( levelFileName.c_str() ) ;
if( !levelFile )
	{
	clog	<< "Unable to open output file: "
		<< levelFileName
		<< endl ;
	return 0 ;
	}

ofstream valueFile( valueFileName.c_str() ) ;
if( !valueFile )
	{
	clog	<< "Unable to open output file: "
		<< valueFileName
		<< endl ;
	levelFile.close() ;
	return 0 ;
	}

ofstream subtreeFile( subtreeFileName.c_str() ) ;
if( !subtreeFile )
	{
	clog	<< "Unable to open output file: "
		<< "mtrie_subtree.dat"
		<< endl ;
	levelFile.close() ;
	valueFile.close() ;
	return 0 ;
	}

clog	<< endl
	<< "Writing level information to: "
	<< levelFileName << endl
	<< "Writing value information to: "
	<< valueFileName << endl
	<< "Writing subtree information to: "
	<< subtreeFileName
	<< endl ;

typedef vector< size_t > vectorType ;
vectorType levelVec ;
vectorType subtreeVec ;
vectorType valueVec ;

for( int i = 1 ; i < argc ; ++i )
	{
	ifstream inFile( argv[ i ] ) ;
	if( !inFile )
		{
		cout	<< "Unable to open file: "
			<< argv[ i ]
			<< endl ;
		return 0 ;
		}

	clog	<< endl
		<< "Reading "
		<< argv[ i ]
		<< "..." ;

	string line ;
	while( getline( inFile, line ) )
		{
		if( line.empty() )
			{
			continue ;
			}

		StringTokenizer st( line ) ;
		if( st.size() != 3 )
			{
			clog	<< "Error: Invalid number of tokens in line: "
				<< line
				<< endl ;
			inFile.close() ;
			return -1 ;
			}

		size_t theLevel =
			static_cast< size_t >( atoi( st[ 0 ].c_str() ) ) ;
		size_t numSubtrees =
			static_cast< size_t >( atoi( st[ 1 ].c_str() ) ) ;
		size_t numValues =
			static_cast< size_t >( atoi( st[ 2 ].c_str() ) ) ;

		levelVec.push_back( theLevel ) ;
		subtreeVec.push_back( numSubtrees ) ;
		valueVec.push_back( numValues ) ;

		++totalNodes ;
		totalValues += numValues ;
		totalSubtrees += numSubtrees ;

		if( numValues > maxNumValues )
			{
			maxNumValues = numValues ;
			}
		if( theLevel > maxLevel )
			{
			maxLevel = theLevel ;
			}
		if( numSubtrees > maxNumSubtrees )
			{
			maxNumSubtrees = numSubtrees ;
			}
		} // while( getline() )

	inFile.close() ;
	} // for( i )


std::copy( levelVec.begin(), levelVec.end(),
	std::ostream_iterator< size_t >( levelFile, " " ) ) ;
std::copy( valueVec.begin(), valueVec.end(),
	std::ostream_iterator< size_t >( valueFile, " " ) ) ;
std::copy( subtreeVec.begin(), subtreeVec.end(),
	std::ostream_iterator< size_t >( subtreeFile, " " ) ) ;

levelFile.close() ;
valueFile.close() ;
subtreeFile.close() ;

clog	<< endl
	<< endl
	<< "Information for all "
	<< numFiles
	<< " input files" << endl
	<< endl
	<< "Number of hosts: "
	<< totalValues << endl
	<< "Number of hosts per file: "
	<< (totalValues / numFiles) << endl
	<< "Number of nodes: "
	<< totalNodes << endl
	<< "Number of nodes per file: "
	<< (totalNodes / numFiles) << endl
	<< "Max level: "
	<< maxLevel << endl
	<< "Maximum number of subtrees: "
	<< maxNumSubtrees << endl
	<< "Maximum number of values: "
	<< maxNumValues << endl
	<< "Average number of values per node: "
	<< ((double) totalValues / (double) totalNodes) << endl
	<< "Average number of subtrees per node: "
	<< ((double) totalSubtrees / (double) totalNodes) << endl
	<< endl ;

return 0 ;
}
