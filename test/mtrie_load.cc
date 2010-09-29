/**
 * mtrie_load.cc
 *
 * $Id: mtrie_load.cc,v 1.5 2005/01/12 03:50:38 dan_karrels Exp $
 */

#include	<map>
#include	<string>
#include	<vector>
#include	<fstream>
#include	<iostream>

#include	<ctime>

#include	"gnuworld_config.h"

#ifdef HAVE_GETOPT_H
#include	<getopt.h>
#endif

#include	"misc.h"
#include	"MTrie.h"
#include	"match.h"
#include	"StringTokenizer.h"

using namespace std ;
using namespace gnuworld ;

MTrie< string >* hostTrie = 0 ;

typedef multimap< string, string, noCaseCompare > hostMapType ;
hostMapType* hostMap = 0 ;

list< pair< string, string > > mm_find( const string& ) ;

void usage( const string& progName )
{
cout	<< "Usage: "
	<< progName
	<< " -f <socket log file> [options]"
	<< endl ;
cout	<< endl
	<< "Options: "
	<< endl
	<< "-q\tRun quietly"
	<< endl
	<< "-n\t<numIterations> Run a given number of "
	<< "iterations of each host [1]"
	<< endl
	<< "-m\t<input match file> File of wildcard strings to lookup"
	<< endl
	<< "-t\tUse the matching trie [default is multimap]"
	<< endl
	<< endl ;
}

int main( int argc, char** argv )
{
if( argc < 2 )
	{
	usage( argv[ 0 ] ) ;
	return 0 ;
	}

bool verbose = true ;
bool useTrie = false ;
size_t numIterations = 1 ;
string socketFileName ;
string matchFileName ;
vector< string > matchStrings ;

int c = 0 ;
while( (c = getopt( argc, argv, "qn:f:m:t" )) != EOF )
	{
	switch( c )
		{
		case 'q':
			verbose = false ;
			break ;
		case 'f':
			socketFileName = optarg ;
			break ;
		case 'm':
			matchFileName = optarg ;
			break ;
		case 'n':
			numIterations = static_cast< size_t >(
				atoi( optarg ) ) ;
			break ;
		case 't':
			useTrie = true ;
			break ;
		default:
			cout	<< "Unknown option: "
				<< c
				<< endl ;
			return -1 ;
		} // switch()
	} // while()

if( socketFileName.empty() )
	{
	cout	<< "Error: You must specify a socket file"
		<< endl ;
	usage( argv[ 0 ] ) ;
	return 0 ;
	}

if( matchFileName.empty() )
	{
	cout	<< "Error: You must specify a match strings file"
		<< endl ;
	usage( argv[ 0 ] ) ;
	return 0 ;
	}

ifstream socketFile( socketFileName.c_str() ) ;
if( !socketFile )
	{
	cout	<< "Unable to open socket file: "
		<< socketFileName
		<< endl ;
	return 0 ;
	}

ifstream matchFile( matchFileName.c_str() ) ;
if( !matchFile )
	{
	cout	<< "Unable to open match file: "
		<< matchFileName
		<< endl ;
	socketFile.close() ;
	return 0 ;
	}

if( useTrie )
	{
	hostTrie = new MTrie< string > ;
	}
else
	{
	hostMap = new hostMapType ;
	}

cout	<< endl
	<< "Socket file: "
	<< socketFileName << endl
	<< "Match file: "
	<< matchFileName << endl
	<< "Number of iterations: "
	<< numIterations << endl
	<< "Verbose: "
	<< (verbose ? "true" : "false")
	<< endl
	<< "Using test mode: "
	<< (useTrie ? "matching trie" : "multimap")
	<< endl ;

cout	<< endl
	<< "Reading match file "
	<< matchFileName
	<< "..." ;
cout.flush() ;

size_t numFound = 0 ;
string line ;

while( getline( matchFile, line ) )
	{
	if( line.empty() )
		{
		continue ;
		}
	matchStrings.push_back( line ) ;
	++numFound ;
	}
matchFile.close() ;

cout	<< numFound
	<< " match strings"
	<< endl ;

cout	<< "Reading socket file "
	<< socketFileName
	<< "..." ;
cout.flush() ;

numFound = 0 ;
while( getline( socketFile, line ) )
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
	if( useTrie )
		{
		hostTrie->insert( spaceTokens[ 6 ], spaceTokens[ 6 ] ) ;
		}
	else
		{
		hostMap->insert( make_pair( spaceTokens[ 6 ],
			spaceTokens[ 6 ] ) ) ;
		}
	++numFound ;

	} // while( getline() )
socketFile.close() ;

cout	<< numFound
	<< " hosts"
	<< endl ;

clock_t startTime = 0 ;
clock_t stopTime = 0 ;
clock_t totalTime = 0 ;

cout.flush() ;
for( vector< string >::const_iterator vItr = matchStrings.begin() ;
	vItr != matchStrings.end() ; ++vItr )
	{
	string findMe( *vItr ) ;

	if( verbose )
		{
		cout	<< endl
			<< "Match string: "
			<< findMe
			<< endl ;
		cout.flush() ;
		}

	if( useTrie )
		{
		startTime = clock() ;
		for( size_t i = 0 ; i < numIterations ; ++i )
			{
			hostTrie->find( findMe ) ;
			} // for( i )
		stopTime = clock() ;
		}
	else
		{
		startTime = clock() ;
		for( size_t i = 0 ; i < numIterations ; ++i )
			{
			mm_find( findMe ) ;
			} // for( i )
		stopTime = clock() ;
		}
	totalTime += (stopTime - startTime ) ;

	if( verbose )
		{
		cout	<< "Time: "
			<< (stopTime - startTime)
			<< " clocks, "
			<< ((stopTime - startTime) / CLOCKS_PER_SEC)
			<< " seconds"
			<< endl ;
		cout.flush() ;
		}
	} // for( vItr )

cout	<< endl
	<< "Performed "
	<< numIterations
	<< " iterations"
	<< endl
	<< "Total time: "
	<< totalTime
	<< " clocks, "
	<< (totalTime / CLOCKS_PER_SEC)
	<< " seconds"
	<< endl
	<< endl ;

// delete of NULL is ok, so no worries here
delete hostTrie ; hostTrie = 0 ;
delete hostMap ; hostMap = 0 ;

return 0 ;
}

list< pair< string, string > > mm_find( const string& findMe )
{
list< pair< string, string > > returnMe ;

// Must perform a linear search for
// this structure if either '?' or '*'
// is present.
if( findMe.find( '?' ) != string::npos ||
	findMe.find( '*' ) != string::npos )
	{
	for( hostMapType::const_iterator hmItr =
		hostMap->begin() ;
		hmItr != hostMap->end() ;
		++hmItr )
		{
		if( !match( findMe, hmItr->first ) )
			{
			returnMe.push_back( make_pair(
				hmItr->first, hmItr->second ) ) ;
			}
		} // for( hmItr )
	} // if( '?' || '*' )
else
	{
	hostMapType::const_iterator hmItr = hostMap->find( findMe ) ;
	for( ; (hmItr != hostMap->end()) && (hmItr->first == findMe) ;
		++hmItr )
		{
		returnMe.push_back( make_pair(
				hmItr->first, hmItr->second ) ) ;
		}
	} // else()

return returnMe ;
}
