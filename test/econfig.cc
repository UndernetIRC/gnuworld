/**
 * econfig.cc
 * Author: Daniel Karrels dan@karrels.com
 */

#include	<string>
#include	<iostream>

#include	"EConfig.h"
#include	"ELog.h"
#include	"StringTokenizer.h"

using namespace std ;
using namespace gnuworld ;

ELog gnuworld::elog ;

void	printConfig( const EConfig& ) ;
void	printMenu() ;

void	findEntry( const EConfig& ) ;
void	deleteEntry( EConfig& ) ;
void	addEntry( EConfig& ) ;

int main( int argc, const char** argv )
{
if( argc != 2 )
	{
	cout	<< "Usage: "
		<< argv[ 0 ]
		<< " <conf file>"
		<< endl ;
	return -1 ;
	}
elog.setStream( &cout ) ;

cout	<< "Opening file "
	<< argv[ 1 ]
	<< "..."
	<< endl ;

EConfig conf( argv[ 1 ] ) ;

bool keepRunning = true ;
while( keepRunning )
	{
	cout	<< endl ;
	printConfig( conf ) ;
	printMenu() ;

	cout	<< endl
		<< "Please enter a choice: " ;
	cout.flush() ;

	string line ;
	std::getline( cin, line ) ;

	int choice = ::atoi( line.c_str() ) ;

	switch( choice )
		{
		case 0:
			keepRunning = false ;
			break ;
		case 1:
			findEntry( conf ) ;
			break ;
		case 2:
			deleteEntry( conf ) ;
			break ;
		case 3:
			addEntry( conf ) ;
			break ;
		default:
			cout	<< "Unrecognized input: "
				<< choice
				<< endl ;
			break ;
		} // switch()
	} // while( keepRunning)

return 0 ;
}

void printConfig( const EConfig& conf )
{
cout	<< "-------- Current Configuration --------"
	<< endl
	<< endl ;

for( EConfig::const_iterator itr = conf.begin() ;
	itr != conf.end() ; ++itr )
	{
	cout	<< itr->first
		<< '/'
		<< itr->second
		<< endl ;
	}
}

void printMenu()
{
cout	<< endl
	<< "========== Menu =========="
	<< endl ;
cout	<< "0: Exit"
	<< endl ;
cout	<< "1: Find an entry"
	<< endl ;
cout	<< "2: Delete an entry"
	<< endl ;
cout	<< "3: Add an entry"
	<< endl ;
}

void findEntry( const EConfig& conf )
{
cout	<< "Please enter the key to find: " ;
cout.flush() ;

string findMe ;
std::getline( cin, findMe ) ;
cout	<< endl ;

bool foundIt = false ;
for( EConfig::const_iterator itr = conf.Find( findMe ) ;
	(itr != conf.end()) && (itr->first == findMe) ;
	++itr )
	{
	foundIt = true ;

	cout	<< "Key: "
		<< itr->first
		<< ", Value: "
		<< itr->second
		<< endl ;
	}
if( !foundIt )
	{
	cout	<< "Unable to find key \""
		<< findMe
		<< "\" in the config file"
		<< endl ;
	}
}

void deleteEntry( EConfig& conf )
{


}

void addEntry( EConfig& conf )
{
cout	<< "Please enter key and value to add: " ;
cout.flush() ;

string line ;
std::getline( cin, line ) ;

cout	<< "addEntry> line: "
	<< line
	<< endl ;

StringTokenizer st( line ) ;
if( st.size() < 2 )
	{
	cout	<< "You must enter at least 2 white space delimited "
		<< "tokens."
		<< endl ;
	return ;
	}

if( !conf.Add( st[ 0 ], st.assemble( 1 ) ) )
	{
	cout	<< "Unable to add: "
		<< st[ 0 ]
		<< '/'
		<< st.assemble( 1 )
		<< endl ;
	}
else
	{
	cout	<< "Added "
		<< st[ 0 ]
		<< '/'
		<< st.assemble( 1 )
		<< endl ;
	}
}

