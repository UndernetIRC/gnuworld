/**
 * mtrie_perf_summary.cc
 *
 * $Id: mtrie_perf_summary.cc,v 1.1 2003/07/27 22:26:47 dan_karrels Exp $
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
}

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
	cout	<< "Unable to open file: "
		<< argv[ 1 ]
		<< endl ;
	return 0 ;
	}

clog	<< endl
	<< "Reading "
	<< argv[ 1 ]
	<< "..." ;

size_t totalNodes = 0 ;
size_t totalValues = 0 ;
size_t totalSubtrees = 0 ;

size_t maxLevel = 0 ;
size_t maxNumSubtrees = 0 ;
size_t maxNumValues = 0 ;

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

clog	<< endl
	<< "Information for input file "
	<< argv[ 1 ] << endl
	<< "Number of hosts: "
	<< totalValues << endl
	<< "Number of nodes: "
	<< totalNodes << endl
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
