/**
 * mtrie.cc
 *
 * $Id: mtrie.cc,v 1.6 2004/05/18 16:51:05 dan_karrels Exp $
 */

#include	<string>
#include	<fstream>
#include	<iostream>

#include	"MTrie.h"
#include	"ELog.h"
#include	"StringTokenizer.h"

using namespace std ;
using namespace gnuworld ;

void		printHelp() ;
void		handleSearch( const StringTokenizer& ) ;
void		handleErase( const StringTokenizer& ) ;
void		handleLevelDebug( const StringTokenizer& ) ;
void		handleMinLength( const StringTokenizer& ) ;

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
	if( st.empty() )
		{
		continue ;
		}

	if( st[ 0 ] == "help" )
		{
		printHelp() ;
		}
	else if( st[ 0 ] == "search" )
		{
		handleSearch( st ) ;
		}
	else if( st[ 0 ] == "erase" )
		{
		handleErase( st ) ;
		}
	else if( st[ 0 ] == "leveldebug" )
		{
		handleLevelDebug( st ) ;
		}
	else if( st[ 0 ] == "minlength" )
		{
		handleMinLength( st ) ;
		}
	else if( st[ 0 ] == "exit" || st[ 0 ] == "quit" )
		{
		}
	else
		{
		cout	<< "Unrecognized command: "
			<< st[ 0 ]
			<< endl ;
		}

	cout	<< endl ;
	} while( (line != "exit") && (line != "quit") ) ;

return 0 ;
}

void printHelp()
{
cout	<< endl
	<< "--- Help menu ---"
	<< endl
	<< endl
	<< "help\t\t\tThis menu"
	<< endl
	<< "exit/quit\t\tTerminate program"
	<< endl
	<< "search <mask>\t\tSearch for a given mask"
	<< endl
	<< "erase <mask>\t\tErase all values matching the given mask"
	<< endl
	<< "leveldebug <level num>\tReport information about a given "
	<< "level"
	<< endl
	<< "minlength <min length>\tFind keys with at least <min "
	<< "length> tokens"
	<< endl ;
}

void handleSearch( const StringTokenizer& st )
{
//cout	<< "handleSearch> line: "
//	<< line
//	<< endl ;

if( st.size() != 2 )
	{
	cout	<< "Error: Search requires exactly 1 argument"
		<< endl ;
	return ;
	}

// st[ 1 ] == key
typedef list< MTrie< string >::value_type > valuesListType ;
valuesListType values = hostTrie.find( st[ 1 ] ) ;

for( valuesListType::const_iterator vItr = values.begin() ;
	vItr != values.end() ; ++vItr )
	{
	cout	<< "Match: "
		<< vItr->second
		<< endl ;
	}
cout	<< "Search found "
	<< values.size()
	<< " match"
	<< ((values.size() != 1) ? "es" : "")
	<< endl ;
}

void handleErase( const StringTokenizer& st )
{
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

void handleLevelDebug( const StringTokenizer& st )
{
if( st.size() != 2 )
	{
	cout	<< "Error: leveldebug requires exactly 1 argument"
		<< endl ;
	return ;
	}

size_t searchLevel = static_cast< size_t >( atoi( st[ 1 ].c_str() ) ) ;

hostTrie.levelDebug( clog, searchLevel ) ;
}

void handleMinLength( const StringTokenizer& st )
{
if( st.size() != 2 )
	{
	cout	<< "Error: maxlength requires exactly 1 argument"
		<< endl ;
	return ;
	}

size_t minLength = static_cast< size_t >( atoi( st[ 1 ].c_str() ) ) ;

list< string > maxLengthStrings = hostTrie.findMinLength( minLength ) ;
if( !maxLengthStrings.empty() )
	{
	clog	<< "Found "
		<< maxLengthStrings.size()
		<< " hostnames with at least "
		<< minLength
		<< " tokens."
		<< endl
		<< endl ;
	}

for( list< string >::const_iterator itr = maxLengthStrings.begin() ;
	itr != maxLengthStrings.end() ; ++itr )
	{
	StringTokenizer tokens( *itr, '.' ) ;
	clog	<< "Length: "
		<< tokens.size()
		<< ", key: "
		<< *itr
		<< endl ;
	}
}
