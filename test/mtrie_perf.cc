/**
 * mtrie_perf.cc
 *
 * $Id: mtrie_perf.cc,v 1.3 2004/05/18 16:51:05 dan_karrels Exp $
 */

#include	<string>
#include	<fstream>
#include	<iostream>

#include	"MTrie.h"
#include	"ELog.h"
#include	"StringTokenizer.h"

using namespace std ;
using namespace gnuworld ;

MTrie< string > hostTrie ;

void usage( const string& progName )
{
cout	<< "Usage: "
	<< progName
	<< " <socket log file>"
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

string outputFileName( "mtrie_perf_" ) ;
StringTokenizer fileTokens( argv[ 1 ], '/' ) ;
outputFileName += fileTokens[ fileTokens.size() - 1 ] ;

ofstream outFile( outputFileName.c_str() ) ;
if( !outFile )
	{
	cout	<< "Unable to open output file: "
		<< outputFileName
		<< endl ;
	inFile.close() ;
	return -1 ;
	}

clog	<< endl
	<< "Reading "
	<< argv[ 1 ]
	<< "..." ;

string line ;
size_t numFound = 0 ;
while( getline( inFile, line ) )
	{
	if( line.empty() )
		{
		continue ;
		}

//Ay N tealie- 2 1053874792 ~tealie 7-11.sbg.org +i BCXaYv AyADd :Tealie
	StringTokenizer spaceTokens( line ) ;
	if( spaceTokens[ 1 ] == "EB" )
		{
		break ;
		}

	if( spaceTokens.size() < 8 )
		{
		continue ;
		}

	if( spaceTokens[ 1 ] != "N" )
		{
		continue ;
		}

	StringTokenizer dotTokens( spaceTokens[ 6 ], '.' ) ;
	if( dotTokens.size() < 2 )
		{
		continue ;
		}

	// Got one
	hostTrie.insert( spaceTokens[ 6 ], spaceTokens[ 6 ] ) ;
	++numFound ;

	} // while( getline() )
inFile.close() ;

clog	<< endl
	<< "Read "
	<< numFound
	<< " hosts"
	<< endl
	<< endl ;

clog	<< "Writing debug information to "
	<< outputFileName
	<< "..." ;
clog.flush() ;

hostTrie.dumpDebug( outFile ) ;
clog	<< "done"
	<< endl ;

outFile.close() ;
return 0 ;
}
