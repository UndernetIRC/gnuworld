/**
 * mtrie_perf_summary.cc
 *
 * $Id: mtrie_perf_summary.cc,v 1.2 2003/07/28 15:28:57 dan_karrels Exp $
 */

#include	<string>
#include	<fstream>
#include	<iostream>

#include	"ELog.h"
#include	"StringTokenizer.h"

using namespace std ;
using namespace gnuworld ;

ELog gnuworld::elog ;

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
