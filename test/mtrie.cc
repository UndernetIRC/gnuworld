/**
 * mtrie.cc
 *
 * $Id: mtrie.cc,v 1.3 2003/07/24 04:03:19 dan_karrels Exp $
 */

#include	<string>
#include	<fstream>
#include	<iostream>

#include	"MTrie.h"
#include	"ELog.h"
#include	"StringTokenizer.h"

using namespace std ;
using namespace gnuworld ;

ELog gnuworld::elog ;

void		printHelp() ;
void		handleSearch( const string& line ) ;
void		handleErase( const string& line ) ;

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

cout	<< "Entering command line mode, use \"exit\" to end"
	<< endl
	<< endl ;

do
	{
	cout	<< "> " ;
	cout.flush() ;

	std::getline( cin, line ) ;

	if( line.empty() )
		{
		continue ;
		}

	StringTokenizer st( line ) ;

	if( st[ 0 ] == "help" )
		{
		printHelp() ;
		}
	else if( st[ 0 ] == "search" )
		{
		handleSearch( line ) ;
		}
	else if( st[ 0 ] == "erase" )
		{
		handleErase( line ) ;
		}
	else if( st[ 0 ] == "exit" )
		{
		}
	else
		{
		cout	<< "Unrecognized command: "
			<< st[ 0 ]
			<< endl ;
		}

	cout	<< endl ;
	} while( (line != "exit") ) ;

return 0 ;
}

void printHelp()
{
cout	<< endl
	<< "--- Help menu ---"
	<< endl
	<< endl
	<< "help:\t\tThis menu"
	<< endl
	<< "search <mask>:\tSearch for a given mask"
	<< endl ;
}

void handleSearch( const string& line )
{
//cout	<< "handleSearch> line: "
//	<< line
//	<< endl ;

StringTokenizer st( line ) ;
if( st.size() != 2 )
	{
	cout	<< "Error: Search requires exactly 1 argument"
		<< endl ;
	return ;
	}

// st[ 1 ] == key
typedef list< MTrie< string >::value_type > valuesListType ;
valuesListType values = hostTrie.find( st[ 1 ] ) ;

cout	<< "Search found "
	<< values.size()
	<< " match"
	<< ((values.size() != 1) ? "es" : "")
	<< endl ;

for( valuesListType::const_iterator vItr = values.begin() ;
	vItr != values.end() ; ++vItr )
	{
	cout	<< "Match: "
		<< vItr->second
		<< endl ;
	}
}

void handleErase( const string& line )
{
StringTokenizer st( line ) ;
if( st.size() != 2 )
	{
	cout	<< "Error: Erase requires exactly 1 argument"
		<< endl ;
	return ;
	}

MTrie< string >::size_type eraseCount = hostTrie.erase( st[ 1 ] ) ;
clog	<< "Erased "
	<< eraseCount
	<< " element(s)"
	<< endl ;
}
