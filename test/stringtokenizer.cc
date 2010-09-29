/**
 * stringtokenizer.cc
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
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
 * $Id: stringtokenizer.cc,v 1.8 2005/01/12 03:50:38 dan_karrels Exp $
 */

#include	<iostream>
#include	<fstream>
#include	<string>

#include	<ctime>
#include	<cstdlib>

#include	"gnuworld_config.h"

#ifdef HAVE_GETOPT_H
#include	<getopt.h>
#endif

#include	"StringTokenizer.h"
#include	"ELog.h"

using namespace std ;
using namespace gnuworld ;

void	usage( const string& ) ;
bool	getData( ifstream& inputFile, vector< string >& ) ;

size_t	maxLines = 10000 ;

int main( int argc, char** argv )
{
int c = EOF ;
bool verbose = false ;
size_t totalLines = 0 ;
string inputFileName ;

if( argc < 3 )
	{
	usage( argv[ 0 ] ) ;
	return 0 ;
	}

while( (c = getopt( argc, argv, "f:hn:v" )) != EOF )
	{
	switch( c )
		{
		case 'f':
			inputFileName = optarg ;
			break ;
		case 'h':
			usage( argv[ 0 ] ) ;
			return 0 ;
		case 'n':
			maxLines = atoi( optarg ) ;
			break ;
		case 'v':
			verbose = true ;
			break ;
		default:
			usage( argv[ 0 ] ) ;
			return 0 ;
		}
	}

if( maxLines < 1 )
	{
	cout	<< "Error: Please specify a positive number "
		<< "for maximum lines to read from file"
		<< endl ;
	return 0 ;
	}

ifstream inputFile( inputFileName.c_str() ) ;
if( !inputFile )
	{
	cout	<< "Error opening input file: "
		<< inputFileName
		<< endl ;
	return -1 ;
	}

cout	<< "Running in verbose mode"
	<< endl ;

cout	<< endl
	<< "Reading "
	<< maxLines
	<< " lines of text at a time from "
	<< inputFileName
	<< endl ;

cout	<< "Working" ;
cout.flush() ;

// Read the entire file into memory
vector< string > lines ;
clock_t begin, end, totalClocks1 = 0 ;

while( getData( inputFile, lines ) )
	{
	for( vector< string >::const_iterator ptr = lines.begin(),
		endPtr = lines.end() ; ptr != endPtr ; ++ptr )
		{
		begin = clock() ;
		StringTokenizer st( *ptr ) ;
		end = clock() ;

		totalClocks1 += (end - begin) ;

		++totalLines ;

		if( verbose )
			{
			cout	<< "[DATA] "
				<< *ptr
				<< endl ;
			cout	<< "[TOKENIZED] ("
				<< st.size()
				<< ") "
				<< st
				<< endl ;
			}
		}

	cout	<< "." ;
	cout.flush() ;

	} // while()

cout	<< "Done!"
	<< endl ;

cout	<< "Tokenized "
	<< totalLines
	<< " lines in "
	<< (totalClocks1 / CLOCKS_PER_SEC)
	<< " seconds"
	<< endl ;
cout	<< endl ;

return 0 ;
}

void usage( const string& progName )
{
cout	<< "Usage: "
	<< progName
	<< "[options]"
	<< endl ;
cout	<< "Options:"
	<< endl ;
cout	<< "-f <filename> (required)"
	<< endl ;
cout	<< "-h Prints this menu"
	<< endl ;
cout	<< "-n <number of lines> Reads <number of lines> lines of "
	<< "text from the given input "
	<< endl
	<< "                     file at a time. ("
	<< maxLines
	<< ")"
	<< endl ;
cout	<< "-v (false) Run in verbose, be careful of this :)"
	<< endl ;
}

bool getData( ifstream& inputFile, vector< string >& lines )
{
size_t i = 0 ;
string line ;

lines.clear() ;

for( ; getline( inputFile, line ) && (i <= maxLines) ; ++i )
	{
	lines.push_back( line ) ;
	}

return (i != 0) ;
}
