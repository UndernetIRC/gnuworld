/**
 * econfig.cc
 * Author: Daniel Karrels dan@karrels.com
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
 * $Id: econfig.cc,v 1.5 2004/05/18 16:51:05 dan_karrels Exp $
 */

#include	<string>
#include	<iostream>

#include	"EConfig.h"
#include	"ELog.h"
#include	"StringTokenizer.h"

using namespace std ;
using namespace gnuworld ;

void	printConfig( const EConfig& ) ;
void	printMenu() ;

void	findEntry( const EConfig& ) ;
void	deleteEntry( EConfig& ) ;
void	addEntry( EConfig& ) ;
void	replaceEntry( EConfig& ) ;
void	addComment( EConfig& ) ;

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
		case 4:
			replaceEntry( conf ) ;
			break ;
		case 5:
			addComment( conf ) ;
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
cout	<< "4: Replace an entry"
	<< endl ;
cout	<< "5: Add comment"
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
cout	<< endl ;

// First output all entries with a corresponding zero based
// index identifier.
unsigned short int counter = 0 ;
for( EConfig::const_iterator itr = conf.begin() ; itr != conf.end() ;
	++itr )
	{
	cout	<< counter
		<< ": "
		<< itr->first
		<< '/'
		<< itr->second
		<< endl ;
	++counter ;
	}

cout	<< endl ;
cout	<< "Please enter the number to remove: " ;
cout.flush() ;

string line ;
std::getline( cin, line ) ;

unsigned int removeMe = ::atoi( line.c_str() ) ;
if( removeMe >= conf.size() )
	{
	cout	<< "Invalid number."
		<< endl ;
	return ;
	}

// Find the entry to remove, and Delete it
bool removeOK = true ;
EConfig::iterator itr = conf.begin() ;
for( ; itr != conf.end() ; ++itr )
	{
	if( 0 == removeMe-- )
		{
		// Found the one to remove
		removeOK = conf.Delete( itr ) ;
		break ;
		}
	}

if( removeOK )
	{
	cout	<< "Successfully removed item"
		<< endl ;
	}
else
	{
	cout	<< "Removal failed."
		<< endl ;
	}
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

void replaceEntry( EConfig& conf )
{
cout	<< endl ;

// First output all entries with a corresponding zero based
// index identifier.
unsigned short int counter = 0 ;
for( EConfig::const_iterator itr = conf.begin() ; itr != conf.end() ;
	++itr )
	{
	cout	<< counter
		<< ": "
		<< itr->first
		<< '/'
		<< itr->second
		<< endl ;
	++counter ;
	}

cout	<< endl ;
cout	<< "Please enter the number to replace: " ;
cout.flush() ;

string line ;
std::getline( cin, line ) ;

unsigned int replaceMe = ::atoi( line.c_str() ) ;
if( replaceMe >= conf.size() )
	{
	cout	<< "Invalid number."
		<< endl ;
	return ;
	}

cout	<< "Please enter new value: " ;
cout.flush() ;

string newValue ;
std::getline( cin, newValue ) ;

if( newValue.empty() )
	{
	cout	<< "New value must be non-empty"
		<< endl ;
	return ;
	}

// Find the entry to replace
EConfig::iterator itr = conf.begin() ;
for( ; itr != conf.end() ; ++itr )
	{
	if( 0 == replaceMe-- )
		{
		// Found the one to replace
		break ;
		}
	}

if( conf.Replace( itr, newValue ) )
	{
	cout	<< "Successfully replaced item"
		<< endl ;
	}
else
	{
	cout	<< "Replacement failed."
		<< endl ;
	}
}

void addComment( EConfig& conf )
{
cout	<< "Enter new comment line: " ;
cout.flush() ;

string line ;
std::getline( cin, line ) ;

if( !conf.AddComment( line ) )
	{
	cout	<< "Unable to add comment line: "
		<< line
		<< endl ;
	}
else
	{
	cout	<< "Successfully added comment line: "
		<< line
		<< endl ;
	}
}
